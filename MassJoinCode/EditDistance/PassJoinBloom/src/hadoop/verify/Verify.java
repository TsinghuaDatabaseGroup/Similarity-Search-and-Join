package hadoop.verify;

import java.io.IOException;
import java.util.Date;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

import core.Config;
import core.IntPairWritable;

public class Verify {
	public static void main(String[] args) throws IOException {
		Configuration configuration = new Configuration();
		JobConf conf = new JobConf(configuration);
		Job job = new Job(conf);
		
		job.setJobName(Verify.class.getSimpleName());
		job.setJarByClass(Verify.class);
		job.setNumReduceTasks(Config.NUM_OF_REDUCE);
		
		job.setMapperClass(VerifyMap.class);
		job.setMapOutputKeyClass(IntPairWritable.class);
		job.setMapOutputValueClass(Text.class);
		job.setReducerClass(VerifyReduce.class);
		job.setOutputKeyClass(Text.class);
		job.setInputFormatClass(SequenceFileInputFormat.class);
		
		FileInputFormat.addInputPath(job, new Path("middle"));
		Path outputPath = new Path("result");
		FileOutputFormat.setOutputPath(job, outputPath);
		
		Date startTime = new Date();
		System.out.println("Verify started: " + startTime);
		
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
		System.out.println("Verify ended: " + end_time);
		System.out.println("The verify take "
				+ (end_time.getTime() - startTime.getTime()) / (float) 1000.0
				+ " seconds.");
	}

}
