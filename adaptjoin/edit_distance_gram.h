#pragma once
#include "edit_distance.h"

class EditDistanceGram : public EditDistance
{
	static const int MAX_STRING_LEN = 1024;
	static uchar matrix1[MAX_STRING_LEN+1];
	static uchar matrix2[MAX_STRING_LEN+1];
public:
	EditDistanceGram(int q):EditDistance(q){}
	~EditDistanceGram(void) {}
	int GetProbePrefixSize(int query_gram_set_size, int edth)
	{
		return min(edth*q_+1,query_gram_set_size);
	}
	int GetIndexPrefixSize(int data_gram_set_size, int edth)
	{
		return min(edth*q_+1,data_gram_set_size);
	}
	int GetRequiredOverlap(int query_gram_set_size, int edth)
	{
		return max(0, query_gram_set_size-edth*q_);
	}
	int GetRequiredOverlap(int query_string_length, int data_string_length, int edth)
	{
		return max(0, max(query_string_length,data_string_length)-q_*edth);
	}
	int GetProbePrefixSize(int query_string_length, int data_string_length, int edth)
	{
		int overlap = GetRequiredOverlap(query_string_length, data_string_length, edth);
		return min(query_string_length, query_string_length-overlap+1);
	}
	int GetIndexPrefixSize(int query_string_length, int data_string_length, int edth)
	{
		int overlap = GetRequiredOverlap(query_string_length, data_string_length, edth);
		return min(data_string_length, data_string_length-overlap+1);
	}
};
