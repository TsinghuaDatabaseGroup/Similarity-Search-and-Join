#include "delta_inverted_index.h"


bool DeltaInvertedIndex::CmpDeltaId(const pair<int,int>& delta_id_and_object_id_1, const pair<int,int>& delta_id_and_object_id_2)
{
	return delta_id_and_object_id_1.first == delta_id_and_object_id_2.first ?
		delta_id_and_object_id_1.second < delta_id_and_object_id_2.second : delta_id_and_object_id_1.first < delta_id_and_object_id_2.first;
}


void DeltaInvertedIndex::AddDeltaObjectList(int token, vector<pair<int,int> >& delta_id_and_object_id_vec)
{
	if (delta_id_and_object_id_vec.size() == 0) return;
	assert(token_to_delta_object_list.count(token) == 0);

	sort(delta_id_and_object_id_vec.begin(), delta_id_and_object_id_vec.end(), CmpDeltaId);


	// for saving index space
	int delta_id_number = 0;
	int previous_delta_id = delta_id_and_object_id_vec[0].first;
	vector<int> object_id_number_vec;
	int object_id_number = 1;
	for (uint i=1; i<delta_id_and_object_id_vec.size(); ++i)
	{
		if (delta_id_and_object_id_vec[i].first != previous_delta_id)
		{
			object_id_number_vec.push_back(object_id_number);
			previous_delta_id = delta_id_and_object_id_vec[i].first;
			delta_id_number++;
			object_id_number = 1;
		}
		else
			object_id_number++;
	}
	delta_id_number++;
	object_id_number_vec.push_back(object_id_number);

	vector<DeltaObjectList> delta_object_list_vec(delta_id_number);
	int delta_id_first_pos = 0;
	for (uint i=0; i<object_id_number_vec.size(); ++i)
	{
		delta_object_list_vec[i].delta_id = delta_id_and_object_id_vec[delta_id_first_pos].first;
		delta_object_list_vec[i].list.reserve(object_id_number_vec[i]);
		for (int k=0; k<object_id_number_vec[i]; ++k)
		{
			int object_id = delta_id_and_object_id_vec[delta_id_first_pos+k].second;
			delta_object_list_vec[i].list.push_back(object_id);
		}
		delta_id_first_pos += object_id_number_vec[i];
	}
	token_to_delta_object_list.insert(HashMap<int,vector<DeltaObjectList> >::value_type(token, delta_object_list_vec));
}
