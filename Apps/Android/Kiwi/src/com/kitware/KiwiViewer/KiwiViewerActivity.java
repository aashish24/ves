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

import android.content.res.AssetManager;

import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.LinearLayout;
import android.widget.Button;
import android.widget.ListAdapter;
import android.widget.ListView;
import android.widget.RelativeLayout;

public class KiwiViewerActivity extends Activity {

    KiwiGLSurfaceView mView;

    LinearLayout mRootLayout;
    LinearLayout mButtonLayout;

    Button  mLoadButton;
    Button  mResetViewButton;
    
    ListView mDatasetList;

    static AssetManager assetManager;

    @Override protected void onCreate(Bundle icicle) {
      super.onCreate(icicle);

      this.setContentView(R.layout.kiwivieweractivity);
      
      mView = (KiwiGLSurfaceView) this.findViewById(R.id.glSurfaceView);

      assetManager = getAssets();
      String storageDir = Environment.getExternalStorageDirectory().getAbsolutePath();
      KiwiNative.loadAssets(assetManager, storageDir);

      mLoadButton = (Button) this.findViewById(R.id.loadButton);
      mResetViewButton = (Button) this.findViewById(R.id.resetButton);


      mLoadButton.setOnClickListener(new Button.OnClickListener() {
          public void onClick(View v) {
              // todo- maybe check storage state first?
              // Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)
              //mView.loadNextDataset();
              String[] test = {"Foo","Bar","Baz"};
              ListAdapter dataSetAdapter = new ArrayAdapter<String>(getBaseContext(),android.R.layout.simple_list_item_1,test);
              mDatasetList = new ListView(getBaseContext());
              mDatasetList.setAdapter(dataSetAdapter);
              RelativeLayout layout = (RelativeLayout) findViewById(R.id.RelativeLayout1);
              layout.addView(mDatasetList);
          }
      });

      mResetViewButton.setOnClickListener(new Button.OnClickListener() {
          public void onClick(View v) {
              mView.resetCamera();
          }
      });

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
