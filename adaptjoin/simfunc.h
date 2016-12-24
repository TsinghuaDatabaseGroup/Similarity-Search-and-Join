#pragma once
#include <vector>
#include <string>
#include "TypeDefs.h"
#include "GlobFunc.h"
using namespace std;

class SimFunc
{
public:
	SimFunc(void);
	~SimFunc(void);
	virtual int GetProbePrefixSize(int token_set_length, double theta) = 0;
	virtual int GetIndexPrefixSize(int token_set_length, double theta) = 0;
	virtual int GetLengthLowerBound(int token_set_length, double theta) = 0;
	virtual int GetLengthUpperBound(int token_set_length, double theta) = 0;
	virtual int GetRequiredOverlap(int query_token_set_length, int data_token_set_length, double theta) = 0;
	virtual double GetSimilarityValue(const vector<int>& token_set1, const vector<int>& token_set2) {return 0;}
	virtual double GetSimilarityValue(int overlap, int token_set1_size, int token_set2_size) {return 0;}
	int GetProbePrefixSize(int query_token_set_length, int data_token_set_length, double theta)
	{
		int overlap = GetRequiredOverlap(query_token_set_length, data_token_set_length, theta);
		return min(query_token_set_length, query_token_set_length-overlap+1);
	}
	int GetIndexPrefixSize(int query_token_set_length, int data_token_set_length, double theta)
	{
		int overlap = GetRequiredOverlap(query_token_set_length, data_token_set_length, theta);
		return min(data_token_set_length, data_token_set_length-overlap+1);
	}
	int CalculateOverlap(const vector<int>& token_set1, const vector<int>& token_set2, int start1=0, int start2=0)
	{
		int overlap = 0;
		while (start1 < int(token_set1.size()) && start2 < int(token_set2.size())) {
			if (token_set1[start1] == token_set2[start2]) ++start1, ++start2, ++overlap;
			else {
				if (token_set1[start1] < token_set2[start2]) ++start1;
				else ++start2;
			}
		}
		return overlap;
	}
	void tokenize();

	//bool verifySimilarity(const vector<int>& set1, const vector<int>& set2, double theta); // set1, set2 are sorted
	//int calcOverlap(const vector<int>& set1, const vector<int>& set2);
};
