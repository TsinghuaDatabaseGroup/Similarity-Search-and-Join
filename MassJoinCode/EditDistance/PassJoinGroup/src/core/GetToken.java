package core;

public class GetToken {
	
	public static int getToken(String record, int start, int length){
		String sub = record.substring(start, start+length);
		int code = sub.hashCode();
		return code;
	}

	public static byte[] getTokenGroup(String record) {
		byte[] group = new byte[Config.NUM_OF_GROUP];
		for (int i = 0; i < Config.NUM_OF_GROUP; i++)
			group[i] = 0;
		for (int index = 0; index < record.length(); index++) {
			int i = Math.abs(record.charAt(index) % Config.NUM_OF_GROUP);
			group[i]++;
		}
		return group;
	}
}
