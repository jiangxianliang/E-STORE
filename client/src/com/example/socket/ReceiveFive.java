 package com.example.socket;


import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.FilterInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Date;
import java.util.Scanner;

import com.example.battery.GetBatteryLevel;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Environment;
import android.os.Handler;
import android.util.Log;
import android.widget.Button;
import android.widget.ImageView;



public class ReceiveFive{
   private static final String HOST = "115.156.209.159";

    private static final int PORT = 31120;
    private Socket socket = null;
    private Handler mHandler = null;
    private boolean stop = true;
    private File fr = null;
    private String uploadFile1="/sdcard/1.jpg";
    private String uploadFile2="/sdcard/2.jpg"; 
    private String uploadFile3="/sdcard/3.jpg"; 
    private String uploadFile4="/sdcard/4.jpg"; 
    private String uploadFile5="/sdcard/5.jpg"; 
    private DataOutputStream dos = null;
   // private int battery = 0;
   // private DataInputStream dataInput = null;
    private ImageView imageView = null;
    private DataInputStream dataInput = null;

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
    public int receiveFivePhotos(int intBattery) {

    	//final int battery = intBattery;
        //fr = new File("/sdcard/lshArray.txt");
    	String spfile = getSDPath() +"/" + "siftPoints.txt";
        fr = new File(spfile);
        for(int ii = 1; ii < 11; ii++){
      	  //File file = new File("/sdcard/"+ii+".jpg");
        	File file = new File(getSDPath()+"/"+ii+".jpg");
//      	  java.io.FileOutputStream fos = new FileOutputStream("/sdcard/"+kk+".jpg",false);
      	  if (file.isFile() && file.exists()) {  
      	        file.delete();   
      	    }
        }
            	Runnable downloadRun = new Runnable(){   
			@Override  
				public void run() {   
			    // TODO Auto-generated method stub 
//				BroadcastReceiver mBatteryInfoReceiver = new BroadcastReceiver() {
//			        @Override
//			        public void onReceive(Context context, Intent intent) {
//			                String action = intent.getAction();
//			                if (Intent.ACTION_BATTERY_CHANGED.equals(action)) {
//
//			                        int level = intent.getIntExtra("level", 0);
//			                        int scale = intent.getIntExtra("scale", 100);
//			                         battery = level * 100 / scale;
//			                         Log.v("battery",Integer.toString(battery));
////			                        tvBatteryLevel.setText("Battery level: "
////			                                        + String.valueOf(level * 100 / scale) + "%");
//			                }
//			        }
//			};
					try {
						Log.v("123456", Integer.toString(123));
						socket = new Socket(HOST, PORT);
						Log.v("123456", Integer.toString(12322));
						//DataInputStream dataInput = new DataInputStream(socket.getInputStream());
						dos = new DataOutputStream(socket.getOutputStream());
						dataInput = new DataInputStream(socket.getInputStream());
//					} catch (IOException ex) {
//						ex.printStackTrace();
//					}
//					//s上传特征向量
//		          	try{
						 int dev_tag = 2;
						 int task_tag = 1;
		          		int imagecount = 1;

		          		 String filename = "query.jpg";
		          		 int sLength = filename.getBytes().length;
		          		 
		          		 dos.writeInt(uploadFeatures.BigtoLittle32(dev_tag));
		          		 dos.writeInt(uploadFeatures.BigtoLittle32(task_tag));

		          		 
		          		FileInputStream fisBattery = new FileInputStream(new File(getSDPath()+"/"+"battery.txt"));
		          		int battery = fisBattery.read();
		          		Log.v("battery", Integer.toString(battery));
		          		dos.writeInt(uploadFeatures.BigtoLittle32(battery));
		          		
		          		 dos.writeInt(uploadFeatures.BigtoLittle32(imagecount));
		          		 dos.writeInt(uploadFeatures.BigtoLittle32(sLength));
		          		 
		          		 dos.write(filename.getBytes());

		          		 FileInputStream fis = new FileInputStream(fr);
		          		 BufferedReader bf = new BufferedReader(new InputStreamReader(fis));
		          		 int keypointCount = Integer.parseInt(bf.readLine());
		          		Log.v("kpcount", Integer.toString(keypointCount));
		          		dos.writeInt(uploadFeatures.BigtoLittle32(keypointCount));
		          		 String keypoints = "";

		          		byte[] kpBuffer = new byte[1024];

		          		int j = 0;
		          		 while((keypoints = bf.readLine()) != null){
		          			for(j = 0; j < 1024; j++)
		          					kpBuffer[j] = '#';
		          			byte[] temp = keypoints.getBytes();
		          			for(j = 0; j < keypoints.getBytes().length; j++)
		          				kpBuffer[j] = temp[j]; 
		          			int offset = keypoints.getBytes().length;
		          			for(int i = offset; i < 1024; i++)
			          			kpBuffer[i] = '#';
		          			Log.v("kp",keypoints);
		          			dos.write(kpBuffer);
		          			
		          		 }
		          		 dos.flush();
		          		 fis.close();
		          		 
//		          		   
//		          	 }catch(IOException e){
//		          		   e.printStackTrace();
//		          	 }
//					
//		          	//接收图片
//		          	 try {

		                  Log.v("dev", Integer.toString(uploadFeatures.BigtoLittle32(dataInput.readInt())));
		                  Log.v("task", Integer.toString(uploadFeatures.BigtoLittle32(dataInput.readInt())));
		                  //Log.v("123456", Integer.toString(dataInput.readInt()));
		                     
		                  //Log.v("size", Integer.toString(dataInput.readInt()));
		                  
		                  //Log.v("123456", Integer.toString(dataInput.readInt()));
		                  int count = uploadFeatures.BigtoLittle32(dataInput.readInt());
		                  Log.v("count", Integer.toString(count));
		                  /*byte[] frontData = new byte[100];
		                  int len = 0;
		                  while (len < 100) {  
		                	  len += dataInput.read(frontData, len, 100- len);  
		                  }	
		                  String time = new String(frontData);
		                  Log.v("time", time);
		                  Log.v("rank", Integer.toString(dataInput.readInt()));
		                  byte[] frontData1 = new byte[256];
		                  len = 0;
		                  while (len < 256) {  
		                	  len += dataInput.read(frontData1, len, 256- len);  
		                  }
		                  String imageName = new String(frontData1);
		                  Log.v("imageName", imageName);
		          		 
		                  byte[] frontData2 = new byte[8];
		                  len = 0;
		                  while (len < 8) {  
		                	  len += dataInput.read(frontData2, len, 8- len);  
		                  }
			          	  String ss = new String(frontData2);
			          	  Log.v("ss", ss);
							*/
		                  
				        	// int picSize = uploadFeatures.BigtoLittle32(picSizetest);
				        	// Log.v("picSize", Integer.toString(picSize));
				        	 //Log.v("picSizetest", Integer.toBinaryString(picSizetest));
		                   //  Log.v("picSize", Integer.toBinaryString(picSize));
		                  int len =0;
		                  byte[] nameBuffer = new byte[256];
					      for(int jj = 0; jj < count; jj++){
					        	 int kk = jj + 1;
						          	len = 0;
						          	
						          	while (len < 256) {  
				                   		 len += dataInput.read(nameBuffer, len, 256-len);  
				                   	 }
						          	Log.v("len",Integer.toString(len));
						          	String imageName = new String(nameBuffer);
						          	Log.v("imageName"+kk, imageName);
					        	 
						          	int picSize = dataInput.readInt();
					        	 java.io.FileOutputStream fos = new FileOutputStream(getSDPath()+"/"+kk+".jpg",false);
					        	 
					        	 
			                     Log.v("similarity"+kk, Integer.toString(dataInput.readInt()));
			                     byte[] picdata = new byte[picSize];
			                     int len1 = 0;  
		                    	 while (len1 < picSize) {  
		                    		 len1 += dataInput.read(picdata, len1, picSize - len1);  
		                    	 }
					             fos.write(picdata,0,picSize);
					             
					             
					             /*byte[] middledata = new byte[10000-picSize];
			                     len1 = 0;  
		                    	 while (len1 < 10000-picSize) {  
		                    		 len1 += dataInput.read(middledata, len1, (10000-picSize) - len1);  
		                    	 }/*
//					             if(jj < count){
					            	//Log.v("rank"+kk, Integer.toString(dataInput.readInt()));
						          	
						       
						                    
						           /* byte[] frontData4 = new byte[8];
							        len = 0;
							        while (len < 8) {  
					                   	 len += dataInput.read(frontData4, len, 8- len);  
					                   	}
							        String ss1 = new String(frontData4);
							        Log.v("ss"+kk, ss1);*/
//				                 }
					             
					      }
//					      fos.close();
			             dos.close();
			             dataInput.close();
	                     socket.close();
		          	 } catch (IOException e) {
							// TODO Auto-generated catch block
		          		 e.printStackTrace();
		          	 }

				}

	
			  }; 
			  new Thread(downloadRun).start();  

			  return 1;
	    }

}
