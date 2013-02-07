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
#include <vesBackground.h>
#include <vesCamera.h>
#include <vesGeometryData.h>
#include <vesMapper.h>
#include <vesMaterial.h>
#include <vesRenderer.h>
#include <vesVertexAttribute.h>
#include <vesVertexAttributeKeys.h>
#include <vesViewport.h>

#include <vesKiwiBaseApp.h>
#include <vesKiwiBaselineImageTester.h>
#include <vesKiwiDataLoader.h>
#include <vesKiwiPolyDataRepresentation.h>

#include <vtkPolyData.h>

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


class vesGradientBackgroundApp : public vesKiwiBaseApp {
public:

  vesGradientBackgroundApp()
  {
    this->DataRep = 0;

    this->renderer()->background()->setGradientColor(
      vesVector4f(0.0f/255.0f, 0.0f/255.0f, 2.0f/255.0f, 1.0),
      vesVector4f(55.0f/255.0f, 55.0f/255.0f, 113.0f/255.0f, 1.0));
  }

  ~vesGradientBackgroundApp()
  {
    this->unloadData();
  }

  void initClipMaterial()
  {
    vesSharedPtr<vesMaterial> material
      = this->addMaterial();
    this->addVertexPositionAttribute(material);
    this->addVertexNormalAttribute(material);
    this->addVertexColorAttribute(material);
    this->addVertexTextureCoordinateAttribute(material);
    this->ClipMaterial = material;
  }

  void unloadData()
  {
    if (this->DataRep) {
      this->DataRep->removeSelfFromRenderer(this->renderer());
      delete this->DataRep;
      this->DataRep = 0;
    }
  }

  void loadData(const std::string& filename)
  {
    this->unloadData();

    vesKiwiDataLoader loader;
    vtkSmartPointer<vtkPolyData> polyData = vtkPolyData::SafeDownCast(loader.loadDataset(filename));
    assert(polyData.GetPointer());

    vesKiwiPolyDataRepresentation* rep = new vesKiwiPolyDataRepresentation();
    rep->initializeWithMaterial(this->ClipMaterial);
    rep->setPolyData(polyData);
    rep->addSelfToRenderer(this->renderer());
    this->DataRep = rep;
  }

  vesSharedPtr<vesMaterial> ClipMaterial;
  vesKiwiPolyDataRepresentation* DataRep;
};

class vesTestHelper {
public:
  vesTestHelper() :
    IsTesting(false)
  {
  }

  ~vesTestHelper()
  {
  }

  vesGradientBackgroundApp* app() {
    return &this->App;
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

  void resizeView(int width, int height) {
    this->App.resizeView(width, height);
  }

  void resetView() {
    this->App.resetView();
  }

  void render() {
    this->App.render();
  }

private:
  vesGradientBackgroundApp App;

  std::string       SourceDirectory;
  std::string       DataDirectory;
  bool              IsTesting;
};

//----------------------------------------------------------------------------
vesTestHelper* testHelper;

//----------------------------------------------------------------------------
void LoadData()
{
  std::string filename = testHelper->sourceDirectory() +
    std::string("/Apps/iOS/Kiwi/Kiwi/Data/bunny.vtp");

  testHelper->app()->loadData(filename);
  testHelper->app()->resetView();
}

//----------------------------------------------------------------------------
void LoadDefaultData()
{
  LoadData();
}

//----------------------------------------------------------------------------
bool DoTesting()
{
  const double threshold = 10.0;
  const std::string testName = "Gradient Background";

  vesKiwiBaselineImageTester baselineTester;
  baselineTester.setApp(testHelper->app());
  baselineTester.setBaselineImageDirectory(testHelper->dataDirectory());
  return baselineTester.performTest(testName, threshold);
}

//----------------------------------------------------------------------------
std::string GetFileContents(const std::string& filename)
{
  std::ifstream file(filename.c_str());
  std::stringstream buffer;
  if (file) {
    buffer << file.rdbuf();
    file.close();
  }
  return buffer.str();
}

//----------------------------------------------------------------------------
void InitRendering()
{
  testHelper->app()->initClipMaterial();
}

//----------------------------------------------------------------------------
bool InitTest(int argc, char* argv[])
{
  if (argc < 2) {
    printf("Usage: %s <path to VES source directory> [path to testing data directory]\n", argv[0]);
    return false;
  }

  testHelper = new vesTestHelper();
  testHelper->setSourceDirectory(argv[1]);

  if (argc == 3) {
    testHelper->setDataDirectory(argv[2]);
    testHelper->setTesting(true);
  }
  return true;
}

//----------------------------------------------------------------------------
void FinalizeTest()
{
  delete testHelper;
}

}; // end namespace


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
  int res = gf_dev_attach(&gfdev, NULL, &info);
  if (res != GF_ERR_OK)
  {
     fprintf(stderr, "gf_dev_attach() %d\n", res);
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
  LoadDefaultData();

  // render once
  testHelper->resizeView(width, height);
  testHelper->resetView();
  testHelper->render();
  glFinish();
  eglWaitGL();
  eglSwapBuffers(display, surface);

  // begin the event loop if not in testing mode
  bool testPassed = true;
  if (!testHelper->isTesting()) {
    do {
       testHelper->render();
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
