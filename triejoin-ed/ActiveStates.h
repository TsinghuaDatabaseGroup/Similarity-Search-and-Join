#pragma once

#include <cassert>
#include <vector>
#include <string>
#include "RadixTrie.h"
#include "common/type.h"

using namespace std;

typedef HashMap<VTrieNode, int, VTrieNodeHash> StateMap;
typedef HashMap<VTrieNode, int, VTrieNodeHash>::iterator StateMapIT;

class ActiveNode
{
public:
	VTrieNode vnode;
	unsigned char ed;
	ActiveNode(RadixTrieNode* node, int pos, unsigned char ed):vnode(node,pos),ed(ed){}
	ActiveNode(const VTrieNode& _vnode, unsigned char ed):vnode(_vnode),ed(ed){}
};

class ActiveStates
{
public:
	static StateMap visited;
public:
	static unsigned char EDTH;
	vector<ActiveNode> states;
	VTrieNode vnode;
public:
	ActiveStates(void) {}
	ActiveStates(const ActiveNode& anode){states.push_back(anode);}
	void updateFrom(const ActiveStates* prevActiveStates, char c);
	inline void add(const ActiveNode& activeNode);
	void getSimilarWordIds(vector<int>& ids);
	void getSimilarNode(vector<ActiveNode* >& nodes);
	~ActiveStates(void) {}
};
inline void ActiveStates::add(const ActiveNode& activeNode)
{
	//if (tnode->maxLength+ActiveStates::EDTH<activeNode.node->minLength || activeNode.node->maxLength+ActiveStates::EDTH<tnode->minLength)
	//	return ;
//	if (tnode->minId == activeNode.node->minId && tnode->minId == tnode->maxId && (activeNode.node->depth<tnode->depth||activeNode.node->minId == activeNode.node->maxId))
	//	return ;

	// prune leaf node
	if (activeNode.vnode.isLeafNode()
		|| (activeNode.vnode.node == vnode.node && (activeNode.vnode.pos < vnode.pos || activeNode.vnode.count1()))
		|| vnode.getMaxLength()+ActiveStates::EDTH<activeNode.vnode.getMinLength() || activeNode.vnode.getMaxLength()+ActiveStates::EDTH<vnode.getMinLength()
		)
		return;

	StateMapIT it = visited.find(activeNode.vnode);
	if (it == visited.end())
	{
		visited.insert(StateMap::value_type(activeNode.vnode, states.size()));
		states.push_back(activeNode);
	}
	else
	{
		if (states[it->second].ed > activeNode.ed)
		{
			states[it->second].ed = activeNode.ed;
		}
	}
}
