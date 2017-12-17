/*
 * util.h
 *
 *  Created on: 2014-12-11
 *      Author: wangjin
 */

#ifndef UTIL_H_
#define UTIL_H_
#define __STDC_FORMAT_MACROS
#include <stdint.h>
#include <tr1/unordered_map>
#include <vector>
#include <string>
#include <set>
#include <iostream>
#include <fstream>
#include <queue>
#include <algorithm>
#define STORE_HASH
#define PGSZ 8192
using namespace std;

typedef long  ADDR;
typedef std::tr1::unordered_map<char, int> ContentMap;
#if defined (STORE_HASH)
typedef std::tr1::unordered_map<uint64_t, ADDR> InvertedMap ;
typedef std::tr1::unordered_map<uint64_t, vector<int> > InvertedList;
typedef std::tr1::unordered_map <uint64_t, vector<ADDR> > AddressList ;
#else
typedef std::tr1::unordered_map<string, ADDR> InvertedMap;
typedef std::tr1::unordered_map<string, vector<int> > InvertedList;
typedef std::tr1::unordered_map <string, vector<ADDR> > AddressList ;
#endif

struct ssIndex{
int stPos;//the start point of substring in query
int sslen;//the length of substring in query
int pid;//corresponding pid
int len;//length of corresponding string
ssIndex(int _st,int _sl,int _p, int _l): stPos(_st),sslen(_sl),pid(_p),len(_l){}
};
struct match_entry{
	int pid;
	int matchpos;
	match_entry(int _pid, int _matchpos): pid(_pid),matchpos(_matchpos){}
	match_entry():pid(0),matchpos(0){}
	bool operator< (const match_entry& other) const
	{
			return (pid < other.pid);
	}
};
struct minq_entry{
	int id;
	int dist;
	minq_entry(int _id, int _dist): id(_id),dist(_dist){}
	minq_entry():id(0),dist(0){}
	bool operator< (const minq_entry& other) const
	{
		return (dist > other.dist);
	}
};
struct q_entry{
	int id;
	int dist;
	q_entry(int _id, int _dist): id(_id),dist(_dist){}
	q_entry():id(0),dist(0){}
	bool operator< (const q_entry& other) const
	{
		return (dist < other.dist);
	}
};

extern vector<string> dict;
extern vector<string> queries;
extern int minDictlen;
extern int maxDictlen;
extern int N ;
extern int maxLevel;
extern int maxPN;
extern int ***partLen;
extern int ***partPos;
extern int **matrix;
extern bool *visited;
extern bool **curmatchpos;
extern vector<ssIndex> **substrings;
extern priority_queue<q_entry> results;
extern set<match_entry> **matchinfo;
extern ContentMap *cfilter;
//extern long datavisited;
extern int simres;

inline bool strLessT(const string &s1, const string &s2) {
  if (s1.length() < s2.length()) return true;
  else if (s1.length() > s2.length()) return false;
  return s1 < s2;
}

inline int min(int a, int b, int c) {
  return (a <= b && a <= c) ? a : (b <= c ? b : c);
}

inline int max(int a, int b, int c) {
  return (a >= b && a >= c) ? a : (b >= c ? b : c);
}
inline uint64_t strHash(const string &str, int stPos, int len) {
  uint64_t __h = 0; int i = 0;
  while (i < len) __h = __h * 131 + str[stPos + i++];
  return __h;
}
inline int caled(const std::string &s1, const std::string &s2, int THRESHOLD)
{
	int xlen = s1.length();
	int  ylen = s2.length() ;
	if (xlen > ylen + THRESHOLD || ylen > xlen + THRESHOLD) return THRESHOLD + 1;
	if (xlen == 0) return ylen;

	int lam[xlen + 1][2 * THRESHOLD + 1];
	for (int k = 0; k <= THRESHOLD; k++) lam[0][THRESHOLD + k] = k;

	int right = (THRESHOLD + (ylen - xlen)) / 2;
	int left = (THRESHOLD - (ylen - xlen)) / 2;
	for (int i = 1; i <= xlen; i++)
	{
		bool valid = false;
		if (i <= left)
		{
			lam[i][THRESHOLD - i] = i;
			valid = true;
		}
		for (int j = (i - left >= 1 ? i - left : 1);
				j <= (i + right <= ylen ? i + right : ylen); j++)
		{
			if (s1[i - 1] == s2[j - 1])
				lam[i][j - i + THRESHOLD] = lam[i - 1][j - i + THRESHOLD];
			else
				lam[i][j - i + THRESHOLD] = min(lam[i - 1][j - i + THRESHOLD],
						j - 1 >= i - left ? lam[i][j - i + THRESHOLD - 1] : THRESHOLD,
						j + 1 <= i + right ? lam[i - 1][j - i + THRESHOLD + 1] : THRESHOLD) + 1;
			if (abs(xlen - ylen - i + j) + lam[i][j - i + THRESHOLD] <= THRESHOLD) valid = true;
		}
		if (!valid) return THRESHOLD + 1;
	}
	return lam[xlen][ylen - xlen + THRESHOLD];
}
#endif /* UTIL_H_ */
