#pragma once
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <cassert>
#include <cstring>
using namespace std;
typedef unsigned char uchar;
class Verification
{
	int ed;
	static const int _MAX_WORD_NUM = 2550;
	static const int _MAX_STRING_LEN = 2550;
	uchar matrix1[_MAX_STRING_LEN+1];
	uchar matrix2[_MAX_STRING_LEN+1];

private:
	int levenshtein_distance(const char *s, const char *t);
public:
	Verification(int _ed):ed(_ed){}
	int verifyEd(const char *s, const char *t); // if ed(s,t) > ed return -1, otherwise return ed(s,t)
	~Verification(void);
};
