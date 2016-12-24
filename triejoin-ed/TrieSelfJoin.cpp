#include "TrieSelfJoin.h"

long long resultNum;

void TrieSelfJoin::outputSimilarWords(vector<ActiveNode>& states, int RId, const vector<char*>& strings)
{
	assert(RId>=0);
	for (unsigned int j=0; j<states.size(); ++j)
	{
		int SId = states[j].vnode.getId();
		if (SId!=-1&&SId != RId)
		{
			if (RId > SId)
				swap(SId, RId);
			// printf("%d %d %d\n", states[j].ed, RId+1, SId+1);
			// printf("%s\n%s\n\n", strings[RId], strings[SId]);
		}
	}
}

void TrieSelfJoin::outputSimilarWordsOp(vector<ActiveNode>& states, int RId, const vector<char*>& strings)
{
	assert(RId>=0);
	for (unsigned int j=0; j<states.size(); ++j)
	{
		//assert(!states[j].vnode.isLeafNode());
		//if (states[j].vnode.getChild().node == NULL) continue;
		VTrieNode childNode = states[j].vnode.getChild();
		while (childNode.node != NULL
			&& childNode.getVisited())
		{
			if (childNode.isLeafNode())
			{
				int SId =childNode.getId();
				if (SId != RId)
				{
			resultNum++;
					int RIdtmp = RId;
					if (RId > SId)
						swap(RId, SId);
					// printf("%d %d %d\n", states[j].ed, RId+1, SId+1);
					// printf("%s\n%s\n\n", strings[RId], strings[SId]);
					RId = RIdtmp;
				}
				break;
			}
			childNode = childNode.getNext();
		}
	}
}
/*
void TrieSelfJoin::edjoin(const vector<string>& strings)
{
	RadixTrie* ptrie = new RadixTrie();
	for (unsigned i=0; i<strings.size(); ++i)
	{
		if (strings[i].size() > 1000)
			continue;
		ptrie->insert(strings[i].c_str(), i);
	}
	ptrie->test();

	ActiveStatesManage manger;
	ActiveStates* fatherStates = manger.newActiveStates();
	fatherStates->states.push_back(ActiveNode(ptrie->getRoot(),-1,0));
	fatherStates->vnode = VTrieNode(ptrie->getRoot(), -1);
	fatherStates->vnode.setVisited(true);


	VTrieNode child = fatherStates->vnode.getChild();
	AdvancedStack stk;
	stk.push(fatherStates);
	int n = 0;
	while (!stk.empty())
	{
		while(child.node)
		{
			ActiveStates* activeStates = manger.newActiveStates();
			//assert(stk.top()->tnode->depth+1 == child->depth);
			//assert(child.getVisited() == false);
			activeStates->updateFrom(stk.top(), child.getChar());
			assert(activeStates->vnode == child);
			if (child.isLeafNode())
			{
				outputSimilarWords(activeStates->states, child.getId(), strings);
				n++;
			}
			//			if (activeStates->states.size() == 0)
			//				break;
			for (int i=stk.size()-1,k=1; i>=0&&k<=ActiveStates::EDTH; --i, ++k)
			{
				stk.at(i)->states.push_back(ActiveNode(child,k));
			}
			stk.push(activeStates);
			child = child.getChild();
		}
		assert(!stk.empty());
		child = stk.pop()->vnode.getNext();
		manger.deleteActiveStates();
	}
	delete ptrie;
	assert(n == strings.size());
}
*/

void TrieSelfJoin::edjoin(const vector<char*>& strings, int ed)
{
	ActiveStates::EDTH = ed;
	RadixTrie* ptrie = new RadixTrie();
	for (unsigned i=0; i<strings.size(); ++i)
	{
		int len = strlen(strings[i]);
		if (len > 1000 || len==0)
			continue;
		ptrie->insert(strings[i], i);
	}
	//ptrie->test();

	//! important
	ptrie->computeLength();

	ActiveStatesManage manger;
	ActiveStates* fatherStates = manger.newActiveStates();
	fatherStates->states.push_back(ActiveNode(ptrie->getRoot(),-1,0));
	fatherStates->vnode = VTrieNode(ptrie->getRoot(), -1);
	fatherStates->vnode.setVisited(true);


	VTrieNode child = fatherStates->vnode.getChild();
	AdvancedStack stk;
	stk.push(fatherStates);
	//int n = 0;
	while (!stk.empty())
	{
		while(child.node)
		{
			if (child.isLeafNode())
			{
				if (!child.getVisited())
					assert(child.getChild().node == NULL);
				child.setVisited(true);
				outputSimilarWordsOp(stk.top()->states, child.getId(), strings);
				//++n;
				assert(child.getChild().node == NULL);
				child = child.getNext();
				continue;
			}
			ActiveStates* activeStates = manger.newActiveStates();
			//assert(stk.top()->tnode->depth+1 == child->depth);
			//assert(child.getVisited() == false);
			activeStates->updateFrom(stk.top(), child.getChar());
			assert(activeStates->vnode == child);

//			if (activeStates->states.size() == 0)
//				break;
			for (int i=stk.size()-1,k=1; i>=0&&k<=ActiveStates::EDTH; --i, ++k)
			{
				stk.at(i)->states.push_back(ActiveNode(child,k));
			}
			stk.push(activeStates);
			child = child.getChild();
		}
		assert(!stk.empty());
		child = stk.pop()->vnode.getNext();
		manger.deleteActiveStates();
	}
	delete ptrie;
	//assert(n == strings.size());
}


/*

	trie.root.visited = true;
	TrieNode* child = trie.root.child;
	while(child)
	{
		calculateSimilarStringPair(&trie.root, child->c);
		child = child->next;
	}*/

/*
int k = ActiveStates::EDTH;
for (unsigned int i=stk.size()-1; i>=0&&k; --i, --k)
{
StateMapIT it = ActiveStates::visited.find(stk.at(i)->tnode);
if (it != visited.end())
{

}
}
*/
