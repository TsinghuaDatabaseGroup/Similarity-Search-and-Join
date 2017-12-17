package hadoop.passjoin;

import java.io.IOException;
import java.util.ArrayList;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Reducer;


public class JoinReduce extends Reducer<IntWritable, JoinValue, IntWritable, IntWritable>{
	IntWritable outputKey = new IntWritable();
	IntWritable outputValue = new IntWritable();
	
	public void reduce(IntWritable inputKey, Iterable<JoinValue> values, Context context) throws IOException, InterruptedException{
		ArrayList<Integer> segList = new ArrayList<Integer>();		
		ArrayList<Integer> subList = new ArrayList<Integer>();	
		for(JoinValue inputValue:values){
			JoinValue value = new JoinValue(inputValue);
			if(value.getType() == false){
				segList.add(value.getRID());
			} else {
				subList.add(value.getRID());
			}
		}
		
		if (segList.size() == 0 || subList.size() == 0) return;
		
		for(int rid1 : segList){
			outputKey.set(rid1);		
			for(int rid2 : subList) {
				outputValue.set(rid2);
				context.write(outputKey, outputValue);					
			}
		}
	}
}
