package hadoop.aggregation;

import java.io.IOException;
import java.io.ObjectOutputStream;
import java.util.ArrayList;
import java.util.BitSet;
import java.util.HashSet;
import java.util.zip.GZIPOutputStream;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataOutputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.VIntWritable;
import org.apache.hadoop.mapreduce.Reducer;

import core.Config;
import core.GetSeg;
import core.GetSub;


public class AggregationReduce extends Reducer<VIntWritable, IntWritable, NullWritable, NullWritable>{
	FSDataOutputStream outputStream1; 
	FSDataOutputStream outputStream2; 
	
	public void setup(Context context) throws IOException{
		Configuration conf = context.getConfiguration();
		FileSystem fs = FileSystem.get(conf);
		String taskId = conf.get("mapred.task.id");
		
		Path path1 = new Path("segment/" + taskId);
		outputStream1 = fs.create(path1);
		Path path2 = new Path("substring/" + taskId);
		outputStream2 = fs.create(path2);
	}
	
	public void reduce(VIntWritable inputKey, Iterable<IntWritable> values, Context context) throws IOException{
		int type = inputKey.get();
		Configuration conf = context.getConfiguration();
		FileSystem fs = FileSystem.get(conf);
		String taskId = conf.get("mapred.task.id");

		if (type == -1) {
			HashSet<Integer> lengthes = new HashSet<Integer>();
			for (IntWritable value : values) {
				lengthes.add(value.get());
			}
			for (int length : lengthes) {
				int[] segs = GetSeg.getSeg(length, Config.NUM_OF_SEGMENT);
				outputStream1.writeInt(length);
				for (int i = 0; i < segs.length; i++) {
					outputStream1.writeInt(segs[i]);
				}
			}
		} else if (type == -2){
			HashSet<Integer> lengthes = new HashSet<Integer>();
			for (IntWritable value : values) {
				lengthes.add(value.get());
			}
			for (int length : lengthes) {
				ArrayList<int[]> substrings = GetSub.getSubstrings(length);
				// segID, start, seglen, begin, end
				outputStream2.writeInt(length);
				outputStream2.writeInt(substrings.size());
				for (int[] substring : substrings) {
					for (int i = 0; i < 5; i++) {
						outputStream2.writeInt(substring[i]);
					}
				}
			}
		} else {
			Path path3 = new Path("bloomfilter/" + type + "/" + taskId);
			FSDataOutputStream outputStream3 = fs.create(path3);
			GZIPOutputStream gzipOutputStream3 = new GZIPOutputStream(outputStream3);
			ObjectOutputStream objectOutputStream3 = new ObjectOutputStream(gzipOutputStream3);	
			BitSet bitSet = new BitSet(Config.EXCEPT_NUM_OF_ELEMENT / Config.NUM_OF_BITSET);
			for (IntWritable value : values) {
				bitSet.set(value.get());
			}
			objectOutputStream3.writeObject(bitSet);
			objectOutputStream3.close();
		}
	}
	
	public void cleanup(Context context) throws IOException{
		outputStream1.close();
		outputStream2.close();
	}
}
