package hadoop.recordjoin;

import java.io.IOException;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import core.MultiValueWritable;

public class RecordJoinMap extends Mapper<Text, Text, IntWritable, MultiValueWritable>{
    IntWritable outputKey = new IntWritable();
    
	public void map(Text rid, Text inputValue, Context context) throws IOException, InterruptedException{
		outputKey.set(Integer.parseInt(rid.toString()));
		context.write(outputKey, new MultiValueWritable(inputValue));
	}
}
