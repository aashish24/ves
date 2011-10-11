#ifndef VESGLTYPES_H
#define VESGLTYPES_H

#ifdef ANDROID
# include <GLES2/gl2.h>
# include <GLES2/gl2ext.h>
#else
# include <OpenGLES/ES2/gl.h>
# include <OpenGLES/ES2/glext.h>
#endif

struct vesGLTypes
{
  enum vesPrimitiveTypes
    {
    Points        = 0,
    LineStrip     = 1,
    LineLoop      = 2,
    Lines         = 3,
    TriangleStrip = 4,
    TriangleFan   = 5,
    Triangles     = 6
    };
};


#endif // VESGLTYPES_H
