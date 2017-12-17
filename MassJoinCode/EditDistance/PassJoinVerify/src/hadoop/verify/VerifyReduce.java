package hadoop.verify;

import hadoop.recordjoin.RecordJoinOutputValue;

import java.io.IOException;
import java.util.ArrayList;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import core.Config;
import core.EditDistance;
import core.GetData;

public class VerifyReduce extends Reducer<IntWritable, RecordJoinOutputValue, Text, NullWritable>{
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
    
    public void reduce(IntWritable inputKey, Iterable<RecordJoinOutputValue> inputValue,Context context) throws IOException, InterruptedException{
    	int rid1 = inputKey.get();
    	String subRecord = "";
    	ArrayList<RecordJoinOutputValue> recordList = new ArrayList<RecordJoinOutputValue>();
    	
    	for(RecordJoinOutputValue input : inputValue) {
    		if(input.getRID() != -1) {
    			recordList.add(new RecordJoinOutputValue(input));
    		} else {
    			subRecord = GetData.clean(input.getRec());
    		}
    	}
    	
    	for(RecordJoinOutputValue recordValue : recordList){
    		String record = recordValue.getRec();
			int ed = EditDistance.getEDOpt(record, subRecord);
			if(ed <= Config.ED_THRESHOLD){
				  result.set(rid1 + "," + subRecord + ":" + recordValue.getRID() + ","+ record+"\n");
				  context.write(result, nullWritable);
			}
    	}	
    }
}