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

public class RecordJoinReduce extends Reducer<IntWritable, MultiValueWritable, IntPairWritable, Text>{
	
	IntPairWritable outputKey = new IntPairWritable();
	Text outputValue = new Text();

	public void reduce(IntWritable inputKey, Iterable<MultiValueWritable> inputValue,Context context) throws IOException, InterruptedException{
		int rid1 = inputKey.get();
		HashSet<Integer> ridSet = new HashSet<Integer>();
		String record = "";
		
		for(MultiValueWritable input : inputValue) {
			Writable value = input.get();
			if (value instanceof Text) {
		    	record = GetData.clean(((Text)value).toString());
			} else {
				ridSet.add(((IntWritable)value).get());
		    }
		}
		
		outputValue.set(record);
		for (Integer rid2 : ridSet) {
			if (rid1 < rid2)
				outputKey.set(rid1, rid2);
			else
				outputKey.set(rid2, rid1);
	 		context.write(outputKey, outputValue);
		}
	}
}