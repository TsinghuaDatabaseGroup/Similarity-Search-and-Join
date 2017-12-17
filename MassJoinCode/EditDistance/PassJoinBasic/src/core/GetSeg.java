package core;

public class GetSeg {
	private static int start[];
	private static int length[];

	public static void getSeg(int len, int num) {
		start = new int[num];
		length = new int[num];

		int begin = 0;
		int k = (int) (len - Math.floor((double) len / num) * num);
		int len1 = (int) Math.floor((double) len / num);
		int len2 = (int) Math.ceil((double) len / num);
		for (int i = 0; i < num; i++) {
			start[i] = begin;
			if (i < num - k) {
				length[i] = len1;
			} else
				length[i] = len2;
			begin = begin + length[i];
		}
	}

	public static int getStart(int i) {
		return start[i];
	}

	public static int getLength(int i) {
		if (length[i] < 0) {
			return 0;
		} else {
			return length[i];
		}
	}
}
