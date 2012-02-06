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

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.IOException;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.os.Environment;
import android.os.AsyncTask;
import android.util.Log;
import android.net.Uri;
import android.view.View;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ListView;
import android.widget.Toast;
import android.app.ProgressDialog;

public class KiwiViewerActivity extends Activity {

    public static final String TAG = "KiwiViewerActivity";

    protected KiwiGLSurfaceView mView;

    protected ImageButton  mLoadButton;
    protected ImageButton  mInfoButton;
    protected ImageButton  mResetViewButton;

    protected ListView mDatasetList;

    protected String fileToOpen;
    protected int datasetToOpen = -1;

    protected ProgressDialog mProgressDialog;

    public static final int DATASETTABLE_REQUEST_CODE = 1;


    protected void showProgressDialog() {
      mProgressDialog = new ProgressDialog(this);
      mProgressDialog.setMessage("Opening data...");
      mProgressDialog.setIndeterminate(true);
      mProgressDialog.setCancelable(false);
      mProgressDialog.show();
    }


    public void dismissProgressDialog() {
      mProgressDialog.dismiss();
    }


    public void showErrorDialog(String title, String message) {

      AlertDialog dialog = new AlertDialog.Builder(this).create();
      dialog.setIconAttribute(android.R.attr.alertDialogIcon);
      dialog.setTitle(title);
      dialog.setMessage(message);
      dialog.setButton("Ok",  new DialogInterface.OnClickListener() {
        public void onClick(DialogInterface dialog, int which) {
        return;
        }});
      dialog.show();
    }


    protected void openUrlInBrowser(String url) {
        Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
        startActivity(intent);
    }


    protected void handleUriFromIntent(Uri uri) {
      if (uri != null) {
        if (uri.getScheme().equals("file")) {
          String filePath = uri.getPath();
          fileToOpen = filePath;
        }
      }
    }


    @Override protected void onNewIntent(Intent intent) {
      super.onNewIntent(intent);
      handleUriFromIntent(intent.getData());
    }


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
    protected void onCreate(Bundle bundle) {
      super.onCreate(bundle);

      handleUriFromIntent(getIntent().getData());

      this.setContentView(R.layout.kiwivieweractivity);

      mView = (KiwiGLSurfaceView) this.findViewById(R.id.glSurfaceView);

      if (getIntent().getData() == null) {
        String storageDir = getExternalFilesDir(null).getAbsolutePath();
        mView.postLoadDefaultDataset(this, storageDir);
      }
      else {
        KiwiNative.clearExistingDataset();
      }

      mLoadButton = (ImageButton) this.findViewById(R.id.loadDataButton);
      mInfoButton = (ImageButton) this.findViewById(R.id.infoButton);
      mResetViewButton = (ImageButton) this.findViewById(R.id.resetButton);


      mLoadButton.setOnClickListener(new Button.OnClickListener() {
          public void onClick(View v) {
              Intent datasetTableIntent = new Intent();
              datasetTableIntent.setClass(KiwiViewerActivity.this, DatasetListActivity.class);
              String[] datasets = createDatasetArray();
              datasetTableIntent.putExtra("com.kitware.KiwiViewer.bundle.DatasetList", datasets);
              startActivityForResult(datasetTableIntent, DATASETTABLE_REQUEST_CODE);
          }
      });

      mInfoButton.setOnClickListener(new Button.OnClickListener() {
          public void onClick(View v) {

              Intent infoIntent = new Intent();
              infoIntent.setClass(KiwiViewerActivity.this, InfoActivity.class);
              startActivity(infoIntent);
          }
      });

      mResetViewButton.setOnClickListener(new Button.OnClickListener() {
          public void onClick(View v) {
              mView.resetCamera();
          }
      });

    }


    private void copyFile(InputStream in, OutputStream out) throws IOException {
        byte[] buffer = new byte[1024];
        int read;
        while((read = in.read(buffer)) != -1) {
          out.write(buffer, 0, read);
        }
    }


    public String copyAssetFileToStorage(String filename) {

      // todo- check storage state first, show alert dialog in case of problem
      // Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)
      //MEDIA_MOUNTED_READ_ONLY

      String storageDir = getExternalFilesDir(null).getAbsolutePath();

      String destFilename = storageDir + "/" + filename;


      File destFile = new File(destFilename);
      if (destFile.exists()) {
        return destFilename;
      }

      InputStream in = null;
      OutputStream out = null;
      try {
        in = getAssets().open(filename);
        out = new FileOutputStream(storageDir + "/" + filename);
        copyFile(in, out);
        in.close();
        in = null;
        out.flush();
        out.close();
        out = null;
      }
      catch(Exception e) {
        Log.e(TAG, e.getMessage());
      }

      return destFilename;
    }


    private class AssetCopier extends AsyncTask<String, Integer, String> {

      protected String doInBackground(String... filename) {
        return copyAssetFileToStorage(filename[0]);
      }

      protected void onPreExecute() {
        showProgressDialog();
      }

      protected void onPostExecute(String filename) {
        mView.loadDatasetPath(filename, KiwiViewerActivity.this);
      }
    }


    public void loadDataset(int index) {
      String filename = KiwiNative.getDatasetFilename(index);
      new AssetCopier().execute(filename);
    }


    public void loadDatasetPath(String filename) {
      showProgressDialog();
      mView.loadDatasetPath(filename, KiwiViewerActivity.this);
    }


    @Override protected void onPause() {
        super.onPause();
        mView.onPause();
    }

    @Override protected void onResume() {
        super.onResume();
        mView.onResume();

        if (fileToOpen != null) {
          loadDatasetPath(fileToOpen);
          fileToOpen = null;
        }
        else if (datasetToOpen >= 0) {
          loadDataset(datasetToOpen);
          datasetToOpen = -1;
        }
    }

    /**
     * Get results from the dataset dialog
     */
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
      Bundle curBundle = null;

      if (data != null) {
        curBundle = data.getExtras();
      }
      if (requestCode == DATASETTABLE_REQUEST_CODE && curBundle != null
          && curBundle.containsKey("com.kitware.KiwiViewer.bundle.DatasetName")) {

        String name = curBundle.getString("com.kitware.KiwiViewer.bundle.DatasetName");
        int offset = curBundle.getInt("com.kitware.KiwiViewer.bundle.DatasetOffset");
        datasetToOpen = offset;
      }

      super.onActivityResult(requestCode, resultCode, data);
    }
}
