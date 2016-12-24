#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>
#include <cmath>
#include <cassert>
#include <cstdlib>
#include "TypeDefs.h"
#include "common/type.h"
using namespace std;

const string delimiters(" ,.()<>-+=/%?_:&~^");

void loadStringSet(const char* filename, vector<string>& stringSet, int k);
void loadStringSet(const char* filename, vector<string>& stringSet);
void LoadIntSearchQuery(const char* query_filename, vector<pair<string, int> >& query_set);
void LoadDoubleSearchQuery(const char* query_filename, vector<pair<string, double> >& query_set);
void toLowerString(string& str);
void tokenizes(const string & str, const string & delimiters, vector<string> & words);
void tokenizes(const string & str, const string & delimiters, vector<string> & words, const char* reserve);
void getDeletionNeighbours(const string& sig, Set<string>& neighbours, int minEd);

inline uint strHash(const char *str, int len)
{

	unsigned int h = 2166136261U;
	unsigned int f = 0;

	for(; f < (unsigned)len;++ f)
		h = 16777619U * h ^ (unsigned int)str[f];
	return h;
}

inline double getMicroSecond(clock_t tbegin, clock_t tend)
{
	return (tend-tbegin)*1000.0/CLOCKS_PER_SEC;
}

inline int myceil(double d)
{
	return int(ceil(d-eps));
}
inline int myfloor(double d)
{
	return int(floor(d+eps));
}
