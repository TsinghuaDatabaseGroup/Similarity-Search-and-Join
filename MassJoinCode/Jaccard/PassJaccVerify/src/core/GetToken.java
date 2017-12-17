package core;

public class GetToken {
	
	public static int getToken(String record, int start, int length){
		String sub = record.substring(start, start+length);
		int code = sub.hashCode();
		return code;
	}

	public static int[] getTokenGroup(int[] tokens) {
		int[] group = new int[Config.NUM_OF_GROUP];
		for (int i = 0; i < Config.NUM_OF_GROUP; i++)
			group[i] = 0;
		for (int token : tokens) {
			int i = Math.abs(token % Config.NUM_OF_GROUP);
			group[i]++;
		}
		return group;
	}
	
	public static int getHash(int[] words, int start, int segLen, int segID) {
		int code = segID + 1;
		for (int i = 0; i < segLen; i++) {
			code = code * 31 + words[start + i];
		}
		return code;
	}
}
