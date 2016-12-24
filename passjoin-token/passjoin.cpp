#include <boost/functional/hash.hpp>
#include "common/token.h"
#include "common/time.h"
#include "common/type.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdio>

using namespace std;

double THRESHOLD;
long long cand_num;
long long res_num;

base_helper *helper;

namespace pattern
{
	int *seg_parts;
	int **seg_part_len;
	int **seg_part_pos;

	struct sub_match
	{
		int match_len;
		int match_part;
		int start_pos;
		int part_len;
	};

	vector<sub_match> *sub_matches;
	HashMap<size_t, vector<int>> **indexes;

	void init(int max_length)
	{
		seg_parts = new int[max_length + 1];
		seg_part_len = new int*[max_length + 1];
		seg_part_pos = new int*[max_length + 1];
		indexes = new HashMap<size_t, vector<int>>*[max_length + 1];
		for (int len = 1; len <= max_length; len++)
		{
			int to_match_len = helper->max_possible_length(len);
			seg_parts[len] = len + to_match_len - 2 * helper->require_overlap(len, to_match_len) + 1 + 1e-6;

			int part = seg_parts[len];
			indexes[len] = new HashMap<size_t, vector<int>>[part];
			seg_part_pos[len] = new int[part + 1];
			seg_part_len[len] = new int[part];

			int larger_num = len - len / part * part;
			for (int k = 0; k < part - larger_num; k++)
			{
				seg_part_len[len][k] = len / part;
			}
			for (int k = part - larger_num; k < part; k++)
			{
				seg_part_len[len][k] = len / part + 1;
			}

			seg_part_pos[len][0] = 0;
			for (int k = 1; k <= part; k++)
			{
				seg_part_pos[len][k] = seg_part_pos[len][k - 1] + seg_part_len[len][k - 1];
			}

			assert(seg_part_pos[len][part] == len);
		}

		sub_matches = new vector<sub_match>[max_length + 1];
		for (int len = 1; len <= max_length; len++)
		{
			for (int match_len = len; match_len <= min(helper->max_possible_length(len), max_length); match_len++)
			{
				int overlap = helper->require_overlap(len, match_len);
				int hr = match_len - overlap;
				int hs = len - overlap;

				for (int part = 0; part < seg_parts[match_len]; part++)
				{
					int p = seg_part_pos[match_len][part];
					int l = seg_part_len[match_len][part];

					int low = max(0, max(p - hr, p - part));
					int high = min(len - l, min(p + hs, p + len - match_len + seg_parts[match_len] - part - 1));
					for (int s = low; s <= high; s++)
					{
						sub_matches[len].push_back(sub_match());
						sub_match &last = sub_matches[len].back();
						last.match_len = match_len;
						last.match_part = part;
						last.start_pos = s;
						last.part_len = l;
					}
				}
			}
		}
	}
};

bool verify(vector<int> &a, vector<int> &b, double threshold)
{
	int posa = 0, posb = 0, count = 0;
	int overlap = helper->require_overlap(a.size(), b.size());

	while (posa < (int)a.size() && posb < (int)b.size())
	{
		if (count + min((int)a.size() - posa, (int)b.size() - posb) < overlap)
			return false;
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

	return helper->calc_similarity(a.size(), b.size(), count) >= threshold;
}

void join(vector<vector<int>> &token)
{
	bool *checked_flag = new bool[token.size()];
	for (int id = 0; id < (int)token.size(); id++)
	{
		int len = token[id].size();
		int part = pattern::seg_parts[len];

		memset(checked_flag, 0, id);
		for (pattern::sub_match &match : pattern::sub_matches[len])
		{
			int hash_value = boost::hash_range(token[id].begin() + match.start_pos, token[id].begin() + match.start_pos + match.part_len);
			auto &potential_candidates = pattern::indexes[match.match_len][match.match_part];
			if (potential_candidates.count(hash_value))
			{
				vector<int> &candidate = potential_candidates[hash_value];
				for (int cand_id : candidate)
				{
					if (!checked_flag[cand_id])
					{
						cand_num++;
						checked_flag[cand_id] = true;
						if (verify(token[id], token[cand_id], THRESHOLD))
						{
							res_num++;
						}
					}
				}
			}
		}

		for (int k = 0; k < part; k++)
		{
			int hash_value = boost::hash_range(token[id].begin() + pattern::seg_part_pos[len][k], token[id].begin() + pattern::seg_part_pos[len][k + 1]);
			pattern::indexes[len][k][hash_value].push_back(id);
		}
	}
	delete[] checked_flag;
}

int main(int argc, char **argv)
{
	if (argc != 4) return -1;
	log_start();

	FILE *in = fopen(argv[2], "r");
	THRESHOLD = atof(argv[3]);

	if (argv[1][0] == 'j')
	{
		helper = new jaccard_helper();
	} else if (argv[1][0] == 'c')
	{
		helper = new cosine_helper();
	} else if (argv[1][0] == 'd')
	{
		helper = new dice_helper();
	} else {
		return -1;
	}

	int count;
	vector<vector<int>> token;
	while (fread(&count, sizeof(int), 1, in))
	{
		fread(&count, sizeof(int), 1, in);
		token.push_back(vector<int>(count));
		fread(token.back().data(), sizeof(int), count, in);
	}

	fclose(in);

	reverse(token.begin(), token.end());

	assert(token.size() != 0);
	pattern::init(token.front().size());

	join(token);

	cout << res_num << endl;
	cout << cand_num << endl;

	return 0;
}

