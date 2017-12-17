package core;

public class Tokenizer {

	public static int[] tokenize(String input){
		String[] splits = input.split(Config.TOKEN_SEPARATOR_REGEX);
		int[] data = new int[splits.length];
		for (int i = 0; i < splits.length; i++) 
			data[i]= splits[i].hashCode(); 
        return data;
	}
}
