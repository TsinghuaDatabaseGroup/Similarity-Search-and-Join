#include "listmerger.h"
#include "heapmerger.h"
#include "mergeoptmerger.h"
#include "mergeskipmerger.h"
#include "divideskipmerger.h"
#include "scancountmerger.h"

#define NO_UNDEF_TYPES

#include "../../common/help.h"
#include "../../common/time.h"
#include "../../common/type.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <string>

#include <cstdlib>
#include <cassert>

using namespace std;

long long res_num;
long long cand_num;
int THRESHOLD;
int Q;

vector<string> records;
void get_grams(const string &record, vector<int> &grams)
{
	for (int sp = 0; sp < (int)record.length() - Q + 1; sp++)
	{
		grams.push_back(DJB_hash(record.c_str() + sp, Q));
	}
}

template<typename Merger>
void perform_join(Merger &merger)
{
	vector<int> grams;
	vector<unsigned> candidates;
	vector<vector<unsigned>*> lists;
	HashMap<int, vector<unsigned>> index;

	for (int k = 0; k < (int)records.size(); k++)
	{
		if ((int)records[k].length() < (THRESHOLD + 1) * Q)
		{
			int lower = lower_bound(records.begin(), records.end(), (int)records[k].length() - THRESHOLD, [](const string &s, int value) {
				return (int)s.length() < value;
			}) - records.begin();
			for (int s = lower; s < k; s++)
			{
				if (records[s].length() + THRESHOLD >= records[k].length())
				{
					cand_num++;
					if (edit_distance(records[s], records[k], THRESHOLD) <= THRESHOLD)
					{
						res_num++;
					}
				}
			}
			continue;
		} else if ((int)records[k].length() < (THRESHOLD + 1) * Q + THRESHOLD) {
			int lower = lower_bound(records.begin(), records.end(), (int)records[k].length() - THRESHOLD, [](const string &s, int value) {
				return (int)s.length() < value;
			}) - records.begin();
			int upper = lower_bound(records.begin(), records.end(), (THRESHOLD + 1) * Q, [](const string &s, int value) {
				return (int)s.length() < value;
			}) - records.begin();
			for (int s = lower; s < upper; s++)
			{
				if (records[s].length() + THRESHOLD >= records[k].length())
				{
					cand_num++;
					if (edit_distance(records[s], records[k], THRESHOLD) <= THRESHOLD)
					{
						res_num++;
					}
				}
			}
		}

		grams.clear();
		candidates.clear();
		lists.clear();

		get_grams(records[k], grams);
		for (int gram : grams)
		{
			lists.push_back(&index[gram]);
		}

		int threshold = records[k].length() - Q + 1 - THRESHOLD * Q;
		merger.merge(lists, threshold, candidates);

		for (int cand : candidates)
		{
			cand_num++;
			if (edit_distance(records[k], records[cand], THRESHOLD) <= THRESHOLD)
			{
				res_num++;
			}
		}

		sort(lists.begin(), lists.end());
		auto end_range = unique(lists.begin(), lists.end());
		for (auto start_range = lists.begin(); start_range != end_range; start_range++)
		{
			(*start_range)->push_back(k);
		}
	}
}

int main(int argc, char **argv)
{
	if (argc != 5) return -1;

	log_start();
	ifstream in(argv[1]);
	THRESHOLD = atoi(argv[2]);
	Q = atoi(argv[4]);

	string line;
	while (getline(in, line))
	{
		records.push_back(move(line));
	}
	sort(records.begin(), records.end(), [](const string &s1, const string &s2) {
		return s1.length() < s2.length();
	});

	int type = atoi(argv[3]);
	if (type == 0)
	{
		HeapMerger<> merger;
		perform_join(merger);
	} else if (type == 1) {
		MergeOptMerger<> merger(true);
		perform_join(merger);
	} else if (type == 2) {
		ScanCountMerger<> merger(records.size());
		perform_join(merger);
	} else if (type == 3) {
		MergeSkipMerger<> merger;
		perform_join(merger);
	} else if (type == 4) {
		DivideSkipMerger<> merger;
		perform_join(merger);
	}

	cout << res_num << endl;
	cout << cand_num << endl;
	return 0;
}

