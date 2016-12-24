#include "QGramTokenizer.h"


QGramTokenizer::QGramTokenizer(void)
{
}

QGramTokenizer::~QGramTokenizer(void)
{
}


void QGramTokenizer::tokenize(const string& str, vector<string>& tokens)
{
	string sPad = str + string(q_ - 1, SUFFIXCHAR);

	for (unsigned i = 0; i < str.length(); i++)
		tokens.push_back(sPad.substr(i, q_)); 
}
