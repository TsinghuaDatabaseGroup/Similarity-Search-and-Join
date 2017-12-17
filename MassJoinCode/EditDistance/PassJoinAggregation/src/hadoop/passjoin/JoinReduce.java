package hadoop.passjoin;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Reducer;

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
		
		int pos1 = 0;
		int pos2 = 0;
		int start = 0;
		int end = 0;
		
		Collections.sort(segList);
		Collections.sort(subList);
		
		for(JoinValue subValue : subList){
			if (start != subValue.getStart()) {
				start = subValue.getStart();
				pos1 = Search.FindStartPos(segList, start);
			}
			if(pos1 == -1) continue;
			if (end != subValue.getEnd()) {
				end = subValue.getEnd();
				pos2 = Search.FindEndPos(segList, end);
			}
			if(pos2 == -1) continue; 
			int rid1 = subValue.getRID();
			outputKey.set(rid1);
			for (int i = pos1; i <= pos2; i++) {
				int rid2 = segList.get(i).getRID();
				outputValue.set(rid2);
				context.write(outputKey, outputValue);
			}
		}
	}
}