#include "ActiveStates.h"
StateMap ActiveStates::visited;
unsigned char ActiveStates::EDTH = 2;



void ActiveStates::updateFrom(const ActiveStates* prevActiveStates, char c)
{
	assert(states.size() == 0);
	visited.clear();
	const vector<ActiveNode>& prevStates = prevActiveStates->states;
	assert(prevActiveStates->vnode.node != NULL);

	vnode = prevActiveStates->vnode.searchChild(c);
	assert(vnode.node != NULL);
	vnode.setVisited(true);
	for (unsigned int i=0; i<prevStates.size(); ++i)
	{
		unsigned char prevEd = prevStates[i].ed;
		const VTrieNode& prevNode = prevStates[i].vnode;

		StateMapIT it = visited.find(prevNode);

		unsigned char minEd = prevEd;
		if (it != visited.end()&& states[it->second].ed < prevEd) 
			minEd = states[it->second].ed;

		if (prevEd+1 <= EDTH)
		{
			add(ActiveNode(prevNode,prevEd+1));
		}
		VTrieNode prevNodeChild = prevNode.searchChild(c);
		if (prevNodeChild.node != NULL
			&& prevNodeChild.getVisited() 
			&& (prevNodeChild.node != vnode.node || prevNodeChild.pos<=vnode.pos))
		{
			add(ActiveNode(prevNodeChild,prevEd));
		}
		minEd+=1;
		if (minEd<=EDTH)
		{
			prevNodeChild = prevNode.getChild();
			// S is the first, R is the second
			while(prevNodeChild.node != NULL
				&&prevNodeChild.getVisited()
				&& (prevNodeChild.node != vnode.node || prevNodeChild.pos<=vnode.pos))
			{
				add(ActiveNode(prevNodeChild,minEd));
				prevNodeChild = prevNodeChild.getNext();
			}
		}
	}
}
/*
void ActiveStates::getSimilarWordIds(vector<int>& ids)
{
	for (unsigned int i=0; i<states.size(); ++i)
	{
		//char buf[256];
		if (states[i].node->id!=-1)
			ids.push_back(states[i].node->id);
	}
}
void ActiveStates::getSimilarNode(vector<ActiveNode*>& nodes)
{
	for (unsigned int i=0; i<states.size(); ++i)
	{
		//char buf[256];
		assert(states[i].node->visited);
		if (states[i].node->id!=-1&&states[i].node != tnode)
			nodes.push_back(&states[i]);
	}
}
*/
