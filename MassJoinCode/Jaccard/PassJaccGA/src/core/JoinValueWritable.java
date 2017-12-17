package core;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;
import java.util.ArrayList;

import org.apache.hadoop.io.Writable;

public class JoinValueWritable implements Writable{
	int[] data;
	boolean type;
	
	public JoinValueWritable(){
		
	}
    
	public JoinValueWritable(JoinValueWritable value){
		set(value.getData(), value.getType());
	}
	
	public void set(JoinValueWritable value){
		set(value.getData(), value.getType());
	}
	
	public void set(ArrayList<Integer> data, boolean type){
		this.type = type;	
		int length = data.size();
		this.data = new int[length];
		for(int i=0;i<length;i++){
			this.data[i] = data.get(i);
		}	
	}
    
	public void set(int[] data, int tail, boolean type){
		this.type = type;
		int length = data.length + 1;
		this.data = new int[length];
		for(int i=0;i<length-1;i++){
			this.data[i] = data[i];
		}
		this.data[length-1] = tail;
	}
	
	public void set(int[] data, boolean type){
		this.type = type;
		int length = data.length;
		this.data = new int[length];
		for(int i=0;i<length;i++){
			this.data[i] = data[i];
		}	
	}
	
	public void set(int rid, int[] data, boolean type){
		this.type = type;
		int length = data.length;
		this.data = new int[length+1];
		for(int i=0;i<length;i++){
			this.data[i] = data[i];
		}
		this.data[length] = rid;
	}
	
	public int[] getData(){
		return data;
	}
	
	public boolean getType(){
		return type;
	}
	
	@Override
	public void readFields(DataInput input) throws IOException {
		// TODO Auto-generated method stub
		int length = input.readInt();
		data = new int[length];
		for(int i=0;i<length;i++){
			data[i] = input.readInt();
		}
		type = input.readBoolean();
	}

	@Override
	public void write(DataOutput output) throws IOException {
		// TODO Auto-generated method stub
		int length = data.length;
		output.writeInt(length);
		for(int i=0;i<length;i++){
			output.writeInt(data[i]);
		}
		output.writeBoolean(type);
	}
}
