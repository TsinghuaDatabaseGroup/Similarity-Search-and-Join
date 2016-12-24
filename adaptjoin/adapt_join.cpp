#include "adapt_join.h"

extern long long totalCandNum;

AdaptJoin::AdaptJoin(void)
{
	theta_ = 0;
	simfunc = NULL;
	edfunc = NULL;
	tokenizer = NULL;
	q_ = 3;
	delimiters_ = "";
}

AdaptJoin::~AdaptJoin(void)
{
	__ClearTokenizer();
}

void AdaptJoin::__ClearTokenizer()
{
	if (tokenizer != NULL)
		delete tokenizer;
	tokenizer = NULL;
}
bool AdaptJoin::cmppair(const pair<int,int>& a, const pair<int,int>& b)
{
	if (a.second == b.second)
		return a.first < b.first;
	return a.second < b.second;
}
bool AdaptJoin::cmplen(const Object& object1, const Object& object2)
{
	return object1.token_set.size() < object2.token_set.size();
}
int AdaptJoin::__EstimateCost(int prefix_size, int prefix_scheme, int sample_times)
{
	int added_token_list_num = prefix_size+prefix_scheme-1;
    static bool init = false;
    static int store[1024];
    if (!init)
    {
        init = true;
        int n = 0, x = 1;
        for (int i=0; i<1024; ++i)
        {
            if (i>x)
            {
                ++n;
                x = 2*x;
            }
            store[i] = n;
        }
    }
    int estimate_candidate_cost = added_token_list_num+sample_times*(added_token_list_num >= 1024 ? 10 : store[added_token_list_num]);
	int estimate_filter_cost = added_token_list_num;
	return estimate_filter_cost+estimate_candidate_cost;
}
double AdaptJoin::__EstimateCandidateSize(int must_candidate_num, FixedSizeHashMap& object_to_count, int prefix_scheme, vector<TokenList*>& added_token_list, int sample_times)
{
	if (added_token_list.size() == 0) return must_candidate_num;

	vector<int> incremental_sum(added_token_list.size());
	int sum = 0;
	for (uint i=0; i<added_token_list.size(); ++i)
	{
		sum += added_token_list[i]->size();
		incremental_sum[i] = sum;
	}
	assert(sum>0);
//cout << "sum = " << sum << endl;
	int yes_num = 0;
	for (int i=0; i<sample_times; ++i)
	{
		int rand_virtual_list_pos = rand()%sum;
		int list_id = upper_bound(incremental_sum.begin(), incremental_sum.end(), rand_virtual_list_pos)-incremental_sum.begin();
		int minus_sum = (list_id ? incremental_sum[list_id-1]:0);
		int object_pos = added_token_list[list_id]->start_position+rand_virtual_list_pos-minus_sum;
		int object_id = added_token_list[list_id]->list[object_pos].object_id;
		yes_num += (object_to_count.Count(object_id)==prefix_scheme-1?1:0);
	}
	return must_candidate_num+yes_num*1.0*sum/sample_times;
}
int AdaptJoin::__UpdateIndexOnePrefixScheme(const vector<Object>& object_collection, int begin, int end, const vector<int>& index_prefix_size_store, int prefix_scheme, ReverseDeltaInvertedIndex& index)
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
		    index.AddDeltaToken(prefix_scheme, object_collection[i].token_set[added_prefix_scheme_pos], i, added_prefix_scheme_pos);
	}
    return begin;
}

void AdaptJoin::InitializeParameters(const char* simfunc_name, double theta)
{
	__ClearTokenizer();
	theta_ = theta;
	if (strcmp(simfunc_name, "jaccard") == 0)
	{
		simfunc = new JaccardSimFunc();
	}
	else if (strcmp(simfunc_name, "cosine") == 0)
	{
		simfunc = new CosineSimFunc();
	}
	else if (strcmp(simfunc_name, "dice") == 0)
	{
		simfunc = new DiceSimFunc();
	}
}

void AdaptJoin::__ConstructNewToken(string& new_token, int c) // 1+token+c --> new_token
{
	char buf[16];
	buf[0] = 1;
    sprintf(buf+1, "%d", c);
	new_token.append(buf);
}
void AdaptJoin::__MapStringToObject(const char *filename, vector<Object>& object_collection)
{
	int count, id = 0;
	FILE *in = fopen(filename, "r");
	max_token_id = -1;
	while (fread(&count, sizeof(int), 1, in))
	{
		object_collection.push_back(Object());
		object_collection.back().string_id = id++;

		fread(&count, sizeof(int), 1, in);
		vector<int> &token_set = object_collection.back().token_set;
		token_set.resize(count);
		fread(token_set.data(), sizeof(int), count, in);
		sort(token_set.begin(), token_set.end());

		if (token_set.back() > max_token_id)
			max_token_id = token_set.back();
	}
	fclose(in);
	sort(object_collection.begin(), object_collection.end(), cmplen);
	max_token_set_size = object_collection.back().token_set.size();
}

