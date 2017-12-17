package core;

import java.util.regex.Pattern;

public class GetData {
	    private static final Pattern rePunctuation = Pattern.compile("[^\\p{L}\\p{N}]"); // L:Letter, N:Number
	    private static final Pattern reSpaceOrAnderscore = Pattern.compile("(_|\\s)+");

	    public static String clean(String in) {
	    	return in;
	    }
	    
	    public static String clean2(String in) {
	        in = rePunctuation.matcher(in).replaceAll(" ");
	        in = reSpaceOrAnderscore.matcher(in).replaceAll(" ");
	        in = in.trim();
	    	in = in.toLowerCase();
	        in = in.replace("[^a-z0-9]", " ");
	        return in;
	    }
}
