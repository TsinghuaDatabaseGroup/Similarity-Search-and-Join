#pragma once
#include <queue>
#include <iostream>
#include <iomanip>
#include "Utility/GlobFunc.h"
#include "Tokenizer/Tokenizer.h"
#include "SignatureIndex.h"
#include "TokenSetSigScheme/TokenSetSigScheme.h"
using namespace std;

struct Result{
	int id1, id2;
	double sim;
	Result(uint id1, uint id2, double sim): id1(id1), id2(id2), sim(sim) {}
};

template <class T>
class FastJoin
{
private:
	TokenSetSigScheme<T>* _tokenSetSigScheme;
	Tokenizer* _tokenizer;
private:
	void _getTokenSets(const vector<string>& stringSet, vector<vector<string> >& tokenSets);
	void _filter(const vector<T>& signatures, const SignatureIndex<T, uint>& indexer, vector<uint>& candidateIds);
	void _verify(int tokenSetId, const vector<string>& tokenSet, const vector<uint>& candidateIds, const vector<vector<string> >& tokenSets, vector<Result>& results);
public:
	FastJoin(TokenSetSigScheme<T>* tokenSetSigScheme, Tokenizer* tokenizer) { _tokenSetSigScheme = tokenSetSigScheme; _tokenizer = tokenizer;}
	~FastJoin(void) {}
public:
	void setTokenSetSigScheme(TokenSetSigScheme<T>* tokenSetSigScheme) {_tokenSetSigScheme =tokenSetSigScheme;}
	void join(vector<string> & stringSet, double delta, double tau, vector<Result>& results);
	void join(vector<string> & stringSet1, vector<string> & stringSet2, double delta, double tau, vector<Result>& results);
	void outputResult(const vector<string>& stringSet1, const vector<string>& stringSet2, const vector<Result>& results, uint begin=0);
};

template<class T>
void FastJoin<T>::outputResult(const vector<string>& stringSet1, const vector<string>& stringSet2, const vector<Result>& results, uint begin)
{
	for (uint i=begin; i<results.size(); ++i)
	{
		const Result& result = results[i];
		cout << result.sim << " " << result.id1 << " " << result.id2 << endl;
		cout << stringSet1[result.id1] << endl;
		cout << stringSet2[result.id2] << endl;
		cout << endl;
	}
}


template<class T>
void FastJoin<T>::_filter(const vector<T>& signatures, const SignatureIndex<T, uint>& indexer, vector<uint>& candidateIds)				
{	
	unordered_set<uint> uniqueIds;
	for (uint i=0; i<signatures.size(); ++i)
	{
		if (indexer.containSignature(signatures[i]))
		{
			const vector<uint>& ids = indexer.getSignatureIndexList(signatures[i]);
			for (uint j=0; j<ids.size(); ++j)
			{
				uint id = ids[j];
				if (!uniqueIds.count(id))
					uniqueIds.insert(id);
			}
		}
	}
	for (unordered_set<uint>::iterator it = uniqueIds.begin(); it!=uniqueIds.end(); ++it)
		candidateIds.push_back(*it);
}
template <class T>
void FastJoin<T>::_verify(int tokenSetId, const vector<string>& tokenSet, const vector<uint>& candidateIds, const vector<vector<string> >& tokenSets, vector<Result>& results)
{
	for (uint i=0; i<candidateIds.size(); ++i)
	{
		uint candidateId = candidateIds[i];
		const vector<string>& tokenSetCandidate = tokenSets[candidateId];
		double sim = _tokenSetSigScheme->similarity(tokenSet, tokenSetCandidate);
		if (sim>0)
			results.push_back(Result(tokenSetId, candidateId, sim));
	}
}

