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
import core.JaccardSimilarity;

public class JoinReduce extends Reducer<IntWritable, JoinValue, IntWritable, IntWritable>{
	IntWritable outputKey = new IntWritable();
	IntWritable outputValue = new IntWritable();
	FileInputStream fileStream;
	DataInputStream inputStream;
	HashMap<Integer, short[]> groupMap1 = new HashMap<Integer, short[]>();
	HashMap<Integer, short[]> groupMap2 = new HashMap<Integer, short[]>();
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
					short [] group = new short[Config.NUM_OF_GROUP];
					for(int i=0;i<Config.NUM_OF_GROUP;i++){
						group[i] = inputStream.readShort();
					}
					groupMap1.put(rid, group);
				}			
			} else if (file.contains("group/" + Config.RECORD2)){
				while (inputStream.available() > 0) {
					int rid = inputStream.readInt();
					short[] group = new short[Config.NUM_OF_GROUP];
					for(int i=0;i<Config.NUM_OF_GROUP;i++){
						group[i] = inputStream.readShort();
					}
					groupMap2.put(rid, group);
				}
			} else if (file.contains("length1/")) {
				while (inputStream.available() > 0) {
					length1.put(inputStream.readInt(), inputStream.readInt());
				}
			} else if (file.contains("length2/")) {
				while (inputStream.available() > 0) {
					length2.put(inputStream.readInt(), inputStream.readInt());
				}
			}
		}
	}
	
	public void reduce(IntWritable inputKey, Iterable<JoinValue> values, Context context) throws IOException, InterruptedException{
		ArrayList<Integer> segList = new ArrayList<Integer>();		
		ArrayList<Integer> subList = new ArrayList<Integer>();	
		for(JoinValue value : values){
			if(value.getType() == false){
				segList.add(value.getRID());
			} else {
				subList.add(value.getRID());
			}
		}
		
		if (segList.size() == 0 || subList.size() == 0) return;
		
		for(int rid1 : segList){
			outputKey.set(rid1);
			int rlen1 = length1.get(rid1);
			short [] group1 = groupMap1.get(rid1);		
			for(int rid2 : subList) {
				int rlen2 = length2.get(rid2);
				short [] group2 = groupMap2.get(rid2);	
				int H1 = (int) Math.floor((rlen1 - rlen2 * Config.SIMILARITY)/(1+Config.SIMILARITY) + Config.EPS);
				int H2 = (int) Math.floor((rlen2 - rlen1 * Config.SIMILARITY)/(1+Config.SIMILARITY) + Config.EPS);
				if(JaccardSimilarity.passContentFilter(group1, group2, H1, H2)) {
					outputValue.set(rid2);
					context.write(outputKey, outputValue);					
				}
			}
		}
	}
}