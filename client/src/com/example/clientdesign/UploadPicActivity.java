package com.example.clientdesign;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.ArrayList;

import com.example.socket.uploadFeatures;


import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.webkit.WebView;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;

public class UploadPicActivity extends Activity {

	
    private static final int PROGRESS=5;
    private Handler handler,progressHandler;

    private static final String HOST = "115.156.209.159";
//    private static final int PORT = 8869;
    private static final int PORT = 31120;
    private String uploadFile = null;
    ArrayList<String> uploadFileList=new ArrayList<String>();
    
    private DataOutputStream out_send = null;
    private Socket socket = null;
    private BufferedReader in = null;
    private PrintWriter out = null;
    private Handler mHandler = null;
    private String content = "";
    private boolean stop = true;
    private File fr = null;
    private DataOutputStream ds = null;
	int xh_count = 0;
	java.io.FileInputStream fos;
	//private
	// 声明进度条对话框
	ProgressDialog xh_pDialog;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.uploadpic);
        Intent intent2 = this.getIntent();
        uploadFileList = intent2.getStringArrayListExtra("age");

		xh_count = 0;

		// 创建ProgressDialog对象
		xh_pDialog = new ProgressDialog(UploadPicActivity.this);

		// 设置进度条风格，风格为圆形，旋转的
		xh_pDialog.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);

		// 设置ProgressDialog 标题
		xh_pDialog.setTitle("上传进度");

		// 设置ProgressDialog提示信息
		xh_pDialog.setMessage("上传中。。。");

		// 设置ProgressDialog标题图标
		xh_pDialog.setIcon(R.drawable.icon);

		// 设置ProgressDialog 的进度条是否不明确 false 就是不设置为不明确
		xh_pDialog.setIndeterminate(false);

		// 设置ProgressDialog 进度条进度
		xh_pDialog.setProgress(100);

		// 设置ProgressDialog 是否可以按退回键取消
		xh_pDialog.setCancelable(true);

		// 让ProgressDialog显示
		xh_pDialog.show();
		Runnable downloadRun = new Runnable(){   
			@Override  
			public void run() {   
			    // TODO Auto-generated method stub 
				try {
		            socket = new Socket(HOST, PORT);
		            out_send = new DataOutputStream(socket.getOutputStream());
		            in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
		            out = new PrintWriter(new BufferedWriter(new OutputStreamWriter(
		                    socket.getOutputStream())), true);

		         
		        } catch (IOException ex) {
		            ex.printStackTrace();
//		            ShowDialog("login exception" + ex.getMessage());
		        }

		            	int length=0;
			            byte[] sendBytes=new byte[1024];
			            
			            try{
			            	 int task_tag = 2;
				             int dev_tag = 2;
				             int uploadImage = 2;
				             int imageCount = uploadFileList.size();
				             int picsize = 0;
				             String filename;
				             out_send.writeInt(uploadFeatures.BigtoLittle32(dev_tag));
				             out_send.writeInt(uploadFeatures.BigtoLittle32(task_tag));
				             out_send.writeInt(uploadFeatures.BigtoLittle32(uploadImage));
				             out_send.writeInt(uploadFeatures.BigtoLittle32(imageCount));
				             Log.v("dev",Integer.toString(dev_tag));
				             Log.v("task",Integer.toString(task_tag));
				             Log.v("imcount",Integer.toString(imageCount));
				             
				             
				             byte[] nameBuffer = new byte[256];
				             byte[] temp;
	                         double douPro = 0;
			            	for(int upload_i = 0; upload_i < uploadFileList.size(); upload_i ++)
			            	{
			            		uploadFile = uploadFileList.get(upload_i);
			            		fos = new FileInputStream(uploadFile);
			            		picsize = fos.available();
			            		filename = uploadFile.substring(uploadFile.lastIndexOf("/")+1);
			            		
				      			temp = filename.getBytes();
				      			for(int j = 0; j < filename.getBytes().length; j++)
				      				nameBuffer[j] = temp[j]; 
				      			int offset = filename.getBytes().length;
				      			for(int i = offset; i < 256; i++)
				          			nameBuffer[i] = '#';
				      			
				      			out_send.write(nameBuffer);
				      			out_send.writeInt(uploadFeatures.BigtoLittle32(picsize));
				      			
				      			Message progressMsg=null;
		                         int progress=0;
					             while((length = fos.read(sendBytes,0,sendBytes.length))>0){
					             
					            	 out_send.write(sendBytes,0,length);
					            	 out_send.flush();
					            	 progress+=length;
					            	 douPro = douPro + ((double)progress/(double)picsize)/imageCount*100;
		                              progressMsg=progressHandler.obtainMessage(PROGRESS,(int)douPro);
		                              progressHandler.sendMessage(progressMsg);
					             }
			            	}
			            
			             Thread.sleep(10);
			             out_send.close();
			             fos.close();
			             //socket.close();
			            }catch(Exception e){
			            }
			
			}   
			  }; 
			  new Thread(downloadRun).start();
			    progressHandler=new Handler(){
			        @Override
			        public void handleMessage(Message msg) {
			            xh_pDialog.setProgress((Integer) msg.obj);
//			            try {
//							Thread.sleep(10);
//						} catch (InterruptedException e) {
//							// TODO Auto-generated catch block
//							e.printStackTrace();
//						}
			            if((Integer) msg.obj > 99){
			            	 AlertDialog.Builder dialog=new AlertDialog.Builder(UploadPicActivity.this);
							    dialog.setTitle("上传")
							    .setIcon(android.R.drawable.ic_dialog_info)
							    .setMessage("上传成功")
							    .setPositiveButton("确定", new DialogInterface.OnClickListener() {		
								@Override
								public void onClick(DialogInterface dialog, int which) {
									// TODO Auto-generated method stub
									finish();
									Intent intent=new Intent(UploadPicActivity.this,MainActivity.class);
									startActivity(intent);
								}
							}).create().show();
			            }
			        }
			    }; 

                
  
    }
    
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }
}

