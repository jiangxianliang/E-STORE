package com.example.andsifttest1;




import java.util.Random;


public class Hash {


	  final int BUFSIZE = 100;
	  final static int MAX_HASH_RND = 536870912;
	  final int CONF_PARAM_SIZE = 6;
	  final static int RAND_MAX = 0x7FFF;
	  final static double M_PI=3.14159265358979323846;
	  //static Random random1 = new Random(1992);
//	  static int[] ranFloat = new int[10000];
//	  static int ranKK = 0;

	  public static double get_uniform_random(Random random1,double range_start,double range_end)
	  {
	    double tmp;
		double ret;
		float randomTest1 = random1.nextFloat();
		//ranFloat[ranKK++] = (int)(randomTest1*1000);
		if(range_start>range_end)
		{
		  tmp = range_start;
		  range_start = range_end;
		  range_end = tmp;
		}
		ret = range_start + ((range_end - range_start)*randomTest1/RAND_MAX);
		return ret;
	  }
	  
	  public static double get_gaussian_random(Random random1)
	  {
	    double x1,x2;
		double z;
		do 
		{
		  x1 = get_uniform_random(random1,0.0,1.0);
		} while(x1 == 0);
		x2 = get_uniform_random(random1,0.0, 1.0);
		z = Math.sqrt(-2.0*Math.log(x1))*Math.cos(2.0*M_PI*x2);
		return z;
	  }

	//return unsigned int
	  public static long get_random_uns32(Random random1,int range_start,int range_end)
	  {
	    int ret;
		int tmp;
		//System.out.print("9999999999999 ");
		float randomTest = random1.nextFloat();
		//ranFloat[ranKK++] = (int)(randomTest*1000);
		if(range_start>range_end)
		{
			tmp = range_start;
			range_start = range_end;
			range_end = tmp;
		}
		
		
		if(RAND_MAX >= range_end-range_start)
		{	
			
			ret = range_start + (int)((range_end - range_start + 1.0)*randomTest/(RAND_MAX + 1.0));
		}
		else 
		{
		  ret = range_start + (int)((range_end - range_start + 1.0)
				*(randomTest*(RAND_MAX + 1)
				+randomTest)/(RAND_MAX*(RAND_MAX + 1)
				+RAND_MAX + 1.0));
		  //System.out.print(ret+" 888888888888888 ");
			
		}
		return ret&0xFFFFFFFF;
	  }
	  
	  public static Hash_table init_hash_tables(MyPublic conf,int num)
	  {
	    int i=0;
		int j=0;
		int k=0;
		Hash_table tables = new Hash_table();
		tables.a = new float[conf.k][conf.dimension];
		tables.b=new float[conf.k];
		tables.c = new int[conf.k];
		Random random1 = new Random(1992);
		for(i=0;i<num;i++)
		{
		  for(j=0;j<conf.k;j++)
		  {
		    for(k=0;k<conf.dimension;k++)
			{
			  float aa=(float)get_gaussian_random(random1);
			  tables.a[j][k] = aa;
			 // System.out.print(aa+" ");
			}
			tables.b[j] = (float)get_uniform_random(random1,0, conf.w);
			//System.out.print(tables.b[j]+" ");
			tables.c[j] = (int)get_random_uns32(random1, 1, MAX_HASH_RND);
			//System.out.print(tables.c[j]+" ");
		  }
		}
		return tables;
	  }
	
	public char[] HashMain(String txtUrl){
		MyPublic conf = new MyPublic();
		Pic_struct pic = new Pic_struct();
//		Bloom_t bloom = new Bloom_t();
		//Hash_table table = new Hash_table();
		Bloom_t bloom = new Bloom_t();
	   bloom.bf = new char[127];
	   bloom.raw_bf = new char[1016];
//		bloom.bf = new char[];
//		bloom.raw_bf = new char[];
		conf.r = 56;
		conf.w = 6;
		conf.k = 4;
		conf.l = 1;
		conf.bf_len = 1009;
		conf.dimension = 128;
		  Hash_table tables1 = new Hash_table();
		  tables1.a = new float[conf.k][conf.dimension];
			tables1.b=new float[conf.k];
			tables1.c = new int[conf.k];
			
			pic = Bloom.read_pic_keypoints(conf,txtUrl);
			tables1 = Hash.init_hash_tables(conf, conf.l);
			bloom.bf = Bloom.compute_bit_vector(pic, conf, tables1);
			//System.out.println("22");
			bloom.raw_bf = Bloom.write_bloom_to_file(bloom.bf,conf);
//			 try{
//		    	   FileWriter fw = new FileWriter("/sdcard/random1.txt");
//		    	   BufferedWriter bw = new BufferedWriter(fw);
//		    	   for(int zz = 0;zz< ranFloat.length;zz++)
//		    		   bw.write(ranFloat[zz]);
//		    	   bw.close();
//		    	   fw.close();
//		    	   }catch(IOException e){
//		    		   e.printStackTrace();
//		    	   }
			return bloom.raw_bf;
			//System.out.println("22");
	}
}
