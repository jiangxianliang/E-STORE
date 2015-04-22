package com.example.clientdesign;


import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

import com.example.andsifttest1.SiftActivity;
import com.example.socket.ReceiveFive;

import android.app.Activity;
import android.app.AlertDialog;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.database.Cursor;
import android.graphics.BitmapFactory;

import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;


public class SearchActivity extends Activity{
	private Button submitBtn, selectBtn;
	private EditText myEditText;
	ImageView imageView;
	String picturePath;

	private TextView tvBatteryLevel;
    private int intBattery = 60;
    private BroadcastReceiver mBatteryInfoReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
            	
                    String action = intent.getAction();
                    if (Intent.ACTION_BATTERY_CHANGED.equals(action)) {

                            int level = intent.getIntExtra("level", 0);
                            int scale = intent.getIntExtra("scale", 100);
                            int battery = level * 100 / scale;
                            try {
                    			//FileWriter fw = new FileWriter("/sdcard/battery.txt");
                            	FileWriter fw = new FileWriter(getSDPath()+"/"+"battery.txt");
                            	BufferedWriter bw = new BufferedWriter(fw);  
                    				bw.write(battery);
                    			bw.close();
                          	    fw.close();
                    		} catch (IOException e) {
                    			// TODO Auto-generated catch block
                    			e.printStackTrace();
                    		}
                            tvBatteryLevel.setText(String.valueOf(level * 100 / scale));
                    }
            }
    };
    public String getSDPath(){ 
	       File sdDir = null; 
	       boolean sdCardExist = Environment.getExternalStorageState()   
	                           .equals(android.os.Environment.MEDIA_MOUNTED);   //判断sd卡是否存在 


	       if   (sdCardExist)      //如果SD卡存在，则获取跟目录
	       {                               
	         sdDir = Environment.getExternalStorageDirectory();//获取跟目录 
	      }   
	       return sdDir.toString(); 
	       
	}
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
    
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.search);
//		Intent intent = getIntent();
//		String action = intent.getAction();
		
		myEditText = (EditText)findViewById(R.id.picPathEditText);
		submitBtn = (Button)findViewById(R.id.submitPicButton);
		selectBtn = (Button)findViewById(R.id.selectButton);
		imageView = (ImageView)findViewById(R.id.ImageView1);
		tvBatteryLevel = (TextView) findViewById(R.id.tvBatteryLevel);
//		CharSequence battery = tvBatteryLevel.getText();
//		String sBattery = battery.toString();
//		intBattery=Integer.valueOf(sBattery).intValue();
		selectBtn.setOnClickListener(new OnClickListener(){
			public void onClick(View v){
				Intent intent = new Intent(Intent.ACTION_PICK,android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
				startActivityForResult(intent, 1);	
				
			}
		});
		submitBtn.setOnClickListener(new OnClickListener(){
			public void onClick(View v){
				SiftActivity sift = new SiftActivity();
				sift.SIFT(picturePath);
				ReceiveFive receiveFive = new ReceiveFive();
				int tag = receiveFive.receiveFivePhotos(intBattery);
				if(tag == 1){
				//if(1==1){
					AlertDialog.Builder dialog=new AlertDialog.Builder(SearchActivity.this);
				    dialog.setTitle("接收")
				    .setIcon(android.R.drawable.ic_dialog_info)
				    .setMessage("有10张查询结果，接收吗？")
				    .setPositiveButton("接收", new DialogInterface.OnClickListener() {		
					@Override
					public void onClick(DialogInterface dialog, int which) {
						//转跳到另外一个Activity
						// TODO Auto-generated method stub
						Intent intent=new Intent(SearchActivity.this,RecvFiveActivity.class);
						startActivity(intent);
					}
				}).setNegativeButton("取消", new DialogInterface.OnClickListener() {
					
					
					public void onClick(DialogInterface dialog, int which) {
						// TODO Auto-generated method stub
						dialog.cancel();//取消弹出框
					}
				}).create().show();
				}
//				Intent intent1 = new Intent(SearchActivity.this, SiftActivity.class);
//				intent1.putExtra("age",picturePath);
//				startActivity(intent1);
//				Intent intent1 = new Intent(SearchActivity.this, ReceivePictures.class);
//				startActivity(intent1);
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
//		private boolean validate(String fileName){
//			int idx = fileName.indexOf(".");
//			String subfix = fileName.substring(idx);
//			if(fileName.equals(""))return false;
//			if(!subfix.equals("jpg")||!subfix.equals("png")||!subfix.equals("jpeg")){
//				return false;
//			}else{
//				return true;
//			}
//		}
//		
//		private void showAlert(String msg){
//			AlertDialog.Builder builder = new AlertDialog.Builder(this);
//			builder.setMessage(msg)
//			       .setCancelable(false)
//			       .setPositiveButton("确定", new DialogInterface.OnClickListener() {
//			           public void onClick(DialogInterface dialog, int id) {
//			           }
//			       });
//			AlertDialog alert = builder.create();
//			alert.show();
//		}

}
