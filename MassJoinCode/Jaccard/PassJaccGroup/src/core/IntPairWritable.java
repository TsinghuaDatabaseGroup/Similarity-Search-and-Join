/**
 * Copyright 2010-2011 The Regents of the University of California
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on
 * an "AS IS"; BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations under
 * the License.
 * 
 * Author: Rares Vernica <rares (at) ics.uci.edu>
 */

package core;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;
import org.apache.hadoop.io.WritableComparable;

public class IntPairWritable implements WritableComparable<IntPairWritable> {

	int first;
	int second;
	
	public IntPairWritable() {
	}
	
	public int getFirst() {
		return this.first;
	}

	public int getSecond() {
		return this.second;
	}

	public IntPairWritable(int i1, int i2) {
		this.first = i1;
		this.second = i2;
	}
	
	public void set(int i1, int i2) {
		this.first = i1;
		this.second = i2;
	}
	
	@Override
	public void readFields(DataInput arg0) throws IOException {
		this.first = arg0.readInt();
		this.second = arg0.readInt();
	}

	@Override
	public void write(DataOutput arg0) throws IOException {
		// TODO Auto-generated method stub
		arg0.writeInt(this.first);
		arg0.writeInt(this.second);
	}

	@Override
	public int compareTo(IntPairWritable o) {
		if (this.first != o.first) {
			return this.first - o.first;
		} else if (this.second != o.second){
			return this.second - o.second;
		}
		return 0;
	}
	
	public boolean equals(Object o){
		IntPairWritable key = (IntPairWritable)o;
		return this.first == key.first && this.second == key.second;
	}
	
	public int hashCode() {
		return 31 * ( 31 + first) + second;
	}
}
