#include "adapt_join.h"

long long totalCandNum = 0;
bool AdaptJoin::cmpfirst(const pair<int,int>& a, const pair<int,int>& b)
{
	return a.first < b.first;
}
bool AdaptJoin::cmpsecond(const pair<int,int>& a, const pair<int,int>& b)
{
	return a.second < b.second;
}

void AdaptJoin::InitializeParameters(int edth, int q, AlgorithmChoice algorithm_choice)
{
	__ClearTokenizer();
	edth_ = edth;
	q_ = q;
	algorithm_choice_ = algorithm_choice;
	tokenizer = new QGramTokenizer(q);
	switch(algorithm_choice)
	{
	case GRAM:edfunc = new EditDistanceGram(q); break;
	case INDEXGRAM: edfunc = new EditDistanceIndexGram(q); break;
	case INDEXCHUNK: edfunc = new EditDistanceIndexChunk(q); break;
	default:assert(0);break;
	}
}
/*void AdaptJoin::__GetChunkSetFromGramSet(const vector<PositionObject>& gram_position_collection, vector<PositionObject>& chunk_position_collection)
{
	chunk_position_collection.resize(gram_position_collection.size());
	for (uint i=0; i<gram_position_collection.size(); ++i)
	{
		PositionObject& position_object = chunk_position_collection[i];
		position_object.string_id = gram_position_collection[i].string_id;

		const vector<pair<int,int> >& gram_set = gram_position_collection[i].token_set;
		position_object.token_set.reserve(gram_set.size()/q_+1);
		for (uint j=0; j<gram_set.size(); ++j)
			if (gram_set[j].second % q_ == 0)
				position_object.token_set.push_back(gram_set[j]);
	}
}*/

