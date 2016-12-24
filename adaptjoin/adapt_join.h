#pragma once
#include "common/type.h"
#include "simfunc.h"
#include "jaccard_simfunc.h"
#include "dice_simfunc.h"
#include "cosine_simfunc.h"
#include "edit_distance.h"
#include "edit_distance_gram.h"
#include "edit_distance_index_gram.h"
#include "edit_distance_index_chunk.h"
#include "unique_id_generator.h"
#include "fixed_size_hash_map.h"
#include "delta_inverted_index.h"
#include "reverse_delta_inverted_index.h"
#include "Tokenizer.h"
#include "StandardTokenizer.h"
#include "QGramTokenizer.h"
#include "ChunkTokenizer.h"
#include "TypeDefs.h"
#include <algorithm>
#include <cassert>
using namespace std;

// #define RESULTOUT
struct Object
{
	int string_id;
	vector<int> token_set;
};
struct PositionObject
{
	int string_id;
	vector<pair<int,int> > token_set; // (gram, pos)
};
enum AlgorithmChoice{GRAM, INDEXGRAM, INDEXCHUNK};
class AdaptJoin
{
	int max_token_set_size;
	int max_token_id;
	double theta_;
	SimFunc* simfunc;
	Tokenizer* tokenizer;
	int q_;
	string delimiters_;
private:
	static bool cmppair(const pair<int,int>& a, const pair<int,int>& b);
	static bool cmplen(const Object& a, const Object& b);
	void __ClearTokenizer();
	int __UpdateIndexOnePrefixScheme(const vector<Object>& object_collection, int begin, int end, const vector<int>& index_prefix_size_store, int prefix_scheme, ReverseDeltaInvertedIndex& index);
	int __EstimateCost(int prefix_size, int prefix_scheme, int sample_times);
	double __EstimateCandidateSize(int must_candidate_num, FixedSizeHashMap& object_to_count, int prefix_scheme, vector<TokenList*>& added_token_list, int sample_times);
	void __ConstructNewToken(string& new_token, int c);
	void __MapStringToObject(const char *filename, vector<Object>& object_collection);
	void __VerifyCandidates(int query_object_id, const vector<int>& candidates, const vector<Object>& object_collection, vector<int>& results);
	void __VerifyCandidatesOp(int query_object_id, const vector<int>& candidates, const vector<Object>& object_collection, FixedSizeHashMap& object_to_count, int prefix_scheme, const vector<int>& index_prefix_size_store, const vector<int>& required_overlap_vec, vector<int>& results);
public:
	AdaptJoin(void);
	~AdaptJoin(void);
	void InitializeParameters(const char* simfunc_name, double theta);
	void PerformSimjoin(const char *filename);

// Edit Distance Join
private:
	EditDistance* edfunc;
	AlgorithmChoice algorithm_choice_;
	int edth_;
	void __MapStringToObjectNoDupStrGram(const vector<string>& string_collection, vector<PositionObject>& position_object_collection, vector<PositionObject>& position_chunk_collection);
	void __MapStringToObjectNoDupIntGram(const vector<string>& string_collection, vector<PositionObject>& position_object_collection, vector<PositionObject>& position_chunk_collection);
	void __MapStringToObjectDupIntGram(const vector<string>& string_collection, vector<PositionObject>& position_object_collection, vector<PositionObject>& position_chunk_collection);
	int __UpdateIndexOnePrefixScheme(const vector<PositionObject>& object_collection, int begin, int end, const vector<int>& index_prefix_size_store, int prefix_scheme, ReverseDeltaInvertedIndex& index);
	//void __GetChunkSetFromGramSet(const vector<PositionObject>& gram_position_collection, vector<PositionObject>& chunk_position_collection);
	void __VerifyCandidatesEd(int query_object_id, const vector<PositionObject>& query_position_collection, const vector<int>& candidates, const vector<PositionObject>& data_position_collection, const vector<string>& string_colleciton, vector<int>& results);
	void __VerifyCandidatesEdOp(int query_object_id, const vector<PositionObject>& query_position_collection, const vector<int>& candidates, const vector<PositionObject>& data_position_collection, const vector<string>& string_colleciton, FixedSizeHashMap& object_to_count, int prefix_scheme, const vector<int>& index_prefix_size_store, int required_overlap, vector<int>& results);
	static bool cmpfirst(const pair<int,int>& a, const pair<int,int>& b);
	static bool cmpsecond(const pair<int,int>& a, const pair<int,int>& b);
	static bool cmprank(const pair<int,int>& rank_and_post_1, const pair<int,int>& rank_and_post_2);
	void OutputResult(int query_string_id, int data_string_id, int edth, const vector<string>& string_collection);
public:
	void PerformEdjoin(const vector<string>& string_colleciton);
	void InitializeParameters(int edth, int q, AlgorithmChoice algorithm_choice);
};