void AdaptJoin::__VerifyCandidates(int query_object_id, const vector<int>& candidates, const vector<Object>& object_collection, vector<int>& results)
{
    const vector<int>& query_token_set = object_collection[query_object_id].token_set;
totalCandNum += candidates.size();
	for (uint i=0; i<candidates.size(); ++i)
	{
		int data_object_id = candidates[i];
        const vector<int>& data_token_set = object_collection[data_object_id].token_set;
        assert((int)data_token_set.size()>=length_lower_bound);
		double sim;
		if ((sim=simfunc->GetSimilarityValue(query_token_set, data_token_set))+1e-6>=theta_)
		{
			results.push_back(data_object_id);
		}
	}
}
void AdaptJoin::__VerifyCandidatesOp(int query_object_id, const vector<int>& candidates, const vector<Object>& object_collection, FixedSizeHashMap& object_to_count, int prefix_scheme, const vector<int>& index_prefix_size_store, const vector<int>& required_overlap_vec, vector<int>& results)
{
	int query_token_set_size = object_collection[query_object_id].token_set.size();
	int query_last_position = simfunc->GetProbePrefixSize(query_token_set_size, theta_)+prefix_scheme-2;
	for (uint i=0; i<candidates.size(); ++i) {
		int data_object_id = candidates[i];
		int overlap = object_to_count.Count(data_object_id);
		assert(overlap >= prefix_scheme);
		int data_token_set_size = object_collection[data_object_id].token_set.size();
		int data_last_position = min(data_token_set_size-1,index_prefix_size_store[data_object_id]+prefix_scheme-2);

		/* optimize verification by comparing the last tokens in x's and y's prefix */
		if (object_collection[query_object_id].token_set[query_last_position] < object_collection[data_object_id].token_set[data_last_position]) {
			if (overlap + query_token_set_size - query_last_position-1  >= required_overlap_vec[data_token_set_size]) {
				overlap += simfunc->CalculateOverlap(object_collection[query_object_id].token_set, object_collection[data_object_id].token_set, query_last_position+1, overlap); // calculate exact similarity
				if (overlap >= required_overlap_vec[data_token_set_size]){
					totalCandNum++;
					results.push_back(data_object_id);
				}
			}
		}
		else {
			if (overlap + data_token_set_size - data_last_position-1  >= required_overlap_vec[data_token_set_size]) {
				overlap += simfunc->CalculateOverlap(object_collection[query_object_id].token_set, object_collection[data_object_id].token_set, overlap, data_last_position+1); // calculate exact similarity
				if (overlap >= required_overlap_vec[data_token_set_size]){
					totalCandNum++;
					results.push_back(data_object_id);
				}
			}
		}
	}

}

