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
  EGL_RED_SIZE, 5,
  EGL_GREEN_SIZE, 5,
  EGL_BLUE_SIZE, 5,
  EGL_ALPHA_SIZE, EGL_DONT_CARE,
  EGL_DEPTH_SIZE, 16,
  EGL_NONE
};

static EGLint attribute_list_tmp[] = {
  EGL_NATIVE_VISUAL_ID, 0,
  EGL_RED_SIZE, 8,
  EGL_GREEN_SIZE, 8,
  EGL_BLUE_SIZE, 8,
  EGL_ALPHA_SIZE, 8,
  EGL_SURFACE_TYPE, 4,
  EGL_DEPTH_SIZE, 1,
  EGL_STENCIL_SIZE, 1,
  EGL_SAMPLE_BUFFERS, 0,
  EGL_LEVEL, 0,
  EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
  EGL_NONE
};

static	gf_display_t		gf_disp;
static	gf_dev_t			gf_dev;
static	EGLConfig			config;
static	EGLContext			econtext;
static	EGLint				num_config;
static	gf_dev_info_t	 	info;
static	gf_display_info_t	disp_info;

gf_layer_t layer;
int layer_idx;
gf_layer_info_t linfo;
gf_3d_target_t target;
EGLint format_idx;


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

int
time_elapsed()
{
  static uint64_t	init_clock_cycles;
  static int	timer_installed;
  static uint64_t	cycles_per_sec;
  uint64_t	timestamp;

  /* Return number of milliseconds since first call */
  if (!timer_installed) {
    init_clock_cycles = ClockCycles();
    timer_installed = 1;
    return 0;
  }

  timestamp = ClockCycles();

  if (timestamp < init_clock_cycles) {
    /* Counter wrapped */
    timestamp += (UINT64_MAX-init_clock_cycles) + 1;
  } else {
    timestamp -= init_clock_cycles;
  }

  if (cycles_per_sec == 0)
    cycles_per_sec =
        SYSPAGE_ENTRY(qtime)->cycles_per_sec;

  if (timestamp > 1000*1000*1000)
    return timestamp / cycles_per_sec * 1000;
  else
    return timestamp * 1000 / cycles_per_sec;
}

