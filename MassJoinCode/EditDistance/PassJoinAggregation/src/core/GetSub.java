package core;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class GetSub {		
	public static ArrayList<int[]> getSubstrings(int length) {
		HashMap<MapKey, ArrayList<Integer>> hashMap = new HashMap<MapKey, ArrayList<Integer>>();
		
		int len1 = length-Config.ED_THRESHOLD;
		int len2 = length+Config.ED_THRESHOLD;
		for(int rlen=len1;rlen<=len2;rlen++){            
			int[] segments = GetSeg.getSeg(rlen, Config.NUM_OF_SEGMENT);
			for(int segID = 0;segID < Config.NUM_OF_SEGMENT;segID++){
				int start = segments[2 * segID];
				int segLen = segments[2 * segID + 1];
				
				int start1=start-segID;
				int end1=start+segID;
				int start2=start+(length-rlen)-(Config.ED_THRESHOLD+Config.X-(segID+1));
				int end2=start+(length-rlen)+(Config.ED_THRESHOLD+Config.X-(segID+1));
				
				start=Math.max(start1, start2);
				int end=Math.min(end1, end2);
				start=Math.max(start, 0);
				end=Math.min(end, length-segLen);
                		
				for(int pos=start;pos<= end;pos++){
					MapKey key = new MapKey(segID, pos, segLen);
					if(hashMap.containsKey(key)){
						hashMap.get(key).add(rlen);
					}
					else{
						ArrayList<Integer> scale = new ArrayList<Integer>();
						scale.add(rlen);
						hashMap.put(key, scale);
					}
				}
			}
		}
		
		ArrayList<int[]> value = new ArrayList<int[]>();
		for(Map.Entry<MapKey, ArrayList<Integer>> entry : hashMap.entrySet()){
			MapKey key = entry.getKey();
			ArrayList<Integer> scale = entry.getValue();
			
			int begin = scale.get(0);
			int end = scale.get(0);
			if (scale.size() == 1) {
				int[] info = {key.getID(), key.getStart(), key.getSegLen(), begin, end};
				value.add(info);
			} else {
			    for(int i=1;i<scale.size();i++) {
			    	if(scale.get(i)-scale.get(i-1) != 1){
			    		end = scale.get(i-1);
			    		int[] info = {key.getID(), key.getStart(), key.getSegLen(), begin, end};
						value.add(info);
			    		begin = scale.get(i);
			    	}
			    }
			    end = scale.get(scale.size()-1);
			    int[] info = {key.getID(), key.getStart(), key.getSegLen(), begin, end};
				value.add(info);
			}
		}
        return value;
	}
}
