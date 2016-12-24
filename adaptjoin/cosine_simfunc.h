#pragma once
#include <math.h>
#include "simfunc.h"

class CosineSimFunc:public SimFunc
{
public:
	vector<double> sqrt_store;
	CosineSimFunc(void);
	~CosineSimFunc(void);
	int GetProbePrefixSize(int token_set_length, double theta)
	{
		return min(myfloor(token_set_length - token_set_length*theta*theta)+1,token_set_length);
	}
	int GetIndexPrefixSize(int token_set_length, double theta)
	{
		return min(myfloor(token_set_length - token_set_length*theta)+1,token_set_length);
	}
	int GetLengthLowerBound(int token_set_length, double theta)
	{
		return myceil(token_set_length*theta*theta);
	}
	int GetLengthUpperBound(int token_set_length, double theta)
	{
		return myceil(token_set_length/theta/theta);
	}
	double CalculateSqrt(int token_set_length)
	{
		if (token_set_length>=(int)sqrt_store.size())
		{
			int fill_size = token_set_length-sqrt_store.size()+1;
			while (fill_size--)
				sqrt_store.push_back(-1);
		}
		if (sqrt_store[token_set_length]!=-1)
			return sqrt_store[token_set_length];
		else
			return sqrt_store[token_set_length]=sqrt(double(token_set_length));

	}
	int GetRequiredOverlap(int query_token_set_length, int data_token_set_length, double theta)
	{
		return myceil(CalculateSqrt(query_token_set_length)*CalculateSqrt(data_token_set_length)*theta);
	}
	double GetSimilarityValue(const vector<int>& token_set1, const vector<int>& token_set2)
	{
		int overlap = CalculateOverlap(token_set1, token_set2);
		return overlap * 1.0/(CalculateSqrt(token_set1.size())*CalculateSqrt(token_set2.size()));
	}
	double GetSimilarityValue(int overlap, int token_set1_size, int token_set2_size)
	{
		return overlap * 1.0/(CalculateSqrt(token_set1_size)*CalculateSqrt(token_set2_size));
	}
};
