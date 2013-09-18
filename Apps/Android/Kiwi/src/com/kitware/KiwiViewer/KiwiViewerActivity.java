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
import java.util.ArrayList;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.res.Configuration;
import android.content.DialogInterface;
import android.content.DialogInterface.OnDismissListener;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.os.Build;
import android.os.Environment;
import android.os.AsyncTask;
import android.util.Log;
import android.text.InputType;
import android.text.SpannableString;
import android.text.util.Linkify;
import android.text.method.LinkMovementMethod;
import android.net.Uri;
import android.view.View;
import android.view.LayoutInflater;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ListView;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;
import android.app.ProgressDialog;

public class KiwiViewerActivity extends Activity {

    public static final String TAG = "KiwiViewerActivity";

    protected KiwiGLSurfaceView mView;

    protected ImageButton  mLoadButton;
    protected ImageButton  mInfoButton;
    protected ImageButton  mResetViewButton;

    protected ArrayList<String> mBuiltinDatasetNames;

    protected String fileToOpen;
    protected String urlToOpen;
    protected int datasetToOpen = -1;

    protected ProgressDialog mProgressDialog = null;

    public static final int DATASETTABLE_REQUEST_CODE = 1;


    protected void showProgressDialog() {
      showProgressDialog("Opening data...");
    }

    protected void showProgressDialog(String message) {
      mProgressDialog = new ProgressDialog(this);
      mProgressDialog.setIndeterminate(true);
      mProgressDialog.setCancelable(false);
      mProgressDialog.setMessage(message);
      mProgressDialog.show();
    }


    public void dismissProgressDialog() {
      if (mProgressDialog != null) {
        mProgressDialog.dismiss();
      }
    }


    public void showErrorDialog(String title, String message) {

      AlertDialog dialog = new AlertDialog.Builder(this).create();
      dialog.setIcon(R.drawable.alert_dialog_icon);
      dialog.setTitle(title);
      dialog.setMessage(message);
      dialog.setButton("Ok",  new DialogInterface.OnClickListener() {
        public void onClick(DialogInterface dialog, int which) {
        return;
        }});
      dialog.show();
    }


    public void showWelcomeDialog() {

      String title = getString(R.string.welcome_title);
      String message = getString(R.string.welcome_message);

      final SpannableString s = new SpannableString(message);
      Linkify.addLinks(s, Linkify.WEB_URLS);

      AlertDialog dialog = new AlertDialog.Builder(this).create();
      dialog.setTitle(title);
      dialog.setIcon(R.drawable.kiwi_small_icon);
      dialog.setMessage(s);
      dialog.setButton("Ok",  new DialogInterface.OnClickListener() {
        public void onClick(DialogInterface dialog, int which) {
        return;
        }});

      dialog.setOnDismissListener(new OnDismissListener() {
        @Override
        public void onDismiss(final DialogInterface iface) {
           maybeLoadDefaultDataset();
        }});

      dialog.show();

      ((TextView)dialog.findViewById(android.R.id.message)).setMovementMethod(LinkMovementMethod.getInstance());
    }


    public void showBrainAtlasDialog() {

      String title = getString(R.string.brainatlas_title);
      String message = getString(R.string.brainatlas_message);

      final SpannableString s = new SpannableString(message);
      Linkify.addLinks(s, Linkify.WEB_URLS);

      AlertDialog dialog = new AlertDialog.Builder(this).create();
      dialog.setIcon(R.drawable.info_icon);
      dialog.setTitle(title);
      dialog.setMessage(s);
      dialog.setButton("Ok",  new DialogInterface.OnClickListener() {
        public void onClick(DialogInterface dialog, int which) {
        return;
        }});

      dialog.show();

      ((TextView)dialog.findViewById(android.R.id.message)).setMovementMethod(LinkMovementMethod.getInstance());

    }

    public void showCanDialog() {

      String title = getString(R.string.can_title);
      String message = getString(R.string.can_message);

      AlertDialog dialog = new AlertDialog.Builder(this).create();
      dialog.setIcon(R.drawable.info_icon);
      dialog.setTitle(title);
      dialog.setMessage(message);
      dialog.setButton("Ok",  new DialogInterface.OnClickListener() {
        public void onClick(DialogInterface dialog, int which) {
        return;
        }});

      dialog.show();
    }

