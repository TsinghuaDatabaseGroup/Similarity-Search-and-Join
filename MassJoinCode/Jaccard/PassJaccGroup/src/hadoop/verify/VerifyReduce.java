package hadoop.verify;


import java.io.IOException;
import java.util.ArrayList;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import core.Config;
import core.GetData;
import core.IntPairWritable;
import core.JaccardSimilarity;
import core.JoinValueWritable;

public class VerifyReduce extends Reducer<IntPairWritable, JoinValueWritable, Text, NullWritable>{
	Text result = new Text();
    NullWritable nullWritable = NullWritable.get();
    
    public void reduce(IntPairWritable inputKey, Iterable<JoinValueWritable> inputValue,Context context) throws IOException, InterruptedException{
    	int[] str1 = null;
    	int[] str2 = null;
    	boolean flag = false;
    	for(JoinValueWritable input : inputValue) {
    		JoinValueWritable value = new JoinValueWritable(input);
    		if (flag == false) {
    			str1 = value.getData();
    			flag = true;
    		} else {
				str2 = value.getData();
			}
    	}
    	
    	double sim = 0;
		// the last element is its id
		sim = JaccardSimilarity.getSimilarity2(str1, str2);
		if (sim >= Config.SIMILARITY) {
			result.set(inputKey.getFirst() + "," + inputKey.getSecond() + ":"
					+ sim + "\n");
			context.write(result, nullWritable);
		}
    }
}