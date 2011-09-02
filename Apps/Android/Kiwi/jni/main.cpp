/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

//BEGIN_INCLUDE(all)
#include <jni.h>
#include <errno.h>

#include <EGL/egl.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/asset_manager.h>

#include <vtkXMLPolyDataReader.h>
#include <vtkPolyDataReader.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkNew.h>
#include <vtkLookupTable.h>
#include <vtkErrorCode.h>
#include <vtkOutputWindow.h>
#include <vtkObjectFactory.h>

#include <vtkPolyDataToTriangleData.h>

#include <vesMultitouchCamera.h>
#include <vesRenderer.h>
#include <vesShader.h>
#include <vesShaderProgram.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "kiwi-native", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "kiwi-native", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_WARN, "kiwi-native", __VA_ARGS__))


class vtkAndroidOutputWindow : public vtkOutputWindow
{
public:
  vtkTypeMacro(vtkAndroidOutputWindow, vtkOutputWindow);
  static vtkAndroidOutputWindow* New();

  void DisplayDebugText(const char* t)
    {
    LOGI(t);
    }
  void DisplayWarningText(const char* t)
    {
    LOGW(t);
    }
  void DisplayErrorText(const char* t)
    {
    LOGE(t);
    }
  void DisplayText(const char* t)
    {
    LOGI(t);
    }
  void DisplayGenericWarningText(const char* t)
    {
    LOGI(t);
    }

private:
  vtkAndroidOutputWindow() { }

  vtkAndroidOutputWindow(const vtkAndroidOutputWindow&);
  void operator=(const vtkAndroidOutputWindow&);

};

vtkStandardNewMacro(vtkAndroidOutputWindow);

/**
 * Our saved state data.
 */
struct saved_state {
    float angle;
    int32_t x0;
    int32_t y0;
    int32_t x1;
    int32_t y1;

    int touchCount;
};

/**
 * Shared state for our app.
 */
struct engine {
    struct android_app* app;

    ASensorManager* sensorManager;
    const ASensor* accelerometerSensor;
    ASensorEventQueue* sensorEventQueue;

    int animating;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int32_t width;
    int32_t height;
    struct saved_state state;

    vesRenderer* renderer;
    vesMapper* mapper;

    int currentDataset;
    std::vector<std::string> datasets;
};


static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error
            = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}


vesTriangleData* load_data(struct engine* engine, std::string filename)
{
  // Copy file from assets to /sdcard/KiwiViewer

  // read file
  LOGI("Reading asset file: %s", filename.c_str());
  AAssetManager* assetManager = engine->app->activity->assetManager;
  AAsset* asset = AAssetManager_open(assetManager, filename.c_str(), AASSET_MODE_UNKNOWN);
  if (asset == NULL) {
      LOGW("could not open asset");
      return 0;
  }
  off_t len = AAsset_getLength(asset);
  const char* input_string = static_cast<const char*>(AAsset_getBuffer(asset));
  LOGI("Read %u bytes", len);

  // write file
  std::string destFilename = "/sdcard/KiwiViewer/" + filename;
  LOGI("Writing to destination file: %s", destFilename.c_str());
  ofstream outfile(destFilename.c_str(), ofstream::binary);
  outfile.write(input_string, len);
  outfile.close();
  AAsset_close(asset);

  // read data
  vtkNew<vtkXMLPolyDataReader> reader;
  reader->SetFileName(destFilename.c_str());
  reader->Update();
  vtkPolyData *data = reader->GetOutput();
  LOGI("Geometry number of points: %d", data->GetNumberOfPoints());
  vesTriangleData* triangle_data = vtkPolyDataToTriangleData::Convert(data);
  vtkPolyDataToTriangleData::ComputeVertexColorFromScalars(data, triangle_data);
  return triangle_data;
}

vesTriangleData* get_next_dataset(struct engine* engine)
{
  if (!engine->datasets.size()) {
    return 0;
  }

  engine->currentDataset++;
  if (engine->currentDataset >= engine->datasets.size()) {
    engine->currentDataset = 0;
  }

  return load_data(engine, engine->datasets[engine->currentDataset]);
}

/**
 * Initialize an EGL context for the current display.
 */
