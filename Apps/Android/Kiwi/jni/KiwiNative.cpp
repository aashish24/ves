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

#include <jni.h>
#include <sys/types.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include <vesKiwiViewerApp.h>

#include <vtksys/SystemTools.hxx>
#include <cassert>
#include <fstream>


#define  LOG_TAG    "KiwiViewer"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

// This include uses the LOGI, LOGW, LOGE macro definitions
#include "vtkAndroidOutputWindow.h"

namespace {

//----------------------------------------------------------------------------
vesKiwiViewerApp* app;
std::string storageDir;
AAssetManager* assetManager;

//----------------------------------------------------------------------------
std::string copyAssetToExternalStorage(std::string filename)
{
  std::string destDirectory = storageDir + "/KiwiViewer";
  std::string destFilename = destDirectory + "/" + filename;

  if (vtksys::SystemTools::FileExists(destFilename.c_str())) {
    return destFilename;
  }

  vtksys::SystemTools::MakeDirectory(destDirectory.c_str());

  LOGI("Reading asset file: %s", filename.c_str());
  AAsset* asset = AAssetManager_open(assetManager, filename.c_str(), AASSET_MODE_UNKNOWN);
  if (asset == NULL) {
      LOGE("Could not open asset: %s", filename.c_str());
      return std::string();
  }

  off_t len = AAsset_getLength(asset);
  const char* input_string = static_cast<const char*>(AAsset_getBuffer(asset));
  //LOGI("Asset file is %u bytes", len);

  LOGI("Writing to destination file: %s", destFilename.c_str());
  std::ofstream outfile(destFilename.c_str(), std::ofstream::binary);
  outfile.write(input_string, len);
  outfile.close();
  AAsset_close(asset);

  return destFilename;
}

//----------------------------------------------------------------------------
void loadDataset(int index)
{
  std::string datasetFilename = app->builtinDatasetFilename(index);
  datasetFilename = copyAssetToExternalStorage(datasetFilename);
  app->loadDataset(datasetFilename);
}

//----------------------------------------------------------------------------
void loadNextDataset()
{
  LOGI("Load next dataset");
  static int currentDatasetIndex = -1;
  currentDatasetIndex++;
  if (currentDatasetIndex >= app->numberOfBuiltinDatasets())
    {
    currentDatasetIndex = 0;
    }

  loadDataset(currentDatasetIndex);
}

//----------------------------------------------------------------------------
std::string getContentsOfAssetFile(const std::string filename)
{
  AAsset* assetFile = AAssetManager_open(assetManager, filename.c_str(), AASSET_MODE_UNKNOWN);
  std::string contents = std::string(static_cast<const char*>(AAsset_getBuffer(assetFile)), AAsset_getLength(assetFile));
  AAsset_close(assetFile);
  return contents;
}

//----------------------------------------------------------------------------
bool setupGraphics(int w, int h)
{
  // Pipe VTK messages into the android log
  vtkAndroidOutputWindow::Install();

  std::string vertex_source = getContentsOfAssetFile("Shader.vsh");
  std::string fragment_source = getContentsOfAssetFile("Shader.fsh");
  //LOGI("vertex_source: %s\n", vertex_source.c_str());
  //LOGI("fragment_source: %s\n", fragment_source.c_str());

  app = new vesKiwiViewerApp();
  app->setVertexShaderSource(vertex_source);
  app->setFragmentShaderSource(fragment_source);
  app->initializeShaderProgram();
  app->initializeRendering();

  vertex_source = getContentsOfAssetFile("BackgroundTexture.vsh");
  fragment_source = getContentsOfAssetFile("BackgroundTexture.fsh");
  app->setVertexShaderSource(vertex_source);
  app->setFragmentShaderSource(fragment_source);
  app->initializeTextureShader();

  loadDataset(app->defaultBuiltinDatasetIndex());
  app->resizeView(w, h);
  app->resetView();

  return true;
}

}; // end namespace
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
extern "C" {
  JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_init(JNIEnv * env, jobject obj,  jint width, jint height);
  JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_reshape(JNIEnv * env, jobject obj,  jint width, jint height);
  JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_rotateCamera(JNIEnv * env, jobject obj,  jfloat dx, jfloat dy);
  JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_render(JNIEnv * env, jobject obj);
  JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_resetCamera(JNIEnv * env, jobject obj);
  JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_loadNextDataset(JNIEnv * env, jobject obj);
  JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_loadAssets(JNIEnv* env, jclass obj, jobject assetManager, jstring filename);
};

JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_init(JNIEnv * env, jobject obj,  jint width, jint height)
{
  LOGI("setupGraphics(%d, %d)", width, height);
  setupGraphics(width, height);
}

JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_reshape(JNIEnv * env, jobject obj,  jint width, jint height)
{
  LOGI("reshape(%d, %d)", width, height);
  app->resizeView(width, height);
}

JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_rotateCamera(JNIEnv * env, jobject obj,  jfloat dx, jfloat dy)
{
  app->handleSingleTouchPanGesture(dx, dy);
}

JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_render(JNIEnv * env, jobject obj)
{
  app->render();
}

JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_resetCamera(JNIEnv * env, jobject obj)
{
  app->resetView();
}

JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_loadNextDataset(JNIEnv * env, jobject obj)
{
  loadNextDataset();
}

JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_loadAssets(JNIEnv* env, jclass obj,
        jobject assetManagerJava, jstring filename)
{
  assetManager = AAssetManager_fromJava(env, assetManagerJava);
  assert(assetManager != NULL);

  const char *javaStr = env->GetStringUTFChars(filename, NULL);
  storageDir = javaStr;
  env->ReleaseStringUTFChars(filename, javaStr);
  LOGI("Using external storage directory %s", storageDir.c_str());
}
