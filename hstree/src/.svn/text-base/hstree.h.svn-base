#ifndef HSTREE_H_
#define HSTREE_H_
#define __STDC_FORMAT_MACROS
#include <tr1/unordered_map>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <queue>

#define STORE_HASH
using namespace std;

#if defined(STORE_HASH)
typedef std::tr1::unordered_map<uint64_t, vector<int> > InvLists;
#else
typedef std::tr1::unordered_map<string, vector<int> > InvLists;
#endif
typedef pair<int,int> Interval;
typedef std::tr1::unordered_map<int, Interval> Group;
typedef std::tr1::unordered_map<char, int> ContentMap;

inline uint64_t strHash(const string &str, int stPos, int len) {
  uint64_t __h = 0; int i = 0;
  while (i < len) __h = __h * 131 + str[stPos + i++];
  return __h;
}
inline int min(int a, int b, int c) {
  return (a <= b && a <= c) ? a : (b <= c ? b : c);
}

inline int max(int a, int b, int c) {
  return (a >= b && a >= c) ? a : (b >= c ? b : c);
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
struct ssIndex{
int stPos;
int sslen;
int pid;
int len;
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
extern int avglen;
extern int maxLevel;
extern int maxPN;
extern int **matrix;
extern int ***partLen;
extern int ***partPos;
extern InvLists ***ivLists;
extern bool *visited;
extern bool **curmatchpos;
extern vector<ssIndex> **substrings;
extern priority_queue<q_entry> results;
extern set<match_entry> **matchinfo;
extern Group grps;
extern ContentMap *cfilter;
extern int cands;
extern int verifytime;
extern vector<int>* buckets;
extern int simres;

/* io.cc */
void readData(char*);
void readQuery(char*);

/*prepare.cc*/
int calLevel(int l);
void initial();
void sortData();
void createIndex();
void printIndex();
/*free.cc*/
void deleteAll();

/*hstopk.cc*/
void hstopk(const string& q, int k);
void pretopk(const string& str, int k);
int edverify(const string& s,const string& t);
void genSubstring(int qlen, int pid, int len, int lv);

/*hsimSearch.cc*/
void hsearch(const string& query, int tau);
void presim(const string& str, int tau);





#endif /* HSTREE_H_ */
