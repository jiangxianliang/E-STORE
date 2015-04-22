package com.example.clientdesign;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import android.app.Activity;
import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.view.Window;
import android.view.animation.AnimationUtils;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.BaseAdapter;
import android.widget.Gallery;
import android.widget.Gallery.LayoutParams;
import android.widget.ImageSwitcher;
import android.widget.ImageView;
import android.widget.Toast;
import android.widget.ViewSwitcher.ViewFactory;

/**
 * ImageSwitcher和Gallery如何展示SD卡中的资源图片
 * @author Andy.Chen
 * @email:Chenjunjun.ZJ@gmail.com
 */
public class RecvFiveActivity extends Activity 
             implements OnItemSelectedListener,ViewFactory{
    
	private List<String> imagePathList; 
	private String[] list; 
	private ImageSwitcher mSwitcher;
	private Gallery mGallery;
	private String uploadFile1="/sdcard/1.jpg";
    private String uploadFile2="/sdcard/2.jpg"; 
    private String uploadFile3="/sdcard/3.jpg"; 
    private String uploadFile4="/sdcard/4.jpg"; 
    private String uploadFile5="/sdcard/5.jpg";
	
	/** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.recv_five);
        
        imagePathList=getImagePathFromSD(); 
        list = imagePathList.toArray(new String[imagePathList.size()]); 
    
		/* 设定Switcher */
		mSwitcher = (ImageSwitcher) findViewById(R.id.switcher);
		mSwitcher.setFactory(this);
		/* 设定载入Switcher的模式 */
		mSwitcher.setInAnimation(AnimationUtils.loadAnimation(this,
				android.R.anim.fade_in));
		/* 设定输出Switcher的模式 */
		mSwitcher.setOutAnimation(AnimationUtils.loadAnimation(this,
				android.R.anim.fade_out));
		mSwitcher.setOnClickListener(new OnClickListener() {

			public void onClick(View v) {
				Toast.makeText(RecvFiveActivity.this, "你点击了ImageSwitch上的图片",
						Toast.LENGTH_SHORT).show();

			}

		});

		mGallery = (Gallery) findViewById(R.id.mygallery);
		/* 新增几ImageAdapter并设定给Gallery对象 */
		mGallery.setAdapter(new ImageAdapter(this, getImagePathFromSD()));

		mGallery.setOnItemSelectedListener(this);

		/* 设定一个itemclickListener事件 */
		mGallery.setOnItemClickListener(new OnItemClickListener() {
			public void onItemClick(AdapterView<?> parent, View v,
					int position, long id) {
				Toast.makeText(RecvFiveActivity.this, "你点击了Gallery上的图片",
						Toast.LENGTH_SHORT).show();
			}
		});

    }
    
    /** 从SD卡中获取资源图片的路径 */
	private List<String> getImagePathFromSD() {
		/* 设定目前所在路径 */
		List<String> it = new ArrayList<String>();
		
		//根据自己的需求读取SDCard中的资源图片的路径
//		String imagePath = Environment.getExternalStorageDirectory().toString()+"/hknational/image";
//		
//		File mFile = new File(imagePath);
//		File[] files = mFile.listFiles();

		/* 将所有文件存入ArrayList中 */
//		for (int i = 0; i < files.length; i++) {
//			File file = files[i];
//			if (checkIsImageFile(file.getPath()))
				it.add(uploadFile1);
				it.add(uploadFile2);
				it.add(uploadFile3);
				it.add(uploadFile4);
				it.add(uploadFile5);
//		}
		return it;
	}

	/** 判断是否相应的图片格式  */
	private boolean checkIsImageFile(String fName) {
		boolean isImageFormat;

		/* 取得扩展名 */
		String end = fName
				.substring(fName.lastIndexOf(".") + 1, fName.length())
				.toLowerCase();

		/* 按扩展名的类型决定MimeType */
		if (end.equals("jpg") || end.equals("gif") || end.equals("png")
				|| end.equals("jpeg") || end.equals("bmp")) {
			isImageFormat = true;
		} else {
			isImageFormat = false;
		}
		return isImageFormat;
	}

	/* 改写BaseAdapter自定义一ImageAdapter class */
	public class ImageAdapter extends BaseAdapter {
		/* 声明变量 */
		int mGalleryItemBackground;
		private Context mContext;
		private List<String> lis;

		/* ImageAdapter的构造符 */
		public ImageAdapter(Context c, List<String> li) {
			mContext = c;
			lis = li;
			/*
			 * 使用res/values/attrs.xml中的<declare-styleable>定义 的Gallery属性.
			 */
			TypedArray mTypeArray = obtainStyledAttributes(R.styleable.Gallery);
			/* 取得Gallery属性的Index id */
			mGalleryItemBackground = mTypeArray.getResourceId(
					R.styleable.Gallery_android_galleryItemBackground, 0);
			/* 让对象的styleable属性能够反复使用 */
			mTypeArray.recycle();
		}

		/* 重写的方法getCount,传回图片数目 */
		public int getCount() {
			return lis.size();
		}

		/* 重写的方法getItem,传回position */
		public Object getItem(int position) {
			return position;
		}

		/* 重写的方法getItemId,传并position */
		public long getItemId(int position) {
			return position;
		}

		/* 重写方法getView,传并几View对象 */
		public View getView(int position, View convertView, ViewGroup parent) {
			/* 产生ImageView对象 */
			ImageView i = new ImageView(mContext);
			/* 设定图片给imageView对象 */
			Bitmap bm = BitmapFactory.decodeFile(lis.get(position).toString());
			i.setImageBitmap(bm);
			/* 重新设定图片的宽高 */
			i.setScaleType(ImageView.ScaleType.FIT_XY);
			/* 重新设定Layout的宽高 */
			i.setLayoutParams(new Gallery.LayoutParams(240, 200));
			/* 设定Gallery背景图 */
			i.setBackgroundResource(mGalleryItemBackground);
			/* 传回imageView对象 */
			return i;
		}
	} 

	@Override
	public View makeView() {
	    ImageView iv = new ImageView(this); 
	    iv.setBackgroundColor(0xFF000000); 
	    iv.setScaleType(ImageView.ScaleType.FIT_CENTER); 
	    iv.setLayoutParams(new ImageSwitcher.LayoutParams( 
	        LayoutParams.FILL_PARENT,LayoutParams.FILL_PARENT)); 
	    return iv; 
	}

	@Override
	public void onItemSelected(AdapterView<?> parent, View view, int position,
			long id) {
		// TODO Auto-generated method stub
		String photoURL = list[position];
		Log.i("A", String.valueOf(position));
		
		mSwitcher.setImageURI(Uri.parse(photoURL));
	}

	@Override
	public void onNothingSelected(AdapterView<?> parent) {
		// TODO Auto-generated method stub
		
	}
}