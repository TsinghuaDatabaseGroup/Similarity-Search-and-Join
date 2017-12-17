package hadoop.passjoin;

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
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.KeyValueTextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

import core.Config;

public class JoinKernel {
	public static void main(String[] args) throws IOException {
		Configuration configuration = new Configuration();
		addCacheFiles(configuration,"segment");
		addCacheFiles(configuration,"substring");
		addCacheFiles(configuration, "group/"+Config.RECORD1);
		addCacheFiles(configuration, "group/"+Config.RECORD2);

		JobConf conf = new JobConf(configuration);
		Job job = new Job(conf);
		
		job.setJobName(JoinKernel.class.getSimpleName());
		job.setJarByClass(JoinKernel.class);
		job.setNumReduceTasks(Config.NUM_OF_REDUCE);	
		
		job.setMapperClass(JoinMap.class);
		job.setReducerClass(JoinReduce.class);
		job.setMapOutputKeyClass(IntWritable.class);
		job.setMapOutputValueClass(JoinValue.class);
		job.setOutputKeyClass(IntWritable.class);
		job.setOutputValueClass(IntWritable.class);
		job.setInputFormatClass(KeyValueTextInputFormat.class);
		job.setOutputFormatClass(SequenceFileOutputFormat.class);
		
		FileInputFormat.addInputPath(job, new Path(Config.INPUT_PATH+"/"+Config.RECORD1));
		FileInputFormat.addInputPath(job, new Path(Config.INPUT_PATH+"/"+Config.RECORD2));
		Path outputPath = new Path("candidates");
		FileOutputFormat.setOutputPath(job, outputPath);
		
		Date startTime = new Date();
		System.out.println("Pass-Join started: " + startTime);
		
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
		System.out.println("Pass-Join ended: " + end_time);
		System.out.println("The pass-join take "
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
