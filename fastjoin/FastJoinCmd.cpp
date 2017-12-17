#include "FastJoin.h"
#include "TokenSigScheme/TokenSigSchemeFactory.h"
#include "TokenSigScheme/TokenSigScheme.h"
#include "TokenSetSigScheme/JaccardTokenSetSigScheme.h"
#include "TokenSetSigScheme/DiceTokenSetSigScheme.h"
#include "TokenSetSigScheme/CosinTokenSetSigScheme.h"
#include "Tokenizer/StandardTokenizer.h"
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

void usage()
{
	cerr << "Usage:" << "./FastJoin [FJACCARD | FCOSINE | FDICE] delta tau file1 [file2]" << endl << endl;
	cerr << "Example: ./FastJoin FJaccard 0.85 0.8 querylog.txt" << endl;
	cerr << "Output: All pairs <s1, s2> in \"querylog.txt\" s.t. FJACCARD_{0.85} (s1, s2) >= 0.8" << endl;
}
int main(int argc, char* argv[])
{
	if (argc != 5 && argc != 6)
	{
		usage();
		exit(0);
	}
	string simFunc(argv[1]);
	toLowerString(simFunc);
	TokenSetSigScheme<int>* setScheme;
	TokenSigScheme* partitionNEDScheme = TokenSigSchemeFactory::getTokenSigScheme(PartitionNED);
	if (simFunc == "fjaccard")		 setScheme = new JaccardTokenSetSigScheme<int>(partitionNEDScheme);
	else if (simFunc == "fcosine")	setScheme = new CosinTokenSetSigScheme<int>(partitionNEDScheme);
	else if (simFunc == "fdice")	setScheme = new DiceTokenSetSigScheme<int>(partitionNEDScheme);
	else { usage();	exit(0);	}

	double delta = atof(argv[2]);
	double tau = atof(argv[3]);
	char* filename1 = argv[4];
	char* filename2;
	vector<string> stringSet1, stringSet2;

	Tokenizer* tokenizer = new StandardTokenizer();
	FastJoin<int> fastjoinInt(setScheme, tokenizer);
	vector<Result> results;
	if (argc == 5)
	{
		loadStringSet(filename1, stringSet1);
		fastjoinInt.join(stringSet1, delta, tau, results);
	}
	if (argc == 6)
	{
		filename2 = argv[5];
		loadStringSet(filename1, stringSet1);
		loadStringSet(filename2, stringSet2);
		fastjoinInt.join(stringSet1, stringSet2, delta, tau, results);
	}
	delete partitionNEDScheme;
	delete setScheme;
	delete tokenizer;
	return 0;
}
