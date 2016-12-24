#pragma once
#include "ActiveStates.h"
#include "ActiveStatesManage.h"
#include "AdvancedStack.h"
#include "Verification.h"
#include "common/type.h"

class PrintedHash
{
public:
	size_t operator()(const pair<int,int>& id) const
	{
		return std::hash<int>()(id.first)+std::hash<int>()(id.second);
		//return tr1::hash(vtNode.node +vtNode.pos);
	}
};

class BiTrieSelfJoin
{
private:
	HashSet<pair<int,int>, PrintedHash> printed;
private:
	char* depInfo(const vector<char*>& strings);
	void outputResult(const vector<int>& RIds, const vector<int>& SIds, const vector<char*>& strings, int edth);
	void similarWords(vector<ActiveNode>& states, VTrieNode& prefixNode, int edth, vector<int>& SIds);
	void edjoinPrefix(RadixTrie* ptrie, const vector<char*>& strings, int edth);
public:
	BiTrieSelfJoin()
	{
	}
	//void outputSimilarWords(vector<ActiveNode>& states, int id, const vector<string>& strings);
	void edjoin(const vector<char*>& strings, int edth);
	~BiTrieSelfJoin() {}
};
