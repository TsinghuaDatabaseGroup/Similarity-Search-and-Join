package core;

public class GetToken {
	
	public static int getToken(String record, int start, int length){
		String sub = record.substring(start, start+length);
		int code = sub.hashCode();
		return code;
	}
 
}
