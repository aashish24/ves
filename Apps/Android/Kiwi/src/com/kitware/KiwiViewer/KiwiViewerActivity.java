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
import android.content.Intent;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ListView;
import android.widget.Toast;

public class KiwiViewerActivity extends Activity {

    public static final String TAG = "KiwiViewerActivity";
  
    protected KiwiGLSurfaceView mView;

    protected Button  mLoadButton;
    protected Button  mResetViewButton;
    
    protected ListView mDatasetList;

    private static AssetManager assetManager;
    
    public static final int DATASETTABLE_REQUEST_CODE = 1;
    
    /**
     * @return a string array of available datasets
     */
    protected String[] createDatasetArray() {
      int numberOfDatasets = KiwiNative.getNumberOfBuiltinDatasets();
      String[] ret = new String[numberOfDatasets];
      for(int i = 0; i < numberOfDatasets; ++i) {
        ret[i] = KiwiNative.getDatasetName(i);
      }
      return ret;
    }

    @Override 
    protected void onCreate(Bundle icicle) {
      
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

              Intent datasetTableIntent =
                new Intent("com.kitware.KiwiViewer.action.DatasetList",
                           getIntent().getData());
              String[] datasets = createDatasetArray();
              datasetTableIntent.putExtra("com.kitware.KiwiViewer.bundle.DatasetList", datasets);
              startActivityForResult(datasetTableIntent, DATASETTABLE_REQUEST_CODE);
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
    
    /**
     * Get results from the dataset dialog
     */
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
      Bundle curBundle = null;
      Log.d(TAG,"Test");
      if (data != null) {
        curBundle = data.getExtras();
      }
      if (requestCode == DATASETTABLE_REQUEST_CODE && curBundle != null
          && curBundle.containsKey("com.kitware.KiwiViewer.bundle.DatasetName")) {
          
        String name = curBundle.getString("com.kitware.KiwiViewer.bundle.DatasetName");
        int offset = curBundle.getInt("com.kitware.KiwiViewer.bundle.DatasetOffset");
        Toast.makeText(getBaseContext(), "Loading " + name + "...", Toast.LENGTH_SHORT).show();
        KiwiNative.loadDatasetWithOffset(offset);
        mView.resetCamera();
      }

      super.onActivityResult(requestCode, resultCode, data);
    }
}