static int engine_init_display(struct engine* engine) {
    // initialize OpenGL ES and EGL

    /*
     * Here specify the attributes of the desired configuration.
     * Below, we select an EGLConfig with at least 8 bits per color
     * component compatible with on-screen windows
     */
    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_DEPTH_SIZE, 8,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_NONE
    };
    const EGLint context_attribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };
    EGLint w, h, dummy, format;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, 0, 0);

    /* Here, the application chooses the configuration it desires. In this
     * sample, we have a very simplified selection process, where we pick
     * the first EGLConfig that matches our criteria */
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);

    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
     * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
     * As soon as we picked a EGLConfig, we can safely reconfigure the
     * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

    ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

    surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);
    context = eglCreateContext(display, config, NULL, context_attribs);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOGW("Unable to eglMakeCurrent");
        return -1;
    }


    vtkAndroidOutputWindow* window = vtkAndroidOutputWindow::New();
    vtkOutputWindow::SetInstance(window);

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);
    LOGI("Width %d, Height %d", w, h);


    engine->display = display;
    engine->context = context;
    engine->surface = surface;
    engine->width = w;
    engine->height = h;
    engine->state.angle = 0;
    engine->state.x0 = 0;
    engine->state.y0 = 0;
    engine->state.x1 = 0;
    engine->state.y1 = 0;
    engine->state.touchCount = 0;

    engine->currentDataset = -1;
    //engine->datasets.push_back("current.vtk");
    engine->datasets.push_back("shuttle.vtp");
    engine->datasets.push_back("AppendedKneeData.vtp");
    engine->datasets.push_back("Buckyball.vtp");
    engine->datasets.push_back("MountStHelen.vtp");
    engine->datasets.push_back("bunny.vtp");
    engine->datasets.push_back("teapot.vtp");
    engine->datasets.push_back("visible-woman-hand.vtp");


    engine->renderer = new vesRenderer();
    engine->renderer->Resize(w, h, 1.0f);


    glClearColor(63/255.0f, 96/255.0f, 144/255.0, 1.0f);

    AAssetManager* assetManager = engine->app->activity->assetManager;
    AAsset* vertex_asset = AAssetManager_open(assetManager, "Shader.vsh", AASSET_MODE_UNKNOWN);
    AAsset* fragment_asset = AAssetManager_open(assetManager, "Shader.fsh", AASSET_MODE_UNKNOWN);
    std::string vertex_source = std::string(static_cast<const char*>(AAsset_getBuffer(vertex_asset)), AAsset_getLength(vertex_asset));
    std::string fragment_source = std::string(static_cast<const char*>(AAsset_getBuffer(fragment_asset)), AAsset_getLength(fragment_asset));
    LOGI("vertex_source: %s\n", vertex_source.c_str());
    LOGI("fragment_source: %s\n", fragment_source.c_str());
    AAsset_close(vertex_asset);
    AAsset_close(fragment_asset);


    vesShaderProgram* shader_program = new vesShaderProgram(
                                   const_cast<char*>(vertex_source.c_str()),
                                   const_cast<char*>(fragment_source.c_str()),
                                   (_uni("u_mvpMatrix"),
                                    _uni("u_normalMatrix"),
                                    _uni("u_ecLightDir"),
                                    _uni("u_opacity")),
                                   (_att("a_vertex"),
                                    _att("a_normal"),
                                    _att("a_vertex_color"))
                                   );
    vesShader* shader = new vesShader(shader_program);
    vesMapper* mapper = new vesMapper();

    vesTriangleData* triangle_data = get_next_dataset(engine);

    mapper->SetTriangleData(triangle_data);
    mapper->SetColor(0.8, 0.8, 0.8, 1.0);
    engine->mapper = mapper;

    vesActor* actor = new vesActor(shader, mapper);
    engine->renderer->AddActor(actor);
    engine->renderer->ResetCamera();

    return 0;
}

/**
 * Just the current frame in the display.
 */
static void engine_draw_frame(struct engine* engine) {
    engine->renderer->ResetCameraClippingRange();
    engine->renderer->Render();

    eglSwapBuffers(engine->display, engine->surface);
}

/**
 * Tear down the EGL context currently associated with the display.
 */
static void engine_term_display(struct engine* engine) {
    if (engine->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (engine->context != EGL_NO_CONTEXT) {
            eglDestroyContext(engine->display, engine->context);
        }
        if (engine->surface != EGL_NO_SURFACE) {
            eglDestroySurface(engine->display, engine->surface);
        }
        eglTerminate(engine->display);
    }
    engine->animating = 0;
    engine->display = EGL_NO_DISPLAY;
    engine->context = EGL_NO_CONTEXT;
    engine->surface = EGL_NO_SURFACE;
}

