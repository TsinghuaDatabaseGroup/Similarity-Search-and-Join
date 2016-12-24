#include "common/help.h"
#include "common/time.h"
#include "common/type.h"

#include <vector>
#include <algorithm>
#include <fstream>
#include <string>

using namespace std;

struct PIndex
{
	int stPos;    // start position of substring
	int Lo;       // start position of segment
	int partLen;  // substring/segment length
	int len;      // length of indexed string
	PIndex(int _s, int _o, int _p, int _l)
		: stPos(_s), Lo(_o), partLen(_p), len(_l) {}
};

int D, N, PN;
vector<string> dict;
int MaxDictLen = 0;
int MinDictLen = 0x7FFFFFFF;

vector<PIndex> **partIndex;
HashMap<int, vector<int>> **invLists;
int **partPos;
int **partLen;
int *dist;

long long candNum = 0;
long long realNum = 0;

template <typename T>
void alloc1d(T* &p, int n)
{
	p = new int[n];
}

void init()
{
	alloc2d(partLen, PN, MaxDictLen + 1);
	alloc2d(partPos, PN + 1, MaxDictLen + 1);
	alloc2d(partIndex, PN, MaxDictLen + 1);
	alloc2d(invLists, PN, MaxDictLen + 1);
	alloc1d(dist, MaxDictLen + 2);

	for (int lp = 0; lp <= MaxDictLen + 1; lp++)
	{
		dist[lp] = N;
	}

	for (int len = MinDictLen; len <= MaxDictLen; len++)
	{
		partPos[0][len] = 0;
		partLen[0][len] = len / PN;
		partPos[PN][len] = len;
	}

	for (int pid = 1; pid < PN; pid++)
	{
		for (int len = MinDictLen; len <= MaxDictLen; len++)
		{
			partPos[pid][len] = partPos[pid - 1][len] + partLen[pid - 1][len];
			if (pid == (PN - len % PN))
			{
				partLen[pid][len] = partLen[pid - 1][len] + 1;
			} else {
				partLen[pid][len] = partLen[pid - 1][len];
			}
		}
	}
}

void prepare()
{
	int clen = 0;
	for (int id = 0; id < N; id++)
	{
		if (clen == (int)dict[id].length()) continue;
		for (int lp = clen + 1; lp <= (int)dict[id].length(); lp++) dist[lp] = id;
		clen = dict[id].length();
	}

	clen = 0;
	for (int id = 0; id < N; id++)
	{
		if (clen == (int)dict[id].length()) continue;
		clen = dict[id].length();

		for (int pid = 0; pid < PN; pid++)
		{
			for (int len = max(clen - D, MinDictLen); len <= clen; len++)
			{
				if (dist[len] == dist[len + 1]) continue;

				for (int stPos = max3(0, partPos[pid][len] - pid,
							partPos[pid][len] + (clen - len) - (D - pid));
						stPos <= min3(clen - partLen[pid][len], partPos[pid][len] + pid,
							partPos[pid][len] + (clen - len) + (D - pid)); stPos++)
				{
					partIndex[pid][clen].emplace_back(stPos, partPos[pid][len], partLen[pid][len], len);
				}
			}
		}
	}
}

void perform_join()
{
	for (int id = 0; id < N; id++)
	{
		HashSet<int> checked_ids;
		int clen = dict[id].length();
		for (int partId = 0; partId < PN; partId++)
		{
			for (int lp = 0; lp < (int)partIndex[partId][clen].size(); lp++)
			{
				int stPos = partIndex[partId][clen][lp].stPos;
				int Lo = partIndex[partId][clen][lp].Lo;
				int pLen = partIndex[partId][clen][lp].partLen;
				int len = partIndex[partId][clen][lp].len;

				int hash_value = DJB_hash(dict[id].c_str() + stPos, pLen);
				if (invLists[partId][len].count(hash_value) == 0) continue;
				for (int cand : invLists[partId][len][hash_value])
				{
					if (checked_ids.find(cand) == checked_ids.end())
					{
						++candNum;
						if (partId == D) checked_ids.insert(cand);
						if (partId == 0 || edit_distance(dict[cand], dict[id], partId, 0, 0, Lo, stPos) <= partId)
						{
							if (partId == 0) checked_ids.insert(cand);
							if (partId == D || edit_distance(dict[cand], dict[id], D - partId, Lo + pLen, stPos + pLen) <= D - partId)
							if (edit_distance(dict[cand], dict[id], D) <= D)
							{
								checked_ids.insert(cand);
								realNum++;
							}
						}
					}
				}
			}
		}

		for (int partId = 0; partId < PN; partId++)
		{
			int pLen = partLen[partId][clen];
			int stPos = partPos[partId][clen];
			invLists[partId][clen][DJB_hash(dict[id].c_str() + stPos, pLen)].push_back(id);
		}
	}
}

int main(int argc, char **argv)
{
	if (argc != 3) return -1;
	log_start();

	// read in threshold D and generate part number PN
	D = atoi(argv[2]);
	PN = D + 1;

	string line;
	ifstream in(argv[1]);
	while (getline(in, line))
	{
		MaxDictLen = max(MaxDictLen, (int)line.length());
		MinDictLen = min(MinDictLen, (int)line.length());
		dict.push_back(move(line));
	}
	N = dict.size();

	sort(dict.begin(), dict.end(), [](const string &s1, const string &s2) {
		return s1.length() < s2.length();
	});

	init();

	prepare();

	perform_join();

	printf("%lld\n", realNum);
	printf("%lld\n", candNum);

	return 0;
}

