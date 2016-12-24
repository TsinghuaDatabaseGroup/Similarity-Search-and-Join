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
vector<vector<int>> tokens;
int widow_bound = -1;

int overlap(int x, int y)
{
	int result = 0;
	int posx = 0;
	int posy = 0;
	while (posx < (int)tokens[x].size() && posy < (int)tokens[y].size())
	{
		if (tokens[x][posx] == tokens[y][posy])
		{
			result++;
			posx++;
			posy++;
		} else if (tokens[x][posx] < tokens[y][posy])
		{
			posx++;
		} else {
			posy++;
		}
	}
	return result;
}

void get_grams(int range_bound)
{
	HashMap<int, int> freq_map;
	tokens.resize(records.size());
	for (int k = range_bound; k < (int)records.size(); k++)
	{
		for (int sp = 0; sp < (int)records[k].length() - Q + 1; sp++)
		{
			int token = DJB_hash(records[k].c_str() + sp, Q);
			tokens[k].push_back(token);
			freq_map[token]++;
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

	for (int k = range_bound; k < (int)records.size(); k++)
	{
		vector<int> &token = tokens[k];
		for (int t = 0; t < (int)token.size(); t++)
		{
			token[t] = freq_map[token[t]];
		}

		sort(token.begin(), token.end());
	}
}

void perform_join(int range_bound)
{
	struct list_item
	{
		int pos = 0;
		vector<int> ids;
	};

	HashMap<int, list_item> index;
	for (int k = range_bound; k < (int)records.size(); k++)
	{
		int count = 0, last_token = -1;
		HashSet<int> occurances;
		for (int token : tokens[k])
		{
			if (count++ >= THRESHOLD * Q + 1) break;
			if (token <= widow_bound || token == last_token) continue;
			last_token = token;

			auto &item = index[token];
			auto &list = item.ids;
			while (item.pos < (int)list.size() && records[list[item.pos]].length() + THRESHOLD < records[k].length()) item.pos++;
			for (int t = item.pos; t < (int)list.size(); t++)
			{
				int cand = list[t];
				if (cand != k)
				{
					occurances.insert(cand);
				}
			}
			list.push_back(k);
		}

		for (int cand: occurances)
		{
			cand_num++;
			int real_overlap = overlap(k, cand);
			int test_value = real_overlap + THRESHOLD * Q;
			if (test_value < (int)tokens[k].size() || test_value < (int)tokens[cand].size()) continue;
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

	get_grams(range_bound);

	perform_join(range_bound);

	cout << res_num << endl;
	cout << cand_num << endl;
	return 0;
}

