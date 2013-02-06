#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <vesActor.h>
#include <vesCamera.h>
#include <vesGeometryData.h>
#include <vesMapper.h>
#include <vesMaterial.h>
#include <vesRenderer.h>
#include <vesVertexAttribute.h>
#include <vesVertexAttributeKeys.h>
#include <vesViewport.h>

#include <GLES/gl.h>
#include <GLES/egl.h>

#include <gf/gf.h>
#include <gf/gf3d.h>


static EGLint attribute_list[] = {
  EGL_NATIVE_VISUAL_ID, 0,
  EGL_NATIVE_RENDERABLE, EGL_TRUE,
  EGL_RED_SIZE, 5,
  EGL_GREEN_SIZE, 5,
  EGL_BLUE_SIZE, 5,
  EGL_DEPTH_SIZE, 16,
  EGL_NONE
};

gf_dev_t    gfdev;
gf_layer_t  layer;
int         layer_idx;

static EGLDisplay display;
static EGLSurface surface;


//----------------------------------------------------------------------------
namespace {

class vesTestDrawPlane {
public:

  vesTestDrawPlane() :
    m_positionVertexAttribute(new vesPositionVertexAttribute()),
    m_normalVertexAttribute(new vesNormalVertexAttribute()),
    m_colorVertexAttribute(new vesColorVertexAttribute()),
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
    this->m_material->addAttribute(this->m_positionVertexAttribute);
    this->m_material->addAttribute(this->m_normalVertexAttribute);
    this->m_material->addAttribute(this->m_colorVertexAttribute);

    this->m_mapper->setGeometryData(this->createPlane());
    this->m_actor->setMapper(this->m_mapper);
    this->m_actor->setMaterial(this->m_material);
    this->m_renderer->addActor(this->m_actor);

    this->m_renderer->camera()->setParallelProjection(false);
    this->m_renderer->setBackgroundColor(0.0, 0.4, 0.6);
  }

  vesSharedPtr<vesGeometryData> createPlane()
  {
    vesGeometryData::Ptr geometryData (new vesGeometryData());
    vesSourceDataP4N4C4f::Ptr sourceData(new vesSourceDataP4N4C4f());

    vesVector4f topLeftColor = vesVector4f(0.5f, 0.0f, 1.0f, 1.0f);
    vesVector4f bottomRightColor = vesVector4f(1.0f, 0.0f, 1.0f, 1.0f);
    vesVector4f color = vesVector4f(0.0f, 0.0f, 0.0f, 1.0f);

    // Points.
    vesVertexDataP4N4C4f v1;
    v1.m_position = vesVector4f(-1.0f, -1.0f, 0.0f, 1.0f);
    v1.m_normal = vesVector4f(0.0f, 0.0f, 0.0f, 0.0f);
    v1.m_color = vesVector4f(color[0], color[1], color[2], color[3]);

    vesVertexDataP4N4C4f v2;
    v2.m_position = vesVector4f(1.0f, -1.0f, 0.0f, 1.0f);
    v2.m_normal = vesVector4f(0.0f, 0.0f, 0.0f, 0.0f);
    v2.m_color = vesVector4f(bottomRightColor[0], bottomRightColor[1],
      bottomRightColor[2], bottomRightColor[3]);

    vesVertexDataP4N4C4f v3;
    v3.m_position = vesVector4f(1.0f, 1.0f, 0.0f,1.0f);
    v3.m_normal = vesVector4f(0.0f, 0.0f, 0.0f, 0.0f);
    v3.m_color = vesVector4f(color[0], color[1], color[2], color[3]);

    vesVertexDataP4N4C4f v4;
    v4.m_position = vesVector4f(-1.0f, 1.0f, 0.0f, 1.0f);
    v4.m_normal = vesVector4f(0.0f, 0.0f, 0.0f, 0.0f);
    v4.m_color = vesVector4f(topLeftColor[0], topLeftColor[1],
        topLeftColor[2], topLeftColor[3]);

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
    //this->m_material->enableVertexColor(!this->m_material->isEnabledVertexColor());
    this->render();
  }


private:

  std::string       SourceDirectory;
  std::string       DataDirectory;
  bool              IsTesting;

