#pragma once
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cassert>

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
	int id;
	//bool visited;
	//char c;
	//short int depth;
	unsigned int length; // 32 bit denotes visited, 1~10 minLength  11~20 maxLength
	char* singleBranch;
public:
	RadixTrieNode(): child(NULL), next(NULL), id(-1),length(0){};
	~RadixTrieNode();
	int getId() {return id;}
	void setVisited(bool visited) {  length = visited ?  length | (1<<31) : length & 0x7fffffff ;}
	bool getVisited() const {return length&(1<<31) ? true:false;}
	char getChar(int pos) const {return singleBranch[pos];}
	RadixTrieNode* getChild() const {return child;}
	RadixTrieNode* getNext() const {return next;}
	int getMinLength() const {return length&0x3ff;}
	int getMaxLength() const {return (length>>10)&0x3ff;}
	void setMinLength(int minLength) {assert(minLength<=1000&&minLength>=1); length&=0xfffffc00; length |= minLength;}
	void setMaxLength(int maxLength) {assert(maxLength<=1000&&maxLength>=1); length&=0xfff003ff; length |= maxLength<<10;}
	int getNumOfNodes() const {return strlen(singleBranch);}
	bool count1() {if (singleBranch[getNumOfNodes()-1] == TERM_CHAR) assert(id!=-1); return id!=-1;}
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
	void setVisited(bool visited) {node->setVisited(visited);}
	bool getVisited() {return node->getVisited();}
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
	bool isLeafNode() const {return pos>=0 && node->getChar(pos) == RadixTrieNode::TERM_CHAR;}
	int getId() const { 	assert(node!=NULL); return isLeafNode() ? node->getId() : -1;	}
	int getMinLength() const {return node->getMinLength();}
	int getMaxLength() const {return node->getMaxLength();}
	bool count1() const {return node->count1();}
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
	void computeLength(RadixTrieNode* node, int depth);
public:
	RadixTrie(void);
	~RadixTrie(void);
	RadixTrieNode* insert(const char* _key, const int id);
	RadixTrieNode* searchKey(const char* _key);
	RadixTrieNode* getRoot() const {return root;}
	void computeLength();
/****** test Radix Trie ******/
private:
	void test(RadixTrieNode* node);
public:
	void test();

};
