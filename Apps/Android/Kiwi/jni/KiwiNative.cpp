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
#include <vtkTimerLog.h>

#include <cassert>
#include <fstream>


#define  LOG_TAG    "KiwiViewer"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

// This include uses the LOGI, LOGW, LOGE macro definitions
#include "vtkAndroidOutputWindow.h"

namespace {

class AndroidAppState {
public:

  std::string currentDataset;
  vesVector3f cameraPosition;
  vesVector3f cameraFocalPoint;
  vesVector3f cameraViewUp;
};

//----------------------------------------------------------------------------
vesKiwiViewerApp* app;
AndroidAppState appState;
std::string storageDir;
AAssetManager* assetManager;

int fpsFrames;
double fpsT0;

//----------------------------------------------------------------------------
double touchDownTime;
double tapPanDistance;;
int tapX;
int tapY;

void maybeTapDown(int x, int y)
{
  touchDownTime = vtkTimerLog::GetUniversalTime();
  tapPanDistance = 0.0;
  tapX = x;
  tapY = y;
}

void maybeTapUp()
{
  double upTime = vtkTimerLog::GetUniversalTime();
  double elapsed = upTime - touchDownTime;

  if (tapPanDistance < 3.0 && elapsed < 0.25) {
    app->handleSingleTouchTap(tapX, tapY);
  }
}

void maybeCancelTap(double dx, double dy)
{
  double dist = sqrtf(dx*dx + dy*dy);
  tapPanDistance += dist;
}

//----------------------------------------------------------------------------
std::string documentsDirectory()
{
  assert(storageDir.size());
  return storageDir + "/KiwiViewer";
}

//----------------------------------------------------------------------------
std::string copyAssetToExternalStorage(std::string filename)
{
  std::string destDirectory = documentsDirectory();
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


bool  interialMotionEnabled;
double lastMovementXYUnitDeltaX;
double lastMovementXYUnitDeltaY;
double lastRotationMotionNorm;

//----------------------------------------------------------------------------
void stopInertialMotion()
{
  interialMotionEnabled = false;
}


//----------------------------------------------------------------------------
void startInertialMotion()
{
  interialMotionEnabled = true;
}

//----------------------------------------------------------------------------
void updateInertialMotion()
{
  if (!interialMotionEnabled) {
    return;
  }

  double deltaX;
  double deltaY;

  if (lastRotationMotionNorm > 0.5) {

    deltaX = lastRotationMotionNorm*lastMovementXYUnitDeltaX;
    deltaY = lastRotationMotionNorm*lastMovementXYUnitDeltaY;

    app->handleSingleTouchPanGesture(deltaX, deltaY);
    lastRotationMotionNorm *= 0.9;
    }
  else {
    lastRotationMotionNorm = 0;
    stopInertialMotion();
  }
}

//----------------------------------------------------------------------------
void loadDataset(std::string filename)
{
  LOGI("loadDataset(%s)", filename.c_str());

  app->loadDataset(filename);

  // we may need to restore this dataset later if the android app
  // loses its GL resources
  appState.currentDataset = filename;
}

//----------------------------------------------------------------------------
void loadDataset(int index)
{
  std::string datasetFilename = app->builtinDatasetFilename(index);
  std::string absoluteFilename = copyAssetToExternalStorage(datasetFilename);
  loadDataset(absoluteFilename);
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
void storeCameraState()
{

  appState.cameraPosition = app->cameraPosition();
  appState.cameraFocalPoint = app->cameraFocalPoint();
  appState.cameraViewUp = app->cameraViewUp();
}

//----------------------------------------------------------------------------
void restoreCameraState()
{
  app->setCameraPosition(appState.cameraPosition);
  app->setCameraFocalPoint(appState.cameraFocalPoint);
  app->setCameraViewUp(appState.cameraViewUp);
}

//----------------------------------------------------------------------------
bool setupGraphics(int w, int h)
{

  // For android 2.3, the app may lose its GL context and so it
  // calls setupGraphics when the app resumes.  We don't have an
  // easy way to re-initialize just the GL resources, so the strategy
  // for now is to delete the whole app instance and then restore
  // the current dataset and camera state.
  bool isResume = app != NULL;
  if (isResume) {
    storeCameraState();
    delete app; app = 0;
    isResume = true;
  }

  // Pipe VTK messages into the android log
  vtkAndroidOutputWindow::Install();

  app = new vesKiwiViewerApp();
  app->resizeView(w, h);

  if (!isResume) {
    loadDataset(app->defaultBuiltinDatasetIndex());
    app->resetView();
  }
  else {
    app->loadDataset(appState.currentDataset);
    restoreCameraState();
  }

  fpsFrames = 0;
  fpsT0 = vtkTimerLog::GetUniversalTime();

  interialMotionEnabled = false;
  lastMovementXYUnitDeltaX = 0;
  lastMovementXYUnitDeltaY = 0;
  lastRotationMotionNorm = 0;

  return true;
}

}; // end namespace
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
extern "C" {
  JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_init(JNIEnv * env, jobject obj,  jint width, jint height);
  JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_reshape(JNIEnv * env, jobject obj,  jint width, jint height);
  JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_handleSingleTouchPanGesture(JNIEnv * env, jobject obj,  jfloat dx, jfloat dy);
  JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_handleTwoTouchPanGesture(JNIEnv * env, jobject obj,  jfloat x0, jfloat y0, jfloat x1, jfloat y1);
  JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_handleTwoTouchPinchGesture(JNIEnv * env, jobject obj,  jfloat scale);
  JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_handleTwoTouchRotationGesture(JNIEnv * env, jobject obj,  jfloat rotation);
  JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_handleSingleTouchDown(JNIEnv * env, jobject obj,  jfloat x, jfloat y);
  JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_handleSingleTouchUp(JNIEnv * env, jobject obj);
  JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_render(JNIEnv * env, jobject obj);
  JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_resetCamera(JNIEnv * env, jobject obj);
  JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_loadAssets(JNIEnv* env, jclass obj, jobject assetManager, jstring filename);
  JNIEXPORT jstring JNICALL Java_com_kitware_KiwiViewer_KiwiNative_getDatasetName(JNIEnv* env, jobject obj, jint offset);
  JNIEXPORT jstring JNICALL Java_com_kitware_KiwiViewer_KiwiNative_getDatasetFilename(JNIEnv* env, jobject obj, jint offset);
  JNIEXPORT jint JNICALL Java_com_kitware_KiwiViewer_KiwiNative_getNumberOfBuiltinDatasets(JNIEnv* env, jobject obj);
  JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_loadDataset(JNIEnv * env, jobject obj, jint datasetIndex);
};

//-----------------------------------------------------------------------------
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

JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_handleSingleTouchPanGesture(JNIEnv * env, jobject obj,  jfloat dx, jfloat dy)
{
  stopInertialMotion();

  // update data for inertial rotation
  lastRotationMotionNorm = sqrtf(dx*dx + dy*dy);
  if (lastRotationMotionNorm > 0)
    {
    lastMovementXYUnitDeltaX = dx / lastRotationMotionNorm;
    lastMovementXYUnitDeltaY = dy / lastRotationMotionNorm;
    }
  else
    {
    lastMovementXYUnitDeltaX = 0.0f;
    lastMovementXYUnitDeltaY = 0.0f;
    }

  app->handleSingleTouchPanGesture(dx, dy);

  maybeCancelTap(dx, dy);
}

JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_handleTwoTouchPanGesture(JNIEnv * env, jobject obj,  jfloat x0, jfloat y0, jfloat x1, jfloat y1)
{
  stopInertialMotion();

  app->handleTwoTouchPanGesture(x0, y0, x1, y1);
}

JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_handleTwoTouchPinchGesture(JNIEnv * env, jobject obj,  jfloat scale)
{
  stopInertialMotion();

  app->handleTwoTouchPinchGesture(scale);
}

JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_handleTwoTouchRotationGesture(JNIEnv * env, jobject obj,  jfloat rotation)
{
  stopInertialMotion();

  app->handleTwoTouchRotationGesture(rotation);
}

JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_handleSingleTouchDown(JNIEnv * env, jobject obj,  jfloat x, jfloat y)
{
  app->handleSingleTouchDown(x, y);

  maybeTapDown(x, y);
}

JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_handleSingleTouchUp(JNIEnv * env, jobject obj)
{
  if (!app->widgetInteractionIsActive()) {
    startInertialMotion();
  }
  else {
    stopInertialMotion();
  }
  app->handleSingleTouchUp();

  maybeTapUp();
}

JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_render(JNIEnv * env, jobject obj)
{
  double currentTime = vtkTimerLog::GetUniversalTime();
  double dt = currentTime - fpsT0;
  if (dt > 1.0) {
    LOGI("fps: %f", fpsFrames/dt);
    fpsFrames = 0;
    fpsT0 = currentTime;
  }

  updateInertialMotion();

  app->render();

  fpsFrames++;
}

JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_resetCamera(JNIEnv * env, jobject obj)
{
  stopInertialMotion();
  app->resetView();
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

JNIEXPORT jstring JNICALL Java_com_kitware_KiwiViewer_KiwiNative_getDatasetName(JNIEnv* env, jobject obj, jint offset)
{
  std::string name = app->builtinDatasetName(offset);
  const char* nameForOutput = name.c_str();  
  LOGI("Returning dataset name: %s", name.c_str());
  return(env->NewStringUTF(name.c_str()));
}

JNIEXPORT jstring JNICALL Java_com_kitware_KiwiViewer_KiwiNative_getDatasetFilename(JNIEnv* env, jobject obj, jint offset)
{
  std::string name = app->builtinDatasetFilename(offset);
  const char* nameForOutput = name.c_str();  
  LOGI("Returning dataset filename: %s", name.c_str());
  return(env->NewStringUTF(name.c_str()));
}

JNIEXPORT jint JNICALL Java_com_kitware_KiwiViewer_KiwiNative_getNumberOfBuiltinDatasets(JNIEnv* env, jobject obj)
{
  return static_cast<jint>(app->numberOfBuiltinDatasets());
}

JNIEXPORT void JNICALL Java_com_kitware_KiwiViewer_KiwiNative_loadDataset(JNIEnv * env, jobject obj, jint datasetIndex)
{
  stopInertialMotion();
  loadDataset(datasetIndex);
}
