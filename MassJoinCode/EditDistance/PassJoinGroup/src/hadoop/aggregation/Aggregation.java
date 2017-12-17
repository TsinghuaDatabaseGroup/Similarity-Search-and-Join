package hadoop.aggregation;

import java.io.IOException;
import java.util.Date;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.VIntWritable;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapred.lib.IdentityReducer;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.KeyValueTextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

import core.Config;

public class Aggregation {
	public static void main(String[] args) throws IOException {
		Configuration configuration = new Configuration();	
		JobConf conf = new JobConf(configuration);
		Job job = new Job(conf);
		
		job.setJobName(Aggregation.class.getSimpleName());
		job.setJarByClass(Aggregation.class);
		job.setNumReduceTasks(0);
		
		job.setMapperClass(AggregationMap.class);
		// job.setMapOutputKeyClass(NullWritable.class);
		// job.setMapOutputValueClass(NullWritable.class);
		
        job.setInputFormatClass(KeyValueTextInputFormat.class);
		
		FileInputFormat.addInputPath(job, new Path(Config.INPUT_PATH+"/"+Config.RECORD1));
		FileInputFormat.addInputPath(job, new Path(Config.INPUT_PATH+"/"+Config.RECORD2));
		Path outputPath = new Path("unused");
		FileOutputFormat.setOutputPath(job, outputPath);
		
		Date startTime = new Date();
		System.out.println("Aggregation started: " + startTime);
		
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
		System.out.println("Aggregation ended: " + end_time);
		System.out.println("The aggregation take "
				+ (end_time.getTime() - startTime.getTime()) / (float) 1000.0
				+ " seconds.");
	}

}
