#include"hstree.h"

using namespace std;

int maxLevel;
int maxPN;
bool *visited;
bool **curmatchpos;//curmatchpos[id][p]: whether pth position of dict[id] has been matched with a substring
set<match_entry> **matchinfo;//pmatch[id][lv]: the set of matched <pid,matchpos in q> of dict[id] in the level lv
int **matrix;
int ***partLen;//the length of ith segment of a string with length j, the level of index is k
int ***partPos;//partpos[i][j][k]: the start position of ith segment of a string with length j, the level of index is k
InvLists ***ivLists; //ivLists[i][j][k]: the corresponding strings with the ith segment in the group of strings with length j and level k
vector<ssIndex> **substrings;//substrings[i][j]: the candidate substrings of the ith segment in the level j
ContentMap *cfilter;
Group grps;
vector<int>* buckets;

inline bool strLessT(const string &s1, const string &s2) {
  if (s1.length() < s2.length()) return true;
  else if (s1.length() > s2.length()) return false;
  return s1 < s2;
}

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

void initial()
{
  maxLevel = calLevel(maxDictlen);
  maxPN = 1<<(maxLevel+1);
  visited = new bool[N];
  cfilter = new ContentMap[N];
  //curmatchpos = new bool *[N];
  matrix = new int *[maxDictlen + 1];
  partLen = new int **[maxPN];
  partPos = new int **[maxPN + 1];
  ivLists = new InvLists **[maxPN];
  substrings = new vector<ssIndex> *[maxPN];
  //matchinfo = new set<match_entry> *[N];
  buckets = new vector<int> [maxPN+1];
/*
   for(int id = 0; id < N; ++id)
   {
     curmatchpos[id] = new bool[maxDictlen + 1];
     matchinfo[id] = new set<match_entry> [maxLevel];
   }
*/
  for(int lp = 0; lp < maxPN; ++lp)
  {
      partLen[lp] = new int *[maxDictlen + 1];
      partPos[lp] = new int *[maxDictlen + 1];
      ivLists[lp] = new InvLists *[maxDictlen + 1];
      substrings[lp] = new vector<ssIndex> [maxLevel];
  }
  partPos[maxPN] = new int *[maxDictlen + 1];

  for(int lp = 0; lp < maxPN; ++lp)
    for(int len = 0; len <= maxDictlen; ++len)
  {
     partLen[lp][len] = new int [maxLevel];
     partPos[lp][len] = new int [maxLevel];
     ivLists[lp][len] = new InvLists [maxLevel];
  }
  for(int len = 0; len <= maxDictlen; ++len)
    partPos[maxPN][len] = new int [maxLevel];

  for(int id = 0; id <= maxDictlen; ++id)
    matrix[id] = new int[maxDictlen + 1];
 /* for(int i = 0; i < N; ++i)
        for(int j = 0; j<= maxDictlen; ++j)
           curmatchpos[i][j] = 0;*/
  for(int i = 0; i < maxDictlen; ++i)
  {
    matrix[i][0] = i;
    matrix[0][i] = i;
  }
  cout<<"initial finished!"<<endl;
}

void sortData()
{
    sort(dict.begin(), dict.end(), strLessT);
}

void createIndex()
{
    //group by length
   int st = 0, ed = 0;
   int cnt = 0;
   int totalsize;
   int pn;
    int tmplen = minDictlen;
  for(int id = 1; id < N ; id++)
  {
    if(dict[id].length() > dict[id - 1]. length())
    {
      ed = id - 1;
      Interval tmpitv = make_pair(st,ed);
      std::pair<int,Interval> tmppair = make_pair(tmplen,tmpitv);
      grps.insert(tmppair);
      st = id;
      tmplen = dict[id].length();
    }
  }
    Interval tmpitv = make_pair(st,(N - 1));
    std::pair<int,Interval> tmppair = make_pair(tmplen,tmpitv);
    grps.insert(tmppair);
   //build index for each group
   for (int len = minDictlen; len <= maxDictlen; ++len)
   {
      Group::iterator gitr;
      gitr = grps.find(len);
      if(gitr == grps.end())
            continue;
      Interval itv = gitr->second;
      int stpos = itv.first;
      int edpos = itv.second;
      int maxlv = calLevel(len);
      //calculate partlen
      partLen[0][len][0] = len >> 1;
      partLen[1][len][0] = (len % 2) ? (partLen[0][len][0] +1):partLen[0][len][0];
      if(maxlv > 1)
      {
          for(int lv = 1; lv < maxlv; ++lv )
          {
              pn = 1 << (lv+1);
              for(int pid = 0; pid < pn; ++pid)
              {
                  if(pid % 2)
                    partLen[pid][len][lv] = (partLen[pid >> 1][len][lv - 1] % 2) ? (partLen[pid >> 1][len][lv - 1] - partLen[pid - 1][len][lv]):partLen[pid - 1][len][lv] ;
                  else
                    partLen[pid][len][lv] = partLen[pid >> 1][len][lv - 1] >> 1;
              }
          }
      }
      //calculate partpos
      for(int lv = 0; lv < maxlv; ++lv)
      {
          pn = 1 << (lv + 1);
          partPos[0][len][lv] = 0;
          partPos[pn][len][lv] = len;
      }
      for(int lv = 0; lv < maxlv; ++lv)
      {
           pn = 1 << (lv + 1);
           for(int pid = 1; pid < pn; ++pid)
               partPos[pid][len][lv] = partPos[pid - 1][len][lv] + partLen[pid - 1][len][lv];

      }
      //put strings in this group into corresponding inverted list
      for (int id = stpos; id <= edpos; ++id)
      {
          for(int lv = 0; lv < maxlv; ++lv)
          {
             pn = 1 << (lv + 1);
             for(int pid = 0; pid < pn; ++pid)
             {
				#if defined(STORE_HASH)
              ivLists[pid][len][lv][strHash(dict[id], partPos[pid][len][lv], partLen[pid][len][lv])].push_back(id);
				#else
              ivLists[pid][len][lv][dict[id].substr(partPos[pid][len][lv], partLen[pid][len][lv])].push_back(id);
				#endif
              cnt++;
             }
          }
      }
   }

    for(int id = 0; id < N; ++id)
        for(int i = 0; i < static_cast<int>(dict[id].length()); ++i)
            if(cfilter[id].find(dict[id].at(i)) != cfilter[id].end())
                cfilter[id][dict[id].at(i)]++;
            else
                cfilter[id].insert(make_pair(dict[id].at(i), 1));
  totalsize = maxLevel*maxPN*maxDictlen*2*sizeof(int);
  totalsize += cnt * sizeof(int);
   cout<<"Index size: "<<(totalsize/(1024*1024))<<" MB"<<endl;
}

