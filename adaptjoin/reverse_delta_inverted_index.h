#pragma once
#include <vector>
#include <string>
#include <cassert>
#include <iostream>
#include "TypeDefs.h"
#include "inverted_index.h"
#include "common/type.h"

using namespace std;

class ReverseDeltaInvertedIndex
{
	typedef vector<TokenList*> DeltaToObjectList;
	vector<DeltaToObjectList > token_to_delta_object_list;
    int max_delta_id;
public:
	ReverseDeltaInvertedIndex(int max_token_id, int max_delta_id):max_delta_id(max_delta_id)
	{
		token_to_delta_object_list.resize(max_token_id+1);
	}
	~ReverseDeltaInvertedIndex(void){
		for (uint i=0; i<token_to_delta_object_list.size(); ++i)
		{
			for (DeltaToObjectList::iterator it = token_to_delta_object_list[i].begin(); it != token_to_delta_object_list[i].end(); ++it)
				if (*it != NULL)
                {
                    delete *it;
                    *it = NULL;
                }
            token_to_delta_object_list[i].clear();
		}
	}
	void AddDeltaToken(int delta_id, int token, int object_id, int token_position)
	{
        --delta_id;
        if (token_to_delta_object_list[token].size() == 0)
            token_to_delta_object_list[token].assign(max_delta_id,(TokenList*)(NULL));
		if (token_to_delta_object_list[token][delta_id] == NULL)
			token_to_delta_object_list[token][delta_id] = new TokenList();
		token_to_delta_object_list[token][delta_id]->list.push_back(IndexEntry(object_id,token_position));
	}
	bool ExistDeltaToken(int delta_id, int token)
	{
    --delta_id;
        return token_to_delta_object_list[token].size() != 0 && token_to_delta_object_list[token][delta_id] != NULL && token_to_delta_object_list[token][delta_id]->size() !=0;
		//DeltaToObjectList::iterator it = token_to_delta_object_list[token].find(delta_id);
		//return it != token_to_delta_object_list[token].end() && it->second->start_position <  int(it->second->list.size());
	}
	TokenList* GetDeltaTokenList(int delta_id, int token)
	{
        --delta_id;
		return token_to_delta_object_list[token].size() != 0 && token_to_delta_object_list[token][delta_id] != NULL && token_to_delta_object_list[token][delta_id]->size() !=0 ? token_to_delta_object_list[token][delta_id]: NULL;
	}
	int GetTokenListSize(int delta_id, int token)
	{
        --delta_id;
        TokenList* token_list = token_to_delta_object_list[token][delta_id];
		return token_list->size();
	}
};
