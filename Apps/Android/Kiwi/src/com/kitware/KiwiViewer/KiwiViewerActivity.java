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

import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.WindowManager;

import android.content.res.AssetManager;

import android.view.View;
import android.view.Gravity;
import android.view.ViewGroup.LayoutParams;
import android.widget.LinearLayout;
import android.widget.Button;

public class KiwiViewerActivity extends Activity {

    KiwiGLSurfaceView mView;


    LinearLayout mRootLayout;
    LinearLayout mButtonLayout;

    Button  mLoadButton;
    Button  mResetViewButton;

    static AssetManager assetManager;

    @Override protected void onCreate(Bundle icicle) {
      super.onCreate(icicle);


      mView = new KiwiGLSurfaceView(getApplication());

      assetManager = getAssets();
      String storageDir = Environment.getExternalStorageDirectory().getAbsolutePath();
      KiwiNative.loadAssets(assetManager, storageDir);

      mRootLayout = new LinearLayout(this);
      mRootLayout.setOrientation(LinearLayout.VERTICAL);
      mRootLayout.setLayoutParams(new LayoutParams(
                              LayoutParams.MATCH_PARENT,
                              LayoutParams.MATCH_PARENT));


      mButtonLayout = new LinearLayout(this);
      mButtonLayout.setOrientation(LinearLayout.HORIZONTAL);
      mButtonLayout.setLayoutParams(new LayoutParams(
                              LayoutParams.MATCH_PARENT,
                              LayoutParams.WRAP_CONTENT));

      mLoadButton = new Button(this);
      mLoadButton.setText("Load Data");

      mResetViewButton = new Button(this);
      mResetViewButton.setText("Reset View");

      mLoadButton.setOnClickListener(new Button.OnClickListener() {
          public void onClick(View v) {
              // todo- maybe check storage state first?
              // Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)
              mView.loadNextDataset();
          }
      });

      mResetViewButton.setOnClickListener(new Button.OnClickListener() {
          public void onClick(View v) {
              mView.resetCamera();
          }
      });

      mButtonLayout.addView(mResetViewButton);
      mButtonLayout.addView(mLoadButton);
      mButtonLayout.setGravity(Gravity.RIGHT);

      this.addContentView(mView, new LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));

      LinearLayout ll = new LinearLayout(this);
      ll.addView(mButtonLayout);
      ll.setGravity(Gravity.RIGHT | Gravity.BOTTOM);
      this.addContentView(ll, new LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));

    }

    @Override protected void onPause() {
        super.onPause();
        mView.onPause();
    }

    @Override protected void onResume() {
        super.onResume();
        mView.onResume();
    }
}
