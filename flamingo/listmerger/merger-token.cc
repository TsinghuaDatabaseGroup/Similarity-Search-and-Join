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

#include <cstdlib>
#include <cassert>

using namespace std;

long long res_num;
long long cand_num;
double THRESHOLD;

vector<vector<int>> records;

int check_overlap(vector<int> &a, vector<int> &b, int overlap)
{
	int posa = 0, posb = 0, count = 0;
	while (posa < (int)a.size() && posb < (int)b.size())
	{
		if (count + min((int)a.size() - posa, (int)b.size() - posb) < overlap)
			return -1;
		if (a[posa] == b[posb])
		{
			count++;
			posa++;
			posb++;
		} else if (a[posa] < b[posb])
		{
			posa++;
		} else {
			posb++;
		}
	}
	return count;
}

bool jac(vector<int> &a, vector<int> &b, double threshold)
{
	double factor = THRESHOLD / (1 + THRESHOLD);
	int require_overlap = factor * (a.size() + b.size()) - 1e-6 + 1;
	int real_overlap = check_overlap(a, b, require_overlap);

	return real_overlap == -1 ? false :
		(real_overlap / (double)(a.size() + b.size() - real_overlap)) >= threshold;
}

bool cosine(vector<int> &a, vector<int> &b, double threshold)
{
	int require_overlap = THRESHOLD * sqrt(a.size() * b.size()) - 1e-6 + 1;
	int real_overlap = check_overlap(a, b, require_overlap);

	return real_overlap == -1 ? false :
		(real_overlap / sqrt(a.size() * b.size())) >= threshold;
}

bool dice(vector<int> &a, vector<int> &b, double threshold)
{
	int require_overlap = THRESHOLD / 2.0 * (a.size() + b.size()) - 1e-6 + 1;
	int real_overlap = check_overlap(a, b, require_overlap);

	return real_overlap == -1 ? false :
	    (2 * real_overlap / (double)(a.size() + b.size())) >= threshold;
}

int jaccard_overlap(int length)
{
	return THRESHOLD * length - 1e-6 + 1;
}

int cosine_overlap(int length)
{
	return THRESHOLD * THRESHOLD * length - 1e-6 + 1;
}

int dice_overlap(int length)
{
	return THRESHOLD / (2 - THRESHOLD) * length - 1e-6 + 1;
}

bool (*verify)(vector<int> &a, vector<int> &b, double threshold);
int (*require_overlap)(int length);

template<typename Merger>
void perform_join(Merger &merger)
{
	vector<unsigned> candidates;
	vector<vector<unsigned>*> lists;
	HashMap<int, vector<unsigned>> index;

	for (int k = 0; k < (int)records.size(); k++)
	{
		lists.clear();
		candidates.clear();

		for (int token : records[k])
		{
			lists.push_back(&index[token]);
		}

		int threshold = require_overlap(records[k].size());
		merger.merge(lists, threshold, candidates);

		for (int cand : candidates)
		{
			cand_num++;
			if (verify(records[k], records[cand], THRESHOLD))
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
// argv[0] metric filename threshold type
	if (argc != 5) return -1;
	log_start();

	if (argv[1][0] == 'j')
	{
		verify = jac;
		require_overlap = jaccard_overlap;
	} else if (argv[1][0] == 'c') {
		verify = cosine;
		require_overlap = cosine_overlap;
	} else if (argv[1][0] == 'd') {
		verify = dice;
		require_overlap = dice_overlap;
	}

	FILE *in = fopen(argv[2], "r");

	int count;
	while (fread(&count, sizeof(int), 1, in))
	{
		fread(&count, sizeof(int), 1, in);
		records.push_back(vector<int>(count));
		fread(records.back().data(), sizeof(int), count, in);
	}
	fclose(in);

	THRESHOLD = atof(argv[3]);
	int type = atoi(argv[4]);
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

