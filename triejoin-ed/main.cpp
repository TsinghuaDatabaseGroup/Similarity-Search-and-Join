#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "TrieSelfJoin.h"
#include "common/time.h"

using namespace std;

extern long long resultNum;

void LoadStrings(vector<char*>& strings, char* filename)
{
	ifstream in(filename);
	if (in.fail())
	{
		cerr << "Can not open \"" << filename << "\"" << endl;
		exit(0);
	}
	string line;
	while (getline(in, line, '\n') != NULL)
	{
		int len = line.size();
		while (len>0 && (line[len-1]=='\n' || line[len-1]=='\r')) --len;
		char* newBuf = (char*)malloc(len+1);
		memcpy(newBuf, line.c_str(), len);
		newBuf[len] = 0;
		strings.push_back(newBuf);
	}
	in.close();
}
int main(int argc, char **argv)
{
	if (argc != 3) return -1;
	log_start();

	char *filename = argv[1];
	int ed = atoi(argv[2]);
	ActiveStates::EDTH = ed;

	vector<char*> strings;
	LoadStrings(strings,filename);
	TrieSelfJoin triejoin;
	triejoin.edjoin(strings, ed);
	for (unsigned int i=0; i<strings.size(); ++i)
		free(strings[i]);
	cout << resultNum << endl;
	cout << resultNum << endl;
	return 0;
}