    public void showHeadImageDialog() {

      String title = getString(R.string.head_image_title);
      String message = getString(R.string.head_image_message);

      AlertDialog dialog = new AlertDialog.Builder(this).create();
      dialog.setIcon(R.drawable.info_icon);
      dialog.setTitle(title);
      dialog.setMessage(message);
      dialog.setButton("Ok",  new DialogInterface.OnClickListener() {
        public void onClick(DialogInterface dialog, int which) {
        return;
        }});

      dialog.show();
    }

    public void showCannotOpenAssetDialog() {

      String title = getString(R.string.cannot_open_asset_title);
      String message = getString(R.string.cannot_open_asset_message);

      AlertDialog dialog = new AlertDialog.Builder(this).create();
      dialog.setIcon(R.drawable.alert_dialog_icon);
      dialog.setTitle(title);
      dialog.setMessage(message);
      dialog.setButton(AlertDialog.BUTTON_NEUTRAL, "Ok",  new DialogInterface.OnClickListener() {
        public void onClick(DialogInterface dialog, int which) {
        return;
        }});

      dialog.setButton(AlertDialog.BUTTON_POSITIVE, "Open in Browser",  new DialogInterface.OnClickListener() {
        public void onClick(DialogInterface dialog, int which) {
          openUrlInBrowser(getString(R.string.external_data_url));
        }});

      dialog.show();
    }

    public void showDownloadFileDialog(final String startUrl) {


      final EditText urlInput = new EditText(this);
      urlInput.setText(startUrl);
      urlInput.setSingleLine();
      urlInput.setInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_URI);

      AlertDialog dialog = new AlertDialog.Builder(this).create();
      dialog.setTitle(getString(R.string.download_file_text));
      dialog.setView(urlInput);
      dialog.setButton(AlertDialog.BUTTON_NEGATIVE, "Cancel",  new DialogInterface.OnClickListener() {
        public void onClick(DialogInterface dialog, int which) {
        return;
        }});

      dialog.setButton(AlertDialog.BUTTON_POSITIVE, "Download",  new DialogInterface.OnClickListener() {
        public void onClick(DialogInterface dialog, int which) {
            String url = urlInput.getText().toString();
            if (!url.startsWith("http://")) {
              showErrorDialog("Unhandled URL", "The URL must begin with http://");
            }
            else {
              downloadAndOpenFile(url);
            }
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
          fileToOpen = uri.getPath();
        }
        else if (uri.getScheme().equals("http")) {
          urlToOpen = uri.toString();
        }
      }
    }


    @Override protected void onNewIntent(Intent intent) {
      super.onNewIntent(intent);
      handleUriFromIntent(intent.getData());
    }



    protected void initBuiltinDatasetNames() {

      if (mBuiltinDatasetNames == null) {
          mBuiltinDatasetNames = new ArrayList<String>();
          mBuiltinDatasetNames.add("teapot.vtp");
          mBuiltinDatasetNames.add("bunny.vtp");
          mBuiltinDatasetNames.add("visible-woman-hand.vtp");
          mBuiltinDatasetNames.add("AppendedKneeData.vtp");
          mBuiltinDatasetNames.add("cturtle.vtp");
          mBuiltinDatasetNames.add("MountStHelen.vtp");
          mBuiltinDatasetNames.add("shuttle.vtp");
          //http://visibleearth.nasa.gov/view.php?id=57730
          mBuiltinDatasetNames.add("nasa-blue-marble.kiwi");
          mBuiltinDatasetNames.add("Buckyball.vtp");
          mBuiltinDatasetNames.add("caffeine.pdb");
          mBuiltinDatasetNames.add("head.vti");
          mBuiltinDatasetNames.add("kiwi.png");

          mBuiltinDatasetNames.add(getString(R.string.pvremote_text));
          mBuiltinDatasetNames.add(getString(R.string.pvweb_text));
          mBuiltinDatasetNames.add(getString(R.string.pointcloudstreaming_text));
          mBuiltinDatasetNames.add(getString(R.string.download_file_text));
      }
    }

    void maybeLoadDefaultDataset() {

      if (getIntent().getData() == null) {
        String storageDir = getExternalFilesDir(null).getAbsolutePath();
        mView.postLoadDefaultDataset(this, storageDir);
      }
      else {
        KiwiNative.clearExistingDataset();
      }
    }


