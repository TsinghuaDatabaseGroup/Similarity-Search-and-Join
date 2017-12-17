#pragma once

#include <vector>
#include "Utility/TypeDefs.h"
using namespace std;
#ifndef _WIN32
#include <tr1/unordered_map>
using namespace tr1;
#else
#include <unordered_map>
using std::tr1::unordered_map;
#endif


template<class SigType, class IndexItem>
class SignatureIndex
{
	typedef unordered_map <SigType, vector<IndexItem> > SignatureIndexMap;
	SignatureIndexMap _signatureListMap;
public:
	SignatureIndex(void);
	bool containSignature(const SigType& signature) const {return _signatureListMap.count(signature)!=0;}
	const vector<IndexItem>& getSignatureIndexList(const SigType& signature) const {return _signatureListMap.find(signature)->second;}
	void addIndexItem(const SigType& signature, const IndexItem& item)
	{
		if (_signatureListMap.count(signature))
		{
			_signatureListMap[signature].push_back(item);
		}
		else
		{
			_signatureListMap.insert(make_pair(signature, vector<IndexItem>(1, item)));
		}
	}
	void addSignatures(const vector<SigType>& signatures, const IndexItem& item)
	{
		for (uint i=0; i<signatures.size(); ++i)
		{
			addIndexItem(signatures[i], item);
		}
	}
	~SignatureIndex(void);
};

template<class SigType, class IndexItem>
SignatureIndex<SigType, IndexItem>::SignatureIndex(void)
{
}
template<class SigType, class IndexItem>
SignatureIndex<SigType, IndexItem>::~SignatureIndex(void)
{
}
