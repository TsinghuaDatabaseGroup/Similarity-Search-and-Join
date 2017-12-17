package hadoop.aggregation;

import java.io.IOException;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map.Entry;

import org.apache.hadoop.conf.Configurable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataOutputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Mapper.Context;
import org.apache.hadoop.mapreduce.lib.input.FileSplit;

import core.Config;
import core.GetData;
import core.GetSeg;
import core.GetToken;
import core.Hash;

public class AggregationMap extends Mapper<Text, Text, NullWritable, NullWritable>{
	FSDataOutputStream outputStream1; 
	FSDataOutputStream outputStream2;
	
	public void setup(Context context) throws IOException{
		Configuration conf = context.getConfiguration();
		// Group Filter
		FileSystem fs = FileSystem.get(conf);
		String taskId = conf.get("mapred.task.id");
		Path path1 = new Path("group/" + Config.RECORD1 + "/" + taskId);
		outputStream1 = fs.create(path1);
		Path path2 = new Path("group/" + Config.RECORD2 + "/" + taskId);
		outputStream2 = fs.create(path2);
	}

	
	public void map(Text id, Text record, Context context) throws IOException, InterruptedException{
		FileSplit inputSplit = (FileSplit)context.getInputSplit();
		String path = inputSplit.getPath().getName();
		String data = GetData.clean(record.toString());
		if (data.length() <= 0) return;
		byte[] group = GetToken.getTokenGroup(data);
		int rid = Integer.parseInt(id.toString());
		
		if (path.contains(Config.RECORD1)) {
			//write back to hdfs
	    	outputStream1.writeInt(rid);
		    for(int i=0;i<group.length;i++)
		        outputStream1.writeByte(group[i]);
		} else if (path.contains(Config.RECORD2)) {
			//write back to hdfs
	    	outputStream2.writeInt(rid);
		    for(int i=0;i<group.length;i++)
		        outputStream2.writeByte(group[i]);
		}
	}
	
	public void cleanup(Context context) throws IOException, InterruptedException{
		outputStream1.close();
		outputStream2.close();
	}
}