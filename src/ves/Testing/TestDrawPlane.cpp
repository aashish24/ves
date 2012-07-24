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

#include <vesActor.h>
#include <vesCamera.h>
#include <vesMapper.h>
#include <vesMaterial.h>
#include <vesModelViewUniform.h>
#include <vesProjectionUniform.h>
#include <vesRenderer.h>
#include <vesShader.h>
#include <vesShaderProgram.h>
#include <vesUniform.h>
#include <vesVertexAttribute.h>
#include <vesVertexAttributeKeys.h>
#include <vesViewport.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>


//----------------------------------------------------------------------------
namespace {

class vesTestDrawPlane {
public:

  vesTestDrawPlane() :
    m_modelViewUniform(new vesModelViewUniform()),
    m_projectionUniform(new vesProjectionUniform()),
    m_positionVertexAttribute(new vesPositionVertexAttribute()),
    m_normalVertexAttribute(new vesNormalVertexAttribute()),
    m_colorVertexAttribute(new vesColorVertexAttribute()),
    m_vertexShader(new vesShader(vesShader::Vertex)),
    m_fragmentShader(new vesShader(vesShader::Fragment)),
    m_shaderProgram(vesShaderProgram::Ptr(new vesShaderProgram())),
    m_material(vesMaterial::Ptr(new vesMaterial())),
    m_mapper(vesMapper::Ptr(new vesMapper())),
    m_actor(vesActor::Ptr(new vesActor())),
    m_renderer(vesRenderer::Ptr(new vesRenderer()))
  {
  }

  ~vesTestDrawPlane()
  {
  }

  std::string sourceDirectory() {
    return this->SourceDirectory;
  }

  void setSourceDirectory(std::string dir) {
    this->SourceDirectory = dir;
  }

  std::string dataDirectory() {
    return this->DataDirectory;
  }

  void setDataDirectory(std::string dir) {
    this->DataDirectory = dir;
  }

  bool isTesting() {
    return this->IsTesting;
  }

  void setTesting(bool testing) {
    this->IsTesting = testing;
  }

  void init()
  {
    const std::string vertexShaderSource =
      "uniform highp mat4 modelViewMatrix;\n \
       uniform highp mat4 projectionMatrix;\n \
       attribute highp vec4 vertexPosition;\n \
       attribute mediump vec4 vertexColor;\n \
       varying mediump vec4 varColor;\n \
       void main()\n \
       {\n \
         gl_Position = projectionMatrix * modelViewMatrix * vertexPosition;\n \
         varColor = vertexColor;\n \
       }";

    const std::string fragmentShaderSource =
      "varying mediump vec4 varColor;\n \
       void main()\n \
       {\n \
         gl_FragColor = varColor;\n \
       }";

    this->m_vertexShader->setShaderSource(vertexShaderSource);
    this->m_fragmentShader->setShaderSource(fragmentShaderSource);

    this->m_shaderProgram->addShader(this->m_vertexShader);
    this->m_shaderProgram->addShader(this->m_fragmentShader);

    this->m_shaderProgram->addUniform(this->m_modelViewUniform);
    this->m_shaderProgram->addUniform(this->m_projectionUniform);
    this->m_shaderProgram->addVertexAttribute(this->m_positionVertexAttribute,
                                              vesVertexAttributeKeys::Position);
    this->m_shaderProgram->addVertexAttribute(this->m_normalVertexAttribute,
                                              vesVertexAttributeKeys::Normal);
    this->m_shaderProgram->addVertexAttribute(this->m_colorVertexAttribute,
                                              vesVertexAttributeKeys::Color);

    this->m_material->addAttribute(this->m_shaderProgram);
    this->m_mapper->setGeometryData(this->createPlane());
    this->m_actor->setMapper(this->m_mapper);
    this->m_actor->setMaterial(this->m_material);
    this->m_renderer->addActor(this->m_actor);

    this->m_renderer->camera()->setParallelProjection(false);
    this->m_renderer->setBackgroundColor(0.0, 0.2, 0.8);
  }

  vesSharedPtr<vesGeometryData> createPlane()
  {
    vesGeometryData::Ptr geometryData (new vesGeometryData());
    vesSourceDataP3N3C3f::Ptr sourceData(new vesSourceDataP3N3C3f());

    vesVector4f topLeftColor = vesVector4f(0.5f, 0.0f, 0.0f, 1.0f);
    vesVector4f bottomRightColor = vesVector4f(0.0f, 0.0f, 0.5f, 1.0f);
    vesVector4f color = vesVector4f(0.5f, 0.5f, 0.5f, 1.0f);

    // Points.
    vesVertexDataP3N3C3f v1;
    v1.m_position = vesVector3f(-1.0f, -1.0f, 0.0f);
    v1.m_normal = vesVector3f(0.0f, 0.0f, 1.0f);
    v1.m_color = vesVector3f(color[0], color[1], color[2]);

    vesVertexDataP3N3C3f v2;
    v2.m_position = vesVector3f(1.0f, -1.0f, 0.0f);
    v2.m_normal = vesVector3f(0.0f, 0.0f, 1.0f);
    v2.m_color = vesVector3f(bottomRightColor[0], bottomRightColor[1], bottomRightColor[2]);

    vesVertexDataP3N3C3f v3;
    v3.m_position = vesVector3f(1.0f, 1.0f, 0.0f);
    v3.m_normal = vesVector3f(0.0f, 0.0f, 1.0f);
    v3.m_color = vesVector3f(color[0], color[1], color[2]);

    vesVertexDataP3N3C3f v4;
    v4.m_position = vesVector3f(-1.0f, 1.0f, 0.0f);
    v4.m_normal = vesVector3f(0.0f, 0.0f, 1.0f);
    v4.m_color = vesVector3f(topLeftColor[0], topLeftColor[1], topLeftColor[2]);

    sourceData->pushBack(v1);
    sourceData->pushBack(v2);
    sourceData->pushBack(v3);
    sourceData->pushBack(v4);

    // Triangle cells.
    vesPrimitive::Ptr triangles (new vesPrimitive());
    vesSharedPtr< vesIndices<unsigned short> > indices (new vesIndices<unsigned short>());
    indices->pushBackIndices(0, 3, 2);
    indices->pushBackIndices(1, 0, 2);
    triangles->setVesIndices(indices);
    triangles->setPrimitiveType(vesPrimitiveRenderType::Triangles);
    triangles->setIndexCount(3);
    triangles->setIndicesValueType(vesPrimitiveIndicesValueType::UnsignedShort);

    geometryData->setName("PlaneGeometryData");
    geometryData->addSource(sourceData);
    geometryData->addPrimitive(triangles);
    return geometryData;
  }

