/*
 * index.cc
 *
 *  Created on: 2014-12-11
 *      Author: wangjin
 */
#include "index.h"

int maxLevel;
int maxPN;
bool *visited;
bool **curmatchpos;
set<match_entry> **matchinfo;
int **matrix;
int ***partLen;
int ***partPos;
vector<ssIndex> **substrings;
ContentMap *cfilter;
Groupset grps;

int calLevel(int length)
{
    int maxlv = 0;
     while(true)
      {
          int pn = 1 << (maxlv+1);
          if(length / pn <= 1)
            break;
          maxlv++;
      }
    return (maxlv+1);
}

int Group::getMaxLevel()
{
   return calLevel(len);
}
int Group::getMaxPN()
{
	int maxlv = calLevel(len);
	return (1<<(maxlv+1));

}
void Group::buildIndex(ofstream &pf)
{
	int maxlv = getMaxLevel();
	int maxpn;
	      //generate invertedlists
         vector<vector<InvertedList> > tmplists;
         tmplists.resize(maxlv);
         for(int i = 0; i < maxlv; ++i)
        	 tmplists[i].resize(getMaxPN());
         for (int id = st; id <= en; ++id)
              {
                  for(int lv = 0; lv < maxlv; ++lv)
                  {
                	  maxpn = 1 << (lv + 1);
                     for(int pid = 0; pid < maxpn; ++pid)
                     {
							#if defined (STORE_HASH)
                    	 	 tmplists[lv][pid][strHash(dict[id], partPos[len][lv][pid], partLen[len][lv][pid])].push_back(id);
							#else
                    	 	tmplists[lv][pid][dict[id].substr(partPos[len][lv][pid], partLen[len][lv][pid])].push_back(id);
							#endif
                     }
                   }
              }
         //write inverted lists into disk
         for(int lv = 0; lv < maxlv; ++lv)
         {
        	 maxpn = 1 << (lv + 1);
        	      for(int pid = 0; pid < maxpn; ++pid)
        	      {
        	    	  InvertedList::iterator inv_itr;
    	    		  ADDR offset = pf.tellp();
        	    	  for(inv_itr = tmplists[lv][pid].begin(); inv_itr != tmplists[lv][pid].end(); ++inv_itr)
        	    	  {
						# if defined (STORE_HASH)
        	    		  int64_t value = inv_itr->first;
						#else
        	    		  string value = inv_itr->first;
						#endif
        	    		  maps[lv][pid].insert(make_pair(value,offset));
        	    		  vector<int> list;
        	    		  vector<int>::iterator itr;
        	    		  for(itr = inv_itr->second.begin(); itr != inv_itr->second.end(); ++itr )
        	    		  {
        	    			  list.push_back(*itr);
        	    		  }
        	    		  saveList(pf, list);
        	    		  offset = pf.tellp();
        	    	  }
        	      }
        }
}
void Group::printGroup()
{
//print basic info
	printf("###############Group Information################\n");
	printf("Group: %d, start: %d, end: %d\n", len, st,en);
//print partlen and part pos
	int maxlv = getMaxLevel()-1;
	    cout<<"*************Print maps************"<<endl;
	    for(int lv = 0; lv < maxlv; ++lv)
	  	{
	  	               int pn = 1 << (lv + 1);
		  	             printf("level: %d\n", lv);
	  	               for(int pid = 0; pid < pn; ++pid)
	  	               {
	  	            	   InvertedMap::iterator mitr;
	  	            	   printf("pid: %d\n", pid);
	  	            	   for(mitr = maps[lv][pid].begin(); mitr != maps[lv][pid].end(); ++mitr)
	  	            		   cout<<"list code: "<<mitr->first << " offset: "<<mitr->second<<endl;
	  	               }
	  	}
}
#if defined (STORE_HASH)
ADDR Group::findOffset(uint64_t hv, int lv, int pid)
{
	ADDR offset = maps[lv][pid].find(hv)->second;
	return offset;
}
#else
ADDR Group::findOffset(string str, int lv, int pid)
{
	ADDR offset = maps[lv][pid].find(str)->second;
	return offset;
}
#endif
void sortData()
{
    sort(dict.begin(), dict.end(), strLessT);
}

void initial()
{
	  maxLevel = calLevel(maxDictlen);
	  maxPN = 1<<(maxLevel+1);
	  visited = new bool[N];
	  cfilter = new ContentMap[N];
	  matrix = new int *[maxDictlen + 1];
	  substrings = new vector<ssIndex> *[maxLevel];
	  partLen = new int **[maxDictlen+1];
	  partPos = new int **[maxDictlen+1];

	  for(int lp = 0; lp <= maxDictlen; ++lp)
	    {
	        partLen[lp] = new int *[maxLevel];
	        partPos[lp] = new int *[maxLevel];
	        matrix[lp] = new int[maxDictlen + 1];
	    }
	    for(int len = 0; len <= maxDictlen; ++len)
	      for(int lv = 0; lv < maxLevel; ++lv)
	    {
	       partLen[len][lv] = new int [maxPN];
	       partPos[len][lv] = new int [maxPN];
	    }
       for(int lv = 0; lv < maxLevel; ++lv)
    	   substrings[lv] = new vector<ssIndex> [maxPN];
	  for(int i = 0; i <= maxDictlen; ++i)
	  {
	    matrix[i][0] = i;
	    matrix[0][i] = i;
	  }
	  cout<<"initial finished!"<<endl;
}

