#include <boost/functional/hash.hpp>
#include "common/help.h"
#include "common/time.h"
#include "common/type.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

#include <cstdlib>
#include <cstring>
#include <cassert>

using namespace std;

long long cand_num;
long long res_num;

const int Q = 1;

int THRESHOLD;
int N1 = 2;
int N2 = 6;
string CHARSET = " `~!@#$%^&*()-_+=[]{};:<>,./?\\|'\"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

int char_table[256];
int power[Q + 1];
int N, K, K2;

int **range_start;
int **range_end;

vector<vector<int>> subs;
int sig_len;

void prepare_power()
{
	power[0] = 1;
	for (int k = 1; k <= Q; k++)
		power[k] = power[k - 1] * CHARSET.length();

	N = power[Q];
	K = 2 * THRESHOLD * Q;
	K2 = (K + 1) / N1 - 1;

	// important fix
	if ((K + 1) % N1 != 0) K2++;

	if (N1 > K + 1 || N1 * N2 <= K + 1)
	{
		exit(-3);
	}
}

void prepare_char_table()
{
	random_shuffle(CHARSET.begin(), CHARSET.end());
	for (int k = 0; k < 256; k++)
	{
		string::size_type pos = CHARSET.find((char)k);
		if (pos == string::npos)
		{
			char_table[k] = -1;
		} else {
			char_table[k] = pos;
		}
	}
}

void prepare_range()
{
	int *t = new int[N1 * N2];
	range_start = new int*[N1];
	for (int k = 0; k < N1; k++)
		range_start[k] = t + k * N2;

	t = new int[N1 * N2];
	range_end = new int*[N1];
	for (int k = 0; k < N1; k++)
		range_end[k] = t + k * N2;

	for (int i = 0; i < N1; i++)
		for (int j = 0; j < N2; j++)
		{
			range_start[i][j] = N * (N2 * i + j) / N1 / N2;
			range_end[i][j] = N * (N2 * i + j + 1) / N1 / N2;
		}
}

void prepare_subsets(int n, int k)
{
	vector<int> sub(k);

	int i;
	for (i = 0; i < k; i++)
		sub[i] = i;
	subs.push_back(sub);

	while (sub[0] < n - k) {
		for (i = 0; i < k; i++)
			if (sub[k - i - 1] < n - i - 1)
				break;
		i = k - i - 1;
		sub[i]++;
		i++;
		for (; i < k; i++)
			sub[i] = sub[i - 1] + 1;
		subs.push_back(sub);
	}

	sig_len = N1 * subs.size();
}

void convert_to_token(vector<string> &input, vector<vector<int>> &token)
{
	for (vector<string>::iterator iter = input.begin();
			iter != input.end(); iter++)
	{
		string &line = *iter;
		assert((int)line.length() >= Q);

		Set<int> unique_token;

		// change token algorithm here to switch ed/jaccard
		for (int start = 0; start < (int)line.length() - Q + 1; start++)
		{
			int num = 0;
			for (int p = 0; p < Q; p++)
			{
				int pos = char_table[(unsigned char)(line[start + p])];
				num += power[p] * pos;
				assert(pos != -1);
			}
			unique_token.insert(num);
		}

		token.push_back(vector<int>());
		token.back().insert(token.back().end(), unique_token.begin(), unique_token.end());
	}
}

void convert_to_signature(vector<string> &input, vector<vector<int>> &token, HashMap<int, vector<int>> *sig_map)
{
	boost::hash<vector<int>> vector_hash;
	bool *checked_flag = new bool[input.size()];
	for (int id = 0; id < (int)token.size(); id++)
	{
		int index = -1;
		vector<int> prepare;
		memset(checked_flag, 0, id);

		for (int i = 0; i < N1; i++)
			for (vector<vector<int>>::iterator sub = subs.begin(); sub != subs.end(); sub++)
			{
				index++;
				prepare.clear();
				for (vector<int>::iterator j = sub->begin(); j != sub->end(); j++)
				{
					vector<int>::iterator s_pos = lower_bound(token[id].begin(), token[id].end(), range_start[i][*j]);
					vector<int>::iterator e_pos = lower_bound(token[id].begin(), token[id].end(), range_end[i][*j]);
					while (s_pos != e_pos)
					{
						prepare.push_back(*s_pos);
						s_pos++;
					}
				}

				int hash_value = vector_hash(prepare);
				vector<int> &candidate = sig_map[index][hash_value];
				for (vector<int>::iterator cand_id = candidate.begin(); cand_id != candidate.end(); cand_id++)
				{
					if (!checked_flag[*cand_id])
					{
						cand_num++;
						checked_flag[*cand_id] = 1;
						if (edit_distance(input[*cand_id], input[id], THRESHOLD) <= THRESHOLD)
						{
							res_num++;
						}
					}
				}
				candidate.push_back(id);
			}
	}
	delete[] checked_flag;
}

int main(int argc, char **argv)
{
	if (argc != 5) return -1;
	log_start();

	ifstream in(argv[1]);
	THRESHOLD = atoi(argv[2]);
	N1 = atoi(argv[3]);
	N2 = atoi(argv[4]);

	assert(Q <= 4);

	string line;
	vector<string> input;
	while (getline(in, line))
	{
		input.push_back(line);
	}
	in.close();

	prepare_power();
	prepare_char_table();
	prepare_subsets(N2, N2 - K2);
	prepare_range();

	vector<vector<int>> token;
	convert_to_token(input, token);

	HashMap<int, vector<int>> sig_map[sig_len];
	convert_to_signature(input, token, sig_map);

	cout << res_num << endl;
	cout << cand_num << endl;

	return 0;
}

