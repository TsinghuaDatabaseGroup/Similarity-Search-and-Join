package core;

public class Config {
  public static final int ED_THRESHOLD = 10;	
  public static final int X=1;
  public static final int NUM_OF_SEGMENT=ED_THRESHOLD+X;

  public static final int NUM_OF_REDUCE = 40;
	public static final int NUM_OF_GROUP = 10;
	
  public static final String INPUT_PATH = "title_15";
  public static final String RECORD1 = "seg";
  public static final String RECORD2 = "sub";

  public static final char TOKEN_SEPARATOR = ' ';
  public static final String TOKEN_SEPARATOR_REGEX = " ";
}
