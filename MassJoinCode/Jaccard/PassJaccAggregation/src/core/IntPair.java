package core;

public class IntPair {

	int first;
	int second;
	
	public IntPair() {
	}
	
	public int getFirst() {
		return this.first;
	}

	public int getSecond() {
		return this.second;
	}

	public IntPair(int i1, int i2) {
		this.first = i1;
		this.second = i2;
	}
	
	public void set(int i1, int i2) {
		this.first = i1;
		this.second = i2;
	}
	
}
