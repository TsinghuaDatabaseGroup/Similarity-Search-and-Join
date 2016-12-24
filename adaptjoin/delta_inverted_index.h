#pragma once

#include <vector>
#include <string>
#include <cassert>
#include <iostream>
#include <algorithm>
#include "TypeDefs.h"
#include "inverted_index.h"
#include "common/type.h"

using namespace std;

struct DeltaObjectList
{
	int delta_id;
	vector<int> list;
};


class DeltaInvertedIndex
{
	HashMap<int,vector<DeltaObjectList> > token_to_delta_object_list;
public:
	DeltaInvertedIndex() {}
	~DeltaInvertedIndex() {}
	static bool CmpDeltaId(const pair<int,int>& delta_id_and_object_id_1, const pair<int,int>& delta_id_and_object_id_2);
	void AddDeltaObjectList(int token, vector<pair<int,int> >& delta_id_and_object_id_vec);
	bool empty() {return token_to_delta_object_list.empty();}
	bool ExistToken(int token) {return token_to_delta_object_list.count(token)!=0;}
	const vector<DeltaObjectList>& GetDeltaObjectListVec(int token) {assert(ExistToken(token)); return token_to_delta_object_list[token];}
	/*bool ExistDeltaToken(int delta_id, int token)
	{
		return delta_id<int(delta_token_to_object_list.size())
			&& delta_token_to_object_list[delta_id]->ExistToken(token);
	}
	TokenList* GetDeltaTokenList(int delta_id, int token)
	{
		assert(ExistDeltaToken(delta_id, token));
		return delta_token_to_object_list[delta_id]->GetTokenList(token);
	}
	int GetTokenListSize(int delta_id, int token)
	{
	   return !ExistDeltaToken(delta_id,token) ? 0:delta_token_to_object_list[delta_id]->GetTokenListSize(token);
	}*/
};
