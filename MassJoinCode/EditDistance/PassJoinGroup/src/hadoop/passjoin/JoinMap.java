package hadoop.passjoin;

import java.io.IOException;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.lib.input.FileSplit;

import core.Config;
import core.GetData;
import core.GetSeg;
import core.GetToken;
import core.Hash;

public class JoinMap extends Mapper<Text, Text, IntWritable, JoinValue>{
	int start,end,segLen;
	int subToken,code;
	IntWritable outputKey = new IntWritable();
	JoinValue outputValue = new JoinValue();
	
	public void map(Text id, Text record, Context context) throws IOException, InterruptedException {
		FileSplit inputSplit = (FileSplit)context.getInputSplit();
		String path = inputSplit.getPath().toString();
		
		int rid = Integer.parseInt(id.toString());
		String data = GetData.clean(record.toString()); //if(rid == 11||rid == 1039867)System.out.println(data);
		if (data.length() <= 0) return;
		int length = data.length();
		
		if(path.contains(Config.RECORD1)){
			GetSeg.getSeg(length, Config.NUM_OF_SEGMENT);
			for(int segID = 0;segID < Config.NUM_OF_SEGMENT;segID++){
				start = GetSeg.getStart(segID);
				segLen = GetSeg.getLength(segID);
				subToken = GetToken.getToken(data, start, segLen);
				code = Hash.getHashCode(subToken, segID, length);
				outputKey.set(code);
				outputValue.set(false, rid);
				context.write(outputKey,outputValue);
			}
		}
		
		if(path.contains(Config.RECORD2)){
			for(int rlen=length-Config.ED_THRESHOLD;rlen<=length+Config.ED_THRESHOLD;rlen++){
				GetSeg.getSeg(rlen, Config.NUM_OF_SEGMENT);
				for(int segID = 0;segID < Config.NUM_OF_SEGMENT;segID++){
					start = GetSeg.getStart(segID);	
					segLen = GetSeg.getLength(segID);
					int start1=start-segID;
					int end1=start+segID;
					int start2=start+(length-rlen)-(Config.ED_THRESHOLD+Config.X-(segID+1));
					int end2=start+(length-rlen)+(Config.ED_THRESHOLD+Config.X-(segID+1));
					start=Math.max(start1, start2);
					end=Math.min(end1, end2);
					start=Math.max(start, 0);
					end=Math.min(end, length-segLen);
	                for(int pos=start;pos<=end;pos++){
	                	subToken = GetToken.getToken(data, pos, segLen);
	                	code = Hash.getHashCode(subToken, segID, rlen);
	    				outputKey.set(code);
	    				outputValue.set(true, rid);
	    				context.write(outputKey,outputValue);
	                }
				}
			}
		}
	}
}