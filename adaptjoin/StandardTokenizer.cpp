#include "StandardTokenizer.h"


StandardTokenizer::~StandardTokenizer(void)
{
}

void StandardTokenizer::tokenize(const string &str, vector<string> &tokens)
{
	string::size_type lastPos = str.find_first_not_of(delimiters_, 0);	
	string::size_type pos = str.find_first_of(delimiters_, lastPos);		
	while (string::npos != pos || string::npos != lastPos)
	{
		string token = str.substr(lastPos, pos - lastPos);			
		tokens.push_back(token);
		lastPos = str.find_first_not_of(delimiters_, pos);
		pos = str.find_first_of(delimiters_, lastPos);
	}
}
