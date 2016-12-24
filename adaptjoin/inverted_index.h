#pragma once

#include <vector>
#include <cassert>
#include "TypeDefs.h"

#include "common/type.h"
using namespace std;

struct IndexEntry
{
	int object_id;
	int position;
	IndexEntry(int object_id_input, int position_input): object_id(object_id_input), position(position_input){}
};
struct TokenList
{
	int start_position;
	vector<IndexEntry> list;
	TokenList():start_position(0){}
	int size() {return list.size()-start_position;}
};

class InvertedIndex
{
	HashMap <int, TokenList* > token_to_list;
public:
	InvertedIndex(void){}
	~InvertedIndex(void){}

	bool ExistToken(int token) const
	{
		HashMap <int, TokenList*>::const_iterator it = token_to_list.find(token);
		return it != token_to_list.end() && it->second->start_position < int(it->second->list.size());
	}
	TokenList* GetTokenList(int token) const
	{
		assert(ExistToken(token));
		return token_to_list.find(token)->second;
	}
	void AddToken(int token, int object_id, int token_position)
	{
		if (!token_to_list.count(token))
			token_to_list.insert(make_pair(token, new TokenList()));
		token_to_list[token]->list.push_back(IndexEntry(object_id,token_position));
	}
	int GetTokenListSize(int token) {return token_to_list[token]->size();}
	void clear() {token_to_list.clear();}

};

