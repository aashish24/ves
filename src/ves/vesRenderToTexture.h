#ifndef VESRENDERTOTEXTURE_H
#define VESRENDERTOTEXTURE_H


#include "vesFBORenderTarget.h"


class vesRenderToTexture : public vesFBORenderTarget
{
public:

           vesRenderToTexture();
  virtual ~vesRenderToTexture();

  bool              setTexture(vesTexture *texture);
  vesTexture*       texture();
  const vesTexture* texture() const;

protected:

  void setup (vesRenderState &renderState);
  void render(vesRenderState &renderState);
};


#endif // VESRENDERTOTEXTURE_H
