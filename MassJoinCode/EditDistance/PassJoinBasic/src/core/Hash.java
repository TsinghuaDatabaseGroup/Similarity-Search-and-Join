package core;

public class Hash {
	public static int getHashCode(int a, int b, int c){
		int code = 1;
		code = code*31 + a;
		code = code*31 + b;
		code = code*31 + c;
		return code;
	}

}
