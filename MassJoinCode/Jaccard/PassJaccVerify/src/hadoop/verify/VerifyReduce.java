package hadoop.verify;


import java.io.IOException;
import java.util.ArrayList;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import core.Config;
import core.GetData;
import core.JaccardSimilarity;
import core.JoinValueWritable;

public class VerifyReduce extends Reducer<IntWritable, JoinValueWritable, Text, NullWritable>{
	Text result = new Text();
    NullWritable nullWritable = NullWritable.get();
    
    public class Record implements Comparable<Record> {
    	public int id;
    	public String record;
    	public Record(int id, String record) {
    		this.id = id;
    		this.record = record;
    	}
    	
		@Override
		public int compareTo(Record o) {
			int tmp = record.compareTo(o.record);
			if (tmp < 0) return -1;
			else if (tmp > 0) return 1;
			else return id - o.id;
		}
    }
    
    public void reduce(IntWritable inputKey, Iterable<JoinValueWritable> inputValue,Context context) throws IOException, InterruptedException{
    	int rid2 = inputKey.get();
    	int[] subRecord = null;
    	double sim = 0;
    	ArrayList<JoinValueWritable> recordList = new ArrayList<JoinValueWritable>();
    	
    	for(JoinValueWritable input : inputValue) {
       		JoinValueWritable value = new JoinValueWritable(input);
    		if(value.getType() == false) {
    			recordList.add(value);
    		} else {
    			subRecord = value.getData();
    		}
    	}
    	
    	for(JoinValueWritable recordValue : recordList){
    		// the last element is its id
    		int[] record = recordValue.getData();
			sim = JaccardSimilarity.getSimilarity(record, subRecord);
			if(sim >= Config.SIMILARITY){
				  result.set(record[record.length - 1] + "," + rid2 + ":"+ sim+"\n");
				  context.write(result, nullWritable);
			}
    	}	
    }
}