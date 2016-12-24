#pragma once
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <vector>
#ifndef NULL
#define NULL 0
#endif
// a virtual radix trie node
using namespace std;

class RadixTrieNode {
private:
	friend class RadixTrie;
	//friend struct VTrieNode;
	RadixTrieNode* child;
	RadixTrieNode* next;
	unsigned int length; // 31 bit denotes S source, 30 bit denotes R source, 29 denotes visited, 1~10 minLength  11~20 maxLength
	unsigned int depth; // 1~10 minDepth  11~20 maxDepth
	vector<int> RIds, SIds;
	//bool visited;
	//char c;
	//short int depth;

	char* singleBranch;
public:
	RadixTrieNode(): child(NULL), next(NULL), length(0){};
	~RadixTrieNode();
	const vector<int>& getRIds() {return RIds;}
	const vector<int>& getSIds() {return SIds;}
	void setId(int id, int source)
	{
		if (source)	SIds.push_back(id);
		else	RIds.push_back(id);
	}
	void setSource(int source)
	{
			if (source) setFromS();
			else setFromR();
	}
	bool fromR() {return length&(1<<30) ? true:false;}
	bool fromS() {return length&(1<<31) ? true:false;}
	void setFromR() {length |= (1<<30);}
	void setFromS() {length |= (1<<31);}
	char getChar(int pos) const {return singleBranch[pos];}
	RadixTrieNode* getChild() const {return child;}
	RadixTrieNode* getNext() const {return next;}
	int getMinLength() const {return length&0x3ff;}
	int getMaxLength() const {return (length>>10)&0x3ff;}
	void setMinLength(int minLength) {assert(minLength<=1000&&minLength>=1); length&=0xfffffc00; length |= minLength;}
	void setMaxLength(int maxLength) {assert(maxLength<=1000&&maxLength>=1); length&=0xfff003ff; length |= maxLength<<10;}
	int getMinDepth() const {return depth&0x3ff;}
	int getMaxDepth() const {return (depth>>10)&0x3ff;}
	void setMinDepth(int minDepth) {assert(minDepth<=1000&&minDepth>=0); depth&=0xfffffc00; depth |= minDepth;}
	void setMaxDepth(int maxDepth) {assert(maxDepth<=1000&&maxDepth>=0); depth&=0xfff003ff; depth |= maxDepth<<10;}
	int getNumOfNodes() const {return strlen(singleBranch);}
	bool count1() {if (singleBranch[getNumOfNodes()-1] == TERM_CHAR) assert(RIds.size()>0 || SIds.size()>0); return RIds.size()+SIds.size()==1;}
	void setVisited(bool visited) {  length = visited ?  length | (1<<29) : length & 0xdfffffff ;}
	bool getVisited() const {return length&(1<<29) ? true:false;}
public:
	static const char TERM_CHAR = 1;
};

struct VTrieNode
{
	RadixTrieNode* node;
	int pos;
	VTrieNode():node(NULL), pos(-1) {}
	VTrieNode(RadixTrieNode* node, int pos):node(node),pos(pos){}
	VTrieNode(const VTrieNode& vnode):node(vnode.node),pos(vnode.pos){}
	bool operator==(const VTrieNode& vnode) const
	{
		return node == vnode.node && pos == vnode.pos;
	}
	const VTrieNode searchChild(char c) const;
	const VTrieNode searchVisitedChild(char c) const;
	bool fromR() {return node->fromR();}
	bool fromS() {return node->fromS();}
	void setFromR() {node->setFromR();}
	void setFromS() {node->setFromS();}

	char getChar() const { return node->getChar(pos);}
	const VTrieNode getChild() const
	{
		if (node->getChar(pos+1))
			return VTrieNode(node, pos+1);
		else
			return VTrieNode(node->getChild(), 0);
	}
	const VTrieNode getNext() const
	{
		if (pos==0)
			return VTrieNode(node->getNext(), 0);
		else
			return VTrieNode(NULL, -1);
	}
	bool isRLeafNode() const {return pos>=0 && node->getChar(pos) == RadixTrieNode::TERM_CHAR && node->fromR();}
	bool isSLeafNode() const {return pos>=0 && node->getChar(pos) == RadixTrieNode::TERM_CHAR && node->fromS();}
	bool isLeafNode() const {return pos>=0 && node->getChar(pos) == RadixTrieNode::TERM_CHAR;}
	const vector<int>& getRIds() const { 	assert(node!=NULL&&isRLeafNode()); return  node->getRIds();	}
	const vector<int>& getSIds() const { 	assert(node!=NULL&&isSLeafNode()); return  node->getSIds();	}
	int getMinLength() const {return node->getMinLength();}
	int getMaxLength() const {return node->getMaxLength();}
	int getDepth() const {if (pos == -1) assert(node->getMinDepth() == 0 && node->getMaxDepth() == 0); return pos == -1? 0: node->getMinDepth()+pos;}
	bool count1() const {return node->count1();}
	void setVisited(bool visited) {node->setVisited(visited);}
	bool getVisited() {return node->getVisited();}
};

class VTrieNodeHash
{
public:
	size_t operator()(const VTrieNode& vnode) const
	{
		return size_t(vnode.node) +vnode.pos;
		//return tr1::hash(vtNode.node +vtNode.pos);
	}
};

class RadixTrie
{
private:
	static const int MAX_STRING = 1022;
	static char key[MAX_STRING+2];
	RadixTrieNode* root;
	//VRadixTrieNode* searchVNode(const char *key);
private:
	void appendTERMCHAR(const char* _key); // // copy _key to key and append a terminate character to key
	void recursiveRemove(RadixTrieNode* node);
	void prepare(RadixTrieNode* node, int depth);
	void getDepthTrieNodes(const VTrieNode& vnode, int nowDepth, int totalDepth, vector<pair<VTrieNode, int> >& vnodes);
public:
	RadixTrie(void);
	~RadixTrie(void);
	RadixTrieNode* insert(const char* _key, const int id, int source); //0 R; 1 S
	RadixTrieNode* searchKey(const char* _key);
	RadixTrieNode* getRoot() const {return root;}
	void prepare();
	void getDepthTrieNodes(int depth, vector<pair<VTrieNode, int> >& vnodes);
/****** test Radix Trie ******/
private:
	void test(RadixTrieNode* node);
public:
	void test();

};
