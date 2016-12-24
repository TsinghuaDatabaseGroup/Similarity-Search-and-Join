#pragma once
#include <string>
#include <vector>
using std::string;
using std::vector;

class Tokenizer
{
public:
	Tokenizer(void);
	virtual ~Tokenizer(void);
	virtual void tokenize(const string& str, vector<string>& tokens) = 0;
};
