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

#if defined(__APPLE__)
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <vesKiwiViewerApp.h>
#include <vesBuiltinShaders.h>
#include <vesSetGet.h>

#include <vtkErrorCode.h>
#include <vtkImageData.h>
#include <vtkImageDifference.h>
#include <vtkImageShiftScale.h>
#include <vtkNew.h>
#include <vtkPNGReader.h>
#include <vtkPNGWriter.h>
#include <vtkSmartPointer.h>

//----------------------------------------------------------------------------
namespace {

class vesTestHelper {
public:

  vesKiwiViewerApp* app() {
    return &this->App;
  }

  std::string sourceDirectory() {
    return this->SourceDirectory;
  }

  void setSourceDirectory(std::string dir) {
    this->SourceDirectory = dir;
  }

  std::string dataDirectory() {
    return this->DataDirectory;
  }

  void setDataDirectory(std::string dir) {
    this->DataDirectory = dir;
  }

  bool isTesting() {
    return this->IsTesting;
  }

  void setTesting(bool testing) {
    this->IsTesting = testing;
  }

private:

  vesKiwiViewerApp App;

  std::string       SourceDirectory;
  std::string       DataDirectory;
  bool              IsTesting;
};

//----------------------------------------------------------------------------
vesTestHelper* testHelper;

int CurrentDataset;

//----------------------------------------------------------------------------
void LoadData(int index)
{
  std::string dataRoot = testHelper->sourceDirectory() + "/Apps/iOS/Kiwi/Kiwi/Data/";
  std::string filename = dataRoot + testHelper->app()->builtinDatasetFilename(index);
  testHelper->app()->loadDataset(filename);
  testHelper->app()->applyBuiltinDatasetCameraParameters(index);
  CurrentDataset = index;
}

//----------------------------------------------------------------------------
void ResetView()
{
  testHelper->app()->applyBuiltinDatasetCameraParameters(CurrentDataset);
}

//----------------------------------------------------------------------------
vtkSmartPointer<vtkImageData> ImageFromFile(const std::string filename)
{
  vtkNew<vtkPNGReader> reader;
  reader->SetFileName(filename.c_str());
  reader->Update();

  if (reader->GetErrorCode() != vtkErrorCode::NoError || reader->GetOutput()->GetNumberOfPoints() <= 1) {
    return 0;
  }

  return reader->GetOutput();
}

//----------------------------------------------------------------------------
vtkSmartPointer<vtkImageData> ImageFromRenderView()
{
  int width = testHelper->app()->viewWidth();
  int height = testHelper->app()->viewHeight();

  vtkSmartPointer<vtkImageData> image = vtkSmartPointer<vtkImageData>::New();
  image->SetDimensions(width, height, 1);
  image->SetScalarTypeToUnsignedChar();
  image->SetNumberOfScalarComponents(3);
  image->AllocateScalars();

  unsigned char* outPtr = static_cast<unsigned char*>(image->GetScalarPointer(0, 0, 0));
  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, (void*)outPtr);
  return image;
}

//----------------------------------------------------------------------------
void WritePNG(vtkImageData* image, const std::string& filename)
{
  vtkNew<vtkPNGWriter> writer;
  writer->SetInput(image);
  writer->SetFileName(filename.c_str());
  writer->Write();
}

