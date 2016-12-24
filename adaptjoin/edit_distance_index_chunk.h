#pragma once
#include "edit_distance.h"

class EditDistanceIndexChunk : public EditDistance
{
public:
	EditDistanceIndexChunk(int q):EditDistance(q){}
	~EditDistanceIndexChunk(void) {}
	int GetProbePrefixSize(int gram_set_size, int edth)
	{
		return min(gram_set_size,gram_set_size-(myceil((gram_set_size-edth)*1.0/q_)-edth)+1);
	}
	int GetIndexPrefixSize(int chunk_set_size, int edth)
	{
		return min(edth+1,chunk_set_size);
	}
	int GetRequiredOverlap(int gram_set_size, int edth)
	{
		return max(0,myceil((gram_set_size-edth)*1.0/q_)-edth);
	}

	int GetRequiredOverlap(int query_string_length, int data_string_length, int edth)
	{
		return max(0,myceil(data_string_length*1.0/q_)-edth);
	}
	int GetProbePrefixSize(int query_string_length, int data_string_length, int edth)
	{
		int overlap = GetRequiredOverlap(query_string_length, data_string_length, edth);
		return min(query_string_length, query_string_length-overlap+1);
	}
	int GetIndexPrefixSize(int query_string_length, int data_string_length, int edth)
	{
		int chunk_set_size = myceil(data_string_length*1.0/q_);
		return GetIndexPrefixSize(chunk_set_size, edth);
	}
};
