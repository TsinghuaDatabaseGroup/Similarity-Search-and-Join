package hadoop.passjoin;

import java.io.BufferedInputStream;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;

import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.Reducer.Context;

import core.Config;
import core.EditDistance;
import core.Search;

public class JoinReduce extends Reducer<IntWritable, JoinValue, IntWritable, IntWritable>{
	IntWritable outputKey = new IntWritable();
	IntWritable outputValue = new IntWritable();
	FileInputStream fileStream;
	DataInputStream inputStream;
	HashMap<Integer, byte[]> groupMap1 = new HashMap<Integer, byte[]>();
	HashMap<Integer, byte[]> groupMap2 = new HashMap<Integer, byte[]>();
	HashMap<Integer, Integer> length1 = new HashMap<Integer, Integer>();
	HashMap<Integer, Integer> length2 = new HashMap<Integer, Integer>();

	public void setup(Context context) throws IOException{
        Path[] files = DistributedCache.getLocalCacheFiles(context.getConfiguration());	
		for (Path path : files) {
			String file = path.toString();
			fileStream = new FileInputStream(file);
			inputStream = new DataInputStream(new BufferedInputStream(fileStream));		
			if(file.contains("group/" + Config.RECORD1)){
				while (inputStream.available() > 0) {
					int rid = inputStream.readInt();
					byte [] group = new byte[Config.NUM_OF_GROUP];
					int length = 0;
					for(int i=0;i<Config.NUM_OF_GROUP;i++){
						group[i] = inputStream.readByte();
						length += group[i];
					}
					groupMap1.put(rid, group);
					length1.put(rid, length);
				}			
			} else if (file.contains("group/" + Config.RECORD2)){
				while (inputStream.available() > 0) {
					int rid = inputStream.readInt();
					byte[] group = new byte[Config.NUM_OF_GROUP];
					int length = 0;
					for(int i=0;i<Config.NUM_OF_GROUP;i++){
						group[i] = inputStream.readByte();
						length += group[i];
					}
					groupMap2.put(rid, group);
					length2.put(rid, length);
				}
			}
		}
	}
	
	public void reduce(IntWritable inputKey, Iterable<JoinValue> values, Context context) throws IOException, InterruptedException{
		ArrayList<JoinValue> segList = new ArrayList<JoinValue>();		
		ArrayList<JoinValue> subList = new ArrayList<JoinValue>();	
		for(JoinValue inputValue:values){
			JoinValue value = new JoinValue(inputValue);
			if(value.getType() == false){
				segList.add(value);
			} else {
				subList.add(value);
			}
		}
		
		if (segList.size() == 0 || subList.size() == 0) return;
		
		int stpos = 0;
		int enpos = 0;
		int start = 0;
		int end = 0;
		
		Collections.sort(segList);
		Collections.sort(subList);
		
		for(JoinValue subValue : subList){
			if (start != subValue.getStart()) {
				start = subValue.getStart();
				stpos = Search.FindStartPos(segList, start);
			}
			if(stpos == -1) continue;
			if (end != subValue.getEnd()) {
				end = subValue.getEnd();
				enpos = Search.FindEndPos(segList, end);
			}
			if(enpos == -1) continue; 
			int rid2 = subValue.getRID();
			byte [] group2 = groupMap2.get(rid2);			
			int rlen2 = length2.get(rid2);
			outputValue.set(rid2);
			for (int i = stpos; i <= enpos; i++) {
				int rid1 = segList.get(i).getRID();
				if(EditDistance.passContentFilter(group2, groupMap1.get(rid1), rlen2, length1.get(rid1))){
					outputKey.set(rid1);
					context.write(outputKey, outputValue);
				}
			}
		}
	}
}