//----------------------------------------------------------------------------
bool DoTesting()
{
  const double threshold = 10.0;
  bool allTestsPassed = true;

  // This loads each builtin dataset, renders it, and saves a screenshot

  // Note, this loop renders but does not bother to swap buffers
  for (int i = 0; i < testHelper->app()->numberOfBuiltinDatasets(); ++i) {
    LoadData(i);

    // Enable the background image for the final image regression test
    if (i == testHelper->app()->numberOfBuiltinDatasets()-1)
      {
      testHelper->app()->setBackgroundTexture(testHelper->sourceDirectory() + "/Apps/iOS/Kiwi/Kiwi/Data/kiwi.png");
      }

    // call the info methods, this helps coverage, though we're not testing the return values
    testHelper->app()->numberOfModelFacets();
    testHelper->app()->numberOfModelVertices();
    testHelper->app()->numberOfModelLines();

    testHelper->app()->render();
    std::string datasetName = testHelper->app()->builtinDatasetName(i);

    std::string inFile = testHelper->dataDirectory() + "/" + datasetName + ".png";
    std::string outFile = datasetName + ".png";

    vtkSmartPointer<vtkImageData> baselineImage = ImageFromFile(inFile);
    vtkSmartPointer<vtkImageData> image = ImageFromRenderView();

    if (!baselineImage) {
      std::cout << "Could not read baseline image: " << inFile << std::endl;
      WritePNG(image, outFile.c_str());
      allTestsPassed = false;
    }
    else {
      vtkNew<vtkImageDifference> imageDiff;
      imageDiff->SetInput(image);
      imageDiff->SetImage(baselineImage);
      imageDiff->Update();

      double thresholdError = imageDiff->GetThresholdedError();

      if (thresholdError > threshold) {

        std::cout << "Dataset '" << datasetName << "' image difference test failed with thresholded error: " << thresholdError << std::endl;

        vtkNew<vtkImageShiftScale> gamma;
        gamma->SetInputConnection(imageDiff->GetOutputPort());
        gamma->SetShift(0);
        gamma->SetScale(10);

        WritePNG(gamma->GetOutput(), datasetName + ".diff.png");
        WritePNG(image, outFile);
        allTestsPassed = false;
      }
      else {
        std::cout << "Dataset '" << datasetName << "' image difference test passed with thresholded error: " << thresholdError << std::endl;
      }

    }
  }

  return allTestsPassed;
}

//----------------------------------------------------------------------------
std::string GetFileContents(const std::string& filename)
{
  std::ifstream file(filename.c_str());
  std::stringstream buffer;
  if (file) {
    buffer << file.rdbuf();
    file.close();
  }
  return buffer.str();
}

//----------------------------------------------------------------------------
void InitRendering()
{

}

//----------------------------------------------------------------------------
bool InitTest(int argc, char* argv[])
{
  if (argc < 2) {
    printf("Usage: %s <path to VES source directory> [path to testing data directory]\n", argv[0]);
    return false;
  }

  testHelper = new vesTestHelper();
  testHelper->setSourceDirectory(argv[1]);

  if (argc == 3) {
    testHelper->setDataDirectory(argv[2]);
    testHelper->setTesting(true);
  }
  return true;
}

//----------------------------------------------------------------------------
void FinalizeTest()
{
  delete testHelper;
}

}; // end namespace
//----------------------------------------------------------------------------

int lastMotionX = 0;
int lastMotionY = 0;


void handleMouseClick(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON) {
    if (state == GLUT_DOWN) {
      testHelper->app()->handleSingleTouchDown(x, y);
      lastMotionX = x;
      lastMotionY = y;
    }
    else if (state == GLUT_UP) {
      testHelper->app()->handleSingleTouchUp();
    }
  }
}

void handleMouseMotion(int x, int y)
{
  testHelper->app()->handleSingleTouchPanGesture(x - lastMotionX, y - lastMotionY);
  lastMotionX = x;
  lastMotionY = y;
}

void handleKeyboard(unsigned char key, int x, int y)
{
  if (key == 'n') {
    CurrentDataset = (CurrentDataset + 1) % testHelper->app()->numberOfBuiltinDatasets();
    LoadData(CurrentDataset);
  }
  else if (key == 'r') {
    ResetView();
  }
  else if (key == 'q') {
    exit(0);
    //glutLeaveMainLoop();
  }
}


void display(void) {
  testHelper->app()->render();
  glutSwapBuffers();
}

void reshape(int width, int height) {
  testHelper->app()->resizeView(width, height);
}

void idle(void) {
  glutPostRedisplay();
}


int main(int argc, char *argv[])
{

  if (!InitTest(argc, argv)) {
    return -1;
  }

  const int winWidth = 800;
  const int winHeight = 600;

  //a basic set up...
  glutInit(&argc, &argv[0]);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(winWidth, winHeight);

  //create the window, the argument is the title
  glutCreateWindow("KiwiViewer");

  //pass the callbacks
  glutKeyboardFunc(handleKeyboard);
  glutMouseFunc(handleMouseClick);
  glutMotionFunc(handleMouseMotion);
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutIdleFunc(idle);


  InitRendering();
  testHelper->app()->resizeView(winWidth, winHeight);
  LoadData(testHelper->app()->defaultBuiltinDatasetIndex());
  ResetView();

  // render once
  testHelper->app()->render();
  glutSwapBuffers();

  // begin the event loop if not in testing mode
  bool testPassed = true;
  if (!testHelper->isTesting()) {
    glutMainLoop();
  }
  else {
    testPassed = DoTesting();
  }

  FinalizeTest();


  return testPassed ? 0 : 1;

}
