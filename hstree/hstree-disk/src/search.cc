/*
 * search.cc
 *
 *  Created on: 2014-12-11
 *      Author: wangjin
 */
# include "search.h"
#include "index.h"
#include <string.h>

InvertedMap::iterator inv_it;
vector<int>::iterator vitr;
Groupset::iterator gitr;
priority_queue<q_entry> results;
priority_queue<q_entry> ctheap;
vector<int> ctbound;
vector<int> matchcnt;
int simres;
extern Groupset grps;

int tau_min;
int tau_max;
int MED;
void genSubstring(int qlen, int pid, int len, int lv)
{
  int threshold = (1<<(lv + 1)) - 1;
  int subst = max(partPos[len][lv][pid] - pid, partPos[len][lv][pid] - (threshold - (qlen - len)) / 2, (qlen - len) + partPos[len][lv][pid] - (threshold - pid));
  int subed = min(partPos[len][lv][pid] + pid, partPos[len][lv][pid] + (threshold + (qlen - len)) / 2, (qlen - len) + partPos[len][lv][pid] + (threshold - pid));
  for(int stpos = subst; stpos <= subed; ++stpos)
           substrings[lv][pid].push_back(ssIndex(stpos,partLen[len][lv][pid],pid,len));
}
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
int edverify(const string& s,const string& t)
{
	if(s == "" && t == "")
		return 0;
	else if (s == "" && t != "")
		return t.length();
	else if (s != "" && t == "")
		return s.length();
	else
	{
    int ls = s.length();
    int lt = t.length();
    for(int i = 1; i<= lt;++i)
        for(int j = 1; j<= ls; ++j)
    {
         int sub = (t[i-1] == s[j-1])? 0 : 1;
         matrix[i][j] = min(matrix[i-1][j] + 1, matrix[i][j-1]+1, matrix[i-1][j-1]+sub);
    }
        return matrix[lt][ls];
	}
}

int thresholdlv(int tau)
{
	if(tau == 0)
		return -1;
	int lv = 0;
	while( (1 << (lv+1))-1 < tau )
	    lv++;
	return lv;
}
int multi_verify(const string& query,int id,int lv, int tau)
{
      string s = dict[id];
      int len = s.length();
      int totaled = 0;
      int tmpres;
      string qtmp,stmp;
      int lastpos = 0, qlast = 0;
      set<match_entry>::iterator itr;
      for(itr = matchinfo[id][lv].begin(); itr != matchinfo[id][lv].end(); ++itr)
      {
          match_entry mt = *itr;
          int pid = mt.pid;
          int matchpos = mt.matchpos;
          qtmp = query.substr(qlast, (matchpos - qlast));
          qlast = matchpos + partLen[len][lv][pid];
          stmp = s.substr(lastpos, (partPos[len][lv][pid] - lastpos));
          lastpos = partPos[len][lv][pid + 1];
          tmpres = edverify(stmp, qtmp);
          totaled += tmpres;
          if(totaled > tau) return totaled;
      }
      //the last part
      qtmp = query.substr(qlast, query.length() - qlast);
      stmp = s.substr(lastpos, s.length() - lastpos);
      tmpres = edverify(qtmp, stmp);
      totaled += tmpres;
      if(totaled > tau) return totaled;
      return totaled;
}
void preprocess(const string & query, int k,bool tag)
{
	//clear all the data structures
	  matchcnt.resize(N,0);
	   memset(visited, 0 , N * sizeof(bool));
	    int qlen = query.length();
	    int maxlv = calLevel(qlen);

	   if(tag) // tag = 1, top k search
	   {
	    for(int i = 0; i < maxLevel; ++i)
	        for(int j = 0; j < maxPN; ++j)
	            substrings[i][j].clear();
	      while(!results.empty())
	    	  results.pop();
	      MED = 0;
	      ctbound.clear();
	        //gen substring for queries[i]
	      for(int len = minDictlen; len <= maxDictlen; ++len)
	     {
	         if(grps.find(len) == grps.end())
	              continue;
	        for(int lv = 0; lv < maxlv; ++lv)
	       {
	          int pn = 1 << (lv + 1);
	          for(int pid = 0; pid < pn; ++pid)
	                 genSubstring(qlen,pid,len,lv);
	       }
	     }
	      //content filter
	      ContentMap mq;
	      for(int pos = 0; pos < static_cast<int>(query.length()); ++pos)
	          if(mq.find(query.at(pos)) != mq.end())
	              mq[query.at(pos)]++;
	          else
	              mq.insert(make_pair(query.at(pos),1));
	      for(int id = 0; id < N; ++id)
	      {
	       int bound = contentDist(mq,query,id);
	       ctbound.push_back(bound);
	       if(id < k)
	       {
	    	   q_entry ctent = q_entry(id,bound);
	    	   ctheap.push(ctent);
	       }
	       else
	       {
	    	   int maxct = ctheap.top().dist;
	    	   if(bound < maxct)
	    	   {
	    		   ctheap.pop();
	    		   q_entry ctent = q_entry(id,bound);
	    		   ctheap.push(ctent);
	    	   }
	       }
	      }//for id
	   }
	   else // similarity search, k is tau
	   {
		    int lv = 0, pn;
			 while( (1 << (lv+1)) -1 < k )
			 	            lv++;
			 pn = 1 << (lv + 1);
			 for(int j = 0; j < maxPN; ++j)
				            substrings[lv][j].clear();
		      for(int len = qlen - k; len <= qlen + k; ++len)
			     {
		 	            if(len < minDictlen) continue;
		 	 		     if(len > maxDictlen) break;
		 	 		     gitr = grps.find(len);
		 	 		     if(gitr == grps.end()) continue;
		    			          for(int pid = 0; pid < pn; ++pid)
		    			                 genSubstring(qlen,pid,len,lv);

			     }
	   }
}

