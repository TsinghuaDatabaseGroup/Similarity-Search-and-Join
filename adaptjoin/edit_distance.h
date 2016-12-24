#pragma once
#include "common/help.h"
#include "TypeDefs.h"
#include "GlobFunc.h"
#include <cstring>
class EditDistance
{
	static const int MAX_STRING_LEN = 1024;
	static uchar matrix1[MAX_STRING_LEN+1];
	static uchar matrix2[MAX_STRING_LEN+1];
protected:
	int q_;
public:
	EditDistance(int q):q_(q){}
	virtual ~EditDistance(void) {}
	virtual int GetProbePrefixSize(int query_gram_set_size, int edth) = 0;
	virtual int GetIndexPrefixSize(int data_gram_set_size, int edth) = 0;
	virtual int GetRequiredOverlap(int query_token_set_length, int edth) = 0;
	virtual int GetRequiredOverlap(int query_string_length, int data_string_length, int edth) = 0;
	virtual int GetProbePrefixSize(int query_string_length, int data_string_length, int edth) = 0;
	virtual int GetIndexPrefixSize(int query_string_length, int data_string_length, int edth) = 0;
	int GetLengthLowerBound(int string_length, int edth)
	{
		return max(string_length-edth,0);
	}
	int GetLengthUpperBound(int string_length, int edth)
	{
		return string_length+edth;
	}
	int CalculateOverlap(const vector<pair<int,int> >& token_set1, const vector<pair<int,int> >& token_set2, int position_different, int start1=0, int start2=0)
	{
		int overlap = 0;
		while (start1 < int(token_set1.size()) && start2 < int(token_set2.size())) {
//			if (token_set1[start1].first == token_set2[start2].first) ++start1, ++start2, ++overlap;
			if (token_set1[start1].first == token_set2[start2].first)
            {
                if (abs(token_set1[start1].first-token_set2[start2].first)<=position_different)
                {
                    ++start1, ++start2, ++overlap;
                }
                else if (token_set1[start1].first<token_set2[start2].first)
                    ++start1;
                else
                    ++start2;
            }

			else {
				if (token_set1[start1].first < token_set2[start2].first) ++start1;
				else ++start2;
			}
		}
		return overlap;
	}
	int CalculateOverlap(const vector<int>& token_set1, const vector<int>& token_set2, int position_different, int start1=0, int start2=0)
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
	int VerifyEditDistance(const string& s, const string& t, int edth)
	{
		return edit_distance(s, t, edth);

		int lens = s.size();
		int lent = t.size();
		if (!lens || !lent)
			return false;
		if (lens>lent&&lens-lent>edth)
		{
			return edth+1;
		}
		if (lent>lens&&lent-lens>edth)
		{
			assert(0);
			return edth+1;
		}
		if (lens >= MAX_STRING_LEN || lent >= MAX_STRING_LEN)
			return false;
		uchar* d1 = matrix1; //'previous' cost array, horizontally
		uchar* d2 = matrix2; // cost array, horizontally
		uchar* _d;  //placeholder to assist in swapping d1 and d2
		memset(d2, -1, lent+1);
		for (int i=0; i<=lent; ++i)
			d1[i] = i;
		//cout << endl;
		for (int i=1; i<=lens; ++i)
		{
			int begin_j = max(i-edth,1);
			int end_j = min(i+edth,lent);
			d2[begin_j-1] = i;
			bool flag = false;
			for (int j=begin_j; j<=end_j; ++j)
			{
				d2[j] = min( min(d1[j],d2[j-1])+1 ,  d1[j-1]+(s[i-1]==t[j-1]?0:1) );
				if (d2[j]<=edth)
					flag = true;
			}
			_d = d1;
			d1 = d2;
			d2 = _d;
			if (!flag)
				return edth+1;
		}
		return d1[lent];
	}
};