  void render()
  {
    this->m_renderer->render();
  }

  void resizeView(int winWidth, int winHeight)
  {
    this->m_renderer->resize(winWidth, winHeight, 1.0f);
  }

  void resetView()
  {
    // dolly so that scene fits window
    this->m_renderer->resetCamera();
  }

  void toggleVisibility()
  {
    this->m_actor->setVisible(!this->m_actor->isVisible());
    this->render();
  }

  void toggleColorVisibility()
  {
    this->m_material->enableVertexColor(!this->m_material->isEnabledVertexColor());
    this->render();
  }


private:

  std::string       SourceDirectory;
  std::string       DataDirectory;
  bool              IsTesting;

  vesSharedPtr<vesModelViewUniform> m_modelViewUniform;
  vesSharedPtr<vesProjectionUniform> m_projectionUniform;
  vesSharedPtr<vesPositionVertexAttribute> m_positionVertexAttribute;
  vesSharedPtr<vesNormalVertexAttribute> m_normalVertexAttribute;
  vesSharedPtr<vesColorVertexAttribute> m_colorVertexAttribute;
  vesSharedPtr<vesTextureCoordinateVertexAttribute> m_textureCoodinateAttribute;
  vesSharedPtr<vesGeometryData> m_backgroundPlaneData;

  vesShader::Ptr m_vertexShader;
  vesShader::Ptr m_fragmentShader;
  vesShaderProgram::Ptr m_shaderProgram;
  vesMaterial::Ptr m_material;
  vesMapper::Ptr m_mapper;
  vesActor::Ptr m_actor;
  vesRenderer::Ptr m_renderer;

};

//----------------------------------------------------------------------------
vesTestDrawPlane* testDrawPlane;

//----------------------------------------------------------------------------
bool DoTesting()
{
  testDrawPlane->render();

  return true;
}

//----------------------------------------------------------------------------
void InitRendering()
{
  testDrawPlane->init();
}

//----------------------------------------------------------------------------
bool InitTest(int argc, char* argv[])
{
  if (argc < 2) {
    printf("Usage: %s <path to VES source directory> [path to testing data directory]\n", argv[0]);
    return false;
  }

  testDrawPlane = new vesTestDrawPlane();
  testDrawPlane->setSourceDirectory(argv[1]);

  if (argc == 3) {
    testDrawPlane->setDataDirectory(argv[2]);
    testDrawPlane->setTesting(true);
  }
  return true;
}

//----------------------------------------------------------------------------
void FinalizeTest()
{
  delete testDrawPlane;
}

}; // end namespace
//----------------------------------------------------------------------------


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
   attr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | ButtonMotionMask;
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

bool haveLastMotion = false;
int lastMotionX = 0;
int lastMotionY = 0;
int currentX = 0;
int currentY = 0;

static void
event_loop(Display *dpy, Window win,
           EGLDisplay egl_dpy, EGLSurface egl_surf)
{
  vesNotUsed(win);

   while (1) {
      int redraw = 1;
      XEvent event;

      XNextEvent(dpy, &event);

      switch (event.type) {
      case Expose:
         redraw = 1;
         break;
      case ConfigureNotify:
         testDrawPlane->resizeView(event.xconfigure.width, event.xconfigure.height);
         break;

      case ButtonPress:
        testDrawPlane->toggleVisibility();
        break;

      case ButtonRelease:
        // Do nothing
        break;

      case MotionNotify:
        // Do nothing
        break;

      case KeyPress:
        testDrawPlane->toggleColorVisibility();
        break;

      default:
         ; /*no-op*/
      }

      if (redraw) {
         testDrawPlane->render();
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
  const char *s;


  if (!InitTest(argc, argv)) {
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


  InitRendering();

  // render once
  testDrawPlane->resizeView(winWidth, winHeight);
  testDrawPlane->resetView();
  testDrawPlane->render();
  eglSwapBuffers(egl_dpy, egl_surf);

  // begin the event loop if not in testing mode
  bool testPassed = true;
  if (!testDrawPlane->isTesting()) {
    event_loop(x_dpy, win, egl_dpy, egl_surf);
  }
  else {
    testPassed = DoTesting();
  }

  FinalizeTest();

  eglDestroyContext(egl_dpy, egl_ctx);
  eglDestroySurface(egl_dpy, egl_surf);
  eglTerminate(egl_dpy);

  XDestroyWindow(x_dpy, win);
  XCloseDisplay(x_dpy);

  return testPassed ? 0 : 1;
}
