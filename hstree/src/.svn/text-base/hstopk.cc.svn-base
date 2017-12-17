#include "hstree.h"
#include <string.h>
using namespace std;

InvLists::iterator inv_it;
vector<int>::iterator vitr;
priority_queue<q_entry> results;
priority_queue<q_entry> ctheap;
vector<int> ctbound;
vector<int> matchnum;

int cands;
int tau_min;
int tau_max;
int MED;

void genSubstring(int qlen, int pid, int len, int lv)
{
  int threshold = (1<<(lv + 1)) - 1;
  int subst = max(partPos[pid][len][lv] - pid, partPos[pid][len][lv] - (threshold - (qlen - len)) / 2, (qlen - len) + partPos[pid][len][lv] - (threshold - pid));
  int subed = min(partPos[pid][len][lv] + pid, partPos[pid][len][lv] + (threshold + (qlen - len)) / 2, (qlen - len) + partPos[pid][len][lv] + (threshold - pid));
  for(int stpos = subst; stpos <= subed; ++stpos)
           substrings[pid][lv].push_back(ssIndex(stpos,partLen[pid][len][lv],pid,len));
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
int multi_verify(const string& query,int id,int lv,int tau = MED)
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
          qlast = matchpos + partLen[pid][len][lv];
          stmp = s.substr(lastpos, (partPos[pid][len][lv] - lastpos));
          lastpos = partPos[pid + 1][len][lv];
          tmpres = edverify(stmp, qtmp);
          totaled += tmpres;
          if(totaled > MED) return totaled;
      }

      qtmp = query.substr(qlast, query.length() - qlast);
      stmp = s.substr(lastpos, s.length() - lastpos);
      tmpres = edverify(qtmp, stmp);
      totaled += tmpres;
      if(totaled > MED) return totaled;
      return totaled;
}

