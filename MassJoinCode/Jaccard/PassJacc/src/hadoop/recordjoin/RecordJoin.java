package hadoop.recordjoin;

import java.io.IOException;
import java.util.Date;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.fs.FileStatus;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.KeyValueTextInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.input.MultipleInputs;

import core.Config;
import core.IntPairWritable;
import core.JoinValueWritable;
import core.MultiValueWritable;

public class RecordJoin {
	public static void main(String[] args) throws IOException {
		Configuration configuration = new Configuration();
		addCacheFiles(configuration, "length2/");
		addCacheFiles(configuration, "length1/");
		JobConf conf = new JobConf(configuration);
		Job job = new Job(conf);
		
		job.setJobName(RecordJoin.class.getSimpleName());
		job.setJarByClass(RecordJoin.class);
		job.setNumReduceTasks(Config.NUM_OF_REDUCE);

		MultipleInputs.addInputPath(job, new Path("record1/"), SequenceFileInputFormat.class, RecordJoinMap.class);
		MultipleInputs.addInputPath(job, new Path("candidates"), SequenceFileInputFormat.class, RecordJoinMapCandidate.class);

		job.setMapOutputKeyClass(IntWritable.class);
		job.setMapOutputValueClass(MultiValueWritable.class);
		job.setReducerClass(RecordJoinReduce.class);
		job.setOutputKeyClass(IntWritable.class);
		job.setOutputValueClass(JoinValueWritable.class);
		
	
		Path outputPath = new Path("middle");
		FileOutputFormat.setOutputPath(job, outputPath);	
		job.setOutputFormatClass(SequenceFileOutputFormat.class);
		
		Date startTime = new Date();
		System.out.println("Record-Join started: " + startTime);
		
		try {
			job.waitForCompletion(true);
		} catch (ClassNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		Date end_time = new Date();
		System.out.println("Record-Join ended: " + end_time);
		System.out.println("The record-join take "
				+ (end_time.getTime() - startTime.getTime()) / (float) 1000.0
				+ " seconds.");
	}
	
	private static void addCacheFiles(Configuration configuration, String path) throws IOException {
		FileSystem fileSystem = FileSystem.get(new Path(path).toUri(), configuration);
		FileStatus fileList[] = fileSystem.listStatus(new Path(path));
		for (FileStatus fileStatus : fileList) {
			if (!fileStatus.isDir()) {
				DistributedCache.addCacheFile(
						fileStatus.getPath().toUri(), 
						configuration);
			}
		}
		fileSystem.close();
	}
}
