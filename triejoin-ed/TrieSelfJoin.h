#pragma once
#include "ActiveStates.h"
#include "ActiveStatesManage.h"
#include "AdvancedStack.h"

class TrieSelfJoin
{
private:
	//void calculateSimilarStringPair(TrieNode* node, char c);
	void outputSimilarWords(vector<ActiveNode>& states, int id, const vector<char*>& strings);
	void outputSimilarWordsOp(vector<ActiveNode>& states, int id, const vector<char*>& strings);
public:
	TrieSelfJoin()
	{
	}
	//void outputSimilarWords(vector<ActiveNode>& states, int id, const vector<string>& strings);
	void edjoin(const vector<char*>& strings, int ed);
	~TrieSelfJoin() {}
};