void pretopk(const string& str, int k)
{

        memset(visited, 0 , N * sizeof(bool));
     for(int i = 0; i <= maxPN; i++)
    	 	 buckets[i].clear();
     matchnum.resize(N,0);
    for(int i = 0; i < maxPN; ++i)
        for(int j = 0; j < maxLevel; ++j)
            substrings[i][j].clear();
        //gen substring for queries[i]
    int qlen = str.length();
    int maxlv = calLevel(qlen);
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
      //clear all the heaps
      while(!results.empty())
    	  results.pop();
      MED = 0;
      ctbound.clear();

      ContentMap mq;
      for(int pos = 0; pos < static_cast<int>(str.length()); ++pos)
          if(mq.find(str.at(pos)) != mq.end())
              mq[str.at(pos)]++;
          else
              mq.insert(make_pair(str.at(pos),1));
      for(int id = 0; id < N; ++id)
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
       int delta = abs(static_cast<int>(str.length() - dict[id].length()));
       int bound = (diff + delta) / 2;
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

      while(!ctheap.empty())
      {
    	  int id = ctheap.top().id;
    	  int ed = edverify(str,dict[id]);
    	  q_entry tmpres = q_entry(id, ed);
    	  results.push(tmpres);
    	  visited[id] = 1;
    	  ctheap.pop();
    	  if(MED < ed)
    		  MED = ed;
      }
}
void mismatch(const string &query, int len)
{
	Group::iterator gitr;
	gitr = grps.find(len);
	int st = (gitr->second).first;
	int en = (gitr->second).second;

	for(int id = st; id <= en; ++id)
	{
	   if(ctbound[id] > MED)
		   visited[id] = 1;
	   if(visited[id] == 0)
	   {
		  int veried = caled(query,dict[id],MED);
		  	visited[id] = 1;
			cands++;
		  if(veried < MED)
		  {
		  	q_entry entry = q_entry(id, veried);
		  	results.pop();
		  	results.push(entry);
		  	MED = results.top().dist;
		  	if(MED <= tau_min)
		  		return;
	     }
	}
   }
}
int matchgreedy(int id, const string& query, int level)
{

	int len = dict[id].length();
	int mlv = thresholdlv(MED);
	for(int lv = level + 1; lv <= mlv; ++lv)
	{
	    int pn = 1 << (1 + lv);
	    if(lv != 1) //pass down the matchinfo into the lower level
	    {
	    	set<match_entry>::iterator itr;
	      for(itr = matchinfo[id][lv-1].begin(); itr != matchinfo[id][lv-1].end(); ++itr)
	      {
	    	  	 match_entry entry = *itr;
	    		 int pid = entry.pid;
	    		 int matchpos = entry.matchpos;
	    		 int npid = 2*pid;
	    		 match_entry tmpent = match_entry(npid,matchpos);
	    		 matchinfo[id][lv].insert(tmpent);
	    		 npid += 1;
	    		 int npos = matchpos + partLen[npid-1][len][lv];
	    		 tmpent = match_entry(npid, npos);
	    		 matchinfo[id][lv].insert(tmpent);
	      }

	    }
	    for(int pid = 0; pid < pn; ++pid)
	    {
	    	bool tag = false;
	    	set<match_entry>::iterator itr;
	    	for(itr = matchinfo[id][lv].begin() ;itr != matchinfo[id][lv].end(); ++itr)
	    	{
	    		match_entry m = *itr;
	    		if(m.pid == pid)
	    		{
	    			tag = true;
	    			break;
	    		}
	    		if(m.pid > pid)
	    			break;
	    	}
	    	if(tag == false) //not matched yet
	    	{
	    		for(int i = 0; i < static_cast<int>(substrings[pid][lv].size()); ++i)
	    		{
	    	       ssIndex idx = substrings[pid][lv][i];
	    	       int subst = idx.stPos;
	               int sublen = idx.sslen;
	    	       int clen = idx.len;
	    	       if(clen != len)  continue;
		           if(curmatchpos[id][subst] == 0)
		           {
		        	  for(int pos = subst; pos < subst + sublen; ++pos)
		        	 		curmatchpos[id][pos] = 1;
	    	     	  uint64_t qhv = strHash(query,subst,sublen);
	    	     	  uint64_t shv = strHash(dict[id],partPos[pid][len][lv],partLen[pid][len][lv]);
	    	     	  if(qhv == shv)
	    	     	  {
	    	     	     match_entry tmpent = match_entry(pid,subst);
	    	    	     matchinfo[id][lv].insert(tmpent);
	    	    	     break;
	    	     	  }
		           }//if curmatchpos
	    	   }//for i
		}//if tag
	  }//for pid
	}//for lv
	int matched = matchinfo[id][mlv].size();
	return matched;
}
void segdeal(const string& query, int lv, int pl)
{
	int pn = 1 << (1 + lv);
	for(int pid = 0; pid < pn; ++pid)
		for(int i = 0; i < static_cast<int>(substrings[pid][lv].size()); ++i)
		{
			ssIndex idx = substrings[pid][lv][i];
			int subst = idx.stPos;
			int sublen = idx.sslen;
			int clen = idx.len;
			if(clen != pl)  continue;
			#if defined(STORE_HASH)
			uint64_t hv = strHash(query,subst,sublen);
			inv_it = ivLists[pid][pl][lv].find(hv);
			#else
			string str = query.substr(subst,sublen);
			inv_it = ivLists[pid][pl][lv].find(str);
			#endif
			if(inv_it != ivLists[pid][pl][lv].end()) //find substr in ivlists
			{
				   vitr = inv_it->second.begin();
				   while(vitr != inv_it->second.end()) //for each id with matched substr
				   {
					   int id = *vitr;
					   if(ctbound[id] > MED)
						   visited[id] = 1;
					   if( visited[id] == 0)
					   {
						   matchnum[id]++;
						   if(MED > tau_max || (MED <= tau_max && matchnum[id] >= pn - MED))
						   {
							   q_entry entry;
							   entry.id = id;
							   int veried = caled(query,dict[id], MED);
							   visited[id] = 1;
							   cands++;
							   if(veried < MED)
							   {
							   		entry.dist = veried;
							   		results.pop();
							   		results.push(entry);
							   		MED = results.top().dist;
							   		if(MED <= tau_min)
							   			return;
							   }
						   }//if med > taumax
					   }
			           vitr++;
				    }//while
			}//if find substr
		}
}
void hstopk(const string &query, int k)
{
	Group::iterator gitr;
	gitr = grps.find(query.length());
   // search in each level
  for (int lv = 0; lv < maxLevel;++lv)
  {
	  tau_min = 1 << lv;
	  tau_max = (1 << (lv + 1)) - 1;
	  if(MED <= tau_min)
		  return;
	  for(int pl = static_cast<int>(query.length()) + tau_max; pl >= static_cast<int>(query.length()) - tau_max; --pl)
	  {
		  if(pl > maxDictlen) continue;
		  if(pl < minDictlen) break;
		  gitr = grps.find(pl);
		  if(gitr == grps.end()) continue;
		  if(calLevel(pl) -1 < lv )
			  mismatch(query,pl);
		  else
		  {
			  int pn = 1<< (lv+1);
			  segdeal(query,lv,pl);
			for(int i = pn; i >= 1;i--)
			{
				if((MED <= tau_max && i < pn - MED))
					break;
				vector<int>::iterator vit;
				for(vit = buckets[i].begin(); vit != buckets[i].end(); ++vit)
				{
					int mlv = calLevel(pl);
					if(matchgreedy(*vit,query,mlv) >= (1<<mlv) - MED)
					{
						int veried = multi_verify(query,*vit, lv);
					    visited[*vit] = 1;
						cands++;
						q_entry entry;
						entry.id = *vit;
						if(veried < MED)
						{
						 entry.dist = veried;
					     results.pop();
						 results.push(entry);
						 MED = results.top().dist;
						 if(MED <= tau_min)
								return;
						}
					}
				}
			}
		  }
	  }
  }
}
