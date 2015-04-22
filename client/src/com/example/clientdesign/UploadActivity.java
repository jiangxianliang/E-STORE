package com.example.clientdesign;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;

import com.example.andsifttest1.SiftActivity;
import com.example.logic.ImgFileListActivity;
import com.example.socket.uploadFeatures;
import com.example.logic.ImgFileListActivity;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.database.Cursor;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.ListView;


public class UploadActivity extends Activity{
	private Button submitBtn, selectBtn;
	private EditText myEditText;
	ImageView imageView;
	String picturePath;
	int xh_count = 0;
 	ListView listView;
	ArrayList<String> listfile=new ArrayList<String>();
	ArrayList<String> listfile_sub=new ArrayList<String>();
	ArrayList<String> uploadImageBuf = new ArrayList<String>();
	
	// 声明进度条对话框
	ProgressDialog xh_pDialog;
	 private BroadcastReceiver mBatteryInfoReceiver = new BroadcastReceiver() {
         @Override
         public void onReceive(Context context, Intent intent) {
                 String action = intent.getAction();
                 if (Intent.ACTION_BATTERY_CHANGED.equals(action)) {

                         int level = intent.getIntExtra("level", 0);
                         int scale = intent.getIntExtra("scale", 100);
                         int battery = level * 100 / scale;
                         try {
                 			FileWriter fw = new FileWriter("/sdcard/battery.txt");
                 			BufferedWriter bw = new BufferedWriter(fw);  
                 				bw.write(battery);
                 			bw.close();
                       	    fw.close();
                 		} catch (IOException e) {
                 			// TODO Auto-generated catch block
                 			e.printStackTrace();
                 		}
                        // tvBatteryLevel.setText(String.valueOf(level * 100 / scale));
                 }
         }
 };
 
 @Override
 public void onResume() {
         super.onResume();

         registerReceiver(mBatteryInfoReceiver, new IntentFilter(
                         Intent.ACTION_BATTERY_CHANGED));
 }

 @Override
 public void onPause() {
         super.onPause();

         unregisterReceiver(mBatteryInfoReceiver);                
 }

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.upload);
		//Intent intent = getIntent();
		//String action = intent.getAction();
		
		//myEditText = (EditText)findViewById(R.id.ulPicPathEditText);
		submitBtn = (Button)findViewById(R.id.ulSubmitPicButton);
		selectBtn = (Button)findViewById(R.id.ulSelectButton);
		
		listView=(ListView) findViewById(R.id.listView1);
		String list_trans;
		Bundle bundle= getIntent().getExtras();
		
		if (bundle!=null) {
			if (bundle.getStringArrayList("files")!=null) {
				listfile= bundle.getStringArrayList("files");
				int list_i;
				for(list_i = 0; list_i < listfile.size(); list_i ++)
				{
					list_trans = listfile.get(list_i);
					list_trans = list_trans.substring(list_trans.lastIndexOf("/")+1);
					listfile_sub.add(list_trans);
					
				}
					//String a = listfile.get(list_i).substring(listfile.get(list_i).lastIndexOf("/")+1);
				listView.setVisibility(View.VISIBLE);
				ArrayAdapter<String> arryAdapter=new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, listfile_sub);
				listView.setAdapter(arryAdapter);
			}
		}
		
		

		selectBtn.setOnClickListener(new OnClickListener(){
			public void onClick(View v){
				//Intent intent = new Intent(Intent.ACTION_PICK,android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
				//startActivityForResult(intent, 1);
				Intent intent = new Intent();
				intent.setClass(UploadActivity.this,ImgFileListActivity.class);
				finish();
				startActivity(intent);
				
			}
		});
		submitBtn.setOnClickListener(new OnClickListener(){
			public void onClick(View v){
				SiftActivity sift = new SiftActivity();
				sift.SIFTList(listfile);
				uploadFeatures upFeature = new uploadFeatures();
				Log.v("0", Integer.toString(0));
				
				int[] imageNumBuf;
				int uploadTag = 0;
				if((imageNumBuf = upFeature.uploadFeat(listfile)) == null)
					uploadTag = -1;
				else
				{
					//imageNumBuf = upFeature.uploadFeat(listfile);
					uploadTag = imageNumBuf.length;
					for(int buf_i = 0; buf_i < uploadTag; buf_i ++)
					{
						uploadImageBuf.add(listfile.get(imageNumBuf[buf_i]));
					}
				}
					
				Log.v("uploadTag################", Integer.toString(uploadTag));
				
				if(uploadTag > 0){
		        	 AlertDialog.Builder dialog=new AlertDialog.Builder(UploadActivity.this);
					    dialog.setTitle("上传")
					    .setIcon(android.R.drawable.ic_dialog_info)
					    .setMessage("需要上传"+uploadTag+"张照片！")
					    .setPositiveButton("上传", new DialogInterface.OnClickListener() {		
						@Override
						public void onClick(DialogInterface dialog, int which) {
							//转跳到另外一个Activity
							// TODO Auto-generated method stub
							Intent intent=new Intent(UploadActivity.this,UploadPicActivity.class);
							finish();
							intent.putStringArrayListExtra("age",uploadImageBuf);
							startActivity(intent);

						}
					}).setNegativeButton("取消", new DialogInterface.OnClickListener() {
						
						
						public void onClick(DialogInterface dialog, int which) {
							// TODO Auto-generated method stub
							dialog.cancel();//取消弹出框
						}
					}).create().show();
					   
		         }else if(uploadTag == 0){
		        	 AlertDialog.Builder dialog=new AlertDialog.Builder(UploadActivity.this);
						    dialog.setTitle("上传")
						    .setIcon(android.R.drawable.ic_dialog_info)
						    .setMessage("有相同图片，无需上传！")
						    .setPositiveButton("确定", new DialogInterface.OnClickListener() {		
							@Override
							public void onClick(DialogInterface dialog, int which) {
								// TODO Auto-generated method stub
								Intent intent=new Intent(UploadActivity.this,MainActivity.class);
								startActivity(intent);
								finish();
							}
						}).create().show();
		        	 
		         }else{
		        	 AlertDialog.Builder dialog=new AlertDialog.Builder(UploadActivity.this);
		        	 dialog.setTitle("上传失败")
					    .setIcon(android.R.drawable.ic_dialog_info)
					    .setMessage("上传失败，请检查您的网络连接情况！")
					    .setPositiveButton("确定", new DialogInterface.OnClickListener() {		
						@Override
						public void onClick(DialogInterface dialog, int which) {
							// TODO Auto-generated method stub
							dialog.cancel();
						}
					}).create().show();
		         }
			}
		});
	}
	
	
	
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data){
		if(requestCode == 1){
			Uri selectedImage = data.getData();
        	String[] filePathColumn = { MediaStore.Images.Media.DATA };
        	Cursor cursor = getContentResolver().query(selectedImage,filePathColumn, null, null, null);
        	cursor.moveToFirst();
        	int columnIndex = cursor.getColumnIndex(filePathColumn[0]);
        	picturePath = cursor.getString(columnIndex);
        	cursor.close();
			 //path = data.getStringExtra("filePath");
			myEditText.setText(picturePath);
			imageView.setImageBitmap(BitmapFactory.decodeFile(picturePath));

		}
	}
	
	 public boolean onKeyDown(int keyCode, KeyEvent event)  
	    {  
	        if (keyCode == KeyEvent.KEYCODE_BACK )  
	        {   
	        	finish();
				Intent intent=new Intent(UploadActivity.this,MainActivity.class);
				startActivity(intent);
	        }  
	          
	        return false;  
	          
	    }

}
