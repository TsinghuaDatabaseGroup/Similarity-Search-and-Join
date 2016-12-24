#include "adapt_join.h"

void AdaptJoin::__MapStringToObjectNoDupStrGram(const vector<string>& string_collection, vector<PositionObject>& position_gram_collection, vector<PositionObject>& position_chunk_collection)
{
	vector<pair<int,int> > id_length(string_collection.size());
	for (uint i=0; i<string_collection.size(); ++i)
	{
		id_length[i] = pair<int,int>(i, string_collection[i].size());
	}
	sort(id_length.begin(), id_length.end(), cmpsecond);
	vector<int> id_to_length_id(string_collection.size());
	for (uint i=0; i<id_length.size(); ++i)
	{
		id_to_length_id[i] = id_length[i].first;
	}

	position_gram_collection.resize(string_collection.size());
	position_chunk_collection.resize(string_collection.size());
	HashMap<string, int> gram_to_count, chunk_to_count;
	//object_to_count.rehash(ESTIMATE_MAX_DISTINCT_TOKEN_NUMBER);
	vector<int> unique_id_count;
	UniqueIdGenerator unique_id_generator;
	string newToken;
	newToken.reserve(100);

	vector<string> token_set;
	for (uint i=0; i<string_collection.size(); ++i)
	{
		gram_to_count.clear(); chunk_to_count.clear();
		token_set.clear();
		tokenizer->tokenize(string_collection[id_to_length_id[i]], token_set);
		position_gram_collection[i].string_id = id_to_length_id[i]; position_gram_collection[i].token_set.reserve(token_set.size());
		position_chunk_collection[i].string_id = id_to_length_id[i]; position_chunk_collection[i].token_set.reserve(token_set.size()/q_+1);

		for (uint j=0; j<token_set.size(); ++j)
		{
			int unique_id;
			const string& token = token_set[j];
			if (!gram_to_count.count(token))
			{
				unique_id = unique_id_generator.GetUniqueId(token);
				gram_to_count.insert(HashMap<string, int>::value_type(token, 1));
			}
			else
			{
				int c = ++gram_to_count[token];
				newToken.assign(token);
				__ConstructNewToken(newToken, c);
				unique_id = unique_id_generator.GetUniqueId(newToken);
			}
			position_gram_collection[i].token_set.push_back(make_pair(unique_id,j));
			assert(unique_id<=int(unique_id_count.size()));
			if (unique_id == (int)unique_id_count.size())
				unique_id_count.push_back(1);
			else
				++unique_id_count[unique_id];

			if (j%q_ == 0)
			{
				if (!chunk_to_count.count(token))
				{
					unique_id = unique_id_generator.GetUniqueId(token);
					chunk_to_count.insert(HashMap<string, int>::value_type(token, 1));
				}
				else
				{
					int c = ++chunk_to_count[token];
					newToken.assign(token);
					__ConstructNewToken(newToken, c);
					unique_id = unique_id_generator.GetUniqueId(newToken);
				}
				position_chunk_collection[i].token_set.push_back(make_pair(unique_id,j));
			}

		}
	}
	max_token_id = unique_id_generator.getMaxId();
	vector<pair<int, int> > unique_id_pos_count;
	for (uint i=0; i<unique_id_count.size(); ++i)
	{
		unique_id_pos_count.push_back(make_pair(i, unique_id_count[i]));
	}
	sort(unique_id_pos_count.begin(), unique_id_pos_count.end(), cmppair);
	for (uint i=0; i<unique_id_pos_count.size(); ++i)
	{
		unique_id_count[unique_id_pos_count[i].first] = i;
	}

	for (uint i=0; i<position_gram_collection.size(); ++i)
	{
		vector<pair<int,int> >& gram_set = position_gram_collection[i].token_set;
		for (uint j=0; j<gram_set.size(); ++j)
		{
			gram_set[j].first = unique_id_count[gram_set[j].first];
		}
		sort(gram_set.begin(), gram_set.end(), cmpfirst);
	}
	for (uint i=0; i<position_chunk_collection.size(); ++i)
	{
		vector<pair<int,int> >& chunk_set = position_chunk_collection[i].token_set;
		for (uint j=0; j<chunk_set.size(); ++j)
		{
			chunk_set[j].first = unique_id_count[chunk_set[j].first];
		}
		sort(chunk_set.begin(), chunk_set.end(), cmpfirst);
	}
	max_token_set_size = position_gram_collection.back().token_set.size();
}


