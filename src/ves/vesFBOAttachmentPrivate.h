#ifndef VESFBORENDERTARGETPRIVATE_H
#define VESFBORENDERTARGETPRIVATE_H

// VES includes
#include "vesTexture.h"

class vesFBORenderTarget::vesInternal
{
public:
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

  typedef std::map<AttachmentType, Attachment> BufferAttachmentMap;
  BufferAttachmentMap         m_bufferAttachmentMap;

  unsigned int                m_frameBufferHandle;

  std::vector<unsigned int>   m_renderBuffersHandle;
};


#endif // VESFBORENDERTARGETPRIVATE_H
