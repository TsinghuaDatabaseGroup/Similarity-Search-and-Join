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
import core.Search;

public class JoinReduce extends Reducer<IntWritable, JoinValue, IntWritable, IntWritable>{
	IntWritable outputKey = new IntWritable();
	IntWritable outputValue = new IntWritable();
	
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
			outputValue.set(rid2);
			for (int i = stpos; i <= enpos; i++) {
				int rid1 = segList.get(i).getRID();
				outputKey.set(rid1);
				context.write(outputKey, outputValue);
			}
		}
	}
}