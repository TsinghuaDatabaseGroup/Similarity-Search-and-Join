#pragma once
#include "edit_distance.h"

class EditDistanceIndexGram : public EditDistance
{
public:
	EditDistanceIndexGram(int q):EditDistance(q){}
	~EditDistanceIndexGram(void) {}
	int GetProbePrefixSize(int chunk_set_size, int edth)
	{
		return min(edth+1,chunk_set_size);
	}
	int GetIndexPrefixSize(int gram_set_size, int edth)
	{
		return min(gram_set_size, gram_set_size-(myceil(gram_set_size*1.0/q_)-edth)+1); // sort by length

		//return min(gram_set_size, gram_set_size-(myceil((gram_set_size-edth)*1.0/q_)-edth)+1);
	}
	int GetRequiredOverlap(int chunk_set_size, int edth)
	{
		return max(chunk_set_size-edth,0); //sort by length

		//return max(chunk_set_size,myceil(gram_set_size*1.0/q_))-edth;
	}

	int GetRequiredOverlap(int query_string_length, int data_string_length, int edth)
	{
		return max(0,myceil(query_string_length*1.0/q_)-edth);
	}
	int GetProbePrefixSize(int query_string_length, int data_string_length, int edth)
	{
		int chunk_set_size = myceil(query_string_length*1.0/q_);
		return GetProbePrefixSize(chunk_set_size, edth);
	}
	int GetIndexPrefixSize(int query_string_length, int data_string_length, int edth)
	{
		int overlap = GetRequiredOverlap(query_string_length, data_string_length, edth);
		return min(data_string_length, data_string_length-overlap+1);
	}
};