void AdaptJoin::__VerifyCandidatesEd(int query_object_id, const vector<PositionObject>& query_position_collection, const vector<int>& candidates, const vector<PositionObject>& data_position_collection, const vector<string>& string_collection, vector<int>& results)
{
	const string& query_string = string_collection[query_position_collection[query_object_id].string_id];
totalCandNum += candidates.size();
	for (uint i=0; i<candidates.size(); ++i)
	{
		int data_object_id = candidates[i];
		const string& data_string = string_collection[data_position_collection[data_object_id].string_id];
		assert((int)data_string.size()>=length_lower_bound);
		int ed;
		if ((ed=edfunc->VerifyEditDistance(query_string, data_string,edth_))<=edth_)
		{
			results.push_back(data_object_id);
#ifdef RESULTOUT
            int query_string_id = query_position_collection[query_object_id].string_id;
            int data_string_id = data_position_collection[data_object_id].string_id;
            if (query_string_id < data_string_id)
            {
                int tmp = query_string_id;
                query_string_id = data_string_id;
                data_string_id = tmp;
            }
			OutputResult(query_string_id, data_string_id, ed, string_collection);
#endif
		}
	}
}
void AdaptJoin::__VerifyCandidatesEdOp(int query_object_id, const vector<PositionObject>& query_position_collection, const vector<int>& candidates, const vector<PositionObject>& data_position_collection, const vector<string>& string_collection, FixedSizeHashMap& object_to_count, int prefix_scheme, const vector<int>& index_prefix_size_store, int required_overlap, vector<int>& results)
{
	int query_token_set_size = query_position_collection[query_object_id].token_set.size();
	int query_last_position = edfunc->GetProbePrefixSize(query_token_set_size, edth_)+prefix_scheme-2;
	const string& query_string = string_collection[query_position_collection[query_object_id].string_id];
	for (uint i=0; i<candidates.size(); ++i) {
		int data_object_id = candidates[i];
		int overlap = object_to_count.Count(data_object_id);
		assert(overlap >= prefix_scheme);
		int data_token_set_size = data_position_collection[data_object_id].token_set.size();
		int data_last_position = min(data_token_set_size-1,index_prefix_size_store[data_object_id]+prefix_scheme-2);

		/* optimize verification by comparing the last tokens in x's and y's prefix */
		if (query_position_collection[query_object_id].token_set[query_last_position].first < data_position_collection[data_object_id].token_set[data_last_position].first) {
			if (overlap + query_token_set_size - query_last_position-1  >= required_overlap) {
				overlap += edfunc->CalculateOverlap(query_position_collection[query_object_id].token_set, data_position_collection[data_object_id].token_set, edth_, query_last_position+1, overlap); // calculate exact similarity
				if (overlap >= required_overlap)
                {
					const string& data_string = string_collection[data_position_collection[data_object_id].string_id];
					int ed;
					if ((ed = edfunc->VerifyEditDistance(query_string, data_string, edth_))<=edth_)
					{
totalCandNum ++;
						results.push_back(data_object_id);
#ifdef RESULTOUT
						int query_string_id = query_position_collection[query_object_id].string_id;
						int data_string_id = data_position_collection[data_object_id].string_id;
					   // cout << "RESULT: ";
						//cout << query_object_id << " " << data_object_id <<  " " << edth_ << endl;
						if (query_string_id < data_string_id)
						{
							int tmp = query_string_id;
							query_string_id = data_string_id;
							data_string_id = tmp;
						}
						OutputResult(query_string_id, data_string_id, ed, string_collection);

						//cout << "[" << string_collection[query_string_id] << "] ";
						//cout << query_string_id << " " << data_string_id << " " << edth_ << " ";

						//cout << "[" << string_collection[query_string_id] << "] ";
						//cout << "[" << string_collection[data_string_id] << "]" << endl;
#endif
					}

				}
			}
		}
		else {
			if (overlap + data_token_set_size - data_last_position-1  >= required_overlap) {
				overlap += edfunc->CalculateOverlap(query_position_collection[query_object_id].token_set, data_position_collection[data_object_id].token_set, edth_, overlap, data_last_position+1); // calculate exact similarity
				if (overlap >= required_overlap)
                {
					const string& data_string = string_collection[data_position_collection[data_object_id].string_id];
					int ed;
					if ((ed = edfunc->VerifyEditDistance(query_string, data_string, edth_))<=edth_)
					{
totalCandNum ++;
						results.push_back(data_object_id);
#ifdef RESULTOUT
						int query_string_id = query_position_collection[query_object_id].string_id;
						int data_string_id = data_position_collection[data_object_id].string_id;
						// cout << "RESULT: ";
						//cout << query_object_id << " " << data_object_id <<  " " << edth_ << endl;
						if (query_string_id < data_string_id)
						{
							int tmp = query_string_id;
							query_string_id = data_string_id;
							data_string_id = tmp;
						}
						OutputResult(query_string_id, data_string_id, ed, string_collection);
						//cout << query_string_id << " " << data_string_id << " " << edth_ << " ";

					   // cout << "[" << string_collection[query_string_id] << "] ";
					   // cout << "[" << string_collection[data_string_id] << "]" << endl;
#endif
					}

				}
			}
		}
	}

}

int AdaptJoin::__UpdateIndexOnePrefixScheme(const vector<PositionObject>& object_collection, int begin, int end, const vector<int>& index_prefix_size_store, int prefix_scheme, ReverseDeltaInvertedIndex& index)
{
	while (begin < end)
	{
		int added_prefix_scheme_pos = index_prefix_size_store[begin]+prefix_scheme-2;
		if (added_prefix_scheme_pos<(int)object_collection[begin].token_set.size())
			break;
		++begin;
	}
	for (int i=begin; i<end; ++i)
	{
		if (!index_prefix_size_store[i]) continue;
		int added_prefix_scheme_pos = index_prefix_size_store[i]+prefix_scheme-2;
		if (added_prefix_scheme_pos<(int)object_collection[i].token_set.size())
			index.AddDeltaToken(prefix_scheme, object_collection[i].token_set[added_prefix_scheme_pos].first, i, object_collection[i].token_set[added_prefix_scheme_pos].second);
	}
	return begin;
}

