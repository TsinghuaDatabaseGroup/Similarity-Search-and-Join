package hadoop.verify;

import java.io.IOException;
import java.util.ArrayList;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import core.Config;
import core.EditDistance;
import core.GetData;
import core.IntPairWritable;

public class VerifyReduce extends Reducer<IntPairWritable, Text, Text, NullWritable>{
	Text result = new Text();
    NullWritable nullWritable = NullWritable.get();
    
    public void reduce(IntPairWritable inputKey, Iterable<Text> inputValue,Context context) throws IOException, InterruptedException{
    	String str1 = "";
    	String str2 = "";
    	boolean flag = false;
    	for(Text input : inputValue) {
    		if (flag == false) {
    			str1 = input.toString();
    			flag = true;
    		} else {
				str2 = input.toString();
			}
    	}
    	
		int ed = EditDistance.getEDOpt(str1, str2);
		if(ed <= Config.ED_THRESHOLD){
			result.set(inputKey.getFirst() + "," + inputKey.getSecond() + ":" + ed + "\n" + str1 + "\n" + str2 + "\n\n");
			context.write(result, nullWritable);
    	}	
    }
}