    @Override
    public void onConfigurationChanged(Configuration newConfig) {
      super.onConfigurationChanged(newConfig);
      mView.stopRendering();
    }


    @Override
    protected void onCreate(Bundle bundle) {
      super.onCreate(bundle);

      initBuiltinDatasetNames();

      handleUriFromIntent(getIntent().getData());

      this.setContentView(R.layout.kiwivieweractivity);

      mView = (KiwiGLSurfaceView) this.findViewById(R.id.glSurfaceView);


      SharedPreferences prefs = getPreferences(MODE_PRIVATE);
      String versionStr = getString(R.string.version_string);

      if (!versionStr.equals(prefs.getString("version_string", ""))) {
        prefs.edit().putString("version_string", versionStr).commit();
        showWelcomeDialog();
      }
      else {
        maybeLoadDefaultDataset();
      }


      mLoadButton = (ImageButton) this.findViewById(R.id.loadDataButton);
      mInfoButton = (ImageButton) this.findViewById(R.id.infoButton);
      mResetViewButton = (ImageButton) this.findViewById(R.id.resetButton);


      mLoadButton.setOnClickListener(new Button.OnClickListener() {
          public void onClick(View v) {
              Intent datasetTableIntent = new Intent();
              datasetTableIntent.setClass(KiwiViewerActivity.this, DatasetListActivity.class);
              datasetTableIntent.putExtra("com.kitware.KiwiViewer.bundle.DatasetList", mBuiltinDatasetNames);
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
              mView.resetCamera(true);
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


    private class BuiltinDataLoader extends AsyncTask<String, Integer, String> {

      public int mBuiltinDatasetIndex;

      BuiltinDataLoader(int builtinDatasetIndex) {
        mBuiltinDatasetIndex = builtinDatasetIndex;
      }

      protected String doInBackground(String... filename) {

        //if (filename[0].equals("textured_sphere.vtp")) {
        //  copyEarthAssets();
        //}

        return copyAssetFileToStorage(filename[0]);
      }

      protected void onPreExecute() {
        showProgressDialog();
      }

      protected void onPostExecute(String filename) {
        mView.loadDataset(filename, -1, KiwiViewerActivity.this);
      }
    }


    public void doPointCloudStreaming() {

      LayoutInflater factory = LayoutInflater.from(this);
      final View pvremoteDialog = factory.inflate(R.layout.pointcloudstreaming_dialog, null);
      new AlertDialog.Builder(KiwiViewerActivity.this)
          //.setIcon(R.drawable.paraview_logo)
          .setTitle("Connect to streaming server:")
          .setView(pvremoteDialog)
          .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
              public void onClick(DialogInterface dialog, int whichButton) {
                EditText hostEdit = (EditText) pvremoteDialog.findViewById(R.id.host_edit);
                EditText sessionIdEdit = (EditText) pvremoteDialog.findViewById(R.id.sessionid_edit);

                String host = hostEdit.getText().toString();
                int port =  Integer.parseInt(sessionIdEdit.getText().toString());

                showProgressDialog("Connecting to server...");
                mView.doPointCloudStreaming(host, port, KiwiViewerActivity.this);
              }
          })
          .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
              public void onClick(DialogInterface dialog, int whichButton) {

              }
          })
          .show();


    }

