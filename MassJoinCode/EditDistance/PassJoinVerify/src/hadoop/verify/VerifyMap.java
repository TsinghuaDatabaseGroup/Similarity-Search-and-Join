package hadoop.verify;

import hadoop.recordjoin.RecordJoinOutputValue;

import java.io.IOException;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import core.IntPairWritable;

public class VerifyMap extends Mapper<Text, Text, IntWritable, RecordJoinOutputValue>{
	IntWritable outputKey = new IntWritable();
	RecordJoinOutputValue outputValue = new RecordJoinOutputValue();
	
	public void map(Text id, Text inputValue, Context context) throws IOException, InterruptedException{
		int rid = Integer.parseInt(id.toString());
		outputKey.set(rid);
		outputValue.set(-1, inputValue.toString());
		context.write(outputKey, outputValue);
	}
}
