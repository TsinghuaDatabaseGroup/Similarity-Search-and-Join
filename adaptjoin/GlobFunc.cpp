#include "GlobFunc.h"

void toLowerString(string& str)
{
	for (uint i=0; i<str.size(); ++i)
	{
		char ch = str[i];
		if (ch >= 'A' && ch <= 'Z')
			str[i] = ch - 'A'+'a';
	}
}
void LoadIntSearchQuery(const char* query_filename, vector<pair<string,int> >& query_set)
{
	ifstream in(query_filename);
	if (in.fail())
	{
		cerr << "Cannot open " << query_filename << endl;
		exit(0);
	}

	string line;
	while (getline(in, line, '\n'))
	{
		string::size_type pos = line.rfind("\t");
		int edth = atoi(line.substr(pos+1).c_str());
		string query = line.substr(0, pos);
		query_set.push_back(pair<string,int>(query, edth));
	}
	in.close();
}
void LoadDoubleSearchQuery(const char* query_filename, vector<pair<string,double> >& query_set)
{
	ifstream in(query_filename);
	if (in.fail())
	{
		cerr << "Cannot open " << query_filename << endl;
		exit(0);
	}

	string line;
	while (getline(in, line, '\n'))
	{
		string::size_type pos = line.rfind("\t");
		double theta = atof(line.substr(pos+1).c_str());
		string query = line.substr(0, pos);
		query_set.push_back(pair<string,double>(query, theta));
	}
	in.close();
}
void loadStringSet(const char* filename, vector<string>& stringSet, int k)
{
	ifstream in(filename);
	if (in.fail())
	{
		cerr << "Cannot open " << filename << endl;
		exit(0);
	}
	string line;
	while (getline(in, line, '\n')&&k--)
	{
		stringSet.push_back(line);
	}
	in.close();
}

void loadStringSet(const char* filename, vector<string>& stringSet)
{
	ifstream in(filename);
	if (in.fail())
	{
		cerr << "Cannot open " << filename << endl;
		exit(0);
	}

	string line;
	while (getline(in, line, '\n'))
	{
		stringSet.push_back(line);
	}
	in.close();
}

void tokenizes(const string & str, const string & delimiters, vector<string> & words)
{
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	string::size_type pos = str.find_first_of(delimiters, lastPos);
	while (string::npos != pos || string::npos != lastPos)
	{
		string aword = str.substr(lastPos, pos - lastPos);
		words.push_back(aword);
		lastPos = str.find_first_not_of(delimiters, pos);
		pos = str.find_first_of(delimiters, lastPos);
	}
}
void tokenizes(const string & str, const string & delimiters, vector<string> & words, const char* reserve)
{
	string::size_type pos = 0;
	string::size_type rep = string::npos;
	while (true)
	{
		string::size_type lastPos = str.find_first_not_of(delimiters, pos);
		if (lastPos == string::npos)
			break;
		if (rep<pos || rep == string::npos)
			rep = str.find_first_of(reserve, pos);
		if (rep != string::npos&&rep<lastPos)
			words.push_back(str.substr(rep,1));
		pos = str.find_first_of(delimiters, lastPos);
		words.push_back(str.substr(lastPos, pos - lastPos));
	}
}
void loadStopWords(Set<string>& stopWords)
{
	ifstream in("stopwords.dat");
	string line;
	while(getline(in,line))
	{
		stopWords.insert(line);
	}
	in.close();
}

void getDeletionNeighbours(const string& sig, Set<string>& neighbours, int minEd)
{
	if (minEd < 0) return;
	neighbours.insert(sig);
	if (minEd== 0)		return;
	for (uint i=0; i<sig.size(); ++i)
	{
		string cpsig(sig);
		cpsig.erase(i, 1);
		getDeletionNeighbours(cpsig,neighbours, minEd-1);
	}
}


