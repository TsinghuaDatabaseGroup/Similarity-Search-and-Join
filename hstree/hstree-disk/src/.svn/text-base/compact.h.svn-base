/*
 * compact.h
 *
 *  Created on: 2014-12-29
 *      Author: wangjin
 */
#ifndef COMPACT_H_
#define COMPACT_H_
#include "util.h"
typedef std::tr1::unordered_map<string, vector<int> > IvList;
struct inv_entry
{
	ADDR key;
	bool pos;
#if defined (STORE_HASH)
	inv_entry():key(0),pos(0){}
#else
	inv_entry():key(""),pos(0){}
#endif
	inv_entry(ADDR _key,bool _pos):key(_key),pos(_pos){}
};
struct compactGroup{
	int len;
	int st;
	int en;
	vector< vector<InvertedMap> > maps;
	compactGroup(){st=0;en=0;len = 0;}
	compactGroup(int _len, int _st, int _en)
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

	void getIvList(uint64_t key, int lv, int pid, vector<ADDR>& address,ifstream &pf);
	void combine(int lv, int pid, vector<ADDR>& address, vector<int>& result,ifstream &pf);
	void buildIndex(ofstream &pf);

};




#endif /* COMPACT_H_ */
