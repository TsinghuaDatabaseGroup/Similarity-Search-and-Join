#pragma once
#include "simfunc.h"

class JaccardSimFunc: public SimFunc
{
public:
	JaccardSimFunc(void);
	~JaccardSimFunc(void);
	int GetProbePrefixSize(int token_set_length, double theta)
	{
		return min(myfloor(token_set_length - token_set_length*theta)+1,token_set_length);
	}
	int GetIndexPrefixSize(int token_set_length, double theta)
	{
		return min(myfloor(token_set_length - 2*token_set_length*theta/(1+theta))+1,token_set_length);
	}
	int GetLengthLowerBound(int token_set_length, double theta)
	{
		return myceil(token_set_length*theta);
	}
	int GetLengthUpperBound(int token_set_length, double theta)
	{
		return myceil(token_set_length/theta);
	}
	int GetRequiredOverlap(int query_token_set_length, int data_token_set_length, double theta)
	{
		return myceil((query_token_set_length+data_token_set_length)*theta/(1+theta));
	}
	double GetSimilarityValue(const vector<int>& token_set1, const vector<int>& token_set2)
	{
		int overlap = CalculateOverlap(token_set1, token_set2);
		return overlap * 1.0/(token_set1.size()+token_set2.size()-overlap);
	}
	double GetSimilarityValue(int overlap, int token_set1_size, int token_set2_size)
	{
		return overlap * 1.0/(token_set1_size+token_set2_size-overlap);
	}
};