int
init_contenxt( EGLDisplay *display, EGLSurface *surface, gf_surface_t *gf_surface,
      int width, int height, int init_flag )
{
  if( init_flag ) {

    /* initialize the graphics device */
    if( gf_dev_attach(&gf_dev, GF_DEVICE_INDEX(0), &info ) != GF_ERR_OK ) {
      perror("gf_dev_attach()");
      return -1;
    }

    printf("Number of displays: %d\n",info.ndisplays);

    for (unsigned int i = 0; i < info.ndisplays; i++) {
        printf("Display %d: ", i);
        if (gf_display_attach(&gf_disp, gf_dev, i, &disp_info) == GF_ERR_OK) {
            printf("%dX%d, refresh = %dHz\n", disp_info.xres,
                disp_info.yres, disp_info.refresh);
            printf("Number of layers: %d\n", disp_info.nlayers);
        } else {
            printf("gf_display_attach() failed\n");
        }
    }

    layer_idx = disp_info.main_layer_index;

    /* get an EGL display connection */
    *display = eglGetDisplay(gf_dev);

    if( *display == EGL_NO_DISPLAY ) {
      fprintf(stderr, "eglGetDisplay() failed\n");
      return -1;
    }

    printf("Layer index: %d\n", layer_idx);
    if(gf_layer_attach(&layer, gf_disp, layer_idx, 0) != GF_ERR_OK)
    {
      fprintf(stderr, "ERROR: gf_layer_attach()\n");
      fprintf(stderr, "ERROR %d\n", gf_layer_attach(&layer, gf_disp, layer_idx, 0));
      return -1;
    }

    // initialize the EGL display connection
    if (eglInitialize(*display, NULL, NULL) != EGL_TRUE) {
      fprintf(stderr, "eglInitialize: error 0x%x\n", eglGetError());
      exit(EXIT_FAILURE);
    }

    GLuint it;
    for (it = 0; ; it++) {
      // Walk through all possible pixel formats for this layer
      if (gf_layer_query(layer, it, &linfo) != GF_ERR_OK) {
        fprintf(stderr, "Couldn’t find a compatible frame "
        "buffer configuration on layer %d\n", gf_layer_query(layer, it, &linfo));
//        exit(EXIT_FAILURE);
      }

      // We want the color buffer format to match the layer format,
      // so request the layer format through EGL_NATIVE_VISUAL_ID.
      attribute_list_tmp[1] = linfo.format;
      // Look for a compatible EGL frame buffer configuration
      if (eglChooseConfig(*display,
      attribute_list_tmp, &config, 1, &num_config) == EGL_TRUE) {
        if (num_config > 0) {
          format_idx = it;
          break;
        }
      }
    }

    // create a 3D rendering target
    if (gf_3d_target_create(&target, layer, NULL, 0, width, height, linfo.format)!=GF_ERR_OK)
    {
      fprintf(stderr, "Unable to create rendering target\n");
      return -1;
    }
    else
    {
      fprintf(stderr, "Created rendering target\n");
    };

    gf_layer_set_src_viewport(layer, 0, 0, width-1, height-1);
    gf_layer_set_dst_viewport(layer, 0, 0, width-1, height-1);
    gf_layer_enable(layer);

    // The layer settings haven't taken effect yet since we haven't
    // called gf_layer_update() yet.  This is exactly what we want,
    // since we haven't supplied a valid surface to display yet.
    // Later, the OpenGL ES library calls will call gf_layer_update()
    // internally, when  displaying the rendered 3D content.

    // create an EGL rendering context
    if( NULL == (econtext = eglCreateContext( *display, config, EGL_NO_CONTEXT, NULL )) ) {
      perror( "eglCreateContext()" );
      return -1;
    }

    // Create an EGL window surface
    *surface = eglCreateWindowSurface (display, config, target, NULL);
    if (surface == EGL_NO_SURFACE)
    {
       fprintf(stderr, "Create surface failed: 0x%x\n", eglGetError());
       return -1;
    }

    // Connect the context to the surface
    if (eglMakeCurrent (display, surface, surface, econtext) == EGL_FALSE)
    {
      fprintf(stderr, "Make current failed: 0x%x\n", eglGetError());
      return -1;
    }


  } else {
    // free the existing surface.
    eglMakeCurrent( *display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroySurface( *display, *surface );
    gf_surface_free( *gf_surface );
  }

  //  We want to allocate a surface that EGL can use to create
  //  a Pixmap surface
  gf_3d_config_info_t cfginfo;
  if (gf_3d_query_config(&cfginfo, gf_dev, display, format_idx) != GF_ERR_OK) {
      fprintf(stderr, "query native failed\n");
      exit(EXIT_FAILURE);
  }

  if (gf_surface_create( gf_surface, gf_dev, width, height,
      cfginfo.format, NULL, cfginfo.create_flags) != GF_ERR_OK ) {
      fprintf(stderr, "create surface failed\n");
      exit(EXIT_FAILURE);
  }

  *surface = eglCreatePixmapSurface(*display, format_idx, *gf_surface, NULL);

  if (*surface == EGL_NO_SURFACE) {
      fprintf(stderr, "Create Pixmap failed: 0x%x\n", eglGetError());
      exit(EXIT_FAILURE);
  }


  /* connect the context to the surface */
  if (eglMakeCurrent( *display, *surface, *surface, econtext) == EGL_FALSE) {
    fprintf(stderr, "Make current failed: 0x%x\n", eglGetError());
    exit(EXIT_FAILURE);
  }

  return 0;
}

int
main(int argc, char *argv[])
{
  const int winWidth = 800, winHeight = 600;
  EGLSurface egl_surf;
  EGLContext egl_ctx;
  EGLDisplay egl_dpy;
  gf_surface_t gf_surface;

  if (!InitTest(argc, argv)) {
    return -1;
  }

  init_contenxt(&egl_dpy, &egl_surf, &gf_surface, winWidth, winHeight, 1);

  InitRendering();

  // render once
  testDrawPlane->resizeView(winWidth, winHeight);
  testDrawPlane->resetView();
  testDrawPlane->render();
  eglSwapBuffers(egl_dpy, egl_surf);

  // begin the event loop if not in testing mode
  bool testPassed = true;
  if (!testDrawPlane->isTesting()) {
    // TODO fix this
//    event_loop(x_dpy, win, egl_dpy, egl_surf);
  }
  else {
    testPassed = DoTesting();
  }

  FinalizeTest();

  eglDestroyContext(egl_dpy, egl_ctx);
  eglDestroySurface(egl_dpy, egl_surf);
  eglTerminate(egl_dpy);

  return testPassed ? 0 : 1;
}
