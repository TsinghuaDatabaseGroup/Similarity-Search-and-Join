package hadoop.recordjoin;

import java.io.IOException;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import core.MultiValueWritable;

public class RecordJoinMapCandidate extends Mapper<IntWritable, IntWritable, IntWritable, MultiValueWritable>{
    
	public void map(IntWritable rid, IntWritable sid, Context context) throws IOException, InterruptedException{
		context.write(rid, new MultiValueWritable(sid));
		context.write(sid, new MultiValueWritable(rid));
	}
}
