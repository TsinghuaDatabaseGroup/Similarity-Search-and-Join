#include "common/help.h"
#include "common/time.h"
#include "common/type.h"

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
vector<vector<pair<int, int>>> qgrams;
vector<vector<pair<int, int>>> qchunks;
int widow_bound = -1;

int overlap(int x, int y)
{
	// int result = 0;
	// for (auto &p : qchunks[x])
	// {
	// 	auto range_pair = equal_range(qgrams[y].begin(), qgrams[y].end(), p, [](const pair<int, int> &p1, const pair<int, int> &p2) {
	// 		return p1.first < p2.first;
	// 	});
	// 	while (range_pair.first != range_pair.second)
	// 	{
	// 		if (abs(p.second - range_pair.first->second) <= THRESHOLD)
	// 		{
	// 			result++;
	// 			break;
	// 		}
	// 		range_pair.first++;
	// 	}
	// }
	int result = 0;
	int posx = 0;
	int posy = 0;
	while (posx < (int)qchunks[x].size() && posy < (int)qgrams[y].size())
	{
		if (qchunks[x][posx].first == qgrams[y][posy].first)
		{
			result++;
			posx++;
			posy++;
		} else if (qchunks[x][posx].first < qgrams[y][posy].first)
		{
			posx++;
		} else {
			posy++;
		}
	}
	return result;
}

void get_chars()
{
	HashMap<int, int> freq_map;
	qgrams.resize(records.size());
	qchunks.resize(records.size());
	for (int k = 0; k < (int)records.size(); k++)
	{
		for (int sp = 0; sp < (int)records[k].length() - Q + 1; sp++)
		{
			int qgram = DJB_hash(records[k].c_str() + sp, Q);
			qgrams[k].emplace_back(qgram, sp);
			freq_map[qgram]++;
		}

		for (int sp = (int)records[k].length() - Q + 1; sp < (int)records[k].length(); sp++)
		{
			int qgram = DJB_hash(records[k].c_str() + sp, (int)records[k].length() - sp, Q - (int)records[k].length() + sp);
			qgrams[k].emplace_back(qgram, sp);
			freq_map[qgram]++;
		}

		for (int sp = 0; sp < (int)records[k].length(); sp += Q)
		{
			qchunks[k].emplace_back(qgrams[k][sp].first, sp);
		}
	}

	struct Package
	{
		int token, freq;
		Package(int token, int freq)
			: token(token), freq(freq) {}
	};

	vector<Package> packages;
	for (auto &kv : freq_map)
	{
		packages.emplace_back(kv.first, kv.second);
	}
	sort(packages.begin(), packages.end(), [](const Package &p1, const Package &p2){
		return p1.freq < p2.freq;
	});

	for (int k = 0; k < (int)packages.size(); k++)
	{
		if (packages[k].freq == 1) widow_bound = k;
		freq_map[packages[k].token] = k;
	}

	auto sort_tokens = [&freq_map](vector<pair<int, int>> &token) {
		for (int t = 0; t < (int)token.size(); t++)
		{
			token[t].first = freq_map.at(token[t].first);
		}

		sort(token.begin(), token.end(), [](const pair<int, int> &t1, const pair<int, int> &t2) {
			if (t1.first != t2.first) return t1.first < t2.first; else return t1.second < t2.second;
		});
	};

	for (int k = 0; k < (int)records.size(); k++)
	{
		sort_tokens(qgrams[k]);
		sort_tokens(qchunks[k]);
	}
}

void perform_join(int range_bound)
{
	struct list_item
	{
		int pos = 0;
		vector<pair<int, int>> ids;
	};

	HashMap<int, list_item> index;
	for (int k = range_bound; k < (int)records.size(); k++)
	{
		int qgram_prefix_length = records[k].length() - (records[k].length() - THRESHOLD - 1) / Q + 1;
		int qchunk_prefix_length = THRESHOLD + 1;

		assert(qgram_prefix_length <= (int)qgrams[k].size());
		assert(qchunk_prefix_length <= (int)qchunks[k].size());

		HashSet<int> occurances;
		for (int count = 0; count < qchunk_prefix_length; count++)
		{
			const pair<int, int> &qchunk = qchunks[k][count];
			if (qchunk.first <= widow_bound) continue;

			auto iter = index.find(qchunk.first);
			if (iter == index.end()) continue;
			auto &item = iter->second;
			auto &list = item.ids;
			while (item.pos < (int)list.size() && records[list[item.pos].first].length() + THRESHOLD < records[k].length()) item.pos++;
			for (int t = item.pos; t < (int)list.size(); t++)
			{
				int cand = list[t].first;
				if (cand != k && abs(list[t].second - qchunk.second) <= THRESHOLD)
				{
					occurances.insert(cand);
				}
			}
		}

		for (int count = 0; count < qgram_prefix_length; count++)
		{
			const pair<int, int> &qgram = qgrams[k][count];
			if (qgram.first <= widow_bound) continue;
			index[qgram.first].ids.emplace_back(k, qgram.second);
		}

		for (int cand : occurances)
		{
			cand_num++;
			int real_overlap = overlap(k, cand);
			if (real_overlap < ((int)records[k].length() + Q - 1) / Q - THRESHOLD) continue;
			if (edit_distance(records[k], records[cand], THRESHOLD) <= THRESHOLD)
			{
				res_num++;
			}
		}

		if ((int)records[k].length() - THRESHOLD >= (THRESHOLD + 1) * Q) continue;
		int bound = lower_bound(records.begin(), records.end(), (int)records[k].length() - THRESHOLD, [](const string &s, int value) {
			return (int)s.length() < value;
		}) - records.begin();
		while (bound != range_bound)
		{
			cand_num++;
			if (edit_distance(records[k], records[bound], THRESHOLD) <= THRESHOLD)
			{
				res_num++;
			}
			bound++;
		}
	}
}

int main(int argc, char **argv)
{
	if (argc != 4) return -1;

	log_start();
	ifstream in(argv[1]);
	THRESHOLD = atoi(argv[2]);
	Q = atoi(argv[3]);

	string line;
	while (getline(in, line))
	{
		records.push_back(move(line));
	}
	sort(records.begin(), records.end(), [](const string &s1, const string &s2) {
		return s1.length() < s2.length();
	});

	int length_bound = (THRESHOLD + 1) * Q;

	// for two strings the length of which are < length_bound;
	int range_bound = lower_bound(records.begin(), records.end(), length_bound, [](const string &s, int value) {
		return (int)s.length() < value;
	}) - records.begin();

	for (int x = 0; x < range_bound; x++)
		for (int y = x + 1; y < range_bound; y++)
		{
			cand_num++;
			if (edit_distance(records[x], records[y], THRESHOLD) <= THRESHOLD)
			{
				res_num++;
			}
		}

	get_chars();

	perform_join(range_bound);

	cout << res_num << endl;
	cout << cand_num << endl;
	return 0;
}

