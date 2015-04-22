package com.example.socket;


import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.Iterator;
import java.util.List;

import android.annotation.SuppressLint;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;


public  class uploadFeatures{

    private static final String HOST = "115.156.209.159";

    private static final int PORT = 31120;
    private Socket socket = null;
    private File fr = null;
    private DataOutputStream dos = null;
    
    private  int uploadImageNum = 0;
    private int battery = 0;
    int[] imageBuf;
//    public static int BigtoLittle32(int tag){
//    	tag=(( (int)(tag) & 0xff000000) >> 24) | (( (int)(tag) & 0x00ff0000) >> 8)   |  (( (int)(tag) & 0x0000ff00) << 8)|(( (int)(tag) & 0x000000ff) << 24); 
//
//		return tag;
//    	
//    }
    public static int BigtoLittle32(int tag){
    	tag=((( tag & 0xff000000) >> 24) |(( tag & 0x00ff0000) >> 8)|((tag & 0x0000ff00) << 8)|((tag & 0x000000ff) << 24)); 

		return tag;
    	
    }
//    public static long BigtoLittleLong(long tag){
//    	tag=(( (long)(tag) & 0xff000000) >> 24) | (( (long)(tag) & 0x00ff0000) >> 8)   |  (( (long)(tag) & 0x0000ff00) << 8)|(( (long)(tag) & 0x000000ff) << 24); 
//
//		return tag;
//    	
//    }

    @SuppressLint("SdCardPath")
	public int[] uploadFeat(final ArrayList<String> filePathList){
    	fr = new File("/sdcard/siftPoints.txt");
    	// TODO Auto-generated method stub 
    	 BroadcastReceiver mBatteryInfoReceiver = new BroadcastReceiver() {
    	        @Override
    	        public void onReceive(Context context, Intent intent) {
    	                String action = intent.getAction();
    	                if (Intent.ACTION_BATTERY_CHANGED.equals(action)) {

    	                        int level = intent.getIntExtra("level", 0);
    	                        int scale = intent.getIntExtra("scale", 100);
    	                         battery = level * 100 / scale;
//    	                        tvBatteryLevel.setText("Battery level: "
//    	                                        + String.valueOf(level * 100 / scale) + "%");
    	                }
    	        }
    	};
    	Runnable downloadRun = new Runnable(){   
			
				public void run() {  
    	try {
    		socket = new Socket(HOST, PORT);
    		dos = new DataOutputStream(socket.getOutputStream());
    		FileInputStream fis = null;
    		
    		int task_tag = 2;
    		int dev_tag = 2;
    		int uploadKeypoint =1;
    		int imagecount = filePathList.size();
    		FileInputStream fisBattery = new FileInputStream(new File("/sdcard/battery.txt"));
    		int battery = fisBattery.read();
      		Log.v("battery", Integer.toString(battery));
    		dos.writeInt(BigtoLittle32(dev_tag));
    		dos.writeInt(BigtoLittle32(task_tag));
    		dos.writeInt(uploadFeatures.BigtoLittle32(uploadKeypoint));
      		dos.writeInt(uploadFeatures.BigtoLittle32(battery));
      		dos.writeInt(uploadFeatures.BigtoLittle32(imagecount));
      		
      		String filename;
      		int sLength = 0;
      		BufferedReader bf;
      		int keypointCount;
      		byte[] kpBuffer = new byte[1024];
      		String keypoints = "";
      		fis = new FileInputStream(fr);		     
		    bf = new BufferedReader(new InputStreamReader(fis));
      		for(int list_i = 0; list_i < imagecount; list_i ++)
      		{
      			filename = filePathList.get(list_i).substring(filePathList.get(list_i).lastIndexOf("/")+1);
      			sLength = filename.getBytes().length;
      			dos.writeInt(BigtoLittle32(sLength));
        		dos.write(filename.getBytes());
        		
        		
          		keypointCount = Integer.parseInt(bf.readLine());
          		Log.v("kpcount", Integer.toString(keypointCount));
          		dos.writeInt(uploadFeatures.BigtoLittle32(keypointCount));
          		
          		
          		while(keypointCount > 0){
          			keypoints = bf.readLine();
          			int j = 0;
          			 for(j = 0; j < 1024; j++)
          					kpBuffer[j] = '#';
          			byte[] temp = keypoints.getBytes();
          			for(j = 0; j < keypoints.getBytes().length; j++)
          				kpBuffer[j] = temp[j]; 
          			int offset = keypoints.getBytes().length;
          			for(int i = offset; i < 1024; i++)
              			kpBuffer[i] = '#';
          			//Log.v("kp",keypoints);
          			dos.write(kpBuffer);
          			keypointCount --;
          		 }
        		
      		}
   		    
      		 
		     dos.flush();
		     fis.close();
		     DataInputStream dataInput = new DataInputStream(socket.getInputStream());
		     Log.v("1",Integer.toString(dataInput.readInt()));
		     Log.v("2",Integer.toString(dataInput.readInt()));

	         uploadImageNum = dataInput.readInt();
		     Log.v("e5",Integer.toString(uploadImageNum));
		     if(uploadImageNum == 0)
		    	 imageBuf = new int[0];
		     else
		     {
		    	 imageBuf = new  int[uploadImageNum];
			     for(int image_i = 0; image_i < uploadImageNum; image_i ++)
			     {
			    	 imageBuf[image_i] = dataInput.readInt()-1;
			    	 Log.v("imageBuf[image_i]",Integer.toString(imageBuf[image_i]));
			     }
			     
		     }
		     
		     
	         /*int nameLen = 0;
	         byte[] nameBuffer = new byte[256];
	         Log.v("len1",Integer.toString(nameLen));
	         while (nameLen < 256 & nameLen > -1) { 
           		 nameLen += dataInput.read(nameBuffer, nameLen, 256-nameLen); 
           	}
          	Log.v("len",Integer.toString(nameLen));
          	*/
          	
          	dos.close(); 
	        socket.close();
	        //Log.v("uploadTag1", Integer.toString(uploadTag));
		       // return uploadTag;
		  } catch (IOException e) {
							// TODO Auto-generated catch block
		       e.printStackTrace();
		  }
			}
		  }; 
		 //new Thread(downloadRun).start();
		 Thread t1 = new Thread(downloadRun);
		 t1.start();
		 try {
			t1.join();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
//		 try {
//			downloadRun.wait();
//		} catch (InterruptedException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		}
//
		// try {
		//	Thread.sleep(10000);
		//} catch (InterruptedException e) {
			// TODO Auto-generated catch block
		//	e.printStackTrace();
	//	}
		// Log.v("uploadTag1", Integer.toString(uploadTag));
	        return imageBuf;

	    
		  

    }


}