/**
 * Process the next input event.
 */
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
    struct engine* engine = (struct engine*)app->userData;
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {


        if (AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_DOWN) {
            engine->state.x0 = AMotionEvent_getX(event, 0);
            engine->state.y0 = AMotionEvent_getY(event, 0);
            return 1;
        }

        const int nTouches = AMotionEvent_getPointerCount(event);

        if (nTouches != engine->state.touchCount) {
          engine->state.touchCount = nTouches;

          engine->state.x0 = AMotionEvent_getX(event, 0);
          engine->state.y0 = AMotionEvent_getY(event, 0);

          if (nTouches > 1)
            {
            engine->state.x1 = AMotionEvent_getX(event, 1);
            engine->state.y1 = AMotionEvent_getY(event, 1);
            }


          if (nTouches == 3) {
            engine->renderer->ResetCamera();
            return 1;
          }

          if (nTouches == 4) {

              delete engine->mapper->GetTriangleData();
              engine->mapper->SetTriangleData(get_next_dataset(engine));
          }

          return 1;
        }





        engine->animating = 1;
        float px0 = engine->state.x0;
        float py0 = engine->state.y0;
        float px1 = engine->state.x1;
        float py1 = engine->state.y1;
        float x0 = AMotionEvent_getX(event, 0);
        float y0 = AMotionEvent_getY(event, 0);
        float x1 = x0;
        float y1 = y0;
        if (AMotionEvent_getPointerCount(event) > 1) {
            x1 = AMotionEvent_getX(event, 1);
            y1 = AMotionEvent_getY(event, 1);
        }

        vesRenderer* ren = engine->renderer;
        vesCamera *camera = ren->GetCamera();

        if (AMotionEvent_getPointerCount(event) == 1) {
            float dx0 = x0 - px0;
            float dy0 = y0 - py0;

            double delta_elevation = -20.0 / ren->GetHeight();
            double delta_azimuth = -20.0 / ren->GetWidth();
            double motionFactor = 10.0;

            double rxf = dx0 * delta_azimuth * motionFactor;
            double ryf = dy0 * delta_elevation * motionFactor;

            camera->Azimuth(rxf);
            camera->Elevation(ryf);
            camera->OrthogonalizeViewUp();
        } else {
          //
          // Pan camera.
          // Implemented based on vtkInteractorStyleTrackballCamera::Pan().
          //

          // Average positions of current and previous two touches.
          // Invert y since vesCamera expects y to go in opposite direction.
          float pcx = (px0 + px1)/2.0;
          float pcy = ren->GetHeight() - (py0 + py1)/2.0;
          float cx = (x0 + x1)/2.0;
          float cy = ren->GetHeight() - (y0 + y1)/2.0;

          // Calculate the focal depth since we'll be using it a lot
          vesVector3f viewFocus = camera->GetFocalPoint();
          vesVector3f viewFocusDisplay = ren->ComputeWorldToDisplay(viewFocus);
          float focalDepth = viewFocusDisplay[2];

          vesVector3f newPos(cx,
                             cy,
                             focalDepth);
          vesVector3f newPickPoint = ren->ComputeDisplayToWorld(newPos);

          vesVector3f oldPos(pcx,
                             pcy,
                             focalDepth);
          vesVector3f oldPickPoint = ren->ComputeDisplayToWorld(oldPos);

          vesVector3f motionVector = oldPickPoint - newPickPoint;

          vesVector3f viewPoint = camera->GetPosition();
          vesVector3f newViewFocus = motionVector + viewFocus;
          vesVector3f newViewPoint = motionVector + viewPoint;
          camera->SetFocalPoint(newViewFocus);
          camera->SetPosition(newViewPoint);

          //
          // Zoom camera.
          // Implemented based on vkInteractorStyleTrackballCamera::Dolly().
          //

          double previousDist = sqrt((px0 - px1) *
                                     (px0 - px1) +
                                     (py0 - py1) *
                                     (py0 - py1));
          double currentDist = sqrt((x0 - x1) *
                                    (x0 - x1) +
                                    (y0 - y1) *
                                    (y0 - y1));
          double dy = currentDist - previousDist;
          double dyf = 10.0 * dy / (ren->GetHeight()/2.0);
          double factor = pow(1.1, dyf);
          camera->Dolly(factor);

          //
          // Roll camera.
          // Implemented based on vkInteractorStyleTrackballCamera::Spin().
          //

          double pi = 3.14159265358979;
          double newAngle = atan2(y0 - y1,
                                  x0 - x1);
          newAngle *= 180.0/pi;

          double oldAngle = atan2(py0 - py1,
                                  px0 - px1);
          oldAngle *= 180.0/pi;

          camera->Roll(newAngle - oldAngle);
          camera->OrthogonalizeViewUp();
        }

        engine->state.x0 = x0;
        engine->state.y0 = y0;
        if (AMotionEvent_getPointerCount(event) > 1) {
            engine->state.x1 = x1;
            engine->state.y1 = y1;
        }

        return 1;
    }
    return 0;
}

