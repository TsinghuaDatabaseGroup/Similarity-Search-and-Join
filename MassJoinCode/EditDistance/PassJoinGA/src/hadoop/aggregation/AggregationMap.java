package hadoop.aggregation;

import java.io.IOException;
import java.util.HashSet;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataOutputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.io.VIntWritable;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.lib.input.FileSplit;

import core.Config;
import core.GetData;
import core.GetToken;

public class AggregationMap extends Mapper<Text, Text, VIntWritable, IntWritable>{
	VIntWritable mone = new VIntWritable(-1);
	VIntWritable mtwo = new VIntWritable(-2);
	HashSet<Integer> length1 = new HashSet<Integer>();
	HashSet<Integer> length2 = new HashSet<Integer>();
	FSDataOutputStream outputStream1;
	FSDataOutputStream outputStream2;

	public void setup(Context context) throws IOException{
		Configuration conf = context.getConfiguration();
		FileSystem fs = FileSystem.get(conf);
		String taskId = conf.get("mapred.task.id");
		Path path1 = new Path("group/" + Config.RECORD1 + "/" + taskId);
		outputStream1 = fs.create(path1);
		Path path2 = new Path("group/" + Config.RECORD2 + "/" + taskId);
		outputStream2 = fs.create(path2);
	}
	
	public void map(Text recordID, Text record, Context context) throws IOException, InterruptedException{
		FileSplit inputSplit = (FileSplit)context.getInputSplit();
		String path = inputSplit.getPath().getName();
		
		int id = Integer.valueOf(recordID.toString());
		String data = GetData.clean(record.toString());	
		byte[] group = GetToken.getTokenGroup(data);
		int length = data.length();
		
		if (path.contains(Config.RECORD1)) {
			//write back to hdfs
	    	outputStream1.writeInt(id);
		    for(int i=0;i<group.length;i++)
		        outputStream1.writeByte(group[i]);
			length1.add(length);
		} else if (path.contains(Config.RECORD2)) {
			//write back to hdfs
	    	outputStream2.writeInt(id);
		    for(int i=0;i<group.length;i++)
		        outputStream2.writeByte(group[i]);
			length2.add(length);
		}	
	}
	
	public void cleanup(Context context) throws IOException, InterruptedException{
		for(int length : length1) context.write(mone, new IntWritable(length));
		for(int length : length2) context.write(mtwo, new IntWritable(length));
		outputStream1.close();
		outputStream2.close();
	}
}
