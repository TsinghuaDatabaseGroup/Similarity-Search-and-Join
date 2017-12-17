package hadoop.passjoin;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;

import org.apache.hadoop.io.Writable;

public class JoinValue implements Writable{
	private boolean type;//0:segment 1:substring
	private int rid;//id of record
	
	public JoinValue(){
		
	}
	
	public JoinValue(boolean type,int rid){	
		this.type=type;
		this.rid=rid;
	}
	
	public JoinValue(JoinValue value){
		this.type = value.type;
		this.rid = value.rid;
	}
	
	public void set(boolean type,int rid){
		this.type=type;
		this.rid=rid;
	}
	
	public boolean getType(){
		return type;
	}
	
	public void setType(boolean type){
		this.type=type;
	}
	
	public int getRID(){
		return rid;	
	}
	
	public void setRID(int rid){
		this.rid=rid;
	}

	@Override
	public void readFields(DataInput input) throws IOException {
		// TODO Auto-generated method stub
		type = input.readBoolean();
		rid = input.readInt();	
	}

	@Override
	public void write(DataOutput output) throws IOException {
		// TODO Auto-generated method stub
		output.writeBoolean(type);
		output.writeInt(rid);
	}
}