/**
 * Process the next main command.
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    struct engine* engine = (struct engine*)app->userData;
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            // The system has asked us to save our current state.  Do so.
            engine->app->savedState = malloc(sizeof(struct saved_state));
            *((struct saved_state*)engine->app->savedState) = engine->state;
            engine->app->savedStateSize = sizeof(struct saved_state);
            break;
        case APP_CMD_INIT_WINDOW:
            if (app->window != NULL) {
                int32_t width = ANativeWindow_getWidth(app->window);
                int32_t height = ANativeWindow_getHeight(app->window);
                ANativeWindow_setBuffersGeometry(app->window, width, height, 1);
                LOGI("Window format is now %d",ANativeWindow_getFormat(app->window));
            }
            // The window is being shown, get it ready.
            if (engine->app->window != NULL) {
                engine_init_display(engine);
                engine_draw_frame(engine);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            engine_term_display(engine);
            break;
        case APP_CMD_GAINED_FOCUS:
            // When our app gains focus, we start monitoring the accelerometer.
            if (engine->accelerometerSensor != NULL) {
                ASensorEventQueue_enableSensor(engine->sensorEventQueue,
                        engine->accelerometerSensor);
                // We'd like to get 60 events per second (in us).
                ASensorEventQueue_setEventRate(engine->sensorEventQueue,
                        engine->accelerometerSensor, (1000L/60)*1000);
            }
            break;
        case APP_CMD_LOST_FOCUS:
            // When our app loses focus, we stop monitoring the accelerometer.
            // This is to avoid consuming battery while not being used.
            if (engine->accelerometerSensor != NULL) {
                ASensorEventQueue_disableSensor(engine->sensorEventQueue,
                        engine->accelerometerSensor);
            }
            // Also stop animating.
            engine->animating = 0;
            engine_draw_frame(engine);
            break;
    }
}

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* state) {
    struct engine engine;

    // Make sure glue isn't stripped.
    app_dummy();

    memset(&engine, 0, sizeof(engine));
    state->userData = &engine;
    state->onAppCmd = engine_handle_cmd;
    state->onInputEvent = engine_handle_input;
    engine.app = state;

    // Prepare to monitor accelerometer
    engine.sensorManager = ASensorManager_getInstance();
    engine.accelerometerSensor = ASensorManager_getDefaultSensor(engine.sensorManager,
            ASENSOR_TYPE_ACCELEROMETER);
    engine.sensorEventQueue = ASensorManager_createEventQueue(engine.sensorManager,
            state->looper, LOOPER_ID_USER, NULL, NULL);

    if (state->savedState != NULL) {
        // We are starting with a previous saved state; restore from it.
        engine.state = *(struct saved_state*)state->savedState;
    }

    // loop waiting for stuff to do.

    while (1) {
        // Read all pending events.
        int ident;
        int events;
        struct android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ident=ALooper_pollAll(engine.animating ? 0 : -1, NULL, &events,
                (void**)&source)) >= 0) {

            // Process this event.
            if (source != NULL) {
                source->process(state, source);
            }

            // If a sensor has data, process it now.
            if (ident == LOOPER_ID_USER) {
                if (engine.accelerometerSensor != NULL) {
                    ASensorEvent event;
                    while (ASensorEventQueue_getEvents(engine.sensorEventQueue,
                            &event, 1) > 0) {
                        //LOGI("accelerometer: x=%f y=%f z=%f",
                        //        event.acceleration.x, event.acceleration.y,
                        //        event.acceleration.z);
                    }
                }
            }

            // Check if we are exiting.
            if (state->destroyRequested != 0) {
                engine_term_display(&engine);
                return;
            }
        }

        if (engine.animating) {
            // Done with events; draw next animation frame.
            engine.state.angle += .01f;
            if (engine.state.angle > 1) {
                engine.state.angle = 0;
            }

            // Drawing is throttled to the screen update rate, so there
            // is no need to do timing here.
            engine_draw_frame(&engine);
        }
    }
}
//END_INCLUDE(all)
