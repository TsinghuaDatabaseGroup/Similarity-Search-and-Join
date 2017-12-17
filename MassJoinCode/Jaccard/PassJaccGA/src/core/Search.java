package core;

import hadoop.passjoin.JoinValue;

import java.util.ArrayList;

public class Search {
	//Find the first element, whose value is larger than target, in a sorted array 
	public static int BSearchUpperBound(ArrayList<JoinValue> list, int target){
		int low = 0;
		int high = list.size()-1;
		if(target >= list.get(high).getStart()) return high+1;
		int mid = (low + high) / 2;
		while(low < high){
			if(list.get(mid).getStart() > target)
				high = mid;
			else
				low = mid + 1;
			mid = (low + high) / 2;
		}
		return mid;
	}
	
	//Find the last element, whose value is less than target, in a sorted array 
	public static int BSearchLowerBound(ArrayList<JoinValue> list, int target){
		int low = 0;
		int high = list.size()-1;
		if(target <= list.get(low).getStart()) return -1;
		int mid = (low + high + 1) / 2;
		while(low < high){
			if(list.get(mid).getStart() < target)
				low = mid;
			else
				high = mid - 1;
			mid = (low + high + 1) / 2;
		}
		return mid;
	}

	public static int FindStartPos(ArrayList<JoinValue> list, int target){
		int pos = BSearchLowerBound(list,target);
		pos = pos + 1;
		if(pos > list.size()-1) return -1;
		return pos;
	}
	
	public static int FindEndPos(ArrayList<JoinValue> list, int target){
		int pos = BSearchUpperBound(list,target);
		pos = pos - 1;
		if(pos < 0) return -1;
		return pos;
	}
}
