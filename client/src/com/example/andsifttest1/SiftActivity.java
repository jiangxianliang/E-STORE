package com.example.andsifttest1;

import mpi.cbg.fly.*;

import java.util.Collections;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;


import com.example.andsifttest1.ImageFeature;

import android.os.Bundle;
import android.os.Environment;
import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;
import android.widget.TextView;
import com.example.clientdesign.R;

import com.example.clientdesign.SearchActivity;
import com.example.andsifttest1.Hash;
public  class SiftActivity{

	private int steps = 3;
	// initial sigma
	private float initial_sigma = 1.6f;
	private int fdsize = 4;
	// feature descriptor orientation bins
	private int fdbins = 8;
	// size restrictions for scale octaves, use octaves < max_size and > min_size only
	private int min_size = 64;
	private int max_size = 1024;
	static PrintStream p;
	FileOutputStream out;
	int pointSize = 10;
	StringBuffer sb=new StringBuffer();
	/**
	 * Set true to double the size of the image by linear interpolation to
	 * ( with * 2 + 1 ) * ( height * 2 + 1 ).  Thus we can start identifying
	 * DoG extrema with $\sigma = INITIAL_SIGMA / 2$ like proposed by
	 * \citet{Lowe04}.
	 * 
	 * This is useful for images scmaller than 1000px per side only. 
	 */ 
	   private boolean upscale = true;
	   TextView TextV;
	
//	protected void onCreate(Bundle savedInstanceState) {
//		super.onCreate(savedInstanceState);
		//setContentView(R.layout.sift_activity);
		// TextV = (TextView)findViewById(R.id.STextView1);
//		 Intent intent2 = this.getIntent();
//		 String urll = intent2.getStringExtra("age");
//		SIFT(urll);
//		Intent intent1 = new Intent(SiftActivity.this, ReceivePictures.class);
//		startActivity(intent1);
//	}
//	
	 private static float normTo1(int b) {
         return (float) (b / 255.0f);
     }
     
     private static int RGB2Grey(int argb) {
        // int a = (argb >> 24) & 0xff;
         int r = (argb >> 16) & 0xff;
         int g = (argb >> 8) & 0xff;
         int b = (argb) & 0xff;

         //int rgb=(0xff000000 | ((r<<16)&0xff0000) | ((g<<8)&0xff00) | (b&0xff));
         int y = (int) Math.round(0.299f * r + 0.587f * g + 0.114f * b);
         return y;
     }

     private FloatArray2D convert(Bitmap img)
     {
         
         FloatArray2D image;
//         PixelGrabber grabber=new PixelGrabber(img, 0, 0, -1,-1, true);
//         try {
//             grabber.grabPixels();
//         } catch (Exception e) {
//             throw new RuntimeException(e);
//         }
         
        // int[] data = (int[]) grabber.getPixels();
         
         int[] data = new int[img.getWidth()*img.getHeight()];//保存所有的像素的数组，图片宽×高

         img.getPixels(data,0,img.getWidth(),0,0,img.getWidth(),img.getHeight());//stride (must be >= bitmap's width
         
         image = new FloatArray2D(img.getWidth(),  img.getHeight());
         for (int d=0;d<data.length;d++)
                     image.data[d] = normTo1(RGB2Grey(data[d]));
         return image;
     }
     
     private List<ImageFeature> convert(List<Feature> features)
     {
         List<ImageFeature> res=new ArrayList<ImageFeature>();
         for (Feature f:features)
         {
             ImageFeature imageFeature=new ImageFeature();
             imageFeature.setDescriptor(( f.descriptor));
             imageFeature.setOrientation(f.orientation);
             imageFeature.setScale(f.scale);
             res.add(imageFeature);
         }
         return res;
     }
	
     private float[] convert(float[] desc)
     {
         for (int i=0;i<desc.length;i++)
         {
            int int_val = (int)(512 * desc[i]);
            int_val = Math.min( 255, int_val );
            int_val = Math.max( 0, int_val );
            desc[i]=int_val;
         }
         return desc; 
     }

