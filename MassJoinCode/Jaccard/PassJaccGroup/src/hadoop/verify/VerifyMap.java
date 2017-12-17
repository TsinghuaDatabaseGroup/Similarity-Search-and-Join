package hadoop.verify;

import java.io.IOException;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import core.IntPairWritable;
import core.JoinValueWritable;

public class VerifyMap extends Mapper<IntPairWritable, Text, IntPairWritable, JoinValueWritable>{
	public void map(IntPairWritable id, JoinValueWritable inputValue, Context context) throws IOException, InterruptedException{
		context.write(id, inputValue);
	}
}
