cmake_minimum_required(VERSION 2.8.5)
project(Kiwi)

set(CMAKE_OSX_SYSROOT iphoneos4.3)
set(CMAKE_OSX_ARCHITECTURES "armv6;armv7;i386")
set(CMAKE_XCODE_EFFECTIVE_PLATFORMS "-iphoneos;-iphonesimulator")

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

find_package(VES REQUIRED)
include_directories(${VES_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(M_SRCS
  EAGLView.mm
  ES2Renderer.mm
  GLViewController.m
  InfoView.m
  LoadDataController.m
  TitleBarViewContainer.m
  kiwiAppDelegate.mm
  main.m
  )

set(HEADERS
  EAGLView.h
  ES2Renderer.h
  GLViewController.h
  InfoView.h
  Kiwi-Prefix.pch
  LoadDataController.h
  TitleBarViewContainer.h
  kiwiAppDelegate.h
  )

set(XIB_FILES
  InfoView.xib
  MainWindow.xib
  TitleBarViewContainer.xib
)

set(RESOURCES
  ${XIB_FILES}
  ../../../../src/shaders/BackgroundTexture.fsh
  ../../../../src/shaders/BackgroundTexture.vsh
  ../../../../src/shaders/Shader.fsh
  ../../../../src/shaders/Shader.vsh
  ../../../../src/shaders/BlinnPhong.fsh
  ../../../../src/shaders/BlinnPhong.vsh
  ../../../../src/shaders/ClipPlane.fsh
  ../../../../src/shaders/ClipPlane.vsh
  ../../../../src/shaders/ToonShader.fsh
  ../../../../src/shaders/ToonShader.vsh

  Images/Default-Landscape~ipad.png
  Images/Default-Portrait~ipad.png
  Images/Default.png
  Images/Default@2x.png
  Images/Kiwi_K.png
  Images/Kiwi_Splash_114.png
  Images/Kiwi_Splash_57.png
  Images/Kiwi_Splash_72.png
  Images/fullscreen_32.png
  Images/iTunesArtwork
  Images/icon.png
  Images/opendata_32.png

  Data/AppendedKneeData.vtp
  Data/Buckyball.vtp
  Data/MountStHelen.vtp
  Data/bunny.vtp
  Data/caffeine.pdb
  Data/current.stl
  Data/current.vtk
  Data/current_ascii.stl
  Data/head.vti
  Data/kiwi.png
  Data/motor.g
  Data/old_well.vtp
  Data/shuttle.vtp
  Data/teapot.vtp
  Data/visible-woman-hand.vtp
  )

add_executable(Kiwi MACOSX_BUNDLE ${M_SRCS} ${HEADERS} ${RESOURCES})

target_link_libraries(Kiwi
  ${VES_LIBRARIES}
  "-framework CoreGraphics"
  "-framework Foundation"
  "-framework UIKit"
  "-framework OpenGLES"
  "-framework QuartzCore"
  )

set_target_properties(Kiwi PROPERTIES
  MACOSX_BUNDLE_GUI_IDENTIFIER "com.kitware.Kiwi"
  MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Kiwi-Info-cmake.plist
  RESOURCE "${RESOURCES}"
  XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
  XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym"
  XCODE_ATTRIBUTE_GCC_PRECOMPILE_PREFIX_HEADER YES
  XCODE_ATTRIBUTE_GCC_PREFIX_HEADER ${CMAKE_CURRENT_LIST_DIR}/Kiwi-Prefix.pch
  XCODE_ATTRIBUTE_INFOPLIST_PREPROCESS YES
  XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 4.3
  )
