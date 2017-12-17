package core;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class EditDistance {
	private static final int LONGEST = 100000;
	public static int[][] matrix=new int[LONGEST][2 * Config.ED_THRESHOLD + 1];
	static {
		for (int i = 0; i <= Config.ED_THRESHOLD; i++)
			matrix[0][i + Config.ED_THRESHOLD] = i;
	}
	public EditDistance() {
	}
	
	public static int getED(String x, String y) {
		int i,j,dis;
		int xLen=x.length();
		int yLen=y.length();
		int[][] Dis=new int[xLen+1][yLen+1];
		for(i=0;i<=xLen;i++) Dis[i][0]=i;
		for(i=0;i<=yLen;i++) Dis[0][i]=i;
		for(i=1;i<=xLen;i++){
			for(j=1;j<=yLen;j++){
				if(x.charAt(i-1)==y.charAt(j-1))
					Dis[i][j]=Dis[i-1][j-1];
				else
					Dis[i][j]=min(Dis[i - 1][j - 1], Dis[i][j - 1], Dis[i - 1][j]) + 1;
			}
		}
		dis=Dis[xLen][yLen];
		return dis;	
	}
	
	public static int getEDOpt(String x, String y) {
		int xlen = x.length();
		int ylen = y.length();
		int delta = ylen - xlen;
		int left = (Config.ED_THRESHOLD - delta) / 2;
		int right = (Config.ED_THRESHOLD + delta) / 2;
		int D = Config.ED_THRESHOLD;
		if (delta > Config.ED_THRESHOLD || delta < Config.ED_THRESHOLD * -1) return D + 1;
		boolean valid = false;
		for (int i = 1; i <= xlen; i++) {
			valid = false;
			if (i <= left) {
				matrix[i][D - i] = i;
				valid = true;
			}
			for (int j = (i - left >= 1 ? i - left : 1); 
			j <= (i + right <= ylen ? i + right : ylen); j++) {
				if (x.charAt(i - 1) == y.charAt(j - 1)) {
					matrix[i][j - i + D] = matrix[i - 1][j - i + D];
				} else {
					matrix[i][j - i + D] = min(matrix[i - 1][j - i + D],
							j - 1 >= i - left ? matrix[i][j - i + D - 1] : D,
							j + 1 <= i + right ? matrix[i - 1][j - i + D + 1] : D) + 1;
				}
				if (Math.abs(xlen - ylen - i + j) + matrix[i][j - i + D] <= D) {
					valid = true;
				}
			}
			if (!valid)
				return D + 1;
		}
		return matrix[xlen][ylen - xlen + D];
	}
	
	private static int min(int a, int b, int c) {
		if (a <= b) {
			if (a <= c) return a;
			return c;
		}
		if (b <= c) return b;
		return c;
	}
	
	public static void main(String[] args) throws IOException {
		BufferedReader buf = new BufferedReader(new InputStreamReader(System.in));
		while (true) {
			String a = (String)buf.readLine();
			String b = (String)buf.readLine();
			System.out.println(getED(a, b) + " " + getEDOpt(a, b));
		}
	}
}
