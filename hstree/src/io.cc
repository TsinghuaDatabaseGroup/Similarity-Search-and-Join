#include"hstree.h"
using namespace std;

vector<string> dict;
vector<string> queries;
int minDictlen = 0x7FFFFFFF, maxDictlen = 0;
int N ;// # of strings
int avglen;
void readData(char* file)
{
  string str;
  ifstream readData(file, std::ios::in);
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

void readQuery(char* file)
{
  string str;
  ifstream readQuery(file, std::ios::in);
  while(getline(readQuery, str))
  {
      queries.push_back(str);
  }
  readQuery.close();
}

