/*========================================================================
  VES --- VTK OpenGL ES Rendering Toolkit

      http://www.kitware.com/ves

  Copyright 2011 Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 ========================================================================*/

#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include <vesRenderer.h>
#include <vesCamera.h>
#include <vesShader.h>
#include <vesShaderProgram.h>
#include <vesMapper.h>
#include <vesActor.h>
#include <vesLegacyReader.h>
#include <vesGMTL.h>

#include <vtkPolyDataToTriangleData.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <vtkXMLPolyDataReader.h>

namespace {

class RenderView {
public:

  vesRenderer* renderer() {
    return this->Renderer;
  }

  void setRenderer(vesRenderer* renderer) {
    this->Renderer = renderer;
  }

  vesShader* shader() {
    return this->Shader;
  }

  void setShader(vesShader* shader) {
    this->Shader = shader;
  }

  vesShaderProgram* shaderProgram() {
    return this->ShaderProgram;
  }

  void setShaderProgram(vesShaderProgram* shaderProgram) {
    this->ShaderProgram = shaderProgram;
  }

  vesCamera* camera() {
    return this->Renderer->GetCamera();
  }

  vesMapper* mapper() {
    return this->Mapper;
  }

  void setMapper(vesMapper* mapper) {
    this->Mapper = mapper;
  }

  std::string sourceDirectory() {
    return this->SourceDirectory;
  }

  void setSourceDirectory(std::string dir) {
    this->SourceDirectory = dir;
  }

  bool isTesting() {
    return this->IsTesting;
  }

  void setTesting(bool testing) {
    this->IsTesting = testing;
  }

  vesActor* actor() {
    return this->Actor;
  }

  void setActor(vesActor* actor) {
    this->Actor = actor;
  }

  void render() {
    this->Renderer->ResetCameraClippingRange();
    this->Renderer->Render();
  }

  void resize(int w, int h) {
    this->Renderer->Resize(w, h, 1.0f);
  }

  void resetView() {
    this->Renderer->ResetCamera();
  }

private:
  vesRenderer*      Renderer;
  vesShaderProgram* ShaderProgram;
  vesShader*        Shader;
  vesMapper*        Mapper;
  vesActor*         Actor;

  std::string       SourceDirectory;
  bool              IsTesting;
};

RenderView* view = new RenderView();



void LoadData() {

  std::string dataFile = view->sourceDirectory() + "/Apps/iOS/Kiwi/Kiwi/Data/AppendedKneeData.vtp";

  vtkNew<vtkXMLPolyDataReader> reader;
  reader->SetFileName(dataFile.c_str());
  reader->Update();
  vesTriangleData* data = vtkPolyDataToTriangleData::Convert(reader->GetOutput());
  vtkPolyDataToTriangleData::ComputeVertexColorFromScalars(reader->GetOutput(), data);

  view->setMapper(new vesMapper());
  view->mapper()->SetTriangleData(data);
  view->setActor(new vesActor(view->shader(), view->mapper()));

  view->actor()->setColor(0.8, 0.8, 0.8, 1.0);
  view->renderer()->AddActor(view->actor());
  view->resetView();
  view->camera()->Dolly(1.5);
}

std::string GetFileContents(const std::string& filename) {
  std::ifstream file(filename.c_str());
  std::stringstream buffer;
  if (file) {
    buffer << file.rdbuf();
    file.close();
  }
  return buffer.str();
}

void InitVes() {

  view->setRenderer(new vesRenderer());

  std::string vertexShaderFile = view->sourceDirectory() + "/src/shaders/Shader.vsh";
  std::string fragmentShaderFile = view->sourceDirectory() + "/src/shaders/Shader.fsh";

  std::string vertexSourceStr = GetFileContents(vertexShaderFile);
  std::string fragmentSourceStr = GetFileContents(fragmentShaderFile);

  view->setShaderProgram(new vesShaderProgram(const_cast<char*>(vertexSourceStr.c_str()),
                                 const_cast<char*>(fragmentSourceStr.c_str()),
                                 (_uni("u_mvpMatrix"),
                                  _uni("u_normalMatrix"),
                                  _uni("u_ecLightDir"),
                                  _uni("u_opacity"),
                                  _uni("u_enable_diffuse")),
                                 (_att("a_vertex"),
                                  _att("a_normal"),
                                  _att("a_vertex_color"))
                                 ));

  view->setShader(new vesShader(view->shaderProgram()));

  glClearColor(63/255.0f, 96/255.0f, 144/255.0, 1.0f);
}

bool ReadCommandLine(int argc, char* argv[]) {

  if (argc < 2) {
    printf("Usage: %s <path to VES source directory> [testing]\n", argv[0]);
    return false;
  }

  view->setSourceDirectory(argv[1]);

  if (argc == 3) {
    view->setTesting(true);
  }

  return true;
}

};



