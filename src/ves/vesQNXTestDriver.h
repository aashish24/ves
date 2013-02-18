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
/// \class vesQNX
/// \ingroup ves

#ifndef VESQNX_H
#define VESQNX_H

#include "vesTestDriver.h"

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

template <typename AppT>
class  vesQNXTestDriver : public vesTestDriver<AppT>
{
public:
  gf_3d_target_t      m_target;
  gf_display_t        m_gfDisp;
  EGLConfig           m_config;
  EGLContext          m_econtext;
  EGLint              m_num_config;
  gf_dev_info_t       m_info;
  gf_layer_info_t     m_linfo;
  gf_display_info_t   m_dispInfo;
  GLuint              m_width, m_height;
  GLuint              m_it;

  gf_dev_t    m_gfdev;
  gf_layer_t  m_layer;
  int         m_layerIdx;

  EGLDisplay m_display;
  EGLSurface m_surface;

public:
  vesQNXTestDriver(AppT* test) : vesTestDriver<AppT>(test)
  {
    this->m_width = 800;
    this->m_height = 600;

    this->m_display = NULL;
    this->m_surface = NULL;
  }


  int init()
  {
    /* initialize the graphics device */
    int res = gf_dev_attach(&m_gfdev, NULL, &m_info);
    if (res != GF_ERR_OK)
    {
       fprintf(stderr, "gf_dev_attach() %d\n", res);
       return -1;
    }

    /* Setup the layer we will use */
    if (gf_display_attach(&m_gfDisp, m_gfdev, 0, &m_dispInfo)!=GF_ERR_OK)
    {
       fprintf(stderr, "gf_display_attach() failed\n");
       return -1;
    }

    m_width = m_dispInfo.xres;
    m_height = m_dispInfo.yres;

    m_layerIdx = m_dispInfo.main_layer_index;

    /* get an EGL display connection */
    m_display=eglGetDisplay(m_gfdev);
    if (m_display==EGL_NO_DISPLAY)
    {
       fprintf(stderr, "eglGetDisplay() failed\n");
       return -1;
    }

    if (gf_layer_attach(&m_layer, m_gfDisp, m_layerIdx, 0)!=GF_ERR_OK)
    {
       fprintf(stderr, "gf_layer_attach() failed\n");
       return -1;
    }

    /* initialize the EGL display connection */
    if (eglInitialize(m_display, NULL, NULL)!=EGL_TRUE)
    {
       fprintf(stderr, "eglInitialize: error 0x%x\n", eglGetError());
       return -1;
    }

    for (m_it=0;; m_it++)
    {
       /* Walk through all possible pixel formats for this layer */
       if (gf_layer_query(m_layer, m_it, &m_linfo)==-1)
       {
          fprintf(stderr, "Couldn't find a compatible frame "
                          "buffer configuration on layer %d\n", m_layerIdx);
          return -1;
       }

       /*
        * We want the color buffer format to match the layer format,
        * so request the layer format through EGL_NATIVE_VISUAL_ID.
        */
       attribute_list[1]=m_linfo.format;

       /* Look for a compatible EGL frame buffer configuration */
       if (eglChooseConfig(m_display, attribute_list, &m_config, 1, &m_num_config)==EGL_TRUE)
       {
          if (m_num_config>0)
          {
             break;
          }
       }

       fprintf(stderr, "Looping\n");
    }

    /* create a 3D rendering m_target */
    if (gf_3d_target_create(&m_target, m_layer, NULL, 0, m_width, m_height, m_linfo.format)!=GF_ERR_OK)
    {
       fprintf(stderr, "Unable to create rendering m_target\n");
       return -1;
    }

    gf_layer_set_src_viewport(m_layer, 0, 0, m_width-1, m_height-1);
    gf_layer_set_dst_viewport(m_layer, 0, 0, m_width-1, m_height-1);
    gf_layer_enable(m_layer);

    /*
     * The layer settings haven't taken effect yet since we haven't
     * called gf_layer_update() yet.  This is exactly what we want,
     * since we haven't supplied a valid surface to display yet.
     * Later, the OpenGL ES library calls will call gf_layer_update()
     * internally, when  displaying the rendered 3D content.
     */

    /* create an EGL rendering context */
    m_econtext=eglCreateContext(m_display, m_config, EGL_NO_CONTEXT, NULL);
    if (m_econtext==EGL_NO_CONTEXT)
    {
       fprintf(stderr, "Create context failed: 0x%x\n", eglGetError());
       return -1;
    }

    /* create an EGL window surface */
    m_surface=eglCreateWindowSurface(m_display, m_config, m_target, NULL);
    if (m_surface==EGL_NO_SURFACE)
    {
       fprintf(stderr, "Create surface failed: 0x%x\n", eglGetError());
       return -1;
    }

    /* connect the context to the surface */
    if (eglMakeCurrent(m_display, m_surface, m_surface, m_econtext)==EGL_FALSE)
    {
       fprintf(stderr, "Make current failed: 0x%x\n", eglGetError());
       return -1;
    }

    return 0;
  }


  void render()
  {
    this->m_test->render();
    glFinish();
    eglWaitGL();
    eglSwapBuffers(m_display, m_surface);
  }


  void start()
  {
    do {
      this->render();
    }while(1);
  }


  void finalize()
  {
  }


  virtual int width()
  {
    return this->m_width;
  }


  virtual int height()
  {
    return this->m_height;
  }
};

#endif // vesQNX_H
