#include "common/time.h"
#include "common/type.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <string>

#include <cstdlib>

using namespace std;

int id;
int THRESHOLD;
long long res_num;
long long cand_num;

HashSet<int> checked_flag;
HashMap<unsigned long long, vector<pair<int, vector<int>>>> index_map;

int check_ed(const vector<int> &p1, const vector<int> &p2)
{
	int i = 0, j = 0, updates = 0;
	while (i < (int)p1.size() && j < (int)p2.size())
	{
		if (p1[i] == p2[j])
		{
			updates++;
			j++;
			i++;
		} else {
			(p1[i] < p2[j] ? i++ : j++);
		}
	}
	return p1.size() + p2.size() - updates;
}

void insert_index(const string &result, const vector<int> &del_pos)
{
	static auto hash_function = hash<string>();
	auto &list = index_map[hash_function(result)];

	for (auto &p : list)
	{
		if (id == p.first) continue;
		if (checked_flag.find(p.first) != checked_flag.end()) continue;

		cand_num++;
		int ed = check_ed(p.second, del_pos);
		if (ed <= THRESHOLD)
		{
			checked_flag.insert(p.first);
			res_num++;
		}
	}

	list.push_back(make_pair(id, del_pos));
}

void perform_deletion(string &&s, int k)
{
	static vector<int> del_pos;

	if (k == 0) {
		insert_index(s, del_pos);
	} else {
		for (int pos = (del_pos.empty() ? 0 : del_pos.back()); pos < (int)s.length(); pos++)
		{
			del_pos.push_back(pos);
			perform_deletion(s.substr(0, pos) + s.substr(pos + 1), k - 1);
			del_pos.pop_back();
		}
	}
}

int main(int argc, char **argv)
{
	if (argc != 3) return -1;

	log_start();
	ifstream in(argv[1]);
	THRESHOLD = atoi(argv[2]);

	string line;
	while (getline(in, line))
	{
		checked_flag.clear();
		insert_index(line, vector<int>());
		for (int k = 1; k <= THRESHOLD; k++)
			perform_deletion(move(line), k);
		id++;
	}
	cout << res_num << endl;
	cout << cand_num << endl;
	return 0;
}

