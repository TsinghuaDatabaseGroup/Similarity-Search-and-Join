#include <boost/functional/hash.hpp>
#include "common/time.h"
#include "common/type.h"

#include <algorithm>
#include <vector>
#include <string>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cmath>

using namespace std;

long long cand_num;
long long res_num;

const int JACCARD = 1;
const int COSINE = 2;
const int DICE = 3;

struct category
{
	static int N;
	static int METHOD;
	static double THRESHOLD;

	// [s_len, e_len]
	int s_len;
	int e_len;

	int N1, N2;
	int K, K2;

	int **range_start;
	int **range_end;

	int sig_len;
	vector<vector<int>> subs;
	HashMap<int, vector<int>> *sig_map;

	void init(int len)
	{
		s_len = len;
		if (METHOD == JACCARD)
		{
			e_len = s_len / THRESHOLD;
			K = 2 * (1 - THRESHOLD) / (1 + THRESHOLD) * e_len;
		} else if (METHOD == COSINE)
		{
			e_len = s_len / THRESHOLD / THRESHOLD;
			K = 2 * e_len * (1 - THRESHOLD);
		} else if (METHOD == DICE)
		{
			e_len = s_len * (2 - THRESHOLD) / THRESHOLD;
			K = 2 * e_len * (1 - THRESHOLD);
		}

		N1 = K + 1;
		N2 = 2;

		K2 = (K + 1) / N1 - 1;

		// important fix
		if ((K + 1) % N1 != 0) K2++;

		if (N1 > K + 1 || N1 * N2 <= K + 1)
		{
			exit(-3);
		}

		int n = N2;
		int k = N2 - K2;
		vector<int> sub(k);

		int s;
		for (s = 0; s < k; s++)
			sub[s] = s;
		subs.push_back(sub);

		while (sub[0] < n - k) {
			for (s = 0; s < k; s++)
				if (sub[k - s - 1] < n - s - 1)
					break;
			s = k - s - 1;
			sub[s]++;
			s++;
			for (; s < k; s++)
				sub[s] = sub[s - 1] + 1;
			subs.push_back(sub);
		}

		sig_len = N1 * subs.size();
		sig_map = new HashMap<int, vector<int>>[sig_len];

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
};

int category::N;
int category::METHOD;
double category::THRESHOLD;

const int MAX_LEN = 3300;
const int MAX_CATEGORY = 100;
category helper[MAX_CATEGORY];

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
	double factor = category::THRESHOLD / (1 + category::THRESHOLD);
	int require_overlap = ceil(factor * (a.size() + b.size()) - 1e-6);
	int real_overlap = check_overlap(a, b, require_overlap);

	if (real_overlap == -1) return false;
	return (real_overlap / (double)(a.size() + b.size() - real_overlap)) >= threshold;
}

bool cosine(vector<int> &a, vector<int> &b, double threshold)
{
	int require_overlap = ceil(category::THRESHOLD * sqrt(a.size() * b.size()) - 1e-6);
	int real_overlap = check_overlap(a, b, require_overlap);

	if (real_overlap == -1) return false;
	return (real_overlap / sqrt(a.size() * b.size())) >= threshold;
}

bool dice(vector<int> &a, vector<int> &b, double threshold)
{
	int require_overlap = ceil(category::THRESHOLD / 2.0 * (a.size() + b.size()) - 1e-6);
	int real_overlap = check_overlap(a, b, require_overlap);

	if (real_overlap == -1) return false;
	return (2 * real_overlap / (double)(a.size() + b.size())) >= threshold;
}

bool (*verify)(vector<int> &a, vector<int> &b, double threshold);

void perform_join(int k, vector<vector<int>> &token, int id, bool *checked_flag)
{
	static boost::hash<vector<int>> vector_hash;

	int index = -1;
	vector<int> prepare;
	for (int i = 0; i < helper[k].N1; i++)
		for (vector<vector<int>>::iterator sub = helper[k].subs.begin(); sub != helper[k].subs.end(); sub++)
		{
			index++;
			prepare.clear();
			for (vector<int>::iterator j = sub->begin(); j != sub->end(); j++)
			{
				vector<int>::iterator s_pos = lower_bound(token[id].begin(), token[id].end(), helper[k].range_start[i][*j]);
				vector<int>::iterator e_pos = lower_bound(token[id].begin(), token[id].end(), helper[k].range_end[i][*j]);
				while (s_pos != e_pos)
				{
					prepare.push_back(*s_pos);
					s_pos++;
				}
			}

			int hash_value = vector_hash(prepare);
			vector<int> &candidate = helper[k].sig_map[index][hash_value];
			for (vector<int>::iterator cand_id = candidate.begin(); cand_id != candidate.end(); cand_id++)
			{
				if (!checked_flag[*cand_id])
				{
					cand_num++;
					checked_flag[*cand_id] = 1;
					if (verify(token[*cand_id], token[id], category::THRESHOLD))
					{
						res_num++;
					}
				}
			}

			if (candidate.size() == 0 ||
					candidate.back() != id)
			{
				candidate.push_back(id);
			}
		}
}

void convert_to_signature(vector<vector<int>> &token)
{
	bool *checked_flag = new bool[token.size()];
	for (int id = 0; id < (int)token.size(); id++)
	{
		int k;
		for (k = 0; k < MAX_CATEGORY; k++)
			if (helper[k].s_len <= (int)token[id].size()
					&& helper[k].e_len >= (int)token[id].size())
				break;

		assert(k < MAX_CATEGORY);
		memset(checked_flag, 0, id);

		perform_join(k, token, id, checked_flag);
		perform_join(k + 1, token, id, checked_flag);
	}
	delete[] checked_flag;
}

int main(int argc, char **argv)
{
	if (argc != 4) return -1;
	log_start();

	if (argv[1][0] == 'j')
	{
		category::METHOD = JACCARD;
		verify = jac;
	} else if (argv[1][0] == 'c') {
		category::METHOD = COSINE;
		verify = cosine;
	} else if (argv[1][0] == 'd') {
		category::METHOD = DICE;
		verify = dice;
	}

	FILE *in = fopen(argv[2], "r");
	category::THRESHOLD = atof(argv[3]);

	int count;
	vector<vector<int>> token;
	while (fread(&count, sizeof(int), 1, in))
	{
		fread(&count, sizeof(int), 1, in);
		token.push_back(vector<int>(count));
		fread(token.back().data(), sizeof(int), count, in);
		if (token.back().back() + 1 > category::N)
		{
			category::N = token.back().back() + 1;
		}
	}

	int len = 1;
	for (int k = 0; k < MAX_CATEGORY; k++)
	{
		helper[k].init(len);
		len = helper[k].e_len + 1;
		if (len > MAX_LEN) break;
	}

	convert_to_signature(token);
	printf("%lld\n", res_num);
	printf("%lld\n", cand_num);

	return 0;
}

