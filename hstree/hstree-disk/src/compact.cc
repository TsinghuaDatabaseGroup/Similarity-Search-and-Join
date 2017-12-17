/*
 * compact.cc
 *
 *  Created on: 2014-12-29
 *      Author: wangjin
 */
#include "compact.h"
#include "storage.h"
InvertedMap::iterator map_it;
int compactGroup::getMaxLevel()
{
    int maxlv = 0;
     while(true)
      {
          int pn = 1 << (maxlv+1);
          if(len / pn <= 1)
            break;
          maxlv++;
      }
    return (maxlv+1);
}
int compactGroup::getMaxPN()
{
	int maxlv = getMaxLevel();
	return (1<<(maxlv+1));
}
void compactGroup:: buildIndex(ofstream &pf)
{
	    int maxlv = getMaxLevel();
		 int pn;
		 set<string>::iterator sitr;
		 InvertedList ivlists[2];
		 vector<vector<set<string> > > tmplists; // for hash code representation, back up the original strings
		 tmplists.resize(maxlv);
		 for(int i = 0; i < maxlv; ++i)
			{
				pn =  1 <<(i+1);
				tmplists[i].resize(pn);
			}
		 vector<vector<AddressList> > addrlists;
		  addrlists.resize(maxlv);
		  for(int i = 0; i < maxlv; ++i)
		{
			  pn =  1 <<(i+1);
			addrlists[i].resize(pn);
		}
		 //generate inverted list
		 for(int id = st; id <= en; ++id )
		 {
			 for(int pid = 0; pid < 2; ++pid)
			 {
				 string str = dict[id].substr(partPos[pid][len][0], partLen[pid][len][0]);
				 tmplists[0][pid].insert(str);
				#if defined STORE_HASH
				 ivlists[pid][strHash(dict[id], partPos[pid][len][0], partLen[pid][len][0])].push_back(id);
				#else
				 ivlists[pid][str].push_back(id);
				#endif
			 }
		 }
		 for(int pid = 0; pid <= 2; ++pid)
		 {
			    InvertedList::iterator inv_itr;
			    ADDR offset = pf.tellp();
		       for(inv_itr = ivlists[0].begin(); inv_itr != ivlists[0].end(); ++inv_itr)
		         {
		 				# if defined (STORE_HASH)
		         	    int64_t value = inv_itr->first;
		 				#else
		         	    string value = inv_itr->first;
		 			   #endif
		         	    maps[0][pid].insert(make_pair(value,offset));
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
		 //generate addrlists
		 for(int lv = 1; lv < maxlv; ++lv)
		 {
			  pn = 1 << lv ;//the max pn in level lv-1
			  for(int pid = 0; pid < pn; ++pid)
			  {
				  for(sitr = tmplists[lv-1][pid].begin(); sitr != tmplists[lv-1][pid].end(); ++sitr)
				  {
					  string tmpstr = *sitr;
					  int len = tmpstr.length();
					  string str1 = tmpstr.substr(0, len/2);
					  string str2 = tmpstr.substr(len/2, len - str1.length());
					  tmplists[lv][2*pid].insert(str1);
					  tmplists[lv][2*pid+1].insert(str2);
					#if defined STORE_HASH
					  uint64_t hv = strHash(tmpstr, 0, tmpstr.length());
					  uint64_t hv1 = strHash(str1, 0, str1.length());
					  uint64_t hv2 = strHash(str2, 0, str2.length());
					  map_it = maps[lv][pid].find(hv);
					  ADDR address = map_it->second;
					  addrlists[lv][2*pid][hv1].push_back(address);
					  addrlists[lv][2*pid+1][hv2].push_back(address);
					#else
					  addrlists[lv][2*pid][str1].push_back(tmpstr);
					  addrlists[lv][2*pid+1][str2].push_back(tmpstr);
					#endif
				  }
			  }
			  for(int pid = 0; pid < pn; ++pid)
			 {
				  AddressList::iterator addr_itr;
				  ADDR offset = pf.tellp();
				  for(addr_itr = addrlists[lv][pid].begin(); addr_itr != addrlists[0][pid].end(); ++addr_itr)
				  {
				  		 	# if defined (STORE_HASH)
				  		        int64_t value = addr_itr->first;
				  		 				#else
				  		         	    string value = addr_itr->first;
				  		 			   #endif
				  		         	    maps[lv][pid].insert(make_pair(value,offset));
				  		         	    vector<int> list;
				  		         	    vector<ADDR>::iterator itr;
				  		         	     for(itr = addr_itr->second.begin(); itr != addr_itr->second.end(); ++itr )
				  		         	    {
				  		         	    		 list.push_back(*itr);
				  		         	    }
				  		         	    saveList(pf, list);
				  		         	     offset = pf.tellp();
				  }
			 }
		 }//for lv
}
void compactGroup:: getIvList(uint64_t key, int lv, int pid, vector<ADDR>& res,ifstream &pf)
{
		set<ADDR>  upper;
		set<ADDR>::iterator sitr;
		vector<ADDR> current;
		vector<ADDR> tmplist;
	    int level = lv;
        current.clear();
        map_it = maps[lv][pid].find(key);
        ADDR offset = map_it->second;
        pf.seekg(offset);
        loadaddrList(pf, current);
	    if(lv == 0)
	    {
	    	res.assign(current.begin(), current.end());
	    	return;
	    }

	    while(level > 1)
	    {
	    	 for(int i = 0; i < current.size(); ++i)
	       {
	    		     tmplist.clear();
	    		     ADDR offset = *sitr;
	    		     pf.seekg(offset);
	    		     loadaddrList(pf, tmplist);
	    		     for(int i = 0; i < tmplist.size();++i)
	    		     {
	    		         	upper.insert(current[i]);
	    		      }
	    	}
	    	 current.clear();
           for(sitr = upper.begin(); sitr != upper.end(); ++sitr)
           {
        	   current.push_back(*sitr);
           }
           upper.clear();
	    	level--;
	    }
	    res.assign(current.begin(), current.end());
}
void compactGroup::combine(int lv, int pid, vector<ADDR>& address, vector<int>& result, ifstream &pf)
{
	vector<int> tmplist;
	set<int> nodupres;
	set<int>::iterator sitr;
	for(int i = 0; i < address.size(); ++i)
	{
			tmplist.clear();
			ADDR offset = address[i];
		    pf.seekg(offset);
			 loadList(pf, tmplist);
			 for(int j = 0; j < tmplist[j]; ++j)
				 nodupres.insert(tmplist[j]);
	}
	 for(sitr = nodupres.begin(); sitr != nodupres.end();++sitr)
		 result.push_back(*sitr);
}




