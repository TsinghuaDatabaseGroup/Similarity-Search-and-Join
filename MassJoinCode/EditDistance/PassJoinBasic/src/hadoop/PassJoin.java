package hadoop;

import hadoop.passjoin.JoinKernel;
import hadoop.recordjoin.RecordJoin;
import hadoop.verify.Verify;

import java.io.IOException;
import java.util.Date;

public class PassJoin {
	public static void main(String[] args) throws IOException {
		Date startTime = new Date();
		System.out.println("Complete-Job started: " + startTime);
		
		JoinKernel.main(args);
		RecordJoin.main(args);
		Verify.main(args);
		
		Date end_time = new Date();
		System.out.println("Complete-Job ended: " + end_time);
		System.out.println("The complete-job took "
				+ (end_time.getTime() - startTime.getTime()) / (float) 1000.0
				+ " seconds.");
	}
}
