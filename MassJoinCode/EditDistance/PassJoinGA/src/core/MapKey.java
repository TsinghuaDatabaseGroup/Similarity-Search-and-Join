package core;

public class MapKey {
	int segID;
	int start;
	int segLen;
	
	public MapKey(int segID, int start, int segLen){
		this.segID = segID;
		this.start = start;
		this.segLen = segLen;
	}
    
	public int getID(){
		return this.segID;
	}
	
	public int getStart(){
		return this.start;
	}
	
	public int getSegLen(){
		return this.segLen;
	}
	
	public boolean equals(Object o){
		MapKey key = (MapKey) o;
		return this.segID == key.segID && this.start == key.start && this.segLen == key.segLen;
	}
	
	public int hashCode(){
		int code = 1;
		code = code*31+segID;
		code = code*31+start;
		code = code*31+segLen;
		return code;
	}
}
