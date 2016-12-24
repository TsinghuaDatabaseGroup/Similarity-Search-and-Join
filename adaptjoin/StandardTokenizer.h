#pragma once
#include "Tokenizer.h"

// tokenize a string based on the predefined delimiters
class StandardTokenizer : public Tokenizer
{
private:
	string delimiters_;
public:
	StandardTokenizer(string delimiters) {delimiters_ = delimiters;}
	~StandardTokenizer(void);
	void tokenize(const string& str, vector<string>& tokens);
};
