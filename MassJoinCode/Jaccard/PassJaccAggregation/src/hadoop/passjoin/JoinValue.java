package hadoop.passjoin;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;

import org.apache.hadoop.io.Writable;

public class JoinValue implements Writable,Comparable<JoinValue>{
	boolean type;
	int rid; 
	int start;
	int end;
	
	public JoinValue(){
	        
	}
	
	public JoinValue(JoinValue value){
		this.type = value.getType();
		this.rid = value.getRID();
		this.start = value.getStart();
		this.end = value.getEnd();
	}
	
	public void set(boolean type, int length, int rid){
		this.type = type;
        this.start = length;
		this.rid = rid;
	}
	
	public void set(boolean type, int rid, int start, int end){
		this.type = type;
		this.rid = rid;
		this.start = start;
		this.end = end;
	}
		
	public boolean getType(){
		return type;
	}
	
	public int getLength(){
		return start;
	}
	
	public int getRID(){
		return rid;	
	}
	
	public int getStart(){
		return start;
	}
	
	public int getEnd(){
		return end;
	}
	
	@Override
	public void readFields(DataInput input) throws IOException {
		// TODO Auto-generated method stub
		type = input.readBoolean();
		rid = input.readInt();	
		start = input.readInt();
		end = input.readInt();
	}

	@Override
	public void write(DataOutput output) throws IOException {
		// TODO Auto-generated method stub
		output.writeBoolean(type);
		output.writeInt(rid);
		output.writeInt(start);
		output.writeInt(end);
	}
 
	public int compareTo(JoinValue value){
		return this.start > value.start? 1:(this.start < value.start? -1:0);
	}
}
