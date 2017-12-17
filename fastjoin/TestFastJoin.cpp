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

void outputResult1(const vector<Result>& results, const vector<string>& stringSet)
{
	vector<string> sortedResults;
	for (uint i=0; i<results.size(); ++i)
	{
		const Result& result = results[i];
		assert(result.id1 != result.id2);
		if (stringSet[result.id1] < stringSet[result.id2])
		{
			//cout << result.sim << " " << result.id1 << " " << result.id2 << endl;
			//cout << stringSet[result.id1] << endl;
			//cout << stringSet[result.id2] << endl;
			sortedResults.push_back(stringSet[result.id1]+" "+stringSet[result.id2]);
			//cout << stringSet[result.id1] << " " << stringSet[result.id2] << endl;
		}
		else
		{
			//cout << result.sim << " " << result.id2 << " " << result.id1 << endl;
			//cout << stringSet[result.id2] << endl;
			//cout << stringSet[result.id1] << endl;
			sortedResults.push_back(stringSet[result.id2]+" "+stringSet[result.id1]);//
			//cout << stringSet[result.id2] << " " << stringSet[result.id1] << endl;
		}
		//cout << endl;
	}
	sort(sortedResults.begin(), sortedResults.end());
	for (uint i=0; i<sortedResults.size(); ++i)
		cout << sortedResults[i] << endl;
}
template<class T>
void outputResult(const vector<Result>& results, const vector<string>& stringSet)
{
	for (uint i=0; i<results.size(); ++i)
	{
		const Result& result = results[i];
		assert(result.id1 != result.id2);
		if (result.id1 < result.id2)
		{
			cout << result.sim << " " << result.id1 << " " << result.id2 << endl;
			cout << stringSet[result.id1] << endl;
			cout << stringSet[result.id2] << endl;
			//cout << stringSet[result.id1] << " " << stringSet[result.id2] << endl;
		}
		else
		{
			cout << result.sim << " " << result.id2 << " " << result.id1 << endl;
			cout << stringSet[result.id2] << endl;
			cout << stringSet[result.id1] << endl;
			//cout << stringSet[result.id2] << " " << stringSet[result.id1] << endl;
		}
		cout << endl;
	}
}
void outputSignatures(char* filename, TokenSigScheme* qgramScheme, vector<string>& stringSet)
{
	ofstream out1(string(filename).append(".query").c_str());
	ofstream out2(string(filename).append(".data").c_str());
	for (uint i=0; i<stringSet.size(); ++i)
	{
		vector<string> querySigs;
		qgramScheme->getQuerySig(stringSet[i], querySigs);
		out1 << stringSet[i];
		for (uint j=0; j<querySigs.size(); ++j)
			out1 << " " << querySigs[j];
		out1 << endl;

		vector<string> dataSigs;
		qgramScheme->getDataSig(stringSet[i], dataSigs);
		out2 << stringSet[i];
		for (uint j=0; j<dataSigs.size(); ++j)
			out2 << " " << dataSigs[j];
		out2 << endl;		
	}
	out1.close();
	out2.close();
}
int main1(int argc, char* argv[])
{
//	FILE* fp = freopen("tokens_author.txt.result", "w", stdout);
	if (argc != 5)
	{
	    cerr << "Usage: FastJoin delta tau filename k" << endl;
	    exit(0);
	}
	double delta = atof(argv[1]);
	double tau = atof(argv[2]);
	char* filename = argv[3];
	int k = atoi(argv[4]);
	TokenSigScheme* partitionNEDScheme = TokenSigSchemeFactory::getTokenSigScheme(PartitionNED);
	TokenSigScheme* qgramScheme = TokenSigSchemeFactory::getTokenSigScheme(QGram);
	TokenSetSigScheme<int>* setScheme1 = new JaccardTokenSetSigScheme<int>(partitionNEDScheme);
	TokenSetSigScheme<string>* setScheme2 = new JaccardTokenSetSigScheme<string>(partitionNEDScheme);
	TokenSetSigScheme<int>* setScheme3 = new JaccardTokenSetSigScheme<int>(qgramScheme);
	TokenSetSigScheme<string>* setScheme4 = new JaccardTokenSetSigScheme<string>(qgramScheme);
	Tokenizer* tokenizer = new StandardTokenizer();

	// Init string set
	vector<string> stringSet;
	loadStringSet(filename, stringSet, k);
	//tokenize string set into token set
	
	
	
	FastJoin<int> fastjoinInt(setScheme1, tokenizer);
	FastJoin<string> fastjoinStr(setScheme2, tokenizer);
	qgramScheme->buildScheme(stringSet, delta);
	outputSignatures(filename, qgramScheme, stringSet);
	partitionNEDScheme->buildScheme(stringSet, delta);
	outputSignatures(filename, partitionNEDScheme, stringSet);
	
	time_t begin = time(NULL);
	// fast join
	vector<Result> results;
	fastjoinInt.join(stringSet, delta, tau, results);
	//outputResult<string>(results, stringSet);
	time_t end = time(NULL);
	cout << "PNED-INT: " << results.size() << " " << end-begin << endl;
	

	
	results.clear();
	begin = time(NULL);
	fastjoinStr.join(stringSet, delta, tau, results);
	end = time(NULL);
	//outputResult<string>(results, stringSet);
	cout << "PNED-STR: " << results.size()<< " " << end-begin << endl;

	fastjoinInt.setTokenSetSigScheme(setScheme3);
	fastjoinStr.setTokenSetSigScheme(setScheme4);

	// fast join
	results.clear();
	begin = time(NULL);
	fastjoinInt.join(stringSet, delta, tau, results);
	end = time(NULL);
	//outputResult<string>(results, stringSet);
	cout << "QGra-INT: " << results.size()<< " " << end-begin << endl;
	
	results.clear();
	begin = time(NULL);
	fastjoinStr.join(stringSet, delta, tau, results);
	end = time(NULL);
	//outputResult<string>(results, stringSet);
	cout << "QGra-STR: " << results.size()<< " " << end-begin << endl;

	//fclose(fp);
	delete tokenizer;
	delete qgramScheme;
	delete partitionNEDScheme;
	delete setScheme1;
	delete setScheme2;
	delete setScheme3;
	delete setScheme4;
}
/*
Tokenizer* tokenizer1 = new StandardTokenizer();
string st1 = "0_0_0_0_0_";
string st2 = "abc_0_def_xyz_mn_";
vector<string> token1, token2;
tokenizer1->tokenize(st1, token1);
tokenizer1->tokenize(st2, token2);
Verification::fuzzyJaccard(token1, token2, 0.85, 0.8);
*/
