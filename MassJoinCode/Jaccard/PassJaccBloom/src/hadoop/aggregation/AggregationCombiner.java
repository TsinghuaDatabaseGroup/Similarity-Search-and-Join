package hadoop.aggregation;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Map.Entry;


import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.VIntWritable;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.Reducer.Context;

import core.Config;
import core.IntPair;


public class AggregationCombiner extends Reducer<VIntWritable, IntWritable, VIntWritable, IntWritable> {
	VIntWritable outputKey = new VIntWritable();
	IntWritable outputValue = new IntWritable();
	HashSet<Integer> length1 = new HashSet<Integer>();
	HashSet<Integer> length2 = new HashSet<Integer>();
	HashSet<IntPair> segment = new HashSet<IntPair>();

	public void reduce(VIntWritable inputKey, Iterable<IntWritable> values,
			Context context) throws IOException {
		if (inputKey.get() == -1) {
			for (IntWritable value : values) {
				length1.add(value.get());
			}
		} else if (inputKey.get() == -2) {
			for (IntWritable value : values) {
				length2.add(value.get());
			}
		} else {
			for (IntWritable value : values) {
				segment.add(new IntPair(inputKey.get(), value.get()));
			}
		}
	}

	public void cleanup(Context context) throws IOException, InterruptedException {
		outputKey.set(-1);
		for (int len1 : length1) {
			outputValue.set(len1);
			context.write(outputKey, outputValue);
		}

		outputKey.set(-2);
		for (int len2 : length2) {
			outputValue.set(len2);
			context.write(outputKey, outputValue);
		}
		for (IntPair value : segment) {
			outputKey.set(value.getFirst());
			outputValue.set(value.getSecond());
			context.write(outputKey, outputValue);
		}
	}
}