/* new window size or exposure */
static void
reshape(int width, int height)
{
   glViewport(0, 0, (GLint) width, (GLint) height);
   view->resize(width, height);
}


static void
init(void)
{
   typedef void (*proc)();

#if 1 /* test code */
   proc p = eglGetProcAddress("glMapBufferOES");
   assert(p);
#endif

   glClearColor(0.0, 0.0, 0.0, 0.0);

   InitVes();
   LoadData();
}


/*
 * Create an RGB, double-buffered X window.
 * Return the window and context handles.
 */
static void
make_x_window(Display *x_dpy, EGLDisplay egl_dpy,
              const char *name,
              int x, int y, int width, int height,
              Window *winRet,
              EGLContext *ctxRet,
              EGLSurface *surfRet)
{
   static const EGLint attribs[] = {
      EGL_RED_SIZE, 1,
      EGL_GREEN_SIZE, 1,
      EGL_BLUE_SIZE, 1,
      EGL_DEPTH_SIZE, 1,
      EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
      EGL_NONE
   };
   static const EGLint ctx_attribs[] = {
      EGL_CONTEXT_CLIENT_VERSION, 2,
      EGL_NONE
   };
   int scrnum;
   XSetWindowAttributes attr;
   unsigned long mask;
   Window root;
   Window win;
   XVisualInfo *visInfo, visTemplate;
   int num_visuals;
   EGLContext ctx;
   EGLConfig config;
   EGLint num_configs;
   EGLint vid;

   scrnum = DefaultScreen( x_dpy );
   root = RootWindow( x_dpy, scrnum );

   if (!eglChooseConfig( egl_dpy, attribs, &config, 1, &num_configs)) {
      printf("Error: couldn't get an EGL visual config\n");
      exit(1);
   }

   assert(config);
   assert(num_configs > 0);

   if (!eglGetConfigAttrib(egl_dpy, config, EGL_NATIVE_VISUAL_ID, &vid)) {
      printf("Error: eglGetConfigAttrib() failed\n");
      exit(1);
   }

   /* The X window visual must match the EGL config */
   visTemplate.visualid = vid;
   visInfo = XGetVisualInfo(x_dpy, VisualIDMask, &visTemplate, &num_visuals);
   if (!visInfo) {
      printf("Error: couldn't get X visual\n");
      exit(1);
   }

   /* window attributes */
   attr.background_pixel = 0;
   attr.border_pixel = 0;
   attr.colormap = XCreateColormap( x_dpy, root, visInfo->visual, AllocNone);
   attr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask;
   mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

   win = XCreateWindow( x_dpy, root, 0, 0, width, height,
                        0, visInfo->depth, InputOutput,
                        visInfo->visual, mask, &attr );

   /* set hints and properties */
   {
      XSizeHints sizehints;
      sizehints.x = x;
      sizehints.y = y;
      sizehints.width  = width;
      sizehints.height = height;
      sizehints.flags = USSize | USPosition;
      XSetNormalHints(x_dpy, win, &sizehints);
      XSetStandardProperties(x_dpy, win, name, name,
                              None, (char **)NULL, 0, &sizehints);
   }

#if USE_FULL_GL /* XXX fix this when eglBindAPI() works */
   eglBindAPI(EGL_OPENGL_API);
#else
   eglBindAPI(EGL_OPENGL_ES_API);
#endif

   ctx = eglCreateContext(egl_dpy, config, EGL_NO_CONTEXT, ctx_attribs );
   if (!ctx) {
      printf("Error: eglCreateContext failed\n");
      exit(1);
   }

   /* test eglQueryContext() */
   {
      EGLint val;
      eglQueryContext(egl_dpy, ctx, EGL_CONTEXT_CLIENT_VERSION, &val);
      assert(val == 2);
   }

   *surfRet = eglCreateWindowSurface(egl_dpy, config, win, NULL);
   if (!*surfRet) {
      printf("Error: eglCreateWindowSurface failed\n");
      exit(1);
   }

   /* sanity checks */
   {
      EGLint val;
      eglQuerySurface(egl_dpy, *surfRet, EGL_WIDTH, &val);
      assert(val == width);
      eglQuerySurface(egl_dpy, *surfRet, EGL_HEIGHT, &val);
      assert(val == height);
      assert(eglGetConfigAttrib(egl_dpy, config, EGL_SURFACE_TYPE, &val));
      assert(val & EGL_WINDOW_BIT);
   }

   XFree(visInfo);

   *winRet = win;
   *ctxRet = ctx;
}


