package hadoop.recordjoin;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;

import org.apache.hadoop.io.Writable;

public class RecordJoinOutputValue implements Writable {
	int rid; 
	String rec;
	
	public RecordJoinOutputValue(){
	}
	
	public RecordJoinOutputValue(RecordJoinOutputValue value){
		this.rid = value.getRID();
		this.rec = value.getRec();
	}
	
	public void set(int rid, String rec){
		this.rid = rid;
		this.rec = rec;
	}
	
	public int getRID(){
		return rid;
	}
	
	public String getRec(){
		return rec;
	}
	
	@Override
	public void readFields(DataInput input) throws IOException {
		// TODO Auto-generated method stub
		rid = input.readInt();	
		rec = input.readUTF();
	}

	@Override
	public void write(DataOutput output) throws IOException {
		// TODO Auto-generated method stub
		output.writeInt(rid);
		output.writeUTF(rec);
	}
}
