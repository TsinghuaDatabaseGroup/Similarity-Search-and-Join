#include "ChunkTokenizer.h"



void ChunkTokenizer::tokenize(const string& str, vector<string>& chunks)
{
	string sPad = str + string((q_ - str.size()%q_)%q_, SUFFIXCHAR);

	for (unsigned i = 0; i < sPad.length(); i+=q_)
		chunks.push_back(sPad.substr(i, q_)); 
}