    public void doPVWeb() {

      LayoutInflater factory = LayoutInflater.from(this);
      final View pvwebDialog = factory.inflate(R.layout.pvweb_dialog, null);
      new AlertDialog.Builder(KiwiViewerActivity.this)
          .setIcon(R.drawable.paraview_logo)
          .setTitle("Join a ParaView Web session:")
          .setView(pvwebDialog)
          .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
              public void onClick(DialogInterface dialog, int whichButton) {
                EditText hostEdit = (EditText) pvwebDialog.findViewById(R.id.host_edit);
                EditText sessionIdEdit = (EditText) pvwebDialog.findViewById(R.id.sessionid_edit);

                String host = hostEdit.getText().toString();
                String sessionId = sessionIdEdit.getText().toString();

                showProgressDialog("Contacting ParaView Web...");
                mView.doPVWeb(host, sessionId, KiwiViewerActivity.this);
              }
          })
          .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
              public void onClick(DialogInterface dialog, int whichButton) {

              }
          })
          .show();

    }

    public void doPVRemote() {

      LayoutInflater factory = LayoutInflater.from(this);
      final View pvremoteDialog = factory.inflate(R.layout.pvremote_dialog, null);
      new AlertDialog.Builder(KiwiViewerActivity.this)
          .setIcon(R.drawable.paraview_logo)
          .setTitle("Setup ParaView Remote:")
          .setView(pvremoteDialog)
          .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
              public void onClick(DialogInterface dialog, int whichButton) {
                EditText hostEdit = (EditText) pvremoteDialog.findViewById(R.id.host_edit);
                EditText sessionIdEdit = (EditText) pvremoteDialog.findViewById(R.id.sessionid_edit);

                String host = hostEdit.getText().toString();
                int port =  Integer.parseInt(sessionIdEdit.getText().toString());

                showProgressDialog("Connecting to ParaView...");
                mView.doPVRemote(host, port, KiwiViewerActivity.this);
              }
          })
          .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
              public void onClick(DialogInterface dialog, int whichButton) {

              }
          })
          .show();

    }

    public void loadDataset(int builtinDatasetIndex) {

      //String filename = KiwiNative.getDatasetFilename(builtinDatasetIndex);
      String filename = mBuiltinDatasetNames.get(builtinDatasetIndex);

      if (filename.equals("pvweb")) {
        doPVWeb();
        return;
      }

      // don't attempt to open large asset files on android api 8
      int sdkVersion = Build.VERSION.SDK_INT;
      if (sdkVersion <= 8
          && (filename.equals("visible-woman-hand.vtp")
              || filename.equals("AppendedKneeData.vtp")
              || filename.equals("cturtle.vtp")
              || filename.equals("model_info.txt"))) {
        showCannotOpenAssetDialog();
        return;
      }

      Log.w(TAG, String.format("execute: %s %d", filename, builtinDatasetIndex));
      new BuiltinDataLoader(builtinDatasetIndex).execute(filename);
    }


    public void loadDataset(String filename) {
      showProgressDialog();
      mView.loadDataset(filename, KiwiViewerActivity.this);
    }

    public void downloadAndOpenFile(String url) {

      String downloadDir = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS).getAbsolutePath();

      Log.i(TAG, String.format("have download dir: %s", downloadDir));
      Log.i(TAG, String.format("have url: %s", url));

      showProgressDialog("Downloading file...");
      mView.downloadAndOpenFile(url, downloadDir, KiwiViewerActivity.this);
    }

    public void postLoadDataset(String filename, boolean result, String errorTitle, String errorMessage) {
      dismissProgressDialog();
      if (!result) {
        showErrorDialog(errorTitle, errorMessage);
      }
      else {
        if (filename.endsWith("model_info.txt")) {
          showBrainAtlasDialog();
        }
        else if (filename.endsWith("can0000.vtp")) {
          showCanDialog();
        }
        else if (filename.endsWith("head.vti")) {
          showHeadImageDialog();
        }
      }
    }


    @Override protected void onPause() {
        super.onPause();
        mView.onPause();
    }

    @Override protected void onResume() {
        super.onResume();
        mView.onResume();

        if (fileToOpen != null) {
          loadDataset(fileToOpen);
          fileToOpen = null;
        }
        if (urlToOpen != null) {
          showDownloadFileDialog(urlToOpen);
          urlToOpen = null;
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

        if (mBuiltinDatasetNames.get(offset).equals(getString(R.string.midas_text))) {
          openUrlInBrowser(getString(R.string.external_data_url));
        }
        else if (mBuiltinDatasetNames.get(offset).equals(getString(R.string.download_file_text))) {
          showDownloadFileDialog(new String());
        }
        else if (mBuiltinDatasetNames.get(offset).equals(getString(R.string.pvremote_text))) {
          doPVRemote();
        }
        else if (mBuiltinDatasetNames.get(offset).equals(getString(R.string.pvweb_text))) {
          doPVWeb();
        }
        else if (mBuiltinDatasetNames.get(offset).equals(getString(R.string.pointcloudstreaming_text))) {
          doPointCloudStreaming();
        }
        else {
          datasetToOpen = offset;
        }
      }

      super.onActivityResult(requestCode, resultCode, data);
    }


  protected void copyEarthAssets() {
    copyAssetFileToStorage("earth.jpg");
  }

}
