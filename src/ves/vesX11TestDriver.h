#ifndef VESX11TESTDRIVER_H
#define VESX11TESTDRIVER_H

#include  "vesTestDriver.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <GLES/gl.h>
#include <GLES/egl.h>

template <typename AppT>
class vesX11TestDriver : public vesTestDriver<AppT>
{
public:
  vesX11TestDriver(AppT* app) : vesTestDriver<AppT>(app),
    m_outStr(NULL),
    m_width(800),
    m_height(600)
  {
    m_xDpy = NULL;
    m_eglSurf = NULL;
    m_eglCtx = NULL;
    m_eglDpy = NULL;

    m_dpyName = NULL;
    m_printInfo = GL_FALSE;

    m_haveLastMotion = false;
    m_lastMotionX = 0;
    m_lastMotionY = 0;
    m_currentX = 0;
    m_currentY = 0;
  }

  void make_x_window(Display *m_xDpy, EGLDisplay m_eglDpy,
                     const char *name,
                     int x, int y, int m_width, int m_height,
                     Window *winRet,
                     EGLContext *ctxRet,
                     EGLSurface *surfRet)
  {
     static const EGLint attribs[] = {
        EGL_RED_SIZE, 1,
        EGL_GREEN_SIZE, 1,
        EGL_BLUE_SIZE, 1,
        EGL_DEPTH_SIZE, 1,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
        EGL_NONE
     };
     static const EGLint ctx_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 1,
        EGL_NONE
     };
     int scrnum;
     XSetWindowAttributes attr;
     unsigned long mask;
     Window root;
     Window m_win;
     XVisualInfo *visInfo, visTemplate;
     int num_visuals;
     EGLContext ctx;
     EGLConfig config;
     EGLint num_configs;
     EGLint vid;

     scrnum = DefaultScreen( m_xDpy );
     root = RootWindow( m_xDpy, scrnum );

     if (!eglChooseConfig( m_eglDpy, attribs, &config, 1, &num_configs)) {
        printf("Error: couldn't get an EGL visual config\n");
        exit(1);
     }

     assert(config);
     assert(num_configs > 0);

     if (!eglGetConfigAttrib(m_eglDpy, config, EGL_NATIVE_VISUAL_ID, &vid)) {
        printf("Error: eglGetConfigAttrib() failed\n");
        exit(1);
     }

     /* The X window visual must match the EGL config */
     visTemplate.visualid = vid;
     visInfo = XGetVisualInfo(m_xDpy, VisualIDMask, &visTemplate, &num_visuals);
     if (!visInfo) {
        printf("Error: couldn't get X visual\n");
        exit(1);
     }

     /* window attributes */
     attr.background_pixel = 0;
     attr.border_pixel = 0;
     attr.colormap = XCreateColormap( m_xDpy, root, visInfo->visual, AllocNone);
     attr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | ButtonMotionMask;
     mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

     m_win = XCreateWindow( m_xDpy, root, 0, 0, m_width, m_height,
                          0, visInfo->depth, InputOutput,
                          visInfo->visual, mask, &attr );

     /* set hints and properties */
     {
        XSizeHints sizehints;
        sizehints.x = x;
        sizehints.y = y;
        sizehints.width  = this->m_width;
        sizehints.height = this->m_height;
        sizehints.flags = USSize | USPosition;
        XSetNormalHints(m_xDpy, m_win, &sizehints);
        XSetStandardProperties(m_xDpy, m_win, name, name,
                                None, (char **)NULL, 0, &sizehints);
     }

  #if USE_FULL_GL /* XXX fix this when eglBindAPI() works */
     eglBindAPI(EGL_OPENGL_API);
  #else
     eglBindAPI(EGL_OPENGL_ES_API);
  #endif

     ctx = eglCreateContext(m_eglDpy, config, EGL_NO_CONTEXT, ctx_attribs );
     if (!ctx) {
        printf("Error: eglCreateContext failed\n");
        exit(1);
     }

     /* test eglQueryContext() */
     {
        EGLint val;
        eglQueryContext(m_eglDpy, ctx, EGL_CONTEXT_CLIENT_VERSION, &val);
        assert(val == 1);
     }

     *surfRet = eglCreateWindowSurface(m_eglDpy, config, m_win, NULL);
     if (!*surfRet) {
        printf("Error: eglCreateWindowSurface failed\n");
        exit(1);
     }

     /* sanity checks */
     {
        EGLint val;
        eglQuerySurface(m_eglDpy, *surfRet, EGL_WIDTH, &val);
        assert(val == m_width);
        eglQuerySurface(m_eglDpy, *surfRet, EGL_HEIGHT, &val);
        assert(val == m_height);
        assert(eglGetConfigAttrib(m_eglDpy, config, EGL_SURFACE_TYPE, &val));
        assert(val & EGL_WINDOW_BIT);
     }

     XFree(visInfo);

