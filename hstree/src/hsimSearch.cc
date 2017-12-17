
#include "hstree.h"
#include<string.h>

int simres;
vector<int> matchcnt;

int contentDist(ContentMap mq, const string& query, int id)
{
	      int diff = 0;
	      ContentMap::iterator mqit;
	      ContentMap::iterator msit;
	      for(mqit = mq.begin(); mqit != mq.end(); ++mqit)
	      	 {
	      	          char key = (*mqit).first;
	      	          int num = (*mqit).second;
	      	          msit = cfilter[id].find(key);
	      	          if(msit != cfilter[id].end())
	      	              diff += abs((*msit).second - num);
	      	          else
	      	              diff += num;
	      	 }
	     int delta = abs(static_cast<int>(query.length() - dict[id].length()));
	     return  ( (diff + delta) / 2);
}
void presim(const string& str, int tau)
{
    int lv = 0;
    while( (1 << (lv+1))-1 < tau )
                lv++;
    int pn = 1 << (lv + 1);
    matchcnt.resize(N,0);
	        memset(visited, 0 , N * sizeof(bool));
	        for(int i = 0; i < maxPN; ++i)
	                substrings[i][lv].clear();
	        int qlen = str.length();
	        for(int len = qlen - tau; len <= qlen + tau; ++len)
	      {
	        	if(len < minDictlen) continue;
	        	 if(len > maxDictlen) break;
	             if(grps.find(len) == grps.end())
	                     continue;

	                 for(int pid = 0; pid < pn; ++pid)
	                        genSubstring(qlen,pid,len,lv);

	      }
}
void hsearch(const string& query, int tau)
{
    ContentMap mq;
     for(int pos = 0; pos < static_cast<int>(query.length()); ++pos)
         if(mq.find(query.at(pos)) != mq.end())
             mq[query.at(pos)]++;
         else
             mq.insert(make_pair(query.at(pos),1));
   int pl,pn;
   InvLists::iterator inv_it;
   vector<int>::iterator vitr;
   Group::iterator gitr;
   for(pl = static_cast<int>(query.length()) - tau; pl <= static_cast<int>(query.length()) + tau; ++pl)
   {
        if(pl < minDictlen) continue;
        if(pl > maxDictlen) break;
        gitr = grps.find(pl);
        if(gitr == grps.end()) continue;
        int st = (gitr->second).first;
        int ed = (gitr->second).second;

        int lv = 0;
        while( (1 << (lv+1))-1 < tau )
            lv++;
        pn = 1 << (1 + lv);
        if(calLevel(pl) - 1 < lv)
        {
            for(int id = st; id <= ed; ++id)
            {

                if( visited[id] == 0)
                {
                	if(contentDist(mq, query,id) <= tau)
                	{
                        int veres = caled(query,dict[id],tau);
                        if(veres <= tau)
                            simres++;
                	}
                	visited[id] = 1;
                }
            }
        }
        else
        {
           for(int pid = 0; pid < pn; ++pid)
              for(int i = 0; i < static_cast<int>(substrings[pid][lv].size());++i)
           {
                ssIndex idx = substrings[pid][lv][i];
                int subst = idx.stPos;
                int sublen = idx.sslen;
                int clen = idx.len;
                if(clen != pl)  continue;
                uint64_t hv = strHash(query,subst,sublen);
                inv_it = ivLists[pid][pl][lv].find(hv);
                if(inv_it != ivLists[pid][pl][lv].end())
                {
                    vitr = inv_it->second.begin();
                    while(vitr != inv_it->second.end())
                    {
                      int id = *vitr;
                      if(visited[id] == 0)
                      {
                         matchcnt[id]++;
                          if( matchcnt[id] >=  pn - tau) //pass the filter
                          {
                        	  if(contentDist(mq, query,id) <= tau)
                        	    {
                        	          int veres = caled(query,dict[id],tau);
                        	           if(veres <= tau)
                        	            simres++;
                        	     }
                        	     visited[id] = 1;
                          }
                      }
                      vitr++;
                    }//while
                }//if inv_it
           }
        } // else
   }//for pl
}

/*
 *     //clear all the data structures
     for(int j = 0; j < N; ++j)
       {
         memset(curmatchpos[j],0,(maxDictlen+1)*sizeof(bool));
         for(int lv = 0; lv < maxLevel; ++lv)
            matchinfo[j][lv].clear();
       }
 *
 * 	  for(int j = 0; j < N; ++j)
	  {
	         memset(curmatchpos[j],0,(maxDictlen+1)*sizeof(bool));
	         matchinfo[j][lv].clear();
	       }
if(curmatchpos[id][subst] == 0)
                         {
                           match_entry tmp = match_entry(pid,subst);
                            matchinfo[id][lv].insert(tmp);
                            for(int pos = subst; pos < subst + sublen; ++pos)
                                   curmatchpos[id][pos] = 1;
                         }
                         */

