==Introduction-==

The Xcode projects in this directory require the kiwi and vtk frameworks.
You can use cmake to run a superbuild that will generate these frameworks.

==Build Instructions==

Step 1)

Create a build directory:

$ mkdir <build dir>

It is recommended that the build directory is not located inside the ves source
directory.

Step 2)

From the build directory, configure the superbuild with cmake:

$ cd <build dir>
$ cmake -DVES_IOS_SUPERBUILD=1 <ves src dir>

Step 3)

Compile the project:

$ make -j4

This may take some time.  The superbuild will checkout the subproject
dependencies and compile them for both iOS device and iOS simulator
architectures.  Then the ves and kiwi libraries will be compiled.

Step 4)

Create a symlink:

$ ln -s <build dir>/CMakeExternals/Install/frameworks ./frameworks

Create the symlink in this directory: <ves src dir>/Apps/iOS.
The symlink makes it easier for the Xcode projects to locate the frameworks
that you have just compiled.  If you choose not to create a symlink, you can
edit the .xcconfig file in the Xcode project directory to explicitly set the
path to the frameworks.  This makes it easy to relocate the Xcode projects.


Now you're ready to run the Xcode projects.  If you get stuck, ask for
help on the VES public mailing list:

    http://public.kitware.com/cgi-bin/mailman/listinfo/ves
