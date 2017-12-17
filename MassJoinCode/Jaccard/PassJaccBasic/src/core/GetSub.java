package core;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class GetSub {
	
	
	public static ArrayList<int[]> getSubstrings(int length) {
		HashMap<MapKey, ArrayList<Integer>> hashMap = new HashMap<MapKey, ArrayList<Integer>>();
		
		int len1 = (int) Math.floor(length / Config.SIMILARITY);
	    int len2 = (int) Math.ceil(length * Config.SIMILARITY);
		for (int rlen = len2; rlen <= len1; rlen++) {
			int segH = (int) Math.floor((rlen - length * Config.SIMILARITY)/(1 + Config.SIMILARITY) + Config.EPS);
			int subH = (int) Math.floor((length - rlen * Config.SIMILARITY)/(1 + Config.SIMILARITY) + Config.EPS);
			int rSegNum = GetSeg.getSegNum(rlen);

			int[] segments = GetSeg.getSeg(rlen, rSegNum);
			for (int segID = 0; segID < rSegNum; segID++) {
				int start = segments[2 * segID];
				int segLen = segments[2 * segID + 1];
				int end = start + segLen - 1;

				int start1 = start - segH;
				int end1 = end + subH;
				int start2 = start - segID;
				int end2 = end + (length - rlen) + rSegNum - segID;
				start1 = Math.max(start1, start2);
				end1 = Math.min(end1, end2);

				start1 = Math.max(start1, 0);
				end1 = Math.min(end1, length - segLen);

				for (int pos = start1; pos <= end1; pos++) {
					MapKey key = new MapKey(segID, pos, segLen);
					if (hashMap.containsKey(key)) {
						hashMap.get(key).add(rlen);
					} else {
						ArrayList<Integer> scale = new ArrayList<Integer>();
						scale.add(rlen);
						hashMap.put(key, scale);
					}
				}
			}
		}

		ArrayList<int[]> value = new ArrayList<int[]>();
		for (Map.Entry<MapKey, ArrayList<Integer>> entry : hashMap.entrySet()) {
			MapKey key = entry.getKey();
			ArrayList<Integer> scale = entry.getValue();

			int begin = scale.get(0);
			int end = scale.get(0);
			if (scale.size() == 1) {
				int[] info = { key.getID(), key.getStart(), key.getSegLen(),
						begin, end };
				value.add(info);
			} else {
				for (int i = 1; i < scale.size(); i++) {
					if (scale.get(i) - scale.get(i - 1) != 1) {
						end = scale.get(i - 1);
						int[] info = { key.getID(), key.getStart(),
								key.getSegLen(), begin, end };
						value.add(info);
						begin = scale.get(i);
					}
				}
				end = scale.get(scale.size() - 1);
				int[] info = { key.getID(), key.getStart(), key.getSegLen(),
						begin, end };
				value.add(info);
			}
		}

		return value;
	}
	public static void main(String[] args) {
		for (int length = 100; length < 200; length++) {
			ArrayList<int[]> arrayList = getSubstrings(length);
			/*for (int[] subs : arrayList) {
				System.out.println(subs[0] + " " + subs[1] + " " + subs[2]
						+ " " + subs[3] + " " + subs[4]);
			}*/
			int num = GetSeg.getSegNum(length);
			System.out.println(arrayList.size() + " " + num);
		}
	}
}
