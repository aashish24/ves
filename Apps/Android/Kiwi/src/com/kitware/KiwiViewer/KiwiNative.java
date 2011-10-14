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

package com.kitware.KiwiViewer;

import android.content.res.AssetManager;

public class KiwiNative {

     static {
         System.loadLibrary("KiwiNative");
     }

     public static native void init(int width, int height);
     public static native void reshape(int width, int height);
     public static native void handleSingleTouchPanGesture(float dx, float dy);
     public static native void handleTwoTouchPanGesture(float x0, float y0, float x1, float y1);
     public static native void handleTwoTouchPinchGesture(float scale);
     public static native void handleTwoTouchRotationGesture(float rotation);
     public static native void handleSingleTouchUp();
     public static native void handleSingleTouchDown(float x, float y);
     public static native void render();
     public static native void resetCamera();
     public static native void loadNextDataset();
     public static native void loadAssets(AssetManager assetManager, String filename);
}
