package hadoop.recordjoin;

import java.io.IOException;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Mapper;

import core.JoinValueWritable;
import core.MultiValueWritable;

public class RecordJoinMap extends Mapper<IntWritable, JoinValueWritable, IntWritable, MultiValueWritable>{
    
	public void map(IntWritable rid, JoinValueWritable inputValue, Context context) throws IOException, InterruptedException{
		context.write(rid, new MultiValueWritable(inputValue));
	}
}
