The Xcode project files for the apps depend on vtkmodular and ves libraries.
To build vtkmodular and ves for the iPhoneSimulator and iPhoneOS, execute
the following commands.  A recent version of CMake is required.

cd CMakeBuild
cmake -P configure.cmake

cd build
make -j4


Next, try out the KiwiViewer iOS app.

cd Kiwi
open Kiwi.xcodeproj
