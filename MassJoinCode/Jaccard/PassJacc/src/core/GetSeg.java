package core;

import java.util.HashMap;

public class GetSeg {
	private static HashMap<Integer, int[]> cache = new HashMap<Integer, int[]>();
	
	public static int[] getSeg(int len,int num){
		if(!cache.containsKey(len)){
			int[] info = new int[2*num];
			
			int begin=0;
			int k=(int) (len-Math.floor((double)len/num)*num);
			int len1=(int) Math.floor((double)len/num);
			int len2=(int) Math.ceil((double)len/num);
			for(int i=0;i<num;i++){
				info[2*i] = begin;
				if(i < num-k)
					info[2*i+1] = len1;
				else
					info[2*i+1] = len2;
				begin = begin + info[2*i+1];
				
			}
			cache.put(len, info);
		}	
		return cache.get(len);
	}
		
	public static int getSegNum(int length){
		//Ha+Hb+1
		int num = (int)Math.floor((1 - Config.SIMILARITY)/(1 + Config.SIMILARITY) * 2 * length + Config.EPS) + 1;
		// int num = (int)Math.floor(Arith.div(Arith.mul(2*length, Arith.sub(1, Config.SIMILARITY)), Arith.add(1, Config.SIMILARITY)))
		//		   +1;
		return num;
	}

}