void htopkSearch(const string& path, const string& query, int k)
{
    //insert first k strings into the result set
	      while(!ctheap.empty())
	      {
	    	  int id = ctheap.top().id;
	    	  int ed = edverify(query,dict[id]);
	    	  q_entry tmpentry = q_entry(id, ed);
	    	  results.push(tmpentry);
	    	  visited[id] = 1;
	    	  ctheap.pop();
	    	  if(MED < ed)
	    		  MED = ed;
	      }
   	 Group group;
   	 ifstream readl;
	 vector<int> tmplist;
   	 readl.open(path.c_str(), ios::in);
	 for (int lv = 0; lv < maxLevel;++lv)
	  {
		  tau_min = 1 << lv;
		  tau_max = (1 << (lv + 1)) - 1;
		  if(MED <= tau_min)
		  {
			  readl.close();
			  return;
		  }
		  int qlength = static_cast<int>(query.length());
		  for(int pl = qlength + tau_max; pl >= qlength - tau_max; --pl)
		 	  {
		 		  if(pl > maxDictlen) continue;
		 		  if(pl < minDictlen) break;
		 		  gitr = grps.find(pl);
		 		 if(gitr == grps.end()) continue;
		 		 group =  grps.find(pl)->second;
		 		 int st = group.st;
		 		 int en = group.en;
		 		  if(calLevel(pl) - 1 < lv)
		 		  {
		 				for(int id = st; id <= en; ++id)
		 				{
		 				   if(ctbound[id] > MED)
		 					   visited[id] = 1;
		 				   if(visited[id] == 0)
		 				   {
		 					  int veried = caled(query,dict[id],MED);
		 					  	visited[id] = 1;
		 					  if(veried < MED)
		 					  {
		 					  	q_entry entry = q_entry(id, veried);
		 					  	results.pop();
		 					  	results.push(entry);
		 					  	MED = results.top().dist;
		 					  	if(MED <= tau_min)
		 					  	{
		 					  		readl.close();
		 					  		return;
		 					  	}
		 				     }
		 				}
		 			   }
		 		  }
		 		  else
		 		  {
		 			 int pn = 1<< (lv+1);
		 			for(int pid = 0; pid < pn; ++pid)
		 					for(int i = 0; i < static_cast<int>(substrings[lv][pid].size()); ++i)
		 					{
		 						  ssIndex idx = substrings[lv][pid].at(i);
		 						  int subst = idx.stPos;
		 						  int sublen = idx.sslen;
		 						  int clen = idx.len;
		 						  if(clen != pl)  continue;
		 						 #if defined(STORE_HASH)
		 						  uint64_t hv = strHash(query,subst,sublen);
								#else
		 							string hv = query.substr(subst,sublen);
								#endif
		 									inv_it = group.maps[lv][pid].find(hv);
		 									if(inv_it != group.maps[lv][pid].end())
		 									{
		 										tmplist.clear();
		 										ADDR offset = inv_it ->second;
		 										readl.seekg(offset);
		 										 loadList(readl, tmplist);
		 										// datavisited += tmplist.size()*sizeof(int);
		 										for(vitr = tmplist.begin(); vitr != tmplist.end(); ++vitr)
		 										{
		 												int id = *vitr;
		 												if(ctbound[id] > MED)
		 													visited[id] = 1;
		 												if(visited[id] == 0)
		 												{
		 														matchcnt[id]++;
		 														if(MED > tau_max || (MED <= tau_max && matchcnt[id] >=  pn - MED))
		 														{
		 															int veried = caled(query,dict[id], MED);
		 															visited[id] = 1;
		 																if(veried < MED)
		 																{
				 															q_entry entry(id,veried);
		 																		results.pop();
		 																		results.push(entry);
		 																		MED = results.top().dist;
		 																		if(MED <= tau_min)
		 																		{
		 																		        	readl.close();
		 																		   			return;
		 																		}
		 																}
		 														 }//if med > taumax, do verify
		 												}
		 										}// for vitr
		 									}//if inv_it
		 					}
		 		  }//if callevel
		 	  }//for pl
	  }//for lv
}

