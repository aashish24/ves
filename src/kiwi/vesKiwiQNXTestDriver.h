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
/// \class vesQNXTestDriver
/// \ingroup ves

#ifndef VESKIWIQNXTESTDRIVER_H
#define VESKIWIQNXTESTDRIVER_H

#include <vesKiwiBaseApp.h>

#include <GLES/gl.h>
#include <GLES/egl.h>

#include <gf/gf.h>
#include <gf/gf3d.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>

#include <iostream>

static EGLint attribute_list[] = {
  EGL_NATIVE_VISUAL_ID, 0,
  EGL_NATIVE_RENDERABLE, EGL_TRUE,
  EGL_RED_SIZE, 5,
  EGL_GREEN_SIZE, 5,
  EGL_BLUE_SIZE, 5,
  EGL_DEPTH_SIZE, 16,
  EGL_NONE
};


class  vesKiwiQNXTestDriver
{
public:
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

  gf_dev_t    gfdev;
  gf_layer_t  layer;
  int         layer_idx;

  EGLDisplay display;
  EGLSurface surface;

  vesKiwiBaseApp* m_test;

public:
  vesKiwiQNXTestDriver(vesKiwiBaseApp* test) {
    this->m_test = test;
  }

  int init() {
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

    return 0;
  }

  void render() {
    this->m_test->render();
    glFinish();
    eglWaitGL();
    eglSwapBuffers(display, surface);
  }


  void start() {
    do {
      this->render();
    }while(1);
  }

  void finalize() {
  }
};

#endif // VESKIWIQNXTESTDRIVER_H