void AdaptJoin::PerformSimjoin(const char *filename)
{
	vector<Object> object_collection;
	__MapStringToObject(filename, object_collection);

	FixedSizeHashMap object_to_count(object_collection.size()+1);
	//DeltaInvertedIndex index(max_token_set_size+1);
    int max_prefix_scheme = 10;

	ReverseDeltaInvertedIndex index(max_token_id+1, max_prefix_scheme);
	vector<vector<int> > candidates(max_token_set_size+1);
	vector<int> prefix_scheme_count(max_token_set_size+1,0);
	int sample_times;
	vector<int> required_overlap_vec;
	long long totalResultNum = 0;
    long long filterCost = 0;

	// On-demand Index
	int current_prefix_scheme = 1;
	vector<int> index_prefix_size_store(object_collection.size(),0);
	double tolerant_cost = 0.1*object_collection.size();
	double current_cost = 0;
    int begin = 0;
	int length_lower_bound_object_id = 0;

	for (int i=0; i<int(object_collection.size()); ++i)
	{
		const vector<int>& token_set = object_collection[i].token_set;
		int query_token_set_size = token_set.size();

		int probe_prefix_size = simfunc->GetProbePrefixSize(query_token_set_size, theta_);
		if (probe_prefix_size<=0)	continue;

		required_overlap_vec.clear();
		required_overlap_vec.resize(query_token_set_size+1);

		int length_lower_bound = simfunc->GetLengthLowerBound(query_token_set_size, theta_);
		for ( ;length_lower_bound_object_id<i && int(object_collection[length_lower_bound_object_id].token_set.size()) < length_lower_bound; ++length_lower_bound_object_id);
		for (int j = length_lower_bound; j <= query_token_set_size; ++j)
			required_overlap_vec[j] = simfunc->GetRequiredOverlap(query_token_set_size, j, theta_);

		assert(probe_prefix_size<=query_token_set_size);

		for (int j=0; j<probe_prefix_size; ++j)
		{
			if (!index.ExistDeltaToken(1, token_set[j]))
				continue;
			TokenList* token_list = index.GetDeltaTokenList(1, token_set[j]);
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
				if (c==0)
				{
					//int data_token_set_size = object_collection[data_object_id].token_set.size();
					//if (query_token_set_size - j < required_overlap_vec[data_token_set_size])
                     //   continue;
					int data_token_set_size = object_collection[data_object_id].token_set.size();
					int position = token_list->list[k].position;
					if (query_token_set_size - j < required_overlap_vec[data_token_set_size]
						|| data_token_set_size - position < required_overlap_vec[data_token_set_size])
					continue;
				}

				object_to_count.PlusOne(data_object_id);
				candidates[c+1].push_back(data_object_id);
			}
		}
		vector<TokenList*> added_token_list;
		int delta_filter_cost;
		int verify_unit_cost = 0.5*query_token_set_size;
		int total_prefix_scheme = query_token_set_size-probe_prefix_size+1;
		int prefix_scheme = 2;
//bool flag = false;
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
				if (current_cost < tolerant_cost||current_prefix_scheme >= max_prefix_scheme)
					break;
				else
				{
					++current_prefix_scheme;
					begin = __UpdateIndexOnePrefixScheme(object_collection, begin, i, index_prefix_size_store, current_prefix_scheme, index);
					current_cost = 0;
				}
			}

			added_token_list.clear();
			int current_probe_prefix_size = probe_prefix_size+prefix_scheme-1;

			delta_filter_cost = 0;
			for (int j=prefix_scheme-1; j<current_probe_prefix_size-1; ++j)
			{
				if (!index.ExistDeltaToken(prefix_scheme, token_set[j]))
					continue;
				added_token_list.push_back(index.GetDeltaTokenList(prefix_scheme, token_set[j]));
				delta_filter_cost += added_token_list.back()->size();
			}
			int added_token = token_set[current_probe_prefix_size-1];
			for (int j=1; j<=prefix_scheme; ++j)
			{
				if (!index.ExistDeltaToken(j, added_token))
					continue;
				added_token_list.push_back(index.GetDeltaTokenList(j, added_token));
				delta_filter_cost += added_token_list.back()->size();
			}
			sample_times = max(int(delta_filter_cost*0.01),5);

			int estimate_cost = __EstimateCost(current_probe_prefix_size+1, prefix_scheme+1, sample_times);

            if (verify_unit_cost*int(candidates[prefix_scheme-1].size())<delta_filter_cost+estimate_cost)
                break;
			double estimate_candidate_size = __EstimateCandidateSize(candidates[prefix_scheme].size(), object_to_count, prefix_scheme, added_token_list, sample_times);
//bool finish = false;
//cout << sample_times << " " << delta_filter_cost << " " << candidates[prefix_scheme].size() << " " << estimate_candidate_size << " ";
			if (verify_unit_cost*int(candidates[prefix_scheme-1].size()) < verify_unit_cost*estimate_candidate_size+delta_filter_cost+estimate_cost)
//finish = true;
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
/*                      if (c == prefix_scheme-1)
                        {
					        int position = token_list->list[k].position;
                            int data_token_set_size = object_collection[data_object_id].token_set.size();
                            if (query_token_set_size - j + prefix_scheme - 1 < required_overlap_vec[data_token_set_size]
                               || data_token_set_size - position + prefix_scheme - 1 < required_overlap_vec[data_token_set_size])
                                continue;
                        }
*/

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
            if (probe_prefix_size+prefix_scheme-1<20)
			{
				__VerifyCandidates(i, candidates[prefix_scheme-1], object_collection, results);
			} else {
				__VerifyCandidatesOp(i, candidates[prefix_scheme-1], object_collection, object_to_count, prefix_scheme-1, index_prefix_size_store, required_overlap_vec, results);
			}
		}
		totalResultNum += results.size();
		int index_prefix_size = simfunc->GetIndexPrefixSize(query_token_set_size, theta_);
		index_prefix_size_store[i] = index_prefix_size; // On-demand Index
		for (int j=0; j<index_prefix_size; ++j)
		{
			index.AddDeltaToken(1, token_set[j], i, j);
		}
		for (uint j=index_prefix_size; j-index_prefix_size+1<(unsigned)current_prefix_scheme && j<(unsigned)query_token_set_size; ++j)
		{
			index.AddDeltaToken(j-index_prefix_size+2, token_set[j], i, j);
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