void hsimSearch(const string& path, const string& query, int tau)
{
	  ContentMap mq;
	     for(int pos = 0; pos < static_cast<int>(query.length()); ++pos)
	         if(mq.find(query.at(pos)) != mq.end())
	             mq[query.at(pos)]++;
	         else
	             mq.insert(make_pair(query.at(pos),1));
	     Group group;
	   	 ifstream readl;
		 vector<int> tmplist;
	   	 readl.open(path.c_str(), ios::in);
	   	 int pl, pn;
	     int lv = 0;
	 	 while( (1 << (lv+1)) -1 < tau )
	 	            lv++;
	 	int qlength = static_cast<int>(query.length());
	 	for(pl = qlength - tau; pl <= qlength + tau; ++pl)
	 	   {
	 		     if(pl < minDictlen) continue;
	 		     if(pl > maxDictlen) break;
	 		     gitr = grps.find(pl);
	 		     if(gitr == grps.end()) continue;
	 		     group =  grps.find(pl)->second;
	 		  	 int st = group.st;
	 		  	 int en = group.en;
	 		  	 if(calLevel(pl) - 1 < lv)
	 		  	 {
	 		  			  		  for(int id = st; id <= en; ++id)
	 		  			  		  {
	 		  			  			  if(contentDist(mq,query,id) >  tau)
	 		  			  				  visited[id] = 1;
	 		  			  		       if(visited[id] == 0 )
	 		  			  		       {
	 		  			  		             int veres = caled(query,dict[id],tau);
	 		  			  		               visited[id] = 1;
	 		  			  		              if(veres <= tau)
	 		  			  		            	  simres++;
	 		  			  		       }
	 		  			  		  }
	 		    }
	 		  	else
	 		   {
	 		  			pn = 1<< (lv+1);
	 		  			for(int pid = 0; pid < pn; ++pid)
	 		  			 		for(int i = 0; i < static_cast<int>(substrings[lv][pid].size()); ++i)
	 		  			 		{
	 		  			 					ssIndex idx = substrings[lv][pid][i];
	 		  			 				    int subst = idx.stPos;
	 		  			 				  	int sublen = idx.sslen;
	 		  			 				  	int clen = idx.len;
	 		  			 				  	 if(clen != pl)  continue;
	 		  			 					 #if defined(STORE_HASH)
	 		  			 				  		    uint64_t hv = strHash(query,subst,sublen);
	 		  			 					#else
	 		  			 				  		    string hv = query.substr(subst,sublen);
	 		  			 					#endif
	 			 								inv_it = group.maps[lv][pid].find(hv);
	 		  			 				  		if(inv_it != group.maps[lv][pid].end())
	 		  			 				  		{
	 		  			 				  		        tmplist.clear();
	 		  			 				  			 	ADDR offset = inv_it ->second;
	 		  			 				  			 	readl.seekg(offset);
	 		  			 				  			 	loadList(readl, tmplist);
	 		  			 				  			 	//datavisited += tmplist.size()*sizeof(int);
	 		  			 				  			for(vitr = tmplist.begin(); vitr != tmplist.end(); ++vitr)
	 		  			 				  			{
	 		  			 				  			         int id = *vitr;
	 		  			 				  					 if(visited[id] == 0&& contentDist(mq,query,id) <= tau)
	 		  			 				  					 {
	 		  			 				  						 	 	 	matchcnt[id]++;
	 		  			 				  					  		     if( matchcnt[id] >= pn - tau) //pass the filter
	 		  			 				  					  		     {
	 		  			 				  					  		            	int veres = caled(query,dict[id], tau);
	 		  			 				  					  		            	visited[id] = 1;
	 		  			 				  					  		            	if(veres <= tau)
	 		  			 				  					  		            	simres++;
	 		  			 				  					  		    }
	 		  			 				  					 }
	 		  			 				  			}
	 		  			 				  		}
	 		  			 		}
	 		   }
	 	  }
	 	readl.close();
}

