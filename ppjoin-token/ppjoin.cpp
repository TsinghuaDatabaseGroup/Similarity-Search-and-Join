#include "common/help.h"
#include "common/token.h"
#include "common/time.h"
#include "common/type.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <cstdlib>
#include <cassert>

using namespace std;

long long res_num;
long long cand_num;
double THRESHOLD;

vector<vector<int>> records;
base_helper *helper;

#ifdef PPJOIN
	const int PRUNE_FLAG = -7;
#endif
#ifdef PLUS
	const int MAX_DEPTH = 2;
#endif

int overlap(int x, int y, int require_overlap, int posx = 0, int posy = 0, int current_overlap = 0)
{
	while (posx < (int)records[x].size() && posy < (int)records[y].size())
	{
		if ((int)records[x].size() - posx + current_overlap < require_overlap
				|| (int)records[y].size() - posy + current_overlap < require_overlap) return -1;
		if (records[x][posx] == records[y][posy])
		{
			current_overlap++;
			posx++;
			posy++;
		} else if (records[x][posx] < records[y][posy])
		{
			posx++;
		} else {
			posy++;
		}
	}
	return current_overlap;
}

#ifdef PLUS
int suffix_filter(vector<int>& xArray, vector<int>& yArray, int xStart, int xEnd, int yStart, int yEnd, int HD, int depth)
{
	if (xEnd <= xStart || yEnd <= yStart) return abs((xEnd - xStart) - (yEnd - yStart));
	int left, right, mid, pos, token, offset;
	int HDLeft, HDRight, HDLeftBound, HDRightBound;
	int xLen = xEnd - xStart, yLen = yEnd - yStart;

	mid = xStart + xLen / 2, token = xArray[mid];

	if (xLen >= yLen) {
		offset = (HD - (xLen - yLen)) / 2 + (xLen - yLen), left = yStart + xLen / 2 - offset;
		offset = (HD - (xLen - yLen)) / 2, right = yStart + xLen / 2 + offset;
	}
	else {
		offset = (HD - (yLen - xLen)) / 2, left = yStart + xLen / 2 - offset;
		offset = (HD - (yLen - xLen)) / 2 + (yLen - xLen), right = yStart + xLen / 2 + offset;
	}

	if ((left >= yStart && yArray[left] > token) || (right < yEnd && yArray[right] < token)) return HD + 1;

	int search_left = left >= yStart ? left : yStart;
	int search_right = right + 1 < yEnd ? right + 1 : yEnd;
	pos = lower_bound(yArray.begin() + search_left, yArray.begin() + search_right, token) - yArray.begin();
	if (pos < yEnd && yArray[pos] == token) {
		HDLeft = HDLeftBound = abs((mid - xStart) - (pos - yStart));
		HDRight = HDRightBound = abs((xEnd - mid - 1) - (yEnd - pos - 1));
		if (HDLeftBound + HDRightBound > HD) return HDLeftBound + HDRightBound;
		if (depth < MAX_DEPTH) {
			HDLeft = suffix_filter(xArray, yArray, xStart, mid, yStart, pos, HD - HDRightBound, depth + 1);
			if (HDLeft + HDRightBound > HD) return HDLeft + HDRightBound;
			HDRight = suffix_filter(xArray, yArray, mid + 1, xEnd, pos + 1, yEnd, HD - HDLeft, depth + 1);
		}
		if (HDLeft + HDRight > HD) return HDLeft + HDRight;
		return HDLeft + HDRight;
	}
	else {
		HDLeft = HDLeftBound = abs((mid - xStart) - (pos - yStart));
		HDRight = HDRightBound = abs((xEnd - mid - 1) - (yEnd - pos));
		if (HDLeftBound + HDRightBound + 1 > HD) return HDLeftBound + HDRightBound + 1;
		if (depth < MAX_DEPTH) {
			HDLeft = suffix_filter(xArray, yArray, xStart, mid, yStart, pos, HD - HDRightBound - 1, depth + 1);
			if (HDLeft + HDRightBound + 1 > HD) return HDLeft + HDRightBound + 1;
			HDRight = suffix_filter(xArray, yArray, mid + 1, xEnd, pos, yEnd, HD - HDLeft - 1, depth + 1);
		}
		if (HDLeft + HDRight + 1 > HD) return HDLeft + HDRight + 1;
		return HDLeft + HDRight + 1;
	}

	return 0;
}
#endif

