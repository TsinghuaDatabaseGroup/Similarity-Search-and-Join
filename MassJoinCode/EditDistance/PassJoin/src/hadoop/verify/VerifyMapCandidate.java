package hadoop.verify;

import hadoop.recordjoin.RecordJoinOutputValue;

import java.io.IOException;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import core.IntPairWritable;

public class VerifyMapCandidate extends Mapper<IntWritable, RecordJoinOutputValue, IntWritable, RecordJoinOutputValue>{
	    
	public void map(IntWritable candidate, RecordJoinOutputValue inputValue, Context context) throws IOException, InterruptedException{
		context.write(candidate, inputValue);
	}
}
