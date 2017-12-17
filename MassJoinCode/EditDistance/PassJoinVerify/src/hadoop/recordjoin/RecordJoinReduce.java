package hadoop.recordjoin;

import java.io.IOException;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.io.Writable;
import org.apache.hadoop.mapreduce.Reducer;

import core.Config;
import core.GetData;
import core.IntPairWritable;
import core.MultiValueWritable;

public class RecordJoinReduce extends Reducer<IntWritable, MultiValueWritable, IntWritable, RecordJoinOutputValue>{
	
	IntWritable outputKey = new IntWritable();
	RecordJoinOutputValue outputValue = new RecordJoinOutputValue();
	
	public void reduce(IntWritable inputKey, Iterable<MultiValueWritable> inputValue,Context context) throws IOException, InterruptedException{
		int rid1 = inputKey.get();
		HashSet<Integer> ridSet = new HashSet<Integer>();
		for(MultiValueWritable input : inputValue) {
			Writable value = input.get();
			if (value instanceof Text) {
		    	outputValue.set(rid1, ((Text)value).toString());
			} else {
				ridSet.add(((IntWritable)value).get());
		    }
		}
		
		for (Integer rid2 : ridSet) {
			outputKey.set(rid2);
	 		context.write(outputKey, outputValue);
		}
	}
}