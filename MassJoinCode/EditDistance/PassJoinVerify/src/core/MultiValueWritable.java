package core;

import org.apache.hadoop.io.GenericWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.io.Writable;

public class MultiValueWritable extends GenericWritable {

	private static Class[] CLASSES = new Class[]{
		IntWritable.class,
		Text.class,
	};
	
	public MultiValueWritable() {
	}
	
	public MultiValueWritable(Writable value) {
		set(value);
	}
	
	@Override
	protected Class<? extends Writable>[] getTypes() {
		// TODO Auto-generated method stub
		return CLASSES;
	}

}
