package core;

public class Config {
	
  public static double SIMILARITY = 0.85;
  public static final int NUM_OF_REDUCE = 40;
  public static final double EPS = 0.00000001;
  public static final int NUM_OF_GROUP = 20;
  public static final int NUM_OF_BITSET = 4;
  public static final int EXCEPT_NUM_OF_ELEMENT = 840000000;

  public static final String INPUT_PATH = "title_10";
  public static final String RECORD1 = "seg";
  public static final String RECORD2 = "sub";

  public static final char TOKEN_SEPARATOR = ' ';
  public static final String TOKEN_SEPARATOR_REGEX = " ";
}