void perform_join()
{
	struct list_item
	{
		int pos = 0;
		vector<pair<int, int>> ids;
	};

	HashMap<int, list_item> index;
	for (int k = 0; k < (int)records.size(); k++)
	{
		const vector<int> &record = records[k];
		int min_length = helper->min_possible_length(record.size());
		int probe_length = helper->probe_length(record.size());
		int index_length = helper->index_length(record.size());

		int require_overlaps[record.size() + 1];
		for (int l = min_length; l <= (int)record.size(); l++)
		{
			require_overlaps[l] = helper->require_overlap(record.size(), l);
		}

		HashMap<int, int> occurances;
		for (int t = 0; t < probe_length; t++)
		{
			int token = record[t];
			auto &list = index[token].ids;
			int &pos = index[token].pos;

			while (pos < (int)list.size() && (int)records[list[pos].first].size() < min_length) pos++;

			for (int p = pos; p < (int)list.size(); p++)
			{
				int cand_id = list[p].first;
#ifdef PPJOIN
				int cand_pos = list[p].second;
				int cand_length = records[cand_id].size();
				auto iter = occurances.find(cand_id);


				if (iter == occurances.end())
				{
#ifdef PLUS
					int hamming_distance = cand_length + records[k].size() - 2 * require_overlaps[cand_length] - (cand_pos + t);
					if (suffix_filter(records[cand_id], records[k], cand_pos, records[cand_id].size(), t, records[k].size(), hamming_distance, 0) > hamming_distance) continue;
#endif
					if (((int)records[k].size() - t) < require_overlaps[cand_length]
							|| (cand_length - cand_pos) < require_overlaps[cand_length]) continue;
					occurances[cand_id] = 1;
				} else {
					int &value = occurances[cand_id];
#ifdef PLUS
					int hamming_distance = cand_length + records[k].size() - 2 * require_overlaps[cand_length] + 2 * value - (cand_pos + t);
					if (suffix_filter(records[cand_id], records[k], cand_pos, records[cand_id].size(), t, records[k].size(), hamming_distance, 0) > hamming_distance)
					{
						value = PRUNE_FLAG;
						continue;
					}
#endif
					if (value + ((int)records[k].size() - t) < require_overlaps[cand_length]
							|| value + (cand_length - cand_pos) < require_overlaps[cand_length])
					{
						value = PRUNE_FLAG;
					} else {
						value++;
					}
				}
#else
				occurances[cand_id]++;
#endif
			}

			if (t < index_length)
			{
				list.emplace_back(k, t);
			}
		}

		for (auto &kv : occurances)
		{
			cand_num++;
#ifdef PPJOIN
			if (kv.second == PRUNE_FLAG) continue;
#endif
			int cand = kv.first;
			int require_overlap = require_overlaps[records[cand].size()];
			int index_length = helper->index_length(records[cand].size());
			if (records[cand][index_length - 1] < records[k][probe_length - 1])
			{
				if (kv.second + (int)records[cand].size() - index_length < require_overlap) continue;
			} else {
				if (kv.second + (int)records[k].size() - probe_length < require_overlap) continue;
			}
			int real_overlap = overlap(k, cand, require_overlaps[records[cand].size()]);
			if (real_overlap != -1 && helper->calc_similarity(records[k].size(), records[cand].size(), real_overlap) >= THRESHOLD)
			{
				res_num++;
			}
		}
	}
}

void readfile(const char *filename, vector<vector<int>> &records)
{
	int count;
	FILE *in = fopen(filename, "r");
	while (fread(&count, sizeof(int), 1, in))
	{
		fread(&count, sizeof(int), 1, in);
		records.push_back(vector<int>(count));
		fread(records.back().data(), sizeof(int), count, in);
	}
}

int main(int argc, char **argv)
{
	if (argc != 4) return -1;
	log_start();

	readfile(argv[2], records);
	THRESHOLD = atof(argv[3]);
	if (argv[1][0] == 'j')
	{
		helper = new jaccard_helper();
	} else if (argv[1][0] == 'c') {
		helper = new cosine_helper();
	} else if (argv[1][0] == 'd') {
		helper = new dice_helper();
	} else {
		return -2;
	}

	perform_join();

	cout << res_num << endl;
	cout << cand_num << endl;
	return 0;
}

