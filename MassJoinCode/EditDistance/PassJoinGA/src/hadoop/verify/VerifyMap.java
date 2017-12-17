package hadoop.verify;

import java.io.IOException;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import core.IntPairWritable;

public class VerifyMap extends Mapper<IntPairWritable, Text, IntPairWritable, Text>{
	    
	public void map(IntPairWritable candidate, Text inputValue, Context context) throws IOException, InterruptedException{
		context.write(candidate, inputValue);
	}
}
