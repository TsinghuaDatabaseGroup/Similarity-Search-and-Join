package hadoop.passjoin;

import java.io.BufferedInputStream;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.util.ArrayList;
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
import core.GetToken;
import core.Hash;

public class JoinMap extends Mapper<Text, Text, IntWritable, JoinValue>{
	int start,end,segLen;
	int subToken,code;
	IntWritable outputKey = new IntWritable();
	JoinValue outputValue = new JoinValue();
	HashMap<Integer, int[]> segMap = new HashMap<Integer, int[]>();
	HashMap<Integer, ArrayList<int[]>> subMap = new HashMap<Integer, ArrayList<int[]>>();
	FileInputStream fileStream;
	DataInputStream inputStream;
	BitSet[] bitSet = new BitSet[Config.NUM_OF_BITSET];
	
	public void setup(Context context) throws IOException{
		Configuration conf = context.getConfiguration();
		Path[] paths = DistributedCache.getLocalCacheFiles(conf);	
		for (Path path : paths) {
			String file = path.toString();
			fileStream = new FileInputStream(file);
			inputStream = new DataInputStream(new BufferedInputStream(fileStream));		
			if(file.contains("segment")){
				while (inputStream.available() > 0) {
					int length = inputStream.readInt();
					int[] segArray = new int[Config.NUM_OF_SEGMENT * 2];
					for (int i = 0; i < Config.NUM_OF_SEGMENT * 2; i++) {
						segArray[i] = inputStream.readInt();
					}
					segMap.put(length, segArray);
				}
			} else if(file.contains("substring")) {
				while (inputStream.available() > 0) {
					int length = inputStream.readInt();
					int size = inputStream.readInt();
					ArrayList<int[]> subList = new ArrayList<int[]>();
					for (int i = 0; i < size; i++) {
						int segID = inputStream.readInt();
						int start = inputStream.readInt();
						int segLen = inputStream.readInt();
						int begin = inputStream.readInt();
						int end = inputStream.readInt();
						int[] substring = {segID, start, segLen, begin, end};
                                      	subList.add(substring);
					}
					subMap.put(length, subList);
				}    
			} else if (file.contains("bloomfilter")) {   // Bloom Filter
				String[] splits = file.split("/");
				int yushu = Integer.parseInt(splits[splits.length - 2]);
				FileInputStream fileStream = new FileInputStream(file);
				DataInputStream inputStream = new DataInputStream(new BufferedInputStream(fileStream));	
				GZIPInputStream gzipInputStream = new GZIPInputStream(inputStream);
				ObjectInputStream objectInputStream = new ObjectInputStream(gzipInputStream);
				try {
					bitSet[yushu] = (BitSet)objectInputStream.readObject();
				} catch (ClassNotFoundException e) {
					e.printStackTrace();
				}
				objectInputStream.close();
			}
		}
	}
	
	
	public void map(Text id, Text record, Context context) throws IOException, InterruptedException {
		FileSplit inputSplit = (FileSplit)context.getInputSplit();
		String path = inputSplit.getPath().toString();
		
		int rid = Integer.parseInt(id.toString());
		String data = GetData.clean(record.toString());
		if (data.length() <= 0) return;
		int length = data.length();
		
		if(path.contains(Config.RECORD1)){
			int[] segments = segMap.get(length);
			for(int segID = 0;segID < Config.NUM_OF_SEGMENT;segID++){
				start = segments[2 * segID];
				segLen = segments[2 * segID + 1];
				subToken = GetToken.getToken(data, start, segLen);
				code = Hash.getHashCode(subToken, segID);
				outputKey.set(code);
				outputValue.set(false, length, rid);
				context.write(outputKey,outputValue);
			}
		}
		
		if(path.contains(Config.RECORD2)){
			ArrayList<int[]> substrings = subMap.get(length);
			//segID, start, seglen, begin, end
			for(int[] sub : substrings){
				subToken = GetToken.getToken(data, sub[1], sub[2]);
				code = Hash.getHashCode(subToken, sub[0]);
				int temp = Math.abs(code) % Config.EXCEPT_NUM_OF_ELEMENT;
				int shang = temp / Config.NUM_OF_BITSET;
				int yushu = temp % Config.NUM_OF_BITSET;
				if (bitSet[yushu].get(shang)) {
					outputKey.set(code);
					outputValue.set(true, rid, sub[3], sub[4]);
					context.write(outputKey, outputValue);
				}
			} 
		}
	}
}
