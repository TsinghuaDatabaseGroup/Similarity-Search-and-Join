/*
 * storage.cc
 *
 *  Created on: 2014-12-11
 *      Author: wangjin
 */
#include "storage.h"

int N;
int minDictlen = 0x7FFF, maxDictlen = 0;
std::vector<string> dict;
std::vector<string> queries;

void readData(const char* path)
{
	  string str;
	  ifstream readData(path, std::ios::in);
	  while(getline(readData, str))
	  {
	      if( static_cast<int>(str.length()) < minDictlen)
	         minDictlen = str.length();
	      if( static_cast<int>(str.length()) > maxDictlen)
	         maxDictlen = str.length();
	      dict.push_back(str);
	  }
	  N = dict.size();
	  readData.close();
}
void readQuery(const char* path)
{
	string str;
	  ifstream readQuery(path, std::ios::in);
	  while(getline(readQuery, str))
	  {
	      queries.push_back(str);
	  }
	  readQuery.close();
}
void saveList(ofstream &pf, std::vector<int>& list)
{
	int num = list.size();
	pf.write((const char*)&num, sizeof(int));
	for(int i = 0; i < num; ++i)
	{
		pf.write((const char*)&(list.at(i)), sizeof(int));
	}
}
void  loadList(ifstream &pf, std::vector<int>& list)
{
	int num;
	pf.read((char*)&num, sizeof(int));
	if(num > 0)
	{
	for(int i = 0; i < num; ++i)
	{
		int tmp;
       pf.read((char*)&tmp, sizeof(int));
       list.push_back(tmp);
	}
	}
}
void saveaddrList(ofstream &pf, std::vector<ADDR>& list)
{
	int num = list.size();
	pf.write((const char*)&num, sizeof(int));
	for(int i = 0; i < num; ++i)
	{
		pf.write((const char*)&(list.at(i)), sizeof(ADDR));
	}
}
void  loadaddrList(ifstream &pf, std::vector<ADDR>& list)
{
	int num;
	pf.read((char*)&num, sizeof(int));
	if(num > 0)
	{
	for(int i = 0; i < num; ++i)
	{
		int tmp;
       pf.read((char*)&tmp, sizeof(ADDR));
       list.push_back(tmp);
	}
	}
}
