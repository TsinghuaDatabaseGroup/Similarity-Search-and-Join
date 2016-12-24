#pragma once
#include "simfunc.h"

class DiceSimFunc: public SimFunc
{
public:
	DiceSimFunc(void);
	~DiceSimFunc(void);
	int GetProbePrefixSize(int token_set_length, double theta)
	{
		return min(myfloor(token_set_length - token_set_length*theta/(2-theta))+1,token_set_length);
	}
	int GetIndexPrefixSize(int token_set_length, double theta)
	{
		return min(myfloor(token_set_length - theta*token_set_length)+1,token_set_length);
	}
	int GetLengthLowerBound(int token_set_length, double theta)
	{
		return myceil(token_set_length*theta/(2-theta));
	}
	int GetLengthUpperBound(int token_set_length, double theta)
	{
		return myceil(token_set_length*(2-theta)/theta);
	}
	int GetRequiredOverlap(int query_token_set_length, int data_token_set_length, double theta)
	{
		return myceil((query_token_set_length+data_token_set_length)*theta/(2));
	}
	double GetSimilarityValue(const vector<int>& token_set1, const vector<int>& token_set2)
	{
		int overlap = CalculateOverlap(token_set1, token_set2);
		return 2.0*overlap/(token_set1.size()+token_set2.size());
	}
	double GetSimilarityValue(int overlap, int token_set1_size, int token_set2_size)
	{
		return 2.0*overlap/(token_set1_size+token_set2_size);
	}
};