  vesSharedPtr<vesPositionVertexAttribute> m_positionVertexAttribute;
  vesSharedPtr<vesNormalVertexAttribute> m_normalVertexAttribute;
  vesSharedPtr<vesColorVertexAttribute> m_colorVertexAttribute;
  vesSharedPtr<vesTextureCoordinateVertexAttribute> m_textureCoodinateAttribute;
  vesSharedPtr<vesGeometryData> m_backgroundPlaneData;

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

//int
//time_elapsed()
//{
//  static uint64_t	init_clock_cycles;
//  static int	timer_installed;
//  static uint64_t	cycles_per_sec;
//  uint64_t	timestamp;

//  /* Return number of milliseconds since first call */
//  if (!timer_installed) {
//    init_clock_cycles = ClockCycles();
//    timer_installed = 1;
//    return 0;
//  }

//  timestamp = ClockCycles();

//  if (timestamp < init_clock_cycles) {
//    /* Counter wrapped */
//    timestamp += (UINT64_MAX-init_clock_cycles) + 1;
//  } else {
//    timestamp -= init_clock_cycles;
//  }

//  if (cycles_per_sec == 0)
//    cycles_per_sec =
//        SYSPAGE_ENTRY(qtime)->cycles_per_sec;

//  if (timestamp > 1000*1000*1000)
//    return timestamp / cycles_per_sec * 1000;
//  else
//    return timestamp * 1000 / cycles_per_sec;
//}

int
main(int argc, char *argv[])
{
  gf_3d_target_t      target;
  gf_display_t        gf_disp;
  EGLConfig           config;
  EGLContext          econtext;
  EGLint              num_config;
  gf_dev_info_t       info;
  gf_layer_info_t     linfo;
  gf_display_info_t   disp_info;
  GLuint              width, height;
  GLuint              it;

  if (!InitTest(argc, argv)) {
    return -1;
    fprintf(stderr, "Looping\n");
  }

  /* initialize the graphics device */
  if (gf_dev_attach(&gfdev, NULL, &info)!=GF_ERR_OK)
  {
     perror("gf_dev_attach()");
     return -1;
  }

  /* Setup the layer we will use */
  if (gf_display_attach(&gf_disp, gfdev, 0, &disp_info)!=GF_ERR_OK)
  {
     fprintf(stderr, "gf_display_attach() failed\n");
     return -1;
  }

  width = disp_info.xres;
  height = disp_info.yres;

  layer_idx = disp_info.main_layer_index;

  /* get an EGL display connection */
  display=eglGetDisplay(gfdev);
  if (display==EGL_NO_DISPLAY)
  {
     fprintf(stderr, "eglGetDisplay() failed\n");
     return -1;
  }

  if (gf_layer_attach(&layer, gf_disp, layer_idx, 0)!=GF_ERR_OK)
  {
     fprintf(stderr, "gf_layer_attach() failed\n");
     return -1;
  }

  /* initialize the EGL display connection */
  if (eglInitialize(display, NULL, NULL)!=EGL_TRUE)
  {
     fprintf(stderr, "eglInitialize: error 0x%x\n", eglGetError());
     return -1;
  }

  for (it=0;; it++)
  {
     /* Walk through all possible pixel formats for this layer */
     if (gf_layer_query(layer, it, &linfo)==-1)
     {
        fprintf(stderr, "Couldn't find a compatible frame "
                        "buffer configuration on layer %d\n", layer_idx);
        return -1;
     }

     /*
      * We want the color buffer format to match the layer format,
      * so request the layer format through EGL_NATIVE_VISUAL_ID.
      */
     attribute_list[1]=linfo.format;

     /* Look for a compatible EGL frame buffer configuration */
     if (eglChooseConfig(display, attribute_list, &config, 1, &num_config)==EGL_TRUE)
     {
        if (num_config>0)
        {
           break;
        }
     }

     fprintf(stderr, "Looping\n");
  }

  /* create a 3D rendering target */
  if (gf_3d_target_create(&target, layer, NULL, 0, width, height, linfo.format)!=GF_ERR_OK)
  {
     fprintf(stderr, "Unable to create rendering target\n");
     return -1;
  }

  gf_layer_set_src_viewport(layer, 0, 0, width-1, height-1);
  gf_layer_set_dst_viewport(layer, 0, 0, width-1, height-1);
  gf_layer_enable(layer);

  /*
   * The layer settings haven't taken effect yet since we haven't
   * called gf_layer_update() yet.  This is exactly what we want,
   * since we haven't supplied a valid surface to display yet.
   * Later, the OpenGL ES library calls will call gf_layer_update()
   * internally, when  displaying the rendered 3D content.
   */

  /* create an EGL rendering context */
  econtext=eglCreateContext(display, config, EGL_NO_CONTEXT, NULL);
  if (econtext==EGL_NO_CONTEXT)
  {
     fprintf(stderr, "Create context failed: 0x%x\n", eglGetError());
     return -1;
  }

  /* create an EGL window surface */
  surface=eglCreateWindowSurface(display, config, target, NULL);
  if (surface==EGL_NO_SURFACE)
  {
     fprintf(stderr, "Create surface failed: 0x%x\n", eglGetError());
     return -1;
  }

  /* connect the context to the surface */
  if (eglMakeCurrent(display, surface, surface, econtext)==EGL_FALSE)
  {
     fprintf(stderr, "Make current failed: 0x%x\n", eglGetError());
     return -1;
  }

  InitRendering();

  // render once
  testDrawPlane->resizeView(width, height);
  testDrawPlane->resetView();
  testDrawPlane->render();
  glFinish();
  eglWaitGL();
  eglSwapBuffers(display, surface);

  // begin the event loop if not in testing mode
  bool testPassed = true;
  if (!testDrawPlane->isTesting()) {
    do {
       testDrawPlane->render();
       glFinish();
       eglWaitGL();
       eglSwapBuffers(display, surface);
    } while(1);
  }
  else {
    testPassed = DoTesting();
  }

  FinalizeTest();

  return testPassed ? 0 : 1;
}
