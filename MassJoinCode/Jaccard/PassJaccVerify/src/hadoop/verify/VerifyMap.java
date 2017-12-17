package hadoop.verify;

import java.io.IOException;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import core.JoinValueWritable;

public class VerifyMap extends Mapper<IntWritable, Text, IntWritable, JoinValueWritable>{
	public void map(IntWritable id, JoinValueWritable inputValue, Context context) throws IOException, InterruptedException{
		context.write(id, inputValue);
	}
}