     *winRet = m_win;
     *ctxRet = ctx;
  }


  virtual int init()
  {
    m_xDpy = XOpenDisplay(m_dpyName);
    if (!m_xDpy) {
      printf("Error: couldn't open display %s\n",
             m_dpyName ? m_dpyName : getenv("DISPLAY"));
      return -1;
    }

    m_eglDpy = eglGetDisplay(m_xDpy);
    if (!m_eglDpy) {
      printf("Error: eglGetDisplay() failed\n");
      return -1;
    }

    if (!eglInitialize(m_eglDpy, &m_eglMajor, &m_eglMinor)) {
      printf("Error: eglInitialize() failed\n");
      return -1;
    }

    m_outStr = eglQueryString(m_eglDpy, EGL_VERSION);
    printf("EGL_VERSION = %s\n", m_outStr);

    m_outStr = eglQueryString(m_eglDpy, EGL_VENDOR);
    printf("EGL_VENDOR = %s\n", m_outStr);

    m_outStr = eglQueryString(m_eglDpy, EGL_EXTENSIONS);
    printf("EGL_EXTENSIONS = %s\n", m_outStr);

    m_outStr = eglQueryString(m_eglDpy, EGL_CLIENT_APIS);
    printf("EGL_CLIENT_APIS = %s\n", m_outStr);

    make_x_window(m_xDpy, m_eglDpy,
                 "OpenGL ES 1.x tri", 0, 0, m_width, m_height,
                 &m_win, &m_eglCtx, &m_eglSurf);

    XMapWindow(m_xDpy, m_win);
    if (!eglMakeCurrent(m_eglDpy, m_eglSurf, m_eglSurf, m_eglCtx)) {
      printf("Error: eglMakeCurrent() failed\n");
      return -1;
    }

    if (m_printInfo) {
      printf("GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
      printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
      printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
      printf("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));
    }

    return 0;
  }


  void render()
  {
    this->m_test->render();
  }


  void start()
  {
    this->event_loop(m_xDpy, m_win, m_eglDpy, m_eglSurf);
  }


  void finalize()
  {
    eglDestroyContext(m_eglDpy, m_eglCtx);
    eglDestroySurface(m_eglDpy, m_eglSurf);
    eglTerminate(m_eglDpy);

    XDestroyWindow(m_xDpy, m_win);
    XCloseDisplay(m_xDpy);
  }


  void event_loop(Display *dpy, Window m_win,
    EGLDisplay m_eglDpy, EGLSurface m_eglSurf)
  {
    vesNotUsed(m_win);

     while (1) {
        int redraw = 0;
        XEvent event;

        XNextEvent(dpy, &event);

        switch (event.type) {
        case Expose:
           redraw = 1;
           break;
        case ConfigureNotify:
//           this->m_test->resizeView(event.xconfigure.width, event.xconfigure.height);
           break;

        case ButtonPress:
          this->m_test->handleSingleTouchDown(event.xbutton.x, event.xbutton.y);

          m_haveLastMotion = true;
          m_lastMotionX = event.xbutton.x;
          m_lastMotionY = event.xbutton.y;
          break;

        case ButtonRelease:
          this->m_test->handleSingleTouchUp();
          m_haveLastMotion = false;
          break;

        case MotionNotify:

          if (m_haveLastMotion) {
            m_currentX = event.xmotion.x;
            m_currentY = event.xmotion.y;
            this->m_test->handleSingleTouchPanGesture(m_currentX - m_lastMotionX, m_currentY - m_lastMotionY);
            m_lastMotionX = m_currentX;
            m_lastMotionY = m_currentY;
            redraw = 1;
          }
          break;

        case KeyPress:
           {
              int panDelta = 100;
              char buffer[10];
              int r, code;
              code = XLookupKeysym(&event.xkey, 0);
              if (code == XK_Left) {
                this->m_test->handleSingleTouchPanGesture(-panDelta, 0);
              }
              else if (code == XK_Right) {
                this->m_test->handleSingleTouchPanGesture(panDelta, 0);
              }
              else if (code == XK_Up) {
                this->m_test->handleSingleTouchPanGesture(0, -panDelta);
              }
              else if (code == XK_Down) {
                this->m_test->handleSingleTouchPanGesture(0, panDelta);
              }
              else {
                 r = XLookupString(&event.xkey, buffer, sizeof(buffer),
                                   NULL, NULL);
                 if (buffer[0] == 27) {
                    /* escape */
                    return;
                 }
                 else if (buffer[0] == 'r') {
                   this->m_test->resetView();
                 }
              }
           }
           redraw = 1;
           break;
        default:
           ; /*no-op*/
        }

        if (redraw) {
           this->m_test->render();
           eglSwapBuffers(m_eglDpy, m_eglSurf);
        }
     }
  }


  virtual int width()
  {
    return this->m_width;
  }


  virtual int height()
  {
    return this->m_height;
  }

public:
  Display *m_xDpy;
  Window m_win;

  EGLSurface m_eglSurf;
  EGLContext m_eglCtx;
  EGLDisplay m_eglDpy;

  char *m_dpyName;
  GLboolean m_printInfo;
  EGLint m_eglMajor, m_eglMinor;

  const char *m_outStr;
  const int m_width;
  const int m_height;

  bool m_haveLastMotion;
  int m_lastMotionX;
  int m_lastMotionY;
  int m_currentX;
  int m_currentY;
};

#endif // VESX11TESTDRIVER_H
