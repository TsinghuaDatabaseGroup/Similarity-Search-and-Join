package core;

public class Tokenizer {

	public static String[] tokenize(String input){
		String[] splits = input.split(Config.TOKEN_SEPARATOR_REGEX);
        return splits;
	}
	
}
