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

#ifndef VESFBORENDERTARGETPRIVATE_H
#define VESFBORENDERTARGETPRIVATE_H

// VES includes
#include "vesRenderTarget.h"
#include "vesTexture.h"

// C++ includes
#include <map>


struct Attachment
{
    Attachment():
      m_texture (0x0)
    {
    }

    int width() const
    {
        if (m_texture) return m_texture->width();
        return 0;
    }

    int height() const
    {
      if (m_texture) return m_texture->height();
      return 0;
    }

    int depth() const
    {
      return 0;
    }

    vesTexture *m_texture;
};


class vesFBORenderTarget::vesInternal
{
public:

  typedef std::map<AttachmentType, Attachment> BufferAttachmentMap;
  BufferAttachmentMap         m_bufferAttachmentMap;

  unsigned int                m_frameBufferHandle;

  std::vector<unsigned int>   m_renderBuffersHandle;
};


#endif // VESFBORENDERTARGETPRIVATE_H