void createIndex(const string& path )
{
	   int start = 0, end = 0;
	   int tmplen = minDictlen;
	   int pn;
	   string idxpath = path+".idx";
	   string mempath = path+".idm";
	   ofstream writel;
	   writel.open(idxpath.c_str(), ios::out);
	   // deal with the first string in dict
	   for(int i = 0; i < static_cast<int>(dict[0].length()); ++i)
	   		 if(cfilter[0].find(dict[0].at(i)) != cfilter[0].end())
	   		     cfilter[0][dict[0].at(i)]++;
	   		  else
	   		  	 cfilter[0].insert(make_pair(dict[0].at(i), 1));
	   //calculate partlen and partpos
	   for (int len = minDictlen; len <= maxDictlen; ++len)
	      {
	         Groupset::iterator gitr;
	         int maxlv = calLevel(len);
	         //calculate partlen
	         partLen[len][0][0] = len >> 1;
	         partLen[len][0][1] = (len % 2) ? (partLen[len][0][0] +1):partLen[len][0][0];
	         if(maxlv > 1)
	         {
	             for(int lv = 1; lv < maxlv; ++lv )
	             {
	                 pn = 1 << (lv+1);
	                 for(int pid = 0; pid < pn; ++pid)
	                 {
	                     if(pid % 2)
	                       partLen[len][lv][pid] = (partLen[len][lv-1][pid >> 1] % 2) ? (partLen[len][lv-1][pid >> 1] - partLen[len][lv][pid-1]):partLen[len][lv][pid-1] ;
	                     else
	                       partLen[len][lv][pid] = partLen[len][lv-1][pid >> 1] >> 1;
	                 }
	             }
	         }
	         //calculate partpos
	         for(int lv = 0; lv < maxlv; ++lv)
	         {
	             pn = 1 << (lv + 1);
	             partPos[len][lv][0] = 0;
	             partPos[len][lv][pn] = len;
	         }
	         for(int lv = 0; lv < maxlv; ++lv)
	         {
	              pn = 1 << (lv + 1);
	              for(int pid = 1; pid < pn; ++pid)
	                  partPos[len][lv][pid] = partPos[len][lv][pid-1] + partLen[len][lv][pid-1];

	         }
	      }

	   //generate inverted index
	   for(int id = 1; id < N; ++id)
	   {
		   if(dict[id].length() > dict[id - 1].length())
		   {
			   end = id - 1;
			   Group tmpgrp = Group(tmplen,start,end);
			   tmpgrp.buildIndex(writel);
			   grps.insert(make_pair(tmplen,tmpgrp));
               start = id;
               tmplen = dict[id].length();
		   }
		   for(int i = 0; i < dict[id].length(); ++i)
		  	   if(cfilter[id].find(dict[id].at(i)) != cfilter[id].end())
		  	  		  cfilter[id][dict[id].at(i)]++;
		  	  	else
		  	  		  cfilter[id].insert(make_pair(dict[id].at(i), 1));
	   }
	   Group tmpgrp = Group(tmplen, start, N - 1);
	  		   tmpgrp.buildIndex(writel);
	  		   grps.insert(make_pair(tmplen,tmpgrp));
	   writel.close();
	   cout<<"create index finished."<<endl;
}

void printIndex()
{
	Groupset::iterator gitr;
	for(gitr = grps.begin(); gitr != grps.end(); ++gitr)
		gitr->second.printGroup();
	  printf("print index finished!\n");
}
/*
 *
 *	  for(int id = 0; id < N; ++id)
	    {
	      curmatchpos[id] = new bool[maxDictlen + 1];
	      matchinfo[id] = new set<match_entry> [maxLevel];
	    }

	  for(int i = 0; i < N; ++i)
	        for(int j = 0; j<= maxDictlen; ++j)
	           curmatchpos[i][j] = 0;
 * 	cout<<"*************Print partlen************"<<endl;
	      for(int lv = 0; lv < maxlv; ++lv)
	      {
	         int pn = 1 << (lv + 1);
	         for(int pid = 0; pid < pn; pid++)
	             printf("%d ", partLen[lv][pid]);
	         printf("\n");
	      }
	      cout<<"*************Print partpos************"<<endl;
	              for(int lv = 0; lv < maxlv; ++lv)
	            {
	               int pn = 1 << (lv + 1);
	               for(int pid = 0; pid <= pn; ++pid)
	                   cout<<partPos[lv][pid]<<" ";
	  	         printf("\n");
	            }
 */
