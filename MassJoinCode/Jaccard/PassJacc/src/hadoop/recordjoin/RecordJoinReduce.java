package hadoop.recordjoin;

import java.io.BufferedInputStream;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;

import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.io.Writable;
import org.apache.hadoop.mapreduce.Reducer;

import core.Config;
import core.GetData;
import core.GetSeg;
import core.IntPairWritable;
import core.JoinValueWritable;
import core.MultiValueWritable;

public class RecordJoinReduce extends Reducer<IntWritable, MultiValueWritable, IntWritable, JoinValueWritable>{
	IntWritable outputKey = new IntWritable();
	JoinValueWritable outputValue = new JoinValueWritable();
	FileInputStream fileStream;
	DataInputStream inputStream;
	HashMap<Integer , Integer> lenMap1 = new HashMap<Integer , Integer>();
	HashMap<Integer , Integer> lenMap2 = new HashMap<Integer , Integer>();
	
	public void setup(Context context) throws IOException{
        Path[] files = DistributedCache.getLocalCacheFiles(context.getConfiguration());	
		for (Path path : files) {
			String file = path.toString();
			fileStream = new FileInputStream(file);
			inputStream = new DataInputStream(new BufferedInputStream(fileStream));		
			if (file.contains("length1/")) {
				while (inputStream.available() > 0) {
					lenMap1.put(inputStream.readInt(), inputStream.readInt());
				}
			} else if (file.contains("length2/")) {
				while (inputStream.available() > 0) {
					lenMap2.put(inputStream.readInt(), inputStream.readInt());
				}
			}
		}
	}
	
	public void reduce(IntWritable inputKey, Iterable<MultiValueWritable> inputValue,Context context) throws IOException, InterruptedException{
		int rid1 = inputKey.get();
		HashMap<Integer,Integer> ridMap = new HashMap<Integer,Integer>();
		
		for(MultiValueWritable input : inputValue) {
			Writable value = input.get();
			if (value instanceof JoinValueWritable) {
				int[] data = ((JoinValueWritable)value).getData();
		    	outputValue.set(data, rid1, false);
			} else {
				int rid2 = ((IntWritable)value).get();
				if (ridMap.containsKey(rid2)) {
					ridMap.put(rid2, ridMap.get(rid2) + 1);
				} else {
					ridMap.put(rid2, 1);
				}
		    }
		}
		
		int length1 = lenMap1.get(rid1);
		int H1 = 0,H2 = 0;

		for(Map.Entry<Integer, Integer> entry : ridMap.entrySet()){
			int rid2 = entry.getKey();
			int length2 = lenMap2.get(rid2);
			//frequence
			int SegNum =  GetSeg.getSegNum(length1);
			H1 = (int) Math.floor((length1 - length2 * Config.SIMILARITY)/(1+Config.SIMILARITY) + Config.EPS);
			H2 = (int) Math.floor((length2 - length1 * Config.SIMILARITY)/(1+Config.SIMILARITY) + Config.EPS);
			int frequence = SegNum - H1 - H2;
			if(entry.getValue() >= frequence){   
				outputKey.set(rid2);
				context.write(outputKey, outputValue);
			}
		}
	}
}