void AdaptJoin::PerformEdjoin(const vector<string>& string_collection)
{
	vector<PositionObject> gram_position_collection, chunk_position_collection;
	__MapStringToObjectNoDupIntGram(string_collection, gram_position_collection, chunk_position_collection);

	const vector<PositionObject>* query_collection_pointer = NULL, *data_collection_pointer = NULL;
	switch(algorithm_choice_)
	{
	case GRAM:query_collection_pointer = &gram_position_collection; data_collection_pointer = &gram_position_collection; break;
	case INDEXGRAM: query_collection_pointer = &chunk_position_collection; data_collection_pointer = &gram_position_collection; break;
	case INDEXCHUNK: query_collection_pointer = &gram_position_collection; data_collection_pointer = &chunk_position_collection; break;
	default:assert(0);break;
	}
	const vector<PositionObject>& query_position_collection = *query_collection_pointer;
	const vector<PositionObject>& data_position_collection = *data_collection_pointer;

	FixedSizeHashMap object_to_count(data_position_collection.size()+1);
    int max_prefix_scheme = 10;
	ReverseDeltaInvertedIndex index(max_token_id+1, max_prefix_scheme);
	vector<vector<int> > candidates(max_token_set_size+1);
	vector<int> prefix_scheme_count(max_token_set_size+1,0);
	int sample_times;
	int required_overlap;
	long long totalResultNum = 0;
    long long filterCost = 0;

	// On-demand Index
	int current_prefix_scheme = 1;
	vector<int> index_prefix_size_store(data_position_collection.size(),0);
	double tolerant_cost = 0.1*data_position_collection.size();
	double current_cost = 0;
    int begin = 0;
    int length_lower_bound_object_id = 0;
	vector<pair<int,int> > chunk_set;
	for (int i=0; i<int(query_position_collection.size()); ++i)
	{

		int string_id = query_position_collection[i].string_id;
		const vector<pair<int,int> >& query_set = query_position_collection[i].token_set;

		int query_token_set_size = query_set.size();

		int probe_prefix_size = edfunc->GetProbePrefixSize(query_token_set_size, edth_);
		if (probe_prefix_size<=0)	continue;
		assert(probe_prefix_size<=query_token_set_size);

		int length_lower_bound = edfunc->GetLengthLowerBound(string_collection[string_id].size(), edth_);
        for ( ;length_lower_bound_object_id<i && int(string_collection[data_position_collection[length_lower_bound_object_id].string_id].length()) < length_lower_bound; ++length_lower_bound_object_id);
		required_overlap = edfunc->GetRequiredOverlap(query_token_set_size, edth_);

		if (required_overlap == 0) // no filter
		{

     		vector<int> candidate;
			for (int j=length_lower_bound_object_id; j<i; ++j)
			{
				if ((int)string_collection[query_position_collection[j].string_id].size()<length_lower_bound)
					continue;
				candidate.push_back(j);
			}
// totalCandNum += candidate.size();
			vector<int> results;
			__VerifyCandidatesEd(i, query_position_collection, candidate, data_position_collection, string_collection, results);

			totalResultNum += results.size();

		}
		else
		{
			for (int j=0; j<probe_prefix_size; ++j)
			{
	//			if (!index.ExistDeltaToken(1, query_set[j].first))
	//				continue;
				TokenList* token_list = index.GetDeltaTokenList(1, query_set[j].first);
                if (token_list == NULL) continue;
	filterCost += token_list->size();

				int k;
				for (k = token_list->start_position;
					k < int(token_list->list.size())
					&& token_list->list[k].object_id < length_lower_bound_object_id;
					++k);
				for (token_list->start_position=k; k<int(token_list->list.size()); ++k)
				{
					// gram position filtering
					int data_object_id = token_list->list[k].object_id;
					int c = object_to_count.Count(data_object_id);
					object_to_count.PlusOne(data_object_id);
					candidates[c+1].push_back(data_object_id);
				}
			}
			vector<TokenList*> added_token_list;
			vector<int> query_gram_positions;
			int delta_filter_cost;
			int verify_unit_cost = 0.25*edth_*string_collection[string_id].size();
			int total_prefix_scheme = query_token_set_size-probe_prefix_size+1;
			int prefix_scheme = 2;
			for (; prefix_scheme<=current_prefix_scheme+1 && prefix_scheme<=total_prefix_scheme; ++prefix_scheme)
			{
				// On-demand Index
				int cost = verify_unit_cost*(candidates[prefix_scheme-1].size()-candidates[prefix_scheme].size())-__EstimateCost(probe_prefix_size+prefix_scheme, prefix_scheme+1, 5);
				if (cost <= 0)
					break;
				else if (prefix_scheme==current_prefix_scheme+1)
				{
					// On-demand Index
					current_cost += cost;
					if (current_cost < tolerant_cost || current_prefix_scheme>=max_prefix_scheme)
						break;
					else
					{
						++current_prefix_scheme;
						begin = __UpdateIndexOnePrefixScheme(data_position_collection, begin, i, index_prefix_size_store, current_prefix_scheme, index);
						current_cost = 0;
					}
				}

				added_token_list.clear();
				query_gram_positions.clear();
				int current_probe_prefix_size = probe_prefix_size+prefix_scheme-1;
				delta_filter_cost = 0;
				for (int j=prefix_scheme-1; j<current_probe_prefix_size-1; ++j)
				{
                    TokenList* token_list = index.GetDeltaTokenList(prefix_scheme, query_set[j].first);
					if (token_list == NULL)
						continue;
					added_token_list.push_back(token_list);
					query_gram_positions.push_back(query_set[j].second);
					delta_filter_cost += added_token_list.back()->size();
				}
				int added_token = query_set[current_probe_prefix_size-1].first;
				for (int j=1; j<=prefix_scheme; ++j)
				{
                    TokenList* token_list = index.GetDeltaTokenList(j, added_token);
					if (token_list == NULL)
						continue;
					added_token_list.push_back(token_list);
					query_gram_positions.push_back(query_set[current_probe_prefix_size-1].second);
					delta_filter_cost += added_token_list.back()->size();
				}

				sample_times = max(int(delta_filter_cost*0.01),5);
				int estimate_cost = __EstimateCost(current_probe_prefix_size+1, prefix_scheme+1, sample_times);
				if (verify_unit_cost*int(candidates[prefix_scheme-1].size())<delta_filter_cost+estimate_cost)
					break;
				double estimate_candidate_size = __EstimateCandidateSize(candidates[prefix_scheme].size(), object_to_count, prefix_scheme, added_token_list, sample_times);
				if (verify_unit_cost*int(candidates[prefix_scheme-1].size()) < verify_unit_cost*estimate_candidate_size+delta_filter_cost+estimate_cost)
            		break;


				for (int j=0; j<int(added_token_list.size()); ++j)
				{
					TokenList* token_list = added_token_list[j];
filterCost += token_list->size();
					int k;
					for (k = token_list->start_position;
						k < int(token_list->list.size())
						&& token_list->list[k].object_id < length_lower_bound_object_id;
					++k);

					for (token_list->start_position=k; k<int(token_list->list.size()); ++k)
					{
						int data_object_id = token_list->list[k].object_id;
						int c = object_to_count.Count(data_object_id);
						if (c >= prefix_scheme-1)
						{
							object_to_count.PlusOne(data_object_id);
							candidates[c+1].push_back(data_object_id);
						}
					}
				}
			}
			prefix_scheme_count[prefix_scheme-1]++;
			vector<int> results;
			if (candidates[prefix_scheme-1].size())
			{
				if (string_collection[string_id].size()<20)		__VerifyCandidatesEd(i, query_position_collection, candidates[prefix_scheme-1], data_position_collection, string_collection, results);
				else __VerifyCandidatesEdOp(i, query_position_collection, candidates[prefix_scheme-1], data_position_collection, string_collection, object_to_count, prefix_scheme-1, index_prefix_size_store, required_overlap, results);
			}
			totalResultNum += results.size();
		}

		const vector<pair<int,int> >& data_set = data_position_collection[i].token_set;
		int index_prefix_size = edfunc->GetIndexPrefixSize(data_set.size(), edth_);
		index_prefix_size_store[i] = index_prefix_size; // On-demand Index
		for (int j=0; j<index_prefix_size; ++j)
		{
			index.AddDeltaToken(1, data_set[j].first, i, data_set[j].second);
		}
		for (uint j=index_prefix_size; j-index_prefix_size+1<(unsigned)current_prefix_scheme && j<data_set.size(); ++j)
//		for (uint j=index_prefix_size; j<=index_prefix_size; ++j)
		{
			index.AddDeltaToken(j-index_prefix_size+2, data_set[j].first, i, data_set[j].second);
		}

		object_to_count.clear();
		for (uint j=1; j<candidates.size(); ++j)
		{
			if (candidates[j].size() == 0)
				break;
			candidates[j].clear();
		}

	}

   cout << totalResultNum << endl;
   cout << totalCandNum << endl;
}

