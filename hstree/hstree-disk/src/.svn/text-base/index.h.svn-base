/*
 * index.h
 *
 *  Created on: 2014-12-11
 *      Author: wangjin
 */

#ifndef INDEX_H_
#define INDEX_H_

#include "util.h"
#include "storage.h"

struct Group{
int len;
int st;
int en;
vector< vector<InvertedMap> > maps;
Group(){st =0; en = 0; len = 0;}
Group(int _len,int _st, int _en)
{
	len = _len;
	st = _st;
    en = _en;
    int maxlv = getMaxLevel();
    int maxpn = getMaxPN();
    maps.resize( maxlv);
    	for(int lv = 0; lv < maxlv; ++lv)
    {
    	maps[lv].resize(maxpn);
    }
}

int getMaxLevel();
int getMaxPN();
void buildIndex(ofstream &pf);
#if defined (STORE_HASH)
ADDR findOffset(uint64_t hv, int lv, int pid);
#else
ADDR findOffset(string str, int lv, int pid);
#endif
void printGroup();//for test
};
typedef std::tr1::unordered_map<int, Group> Groupset;


int calLevel(int l);
void sortData();
void initial();
void createIndex(const string& path);
void prepare();
void printIndex();
#endif /* INDEX_H_ */
