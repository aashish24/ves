/**
 * 
 */
package com.kitware.KiwiViewer;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager.LayoutParams;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.ListView;

/**
 *
 *
 */
public class DatasetListActivity extends Activity {

  public ListView mListView;
  
  @Override
  protected void onCreate(Bundle savedInstanceState) {
    
    super.onCreate(savedInstanceState);
    
    // Make us non-modal, so that others can receive touch events.
    getWindow().setFlags(LayoutParams.FLAG_NOT_TOUCH_MODAL, LayoutParams.FLAG_NOT_TOUCH_MODAL);

    // ...but notify us that it happened.
    getWindow().setFlags(LayoutParams.FLAG_WATCH_OUTSIDE_TOUCH, LayoutParams.FLAG_WATCH_OUTSIDE_TOUCH);
    
    this.setContentView(R.layout.datasetlistactivity);
    String[] datasets = getIntent().getExtras().getStringArray("com.kitware.KiwiViewer.bundle.DatasetList");
    ArrayAdapter<String> adapter = new ArrayAdapter<String>(getBaseContext(),android.R.layout.simple_list_item_1,datasets);
    mListView = (ListView) findViewById(R.id.listView);
    mListView.setAdapter(adapter);
    mListView.setOnItemClickListener(new OnItemClickListener() {

      @Override
      public void onItemClick(AdapterView<?> adapterView, View view, int pos,
          long arg3) {
        String value = (String) adapterView.getItemAtPosition(pos);
        Intent curIntent = new Intent();
        curIntent.putExtra("com.kitware.KiwiViewer.bundle.DatasetName",value);
        curIntent.putExtra("com.kitware.KiwiViewer.bundle.DatasetOffset",pos);
        setResult(Activity.RESULT_OK, curIntent);
        finish();
      }
    });

  }
  
  @Override
  public boolean onTouchEvent(MotionEvent event) {
    // If we've received a touch notification that the user has touched
    // outside the app, finish the activity.
    if (MotionEvent.ACTION_OUTSIDE == event.getAction()) {
      finish();
      return true;
    }

    // Delegate everything else to Activity.
    return super.onTouchEvent(event);
  }

}
