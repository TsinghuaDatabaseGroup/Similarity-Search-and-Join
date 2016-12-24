#pragma once
#include "Tokenizer.h"
class ChunkTokenizer : public Tokenizer
{
	static const unsigned char SUFFIXCHAR = 156;
	int q_;
public:
	ChunkTokenizer(void):q_(3){}
	ChunkTokenizer(int q):q_(q){}
	~ChunkTokenizer(void) {}
	void tokenize(const string& str, vector<string>& chunks);
};
