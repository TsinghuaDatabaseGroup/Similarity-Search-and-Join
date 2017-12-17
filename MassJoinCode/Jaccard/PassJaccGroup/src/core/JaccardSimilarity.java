package core;

public class JaccardSimilarity {
	
    public static double getSimilarity(int[] record, int[] subRecord){
    	int xPos=0,yPos=0,intersect=0;
    	int xCount=0,yCount=0;
    	int xlength = record.length - 1;
    	int ylength = subRecord.length;
    	int xH = (int) Math.floor((xlength - ylength * Config.SIMILARITY)/(1+Config.SIMILARITY) + Config.EPS);
		int yH = (int) Math.floor((ylength - xlength * Config.SIMILARITY)/(1+Config.SIMILARITY) + Config.EPS);
    	
    	while(xPos<xlength&&yPos<ylength){
    		int xToken = record[xPos];
    		int yToken = subRecord[yPos];
    		if(xToken < yToken){
    			xPos++;
    			xCount++;
    			if (xCount + ((xlength - xPos) > (ylength - yPos) ? xlength - xPos - ylength + yPos : 0) > xH) return 0;
    		}
    		else if(xToken > yToken){
    			yPos++;
    			yCount++;
    			if (yCount + ((xlength - xPos) < (ylength - yPos) ? ylength - yPos - xlength + xPos : 0) > yH) return 0;
    		}
    		else{
    			intersect++;
    			xPos++;
    			yPos++;
    		}
    		if(xCount > xH ||yCount > yH){
    			return 0;
    		}	
    	}
    	double sim = (double)intersect /(double)(xlength+ylength-intersect);
    	return sim;  	
    }
    
    public static double getSimilarity2(int[] record, int[] subRecord){
    	int xPos=0,yPos=0,intersect=0;
    	int xCount=0,yCount=0;
    	int xlength = record.length - 1;
    	int ylength = subRecord.length - 1;
    	int xH = (int) Math.floor((xlength - ylength * Config.SIMILARITY)/(1+Config.SIMILARITY) + Config.EPS);
		int yH = (int) Math.floor((ylength - xlength * Config.SIMILARITY)/(1+Config.SIMILARITY) + Config.EPS);
    	
    	while(xPos<xlength&&yPos<ylength){
    		int xToken = record[xPos];
    		int yToken = subRecord[yPos];
    		if(xToken < yToken){
    			xPos++;
    			xCount++;
    			if (xCount + ((xlength - xPos) > (ylength - yPos) ? xlength - xPos - ylength + yPos : 0) > xH) return 0;
    		}
    		else if(xToken > yToken){
    			yPos++;
    			yCount++;
    			if (yCount + ((xlength - xPos) < (ylength - yPos) ? ylength - yPos - xlength + xPos : 0) > yH) return 0;
    		}
    		else{
    			intersect++;
    			xPos++;
    			yPos++;
    		}
    		if(xCount > xH ||yCount > yH){
    			return 0;
    		}	
    	}
    	double sim = (double)intersect /(double)(xlength+ylength-intersect);
    	return sim;  	
    }
    
    
    public static boolean passContentFilter(short[] xgroup, short[] ygroup, int xH, int yH){
    	int xCount=0,yCount=0;
    	for(int i=0;i<Config.NUM_OF_GROUP;i++){
    		if(xgroup[i] < ygroup[i]){
    			yCount = yCount+(ygroup[i] - xgroup[i]);
    			if(yCount > yH)
    				return false;
    		} else{
    			xCount = xCount+(xgroup[i] - ygroup[i]);
    			if(xCount > xH){
    				return false;
    			}
    		}
    	}
    	return true;
    }

}
