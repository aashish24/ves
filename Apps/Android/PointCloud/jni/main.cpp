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
/*
#include <GLES/gl.h>
*/

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/asset_manager.h>

#include <vtkPolyDataReader.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkNew.h>
#include <vtkLookupTable.h>

#include <vtkPolyDataToTriangleData.h>

#include <vesTriangleData.h>
#include <vesMultitouchCamera.h>
#include <vesRenderer.h>
#include <vesShader.h>
#include <vesShaderProgram.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))

/**
 * Our saved state data.
 */
struct saved_state {
    float angle;
    int32_t x0;
    int32_t y0;
    int32_t x1;
    int32_t y1;

    bool isTwoTouches;
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

    float *points;
    unsigned int nPoints;
    vesRenderer* renderer;
};


static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error
            = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

static const char gVertexShader[] = 
    "attribute vec4 vPosition;\n"
    "void main() {\n"
    "  gl_Position = vPosition;\n"
    "}\n";

static const char gFragmentShader[] = 
    "precision mediump float;\n"
    "void main() {\n"
    "  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
    "}\n";

GLuint loadShader(GLenum shaderType, const char* pSource) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &pSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char* buf = (char*) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOGE("Could not compile shader %d:\n%s\n",
                            shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

GLuint createProgram(const char* pVertexSource, const char* pFragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader) {
        return 0;
    }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader) {
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");
        glAttachShader(program, pixelShader);
        checkGlError("glAttachShader");
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = (char*) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGE("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
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

    engine->renderer = new vesRenderer();
    
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
    engine->state.isTwoTouches = false;
    engine->renderer->Resize(w, h, 1.0f);
    
    // Initialize GL state.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
#if 0
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrthof(-2.0, 2.0, -2.0, 2.0, -2.0, 2.0);
    //glFrustumf(-2.0, 2.0, -2.0, 2.0, -2.0, 2.0);
    
#endif
    
    AAssetManager* assetManager = engine->app->activity->assetManager;
    AAsset* asset = AAssetManager_open(assetManager, "cturtle.vtk", AASSET_MODE_UNKNOWN);
    if (asset == NULL) {
        LOGW("could not open asset");
    }
    off_t len = AAsset_getLength(asset);
    const char* input_string = static_cast<const char*>(AAsset_getBuffer(asset));
    LOGI("\n**\nlength is %d", AAsset_getLength(asset));

    vtkSmartPointer<vtkPolyDataReader> read = vtkSmartPointer<vtkPolyDataReader>::New ();
    read->SetInputString (input_string, len);
    read->ReadFromInputStringOn ();
    read->Update ();
    vtkPolyData *data = read->GetOutput ();
    LOGI("a: number of points is %d", data->GetNumberOfPoints());
    vesTriangleData* triangle_data = vtkPolyDataToTriangleData::Convert (data);
    LOGI("b: number of points is %d\n**\n", triangle_data->GetPoints().size());
    vesVector2f range = triangle_data->GetPointScalarRange();
    LOGI("scalar range: %f, %f\n", range[0], range[1]);
    
    AAsset* vertex_asset = AAssetManager_open(assetManager, "Shader.vsh", AASSET_MODE_UNKNOWN);
    AAsset* fragment_asset = AAssetManager_open(assetManager, "Shader.fsh", AASSET_MODE_UNKNOWN);
    
    std::string vertex_source = std::string(static_cast<const char*>(AAsset_getBuffer(vertex_asset)), AAsset_getLength(vertex_asset));
    std::string fragment_source = std::string(static_cast<const char*>(AAsset_getBuffer(fragment_asset)), AAsset_getLength(fragment_asset));
    LOGI("vertex_source: %s\n", vertex_source.c_str());
    LOGI("fragment_source: %s\n", fragment_source.c_str());
    
    createProgram(vertex_source.c_str(), fragment_source.c_str());
    
    vesShaderProgram* shader_program = new vesShaderProgram(
                                   const_cast<char*>(vertex_source.c_str()),
                                   const_cast<char*>(fragment_source.c_str()),
                                   (_uni("u_mvpMatrix"),
                                    _uni("u_normalMatrix"),
                                    _uni("u_ecLightDir"),
                                    _uni("u_scalarRange"),
                                    _uni("s_texture")),
                                   (_att("a_vertex"),
                                    _att("a_normal"),
                                    _att("a_texcoord"),
                                    _att("a_scalar"))
                                   );
    vesShader* shader = new vesShader(shader_program);    
    vesMapper* mapper = new vesMapper();
    mapper->setTriangleData(triangle_data);
    mapper->setDrawPoints(true);
    
    vtkNew<vtkLookupTable> lookupTable;
    lookupTable->Build();
    unsigned char* lookupImage = lookupTable->GetPointer(0);
    glEnable(GL_TEXTURE_2D);
    unsigned int lookupTableID;
    glGenTextures(1,&lookupTableID);
    glBindTexture(GL_TEXTURE_2D, lookupTableID);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 lookupTable->GetNumberOfTableValues(),
                 1,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 lookupImage);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, lookupTableID);
    
    vesActor* actor = new vesActor(shader, mapper);
    engine->renderer->AddActor(actor);
    engine->renderer->ResetCamera();
    
    AAsset_close(asset);
    
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


        // When transitioning from one touch to two touches, we record the position
        // of of the new touch and return.  When transitioning from two touches to
        // one touch, we record the position of the single remaining touch and return.
        if (AMotionEvent_getPointerCount(event) > 1) {
            if (!engine->state.isTwoTouches) {
              engine->state.x1 = AMotionEvent_getX(event, 1);
              engine->state.y1 = AMotionEvent_getY(event, 1);
              engine->state.isTwoTouches = true;
              return 1;
            }
        }
        else if (engine->state.isTwoTouches) {
          engine->state.isTwoTouches = false;
          engine->state.x0 = AMotionEvent_getX(event, 0);
          engine->state.y0 = AMotionEvent_getY(event, 0);
          return 1;
        }

        if (AMotionEvent_getPointerCount(event) > 1 && !engine->state.isTwoTouches) {
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
