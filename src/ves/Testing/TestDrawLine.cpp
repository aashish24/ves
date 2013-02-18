#include <vesConfigure.h>

#include "vesTestBase.h"

class vesTestDrawLine : public vesTestBase
{
public:
  vesTestDrawLine() : vesTestBase()
  {
  }


  void resizeView(int width, int height)
  {
    glViewport(0, 0, (GLint)width, (GLint)height);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glOrthof (0, width, height, 0, 0, 1);
    glMatrixMode (GL_MODELVIEW);

    this->Line[0] = 0;
    this->Line[1] = 0;
    this->Line[2] = width;
    this->Line[3] = height;
  }


  bool initScene()
  {
    glDisable(GL_DEPTH_TEST);

    if (glGetError())
    {
      printf("Oops! I screwed up my OpenGL ES calls somewhere\n");

      return false;
    }

    return true;
  }


  void render()
  {
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor4f (0.0f, 1.0f, 0.0f, 1.0f);
    glVertexPointer (2, GL_FLOAT, 0, this->Line);
    glEnableClientState (GL_VERTEX_ARRAY);

    glDrawArrays (GL_LINES, 0, 2);
  }

private:
  float             Line[4];
};


int main(int argc, char** argv)
{
  vesTestDrawLine testDrawLine;

#ifdef VES_QNX
  vesQNXTestDriver<vesTestDrawLine> testDriver(&testDrawLine);
#elif defined (VES_HOST)
  vesX11TestDriver<vesTestDrawLine> testDriver(&testDrawLine);
#endif

  if (!testDrawLine.init(argc, argv)) {
    return -1;
    fprintf(stderr, "Looping\n");
  }

  testDriver.init();

  testDrawLine.resizeView(testDriver.width(), testDriver.height());

  testDriver.render();

  // begin the event loop if not in testing mode
  bool testPassed = true;
  if (!testDrawLine.isTesting()) {
    testDriver.start();
  }
  else {
    testPassed = testDrawLine.doTesting();
  }

  testDrawLine.finalizeTest();
  testDriver.finalize();

  return testPassed ? 0 : 1;

}
