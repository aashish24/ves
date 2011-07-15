The Xcode project files for the apps depend on vtkmodular and ves libraries.
To build vtkmodular and ves for the iPhoneSimulator and iPhoneOS, execute
the following commands.  A recent version of CMake is recommended.

cd CMakeBuild
mkdir build
cd build
cmake ../
make -j4

Note, the Xcode project files set their search paths for CMakeBuild/build, so
it is important to execute the mkdir and cd commands exactly as written above.
