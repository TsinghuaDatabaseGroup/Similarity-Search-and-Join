package core;

public class GetSeg {
	public static int[] getSeg(int len,int num){
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
		
		return info;
	}

}
