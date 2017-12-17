package hadoop.aggregation;

import java.io.IOException;
import java.util.Arrays;
import java.util.HashSet;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataOutputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.SequenceFile;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.io.VIntWritable;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.lib.input.FileSplit;

import core.Config;
import core.GetData;
import core.GetSeg;
import core.GetToken;
import core.Hash;
import core.JoinValueWritable;
import core.Tokenizer;

public class AggregationMap extends Mapper<Text, Text, VIntWritable, IntWritable>{
	VIntWritable mone = new VIntWritable(-1);
	VIntWritable mtwo = new VIntWritable(-2);
	HashSet<Integer> length1 = new HashSet<Integer>();
	HashSet<Integer> length2 = new HashSet<Integer>();
	FSDataOutputStream outputStream1;
	FSDataOutputStream outputStream2;
	FSDataOutputStream outputStream3;
	FSDataOutputStream outputStream4;
	SequenceFile.Writer dataWriter1; 
	SequenceFile.Writer dataWriter2;  
	IntWritable writeKey = new IntWritable();
	JoinValueWritable writeValue = new JoinValueWritable();
	
	public void setup(Context context) throws IOException{
		Configuration conf = context.getConfiguration();
		FileSystem fs = FileSystem.get(conf);
		String taskId = conf.get("mapred.task.id");
		Path path1 = new Path("group/" + Config.RECORD1 + "/" + taskId);
		outputStream1 = fs.create(path1);
		Path path2 = new Path("group/" + Config.RECORD2 + "/" + taskId);
		outputStream2 = fs.create(path2);
		Path path3 = new Path("length1/"+taskId);
		outputStream3 = fs.create(path3);
		Path path4 = new Path("length2/"+taskId);
		outputStream4 = fs.create(path4);
		Path rePath1 = new Path("record1/"+taskId); 	
		dataWriter1 = SequenceFile.createWriter(fs, conf, rePath1,  writeKey.getClass(), writeValue.getClass()); 
		Path rePath2 = new Path("record2/"+taskId);
		dataWriter2 = SequenceFile.createWriter(fs, conf, rePath2,  writeKey.getClass(), writeValue.getClass()); 
	}
	
	public void map(Text recordID, Text record, Context context) throws IOException, InterruptedException{
		FileSplit inputSplit = (FileSplit)context.getInputSplit();
		String path = inputSplit.getPath().getName();
		int id = Integer.valueOf(recordID.toString());
		String rec = GetData.clean(record.toString());
		int[] data = Tokenizer.tokenize(rec);
		int[] group = GetToken.getTokenGroup(data);
		int length = data.length;
		Arrays.sort(data);
		
		if (path.contains(Config.RECORD1)) {
			//write back to hdfs
	    	outputStream1.writeInt(id);
		    for(int i=0;i<group.length;i++)
		        outputStream1.writeShort(group[i]);
		    
	    	writeKey.set(id);
			writeValue.set(data, true);
	    	dataWriter1.append(writeKey, writeValue);
	    	
			length1.add(length);
			outputStream3.writeInt(id);
			outputStream3.writeInt(length);
		} else if (path.contains(Config.RECORD2)) {
			//write back to hdfs
	    	outputStream2.writeInt(id);
		    for(int i=0;i<group.length;i++)
		        outputStream2.writeShort(group[i]);
		    
	    	writeKey.set(id);
			writeValue.set(data, true);
	    	dataWriter2.append(writeKey, writeValue);
	    	
			length2.add(length);
			outputStream4.writeInt(id);
			outputStream4.writeInt(length);
		}	
	}
	
	public void cleanup(Context context) throws IOException, InterruptedException{
		for(int length : length1) context.write(mone, new IntWritable(length));
		for(int length : length2) context.write(mtwo, new IntWritable(length));
		outputStream1.close();
		outputStream2.close();
		outputStream3.close();
		outputStream4.close();
		dataWriter1.close();
		dataWriter2.close();
	}
}