void AdaptJoin::__MapStringToObjectNoDupIntGram(const vector<string>& string_collection, vector<PositionObject>& position_gram_collection, vector<PositionObject>& position_chunk_collection)
{
	vector<pair<int,int> > id_length(string_collection.size());
	for (uint i=0; i<string_collection.size(); ++i)
	{
		id_length[i] = pair<int,int>(i, string_collection[i].size());
	}
	sort(id_length.begin(), id_length.end(), cmpsecond);
	vector<int> id_to_length_id(string_collection.size());
	for (uint i=0; i<id_length.size(); ++i)
	{
		id_to_length_id[i] = id_length[i].first;
	}

	position_gram_collection.resize(string_collection.size());
	position_chunk_collection.resize(string_collection.size());

	HashMap<int,int> int_gram_to_count;

	vector<string> gram_set;
	for (uint i=0; i<string_collection.size(); ++i)
	{
		gram_set.clear();
		tokenizer->tokenize(string_collection[id_to_length_id[i]], gram_set);
		position_gram_collection[i].string_id = id_to_length_id[i]; position_gram_collection[i].token_set.reserve(gram_set.size());
		position_chunk_collection[i].string_id = id_to_length_id[i]; position_chunk_collection[i].token_set.reserve(gram_set.size()/q_+1);

		for (uint j=0; j<gram_set.size(); ++j)
		{
			int intgram = strHash(gram_set[j].c_str(), gram_set[j].size());

			position_gram_collection[i].token_set.push_back(make_pair(intgram,j));

			++int_gram_to_count[intgram];
		}
	}
	max_token_set_size = position_gram_collection.back().token_set.size();
	vector<pair<int, int> > int_gram_and_count;
	for (HashMap<int,int>::iterator it = int_gram_to_count.begin(); it != int_gram_to_count.end(); ++it)
	{
		int_gram_and_count.push_back(*it);
	}
	sort(int_gram_and_count.begin(), int_gram_and_count.end(), cmpsecond);
	int rank = 0;
	for (vector<pair<int, int> >::iterator it = int_gram_and_count.begin(); it != int_gram_and_count.end(); ++it)
	{
		int_gram_to_count[it->first] = rank++;
	}

	for (uint i=0; i<position_gram_collection.size(); ++i)
	{
		vector<pair<int,int> >& gram_set = position_gram_collection[i].token_set;
		for (uint j=0; j<gram_set.size(); ++j)
		{
			gram_set[j].first = int_gram_to_count[gram_set[j].first];
		}
		sort(gram_set.begin(), gram_set.end(), cmprank);
	}

	// map rank to unique id
	vector<int> max_count(int_gram_and_count.size(),0);
	for (uint i=0; i<position_gram_collection.size(); ++i)
	{
		vector<pair<int,int> >& gram_set = position_gram_collection[i].token_set;
		assert(gram_set.size());
		int last_int_gram = gram_set[0].first;
		int gram_count = 1;
		for (uint j=1; j<gram_set.size(); ++j)
		{
			if (gram_set[j].first!=last_int_gram)
			{
				if (max_count[last_int_gram] < gram_count)
					max_count[last_int_gram] = gram_count;
				last_int_gram = gram_set[j].first;
				gram_count = 1;
			}
			else
				++gram_count;
		}
		if (max_count[last_int_gram] < gram_count)
			max_count[last_int_gram] = gram_count;
	}
	for (uint i=1; i<max_count.size(); ++i)
		max_count[i] += max_count[i-1];
	max_token_id = max_count.back();

	for (uint i=0; i<position_gram_collection.size(); ++i)
	{
		vector<pair<int,int> >& gram_set = position_gram_collection[i].token_set;
		int last_chunk = -1, last_gram = -1;
		int chunk_count = 0, gram_count = 0;
		for (uint j=0; j<gram_set.size(); ++j)
		{
			int base = gram_set[j].first>0 ? max_count[gram_set[j].first-1]:0;
			if (gram_set[j].second % q_ == 0)
			{
				if (gram_set[j].first != last_chunk)
				{
					chunk_count = 1;
					last_chunk = gram_set[j].first;
				}
				else
					++chunk_count;
				position_chunk_collection[i].token_set.push_back(make_pair(base+chunk_count-1, gram_set[j].second));
			}
			if (gram_set[j].first != last_gram)
			{
				gram_count = 1;
				last_gram = gram_set[j].first;
			}
			else
				++gram_count;

			gram_set[j].first = base+gram_count-1;
		}
	}

}


