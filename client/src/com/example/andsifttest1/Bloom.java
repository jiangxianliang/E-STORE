package com.example.andsifttest1;

import java.lang.Math;

import java.io.*;



public class Bloom {
	   final static int CHAR_BITS = 8;
       final static double UH_PRIME_DEFAULT = 4294967291.0;
       public static  int compute_lsh(float[] key,Hash_table table,MyPublic conf)
       {   
    	   int[] tmp;
    	   
    	   tmp = new int[conf.k];
    	   int dimension = conf.dimension;
    	   //key = new float[dimension];
    	   int w = conf.w;
    	   int k = conf.k;
    	   int i,j;
    	   float sum1 = 0;
    	   long sum2 = 0;
    	   long ret;
    	   
    	   for(i = 0;i<k;i++)
    	   {
    		   sum1 = 0;
    		   for(j = 0;j < dimension;j++)
    		   {
    			   sum1 += key[j]*table.a[i][j];
    			   //System.out.println(table.a[i][j]);
    		   }
    		   sum1 +=table.b[i];    		  
    		   sum1 /=w;
    		   tmp[i] = Math.abs((int)sum1);
    		   
    	   }
    	   sum2 = 0;
    	   for(i = 0;i<k;i++)
    	   {
    		   sum2 +=((tmp[i]*table.c[i])%UH_PRIME_DEFAULT);
    	   }
    	   ret = (long)(sum2 % UH_PRIME_DEFAULT);
    	   ret = (long)(ret % conf.bf_len);
    	   //System.out.print(ret+" ");
    	   
    	   return (int)ret;
       }
       
       public static char[] compute_bit_vector(Pic_struct pic,MyPublic conf,Hash_table table)
       {
    	  // BitSet used = new BitSet(1009);
    	   int i,j;
    	   int lsh_result = 0;
    	   Bloom_t bloom = new Bloom_t();
    	   
    	   int real_bf_len = ((conf.bf_len)+CHAR_BITS-1)/CHAR_BITS;
    	   bloom.bf = new char[real_bf_len];
    	   int raw_bf_len = 8 * real_bf_len;
    	   bloom.raw_bf=new char[raw_bf_len];
    	   //System.out.println(conf.l+" "+pic.num);
    	   for(i = 0;i < conf.l;i++)
    	   {
    		   for(j = 0;j <pic.num;j++)
    		   {
    			   //System.out.println("22222");
    			   lsh_result = compute_lsh(pic.keypoints[j],table,conf);
    			   //System.out.print(lsh_result+" ");
    			   bloom.bf[(lsh_result)/CHAR_BITS] |= ((short)1 << ((lsh_result)%CHAR_BITS));
    			   //System.out.println("22222");
    			   if(lsh_result - 1>= 0)
    				   bloom.bf[(lsh_result-1)/CHAR_BITS] |= ((short)1 << ((lsh_result-1)%CHAR_BITS));
    			   if(lsh_result + 1<conf.bf_len)
    				   bloom.bf[(lsh_result+1)/CHAR_BITS] |= ((short)1 << ((lsh_result+1)%CHAR_BITS));
    			   
//    			   used.set(lsh_result);
//    			   if(lsh_result - 1>= 0)
//    				   used.set(lsh_result - 1);
//    			   if(lsh_result + 1<conf.bf_len)
//    				   used.set(lsh_result + 1);
    			   //System.out.println("Bitset1:" + used);
    		   }
    	   }
		return bloom.bf;
       }
       
       public static Pic_struct read_pic_keypoints(MyPublic conf,String txtUrl)
       {
    	   int i,j,k;
    	  // String tmpString = null;
    	   File fr = new File(txtUrl);
    	   Pic_struct pic = new Pic_struct();
    	   //BufferedReader br = null;
    	   try{
    		   BufferedReader br = new BufferedReader(new FileReader(fr));
    		   //DataInputStream isr = new DataInputStream(new FileInputStream(fr));
    		   String []se = null;
    		   String s = "";
    		   int jj = 0;
    		   if((s = br.readLine()) != null)
    			   se = s.split(" ");
    		   pic.num = (int)Double.parseDouble(se[0]);
    		   pic.dimension = (int)Double.parseDouble(se[1]);
    		   int[] seInt = new int[pic.num*130];
    		   //List<Integer> list = new LinkedList<Integer>();
    		   while( (s = br.readLine()) != null) {
           		se = s.split(" ");
           		for(int ii = 0;ii < se.length;ii++){
           			//System.out.print(se[ii]+" ");
           			//Double.parseDouble(se[ii]);
//           			pic.num = (int)Double.parseDouble((String)se[0]);
//           			pic.dimension = (int)Double.parseDouble((String)se[1]);
           			seInt[jj++] = (int)Double.parseDouble(se[ii]);
           		}
           		//System.out.print("\n");
    		   }
//    		   pic.num = seInt[0];
//    		   pic.dimension = seInt[1];
    		  
    		   pic.keypoints = new float[pic.num][pic.dimension];
    		   //int kk = 3;
    		   int kk = 0;
    		   for(i =0;i<pic.num;i++)
    		   {
    			   //tmpString = br.readLine();
    			   for(k = 0;k <128;k++)
    				   {
    					   pic.keypoints[i][k]=seInt[kk++];
    					   //System.out.print(pic.keypoints[i][k]+" ");
    				   }
    			   //System.out.print("\n");
    			 
    		   }
    		   for(i = 0;i <pic.num;i++)
    		   {
    			   for(j = 0;j<pic.dimension;j++)
    			   {
    				   pic.keypoints[i][j]/=conf.r;
    			   }
    		   }
    		   //isr.close();
    		   br.close();
    	   }catch(IOException e){
    		   e.printStackTrace();
    	   }
		return pic;
       }
           
       public static  char[] write_bloom_to_file(char[] bf,MyPublic conf)
       {
    	   Bloom_t bloom = new Bloom_t();
    	   int real_bf_len = ((conf.bf_len)+CHAR_BITS-1)/CHAR_BITS;
    	   int raw_bf_len = 8 * real_bf_len;
    	   bloom.raw_bf=new char[raw_bf_len];
//    	   try{
//    	   FileWriter fw = new FileWriter("/sdcard/result.txt");
//    	   BufferedWriter bw = new BufferedWriter(fw);
    	   char tmp;
    	   int i,j;
    	   for(i = 0;i<real_bf_len;i++)
    	   {
    		   tmp = bf[i];
    		   //System.out.print(tmp);
    		   for(j = 0;j <CHAR_BITS;j++)
    		   {
    			   if((tmp & 0x1)!=0)
    				   bloom.raw_bf[i*CHAR_BITS +j] = '1';
    			   else
    				   bloom.raw_bf[i*CHAR_BITS +j] = '0';  
    			   tmp>>=1;
    		   }
    	   }
//    	   bw.write(bloom.raw_bf);
//    	   bw.close();
//    	   fw.close();
//    	   }catch(IOException e){
//    		   e.printStackTrace();
//    	   }
		return bloom.raw_bf; 
       }
}
