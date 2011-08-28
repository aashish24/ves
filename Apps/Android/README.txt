-- Getting started

The first several sections describe the steps I took to setup my OSX system
for Android development.  Everything I did was based on the instructions found
in the official Android documentation:

  http://developer.android.com/sdk/installing.html


--JDK

My version of OSX (10.6.8) had JDK 6 installed already.  You can open the 
'Java Preferences' app to see.  Just type Java Preferences into spotlight.


-- Ant

My version of OSX had ant installed already.

  $ ant -version
  Apache Ant(TM) version 1.8.2 compiled on June 3 2011


--Install Eclipse

Install Eclipse.  We'll only use Eclipse to run the ADT Plugin.  The ADT plugin
is used to download additional Android SDK components.

I downloaded Eclipse Classic 3.7 for OSX 64 bit from http://www.eclipse.org/downloads/
To run Eclipse, cd into the unzipped folder and run ./eclipse.


--Download Android SDK and ADT plugin for Eclipse

I downloaded android-sdk_r12-mac_x86.zip from http://developer.android.com/sdk/index.html

Next, I launched Eclipse, installed the ADT plugin, and downloaded additional
Android SDK components.  You should follow the instructions here:

  http://developer.android.com/sdk/installing.html

Using the Android SDK and AVD Manager provided by the ADT plugin,  I installed
'Android SDK Platform-tools' and 'SDK Platform Android 3.1, API 12'.
I selected 'SDK Platform Android 3.1, API 12' because that is the version of Android that
is running on my tablet.  Later, when building apps I can specify android-12 as
the platform target.  In order to specify other platform targets, I would have to
download them first.


--Download the NDK

I downloaded android-ndk-r6-darwin-x86.tar.bz2 from http://developer.android.com/sdk/ndk/index.html


--Compiling code with toolchains and ant

This page gives a great overview of building Android apps from the command line:

  http://developer.android.com/guide/developing/building/building-cmdline.html

The only difference is that we will use cmake instead of the ndk-build script that
comes with the Android NDK.  Make sure you have CMake version 2.8.4 or higher.

The main tools used will be 'android' and 'adb'.  These can be found in the Android SDK:

  /path/to/android/android-sdk/tools/android
  /path/to/android/android-sdk/platform-tools/adb


--Building VES

Execute the following commands:

export ANDROID_NDK=/path/to/android/android-ndk-r6

cd /path/to/ves/Apps/Android/CMakeBuild
mkdir build
cd build
cmake ../
make -j4


--Build PointCloud demo

Execute the following commands:

export ANDROID_NDK=/path/to/android/android-ndk-r6

# build native code
cd /path/to/ves/Apps/Android/PointCloud
mkdir build
cd build
cmake -DANDROID_LEVEL=9 -DCMAKE_TOOLCHAIN_FILE=../../CMakeBuild/cmake/android.toolchain.cmake -DVTK_DIR=../../CMakeBuild/build/CMakeExternals/Build/vtkmodular-android ../
make

# copy library
cd ..
mkdir -p libs/armeabi
cp build/jni/libnative-activity.so libs/armeabi/

# create ant build files
# you can run 'android list targets' to find out the available targets

android update project --name PointCloud --path . --target android-12

# build app
ant debug


--Running on your device

Make sure that adb can see your device.  Run 'adb devices' and you should see
your device listed.

I struggled at this step, my device would not show up.  Finally I discovered
that the problem was an app called EasyTether that I had installed on my phone
which included an installed component on my macbook.  The macbook component prevents
adb from working.  See this blog to disable it:

  http://www.intohand.com/blog/?p=116

Execute the following commands:

# install to device
adb -d install -r bin/PointCloud-debug.apk

# launch
adb shell am start -a android.intent.action.MAIN -n com.example.native_activity/android.app.NativeActivity