bool AdaptJoin::cmprank(const pair<int,int>& rank_and_post_1, const pair<int,int>& rank_and_post_2)
{
	if (rank_and_post_1.first == rank_and_post_2.first)
		return rank_and_post_1.second < rank_and_post_2.second;
	return rank_and_post_1.first < rank_and_post_2.first;
}
void AdaptJoin::__MapStringToObjectDupIntGram(const vector<string>& string_collection, vector<PositionObject>& position_gram_collection, vector<PositionObject>& position_chunk_collection)
{
	vector<pair<int,int> > id_length(string_collection.size());
	for (uint i=0; i<string_collection.size(); ++i)
	{
		id_length[i] = pair<int,int>(i, string_collection[i].size());
	}
	sort(id_length.begin(), id_length.end(), cmpsecond);
	vector<int> id_to_length_id(string_collection.size());
	for (uint i=0; i<id_length.size(); ++i)
	{
		id_to_length_id[i] = id_length[i].first;
	}

	position_gram_collection.resize(string_collection.size());
	position_chunk_collection.resize(string_collection.size());

	HashMap<int,int> int_gram_to_count;

	vector<string> gram_set;
	for (uint i=0; i<string_collection.size(); ++i)
	{
		gram_set.clear();
		tokenizer->tokenize(string_collection[id_to_length_id[i]], gram_set);
		position_gram_collection[i].string_id = id_to_length_id[i]; position_gram_collection[i].token_set.reserve(gram_set.size());

		for (uint j=0; j<gram_set.size(); ++j)
		{
			int intgram = strHash(gram_set[j].c_str(), gram_set[j].size());

			position_gram_collection[i].token_set.push_back(make_pair(intgram,j));

			++int_gram_to_count[intgram];
		}
	}
	max_token_id = int_gram_to_count.size();
	vector<pair<int, int> > int_gram_and_count;
	for (HashMap<int,int>::iterator it = int_gram_to_count.begin(); it != int_gram_to_count.end(); ++it)
	{
		int_gram_and_count.push_back(*it);
	}
	sort(int_gram_and_count.begin(), int_gram_and_count.end(), cmpsecond);
	int rank = 0;
	for (vector<pair<int, int> >::iterator it = int_gram_and_count.begin(); it != int_gram_and_count.end(); ++it)
	{
		int_gram_to_count[it->first] = rank++;
	}

	for (uint i=0; i<position_gram_collection.size(); ++i)
	{
		vector<pair<int,int> >& gram_set = position_gram_collection[i].token_set;
		for (uint j=0; j<gram_set.size(); ++j)
		{
			gram_set[j].first = int_gram_to_count[gram_set[j].first];
		}
		sort(gram_set.begin(), gram_set.end(), cmprank);
	}
	max_token_set_size = position_gram_collection.back().token_set.size();
	for (uint i=0; i<position_gram_collection.size(); ++i)
	{
		vector<pair<int,int> >& gram_set = position_gram_collection[i].token_set;
		position_chunk_collection[i].string_id = id_to_length_id[i]; position_chunk_collection[i].token_set.reserve(gram_set.size()/q_+1);
		for (uint j=0; j<gram_set.size(); ++j)
		{
			if (gram_set[j].second % q_ == 0)
				position_chunk_collection[i].token_set.push_back(gram_set[j]);
		}
	}
}

