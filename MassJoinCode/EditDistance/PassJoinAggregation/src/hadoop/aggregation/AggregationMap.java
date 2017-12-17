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
		Path path1 = new Path("length1/"+taskId);
		outputStream1 = fs.create(path1);
		Path path2 = new Path("length2/"+taskId);
		outputStream2 = fs.create(path2);
	}
	
	public void map(Text recordID, Text record, Context context) throws IOException, InterruptedException{
		FileSplit inputSplit = (FileSplit)context.getInputSplit();
		String path = inputSplit.getPath().getName();
		int id = Integer.valueOf(recordID.toString());
		int length = GetData.clean(record.toString()).length();		
		if (path.contains(Config.RECORD1)) {
			length1.add(length);
			outputStream1.writeInt(id);
			outputStream1.writeInt(length);
		} else if (path.contains(Config.RECORD2)) {
			length2.add(length);
			outputStream2.writeInt(id);
			outputStream2.writeInt(length);
		}	
	}
	
	public void cleanup(Context context) throws IOException, InterruptedException{
		for(int length : length1) context.write(mone, new IntWritable(length));
		for(int length : length2) context.write(mtwo, new IntWritable(length));
		outputStream1.close();
		outputStream2.close();
	}
}
