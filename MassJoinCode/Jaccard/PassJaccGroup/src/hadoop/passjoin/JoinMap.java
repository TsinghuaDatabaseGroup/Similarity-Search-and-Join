package hadoop.passjoin;

import java.io.BufferedInputStream;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.BitSet;
import java.util.HashMap;
import java.util.zip.GZIPInputStream;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.lib.input.FileSplit;

import core.Config;
import core.GetData;
import core.GetSeg;
import core.GetSub;
import core.GetToken;
import core.Hash;
import core.JoinValueWritable;

public class JoinMap extends Mapper<IntWritable, JoinValueWritable, IntWritable, JoinValue>{
	int start,end,segLen;
	int subToken,code;
	IntWritable outputKey = new IntWritable();
	JoinValue outputValue = new JoinValue();

	
	public void map(IntWritable id, JoinValueWritable record, Context context) throws IOException, InterruptedException {
		FileSplit inputSplit = (FileSplit)context.getInputSplit();
		String path = inputSplit.getPath().toString();
		
		int rid = id.get();
		int[] token = record.getData();
		if (token.length <= 0) return;
		int length = token.length;
		
		if(path.contains(Config.RECORD1)){	    
			int segNum = GetSeg.getSegNum(length);
			int[] segments = GetSeg.getSeg(length, segNum);
			for (int segID = 0; segID < segNum; segID++) {
				start = segments[2*segID];
				segLen = segments[2*segID+1];
				code = GetToken.getHash(token, start, segLen, segID, length);
				outputKey.set(code);
				outputValue.set(true, rid);
				context.write(outputKey, outputValue);
			}    	
		}
		
		if(path.contains(Config.RECORD2)){
			int len1 = (int) Math.floor(length / Config.SIMILARITY);
		    int len2 = (int) Math.ceil(length * Config.SIMILARITY);
			for (int rlen = len2; rlen <= len1; rlen++) {
				int segH = (int) Math.floor((rlen - length * Config.SIMILARITY)/(1 + Config.SIMILARITY) + Config.EPS);
				int subH = (int) Math.floor((length - rlen * Config.SIMILARITY)/(1 + Config.SIMILARITY) + Config.EPS);
				int rSegNum = GetSeg.getSegNum(rlen);
				int[] segments = GetSeg.getSeg(rlen, rSegNum);
				for (int segID = 0; segID < rSegNum; segID++) {
					int start = segments[2 * segID];
					int segLen = segments[2 * segID + 1];
					int end = start + segLen - 1;

					int start1 = start - segH;
					int end1 = end + subH;
					int start2 = start - segID;
					int end2 = end + (length - rlen) + rSegNum - segID;
					start1 = Math.max(start1, start2);
					end1 = Math.min(end1, end2);

					start1 = Math.max(start1, 0);
					end1 = Math.min(end1, length - segLen);

					for (int pos = start1; pos <= end1; pos++) {
						code = GetToken.getHash(token, pos, segLen, rlen);
						outputKey.set(code);
						outputValue.set(true, rid);
						context.write(outputKey, outputValue);
					}
				}
			}
		}
	}
}
