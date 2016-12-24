#pragma once
#include "Tokenizer.h"

class QGramTokenizer : public Tokenizer
{
	static const unsigned char SUFFIXCHAR = 156;
	int q_;
public:
	QGramTokenizer(void);
	QGramTokenizer(int q):q_(q){}
	~QGramTokenizer(void);
	void tokenize(const string& str, vector<string>& tokens);
};
