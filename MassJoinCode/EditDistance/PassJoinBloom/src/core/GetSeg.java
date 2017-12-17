package core;

import java.util.HashMap;

public class GetSeg {
	public static HashMap<Integer, int[]> hashMap = new HashMap<Integer, int[]>();
	
	public static int[] getSeg(int len,int num){
		if (hashMap.containsKey(len)) 
			return hashMap.get(len);
		int[] info = new int[2 * num];
		
		int begin=0;
		int k=(int) (len-Math.floor((double)len/num)*num);
		int len1=(int) Math.floor((double)len/num);
		int len2=(int) Math.ceil((double)len/num);
		for(int i=0;i<num;i++){
			info[2*i] = begin;
			if(i<num-k){
				info[2*i+1] = len1;
			}
			else
				info[2*i+1] = len2;
			begin = begin + info[2*i+1];			
		}	
		hashMap.put(len, info);
		return info;
	}

}