template <class T>
void FastJoin<T>::_getTokenSets(const vector<string>& stringSet, vector<vector<string> >& tokenSets)
{
	tokenSets.resize(stringSet.size());
	for (uint i=0; i<stringSet.size(); ++i)
	{
		_tokenizer->tokenize(stringSet[i], tokenSets[i]);
	}
}
template <class T>
void FastJoin<T>::join(vector<string> & stringSet, double delta, double tau, vector<Result>& results)
{
	vector<vector<string> > tokenSets;
	_getTokenSets(stringSet, tokenSets);	
	clock_t beginSignature = clock();
	_tokenSetSigScheme->build(tokenSets, delta, tau);
	clock_t endSignature = clock();
	double totalSignature = getMicroSecond(beginSignature, endSignature)/1000.0;
	
	double totalVerify = 0;
	clock_t beginJoin = clock();
	uint candidateNum = 0;
	SignatureIndex<T, uint> indexer;
	for (uint i=0; i<tokenSets.size(); ++i)
	{
		const vector<string>& tokenSet = tokenSets[i];
		vector<T> signatures;
		_tokenSetSigScheme->getQuerySig(tokenSet, signatures);

		vector<uint> candidateIds;
		_filter(signatures, indexer, candidateIds);
		candidateNum += candidateIds.size();

		int begin = results.size();
		clock_t beginVerify = clock();
		_verify(i, tokenSet, candidateIds, tokenSets, results);
		clock_t endVerify = clock();
		totalVerify += getMicroSecond(beginVerify, endVerify)/1000.0;

		outputResult(stringSet, stringSet, results, begin);
		signatures.clear();
		_tokenSetSigScheme->getDataSig(tokenSet, signatures);
		indexer.addSignatures(signatures, i);
	}
	clock_t endJoin = clock();
	double totalJoin = getMicroSecond(beginJoin, endJoin)/1000.0;
	double totalFilter = totalJoin-totalVerify;
        cerr << setiosflags(ios::left) << setw(20) << "Signature Time" << ": " << totalSignature << "s" << endl;
        cerr << setiosflags(ios::left) << setw(20) << "Candidate Time" << ": " << totalFilter << "s" << endl;
        cerr << setiosflags(ios::left) << setw(20) << "Verification Time"  << ": " << totalVerify << "s" << endl;
        cerr << setiosflags(ios::left) << setw(20) << "Total Join Time" << ": " << totalSignature+totalJoin << "s" << endl;
        cerr << setiosflags(ios::left) << setw(20) << "Candidates  Number" << ": " << candidateNum << endl;
        cerr << setiosflags(ios::left) << setw(20) << "Result Number" <<  ": " << results.size() << endl;
}

template <class T>
void FastJoin<T>::join(vector<string> & stringSet1, vector<string> & stringSet2, double delta, double tau, vector<Result>& results)
{
	int size1 = stringSet1.size(), size2 = stringSet2.size();
	vector<vector<string> > tokenSets1, tokenSets2;
	tokenSets1.reserve(size1+size2);
	tokenSets2.reserve(size2);
	_getTokenSets(stringSet1, tokenSets1);
	_getTokenSets(stringSet2, tokenSets2);
	for (uint i=0; i<tokenSets2.size(); ++i)
		tokenSets1.push_back(tokenSets2[i]);
	clock_t beginSignature = clock();
	_tokenSetSigScheme->build(tokenSets1, delta, tau);
	clock_t endSignature = clock();
	double totalSignature = getMicroSecond(beginSignature, endSignature)/1000.0;
	tokenSets1.erase(tokenSets1.begin()+size1, tokenSets1.end());

	double totalVerify = 0;
	clock_t beginJoin = clock();
	uint candidateNum = 0;
	SignatureIndex<T, uint> indexer;
	for (uint i=0; i<tokenSets2.size(); ++i)
	{
		const vector<string>& tokenSet = tokenSets2[i];
		vector<T> signatures;
		_tokenSetSigScheme->getDataSig(tokenSet, signatures);
		indexer.addSignatures(signatures, i);
	}
	
	for (uint i=0; i<tokenSets1.size(); ++i)
	{
		const vector<string>& tokenSet = tokenSets1[i];
		vector<T> signatures;
		_tokenSetSigScheme->getQuerySig(tokenSet, signatures);

		vector<uint> candidateIds;
		_filter(signatures, indexer, candidateIds);
		candidateNum += candidateIds.size();
		int begin = results.size();
		clock_t beginVerify = clock();
		_verify(i, tokenSet, candidateIds, tokenSets2, results);
		clock_t endVerify = clock();
		totalVerify += getMicroSecond(beginVerify, endVerify)/1000.0;
		outputResult(stringSet1, stringSet2, results, begin);
	}

	clock_t endJoin = clock();
	double totalJoin = getMicroSecond(beginJoin, endJoin)/1000.0;
	double totalFilter = totalJoin-totalVerify;
	cerr << setiosflags(ios::left) << setw(20) << "Signature Time" << ": " << totalSignature << "s" << endl;
	cerr << setiosflags(ios::left) << setw(20) << "Candidate Time" << ": " << totalFilter << "s" << endl;
	cerr << setiosflags(ios::left) << setw(20) << "Verification Time"  << ": " << totalVerify << "s" << endl;
	cerr << setiosflags(ios::left) << setw(20) << "Total Join Time" << ": " << totalSignature+totalJoin << "s" << endl;
	cerr << setiosflags(ios::left) << setw(20) << "Candidates  Number" << ": " << candidateNum << endl;
	cerr << setiosflags(ios::left) << setw(20) << "Result Number" <<  ": " << results.size() << endl;
}