static void
event_loop(Display *dpy, Window win,
           EGLDisplay egl_dpy, EGLSurface egl_surf)
{
   while (1) {
      int redraw = 0;
      XEvent event;

      XNextEvent(dpy, &event);

      switch (event.type) {
      case Expose:
         redraw = 1;
         break;
      case ConfigureNotify:
         reshape(event.xconfigure.width, event.xconfigure.height);
         break;
      case KeyPress:
         {
            char buffer[10];
            int r, code;
            code = XLookupKeysym(&event.xkey, 0);
            if (code == XK_Left) {

            }
            else if (code == XK_Right) {

            }
            else if (code == XK_Up) {

            }
            else if (code == XK_Down) {

            }
            else {
               r = XLookupString(&event.xkey, buffer, sizeof(buffer),
                                 NULL, NULL);
               if (buffer[0] == 27) {
                  /* escape */
                  return;
               }
            }
         }
         redraw = 1;
         break;
      default:
         ; /*no-op*/
      }

      if (redraw) {
         view->render();
         eglSwapBuffers(egl_dpy, egl_surf);
      }
   }
}


int
main(int argc, char *argv[])
{
   const int winWidth = 800, winHeight = 600;
   Display *x_dpy;
   Window win;
   EGLSurface egl_surf;
   EGLContext egl_ctx;
   EGLDisplay egl_dpy;
   char *dpyName = NULL;
   GLboolean printInfo = GL_FALSE;
   EGLint egl_major, egl_minor;
   int i;
   const char *s;


  if (!ReadCommandLine(argc, argv))
    {
    return -1;
    }

   x_dpy = XOpenDisplay(dpyName);
   if (!x_dpy) {
      printf("Error: couldn't open display %s\n",
             dpyName ? dpyName : getenv("DISPLAY"));
      return -1;
   }

   egl_dpy = eglGetDisplay(x_dpy);
   if (!egl_dpy) {
      printf("Error: eglGetDisplay() failed\n");
      return -1;
   }

   if (!eglInitialize(egl_dpy, &egl_major, &egl_minor)) {
      printf("Error: eglInitialize() failed\n");
      return -1;
   }

   s = eglQueryString(egl_dpy, EGL_VERSION);
   printf("EGL_VERSION = %s\n", s);

   s = eglQueryString(egl_dpy, EGL_VENDOR);
   printf("EGL_VENDOR = %s\n", s);

   s = eglQueryString(egl_dpy, EGL_EXTENSIONS);
   printf("EGL_EXTENSIONS = %s\n", s);

   s = eglQueryString(egl_dpy, EGL_CLIENT_APIS);
   printf("EGL_CLIENT_APIS = %s\n", s);

   make_x_window(x_dpy, egl_dpy,
                 "OpenGL ES 2.x tri", 0, 0, winWidth, winHeight,
                 &win, &egl_ctx, &egl_surf);

   XMapWindow(x_dpy, win);
   if (!eglMakeCurrent(egl_dpy, egl_surf, egl_surf, egl_ctx)) {
      printf("Error: eglMakeCurrent() failed\n");
      return -1;
   }

   if (printInfo) {
      printf("GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
      printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
      printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
      printf("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));
   }

   init();

   // render once
   reshape(winWidth, winHeight);
   view->render();
   eglSwapBuffers(egl_dpy, egl_surf);

   // begin the event loop if not in testing mode
   if (!view->isTesting()) {
     event_loop(x_dpy, win, egl_dpy, egl_surf);
   }

   eglDestroyContext(egl_dpy, egl_ctx);
   eglDestroySurface(egl_dpy, egl_surf);
   eglTerminate(egl_dpy);


   XDestroyWindow(x_dpy, win);
   XCloseDisplay(x_dpy);

   return 0;
}