	public List<ImageFeature> getFeatures(Bitmap img)
	{
		//String preamb=this.getClass()+": ";
             List<Feature> fs;
			
		FloatArray2DSIFT sift = new FloatArray2DSIFT( fdsize, fdbins );		
		FloatArray2D fa = convert(img);              
		Filter.enhance( fa, 1.0f );
		
		if ( upscale )
		{
			FloatArray2D fat = new FloatArray2D( fa.width * 2 - 1, fa.height * 2 - 1 ); 
			FloatArray2DScaleOctave.upsample( fa, fat );
			fa = fat;
			fa = Filter.computeGaussianFastMirror( fa, ( float )Math.sqrt( initial_sigma * initial_sigma - 1.0 ) );
		}
		else
			fa = Filter.computeGaussianFastMirror( fa, ( float )Math.sqrt( initial_sigma * initial_sigma - 0.25 ) );
		
		long start_time = System.currentTimeMillis();
//		System.out.println(preamb+"processing SIFT ..." );
		sift.init( fa, steps, initial_sigma, min_size, max_size );
		fs = sift.run( max_size );
		Collections.sort( fs );
//		TextV.setText(TextV.getText().toString()+"took " + ( System.currentTimeMillis() - start_time ) + "ms" +"\n");		
//		TextV.setText(TextV.getText().toString()+ fs.size() + " features"+"\n"); 
		Log.v("count", Integer.toString((int)System.currentTimeMillis())); 
		Log.v("count", Integer.toString((int)start_time)); 
		pointSize = fs.size();
		return convert(fs);
	}
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
	 public  void SIFT(String url){
		 BitmapFactory.Options options = new BitmapFactory.Options();
		 options.inSampleSize =2;
		 Bitmap img = BitmapFactory.decodeFile(url, options); 
		List<ImageFeature>imgFeatureList=getFeatures(img);
		Iterator<ImageFeature> it=imgFeatureList.iterator();
		
		String fileName = getSDPath() +"/" + "siftPoints.txt";
		String outName = getSDPath() +"/" + "lshArray.txt";
		try{

			out =new FileOutputStream(fileName);
			//p=new PrintStream(out);
			try {
				//sb.append(pointSize +" "+"128"+"\n");
				sb.append(pointSize+"\n");
				while(it.hasNext()){
					float []desc=it.next().getDescriptor();				
					desc = convert(desc);
					for(int i=0;i<desc.length;i++){
						int descconv = (int)desc[i];
						sb.append(descconv+" ");
					}
					sb.append("\n");
				}
				out.write(sb.toString().getBytes("utf-8"));
			} catch (IOException e) {
           		// TODO Auto-generated catch block
           		e.printStackTrace();
           	}
		} catch (FileNotFoundException e){
	           e.printStackTrace();
	       }
//		Bloom_t bloom = new Bloom_t();
//		bloom.bf = new char[127];
	   /* char[] outChar = new char[1016];
	  
	    
	   Hash HashTest = new Hash();
		outChar = HashTest.HashMain(fileName);
		int[] outInt = new int[1024];
		for(int i = 0;i < 1016; i++)
			outInt[i] = (int)outChar[i];
		for(int ii = 1016;ii < 1024; ii++)
		    outInt[ii] = 0;
		try {
			FileWriter fw = new FileWriter(outName);
			BufferedWriter bw = new BufferedWriter(fw);
			for(int j=0; j < 1024;j++)
				bw.write(outInt[j]);*
			bw.close();
      	    fw.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}*/
    	   
//		for(int i = 0; i < outChar.length; i++)
//			TextV.setText(TextV.getText().toString()+outChar[i]);
//		   
	   }
	 
	 public  void SIFTList(ArrayList<String> urlList){
		 
		 String fileName = getSDPath() +"/" + "siftPoints.txt";
		 String outName = getSDPath() +"/" + "lshArray.txt";
		 String url;
				try{

					out =new FileOutputStream(fileName);
					//p=new PrintStream(out);
					try {
						//sb.append(pointSize +" "+"128"+"\n");
						BitmapFactory.Options options = new BitmapFactory.Options();
						 options.inSampleSize = 4;
						 for(int list_i = 0; list_i < urlList.size(); list_i ++)
						 {
							 url = urlList.get(list_i);
							 Log.v("imagePath", url);
							 
							 Bitmap img = BitmapFactory.decodeFile(url, options);
							 List<ImageFeature> imgFeatureList = getFeatures(img);
							 Iterator<ImageFeature> it = imgFeatureList.iterator();
							 sb.append(pointSize+"\n");
								while(it.hasNext()){
									float []desc=it.next().getDescriptor();				
									desc = convert(desc);
									for(int i=0;i<desc.length;i++){
										int descconv = (int)desc[i];
										sb.append(descconv+" ");
									}
									sb.append("\n");
								}
						 }
						
						out.write(sb.toString().getBytes("utf-8"));
					} catch (IOException e) {
		           		// TODO Auto-generated catch block
		           		e.printStackTrace();
		           	}
				} catch (FileNotFoundException e){
			           e.printStackTrace();
			       }	   
			 
		
		 
		
	   }


}
