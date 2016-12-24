#include "BiTrieSelfJoin.h"

long long resultNum;

void BiTrieSelfJoin::outputResult(const vector<int>& RIds, const vector<int>& SIds, const vector<char*>& strings, int edth)
{
	assert(RIds.size());
	Verification verify(edth);
	for (unsigned int i=0; i<RIds.size(); ++i)
	{
		int RId = RIds[i];
		for (unsigned int j=0; j<SIds.size(); ++j)
		{
			int SId =SIds[j];
			if (RId == SId)
				continue;
			int RIdtmp = RId;
			if (RId > SId)
				swap(SId, RId);
			int ved = verify.verifyEd(strings[RId], strings[SId]);
			if (ved >=0)
			{
				pair<int,int> pid(RId,SId);
				if (!printed.count(pid))
				{
					resultNum++;
					// printf("%d %d %d\n", ved, RId+1, SId+1);
					// printf("%s\n%s\n\n", strings[RId], strings[SId]);
					printed.insert(pid);
				}
			}
			RId = RIdtmp;
		}
	}
}
void BiTrieSelfJoin::similarWords(vector<ActiveNode>& states, VTrieNode& prefixNode, int edth, vector<int>& SIds)
{
//const vector<int>& RIds
	//vector<ActiveNode>& states = activeStates->states;
	HashSet<RadixTrieNode*> visitedNodes;
	int d1 = prefixNode.getDepth();
	for (int i=states.size()-1; i>=0; --i)
	{
		stack<RadixTrieNode*> stk;

		if (!visitedNodes.count(states[i].vnode.node))
		{
			//visitedNodes.insert(states[i].vnode.node);
			stk.push(states[i].vnode.node);
		}
		int d2 = states[i].vnode.getDepth();
		int left = (d1+d2+states[i].ed)/2+1;
		int right = (d1+d2+2+2*edth-states[i].ed)/2+1;
		while (!stk.empty())
		{
			RadixTrieNode* node = stk.top();
			stk.pop();
			assert(node->getVisited());
			const vector<int>& sids = node->getSIds();
			if (sids.size()
				&&node->getMaxDepth()<=right&&node->getMaxDepth()>=left
				&&!visitedNodes.count(node))
			{
				visitedNodes.insert(node);
				for (unsigned j=0; j<sids.size(); ++j)
				{
					SIds.push_back(sids[j]);
				}
			}

			// !op
			RadixTrieNode* childNode = node->getChild();
			while (childNode!=NULL
				&& childNode->getVisited())
			{
				assert(depth == childNode->getMinDepth());
				if (childNode->getMaxDepth() <= right)
				{
					stk.push(childNode);
				}
				childNode = childNode->getNext();
			}
		}
	}


//special case for prefixNode
	stack<RadixTrieNode*> stk;

	//visitedNodes.insert(states[i].vnode.node);
	stk.push(prefixNode.node);

	int d = prefixNode.getDepth();
	int left = d+1;
	int right = (2*d+2+edth)/2+1;
	while (!stk.empty())
	{
		RadixTrieNode* node = stk.top();
		stk.pop();
		const vector<int>& sids = node->getSIds();
		if (sids.size()
			&&node->getMaxDepth()<=right&&node->getMaxDepth()>=left
			&&!visitedNodes.count(node))
		{
			visitedNodes.insert(node);
			for (unsigned j=0; j<sids.size(); ++j)
			{
				SIds.push_back(sids[j]);
			}
		}
		// !op
		RadixTrieNode* childNode = node->getChild();
		while (childNode!=NULL)
		{
			assert(depth == childNode->getMinDepth());
			if (childNode->getMaxDepth() <= right)
			{
				stk.push(childNode);
			}
			childNode = childNode->getNext();
		}
	}
	//if (sids.size() == 1)
	//	return;


}

void BiTrieSelfJoin::edjoinPrefix(RadixTrie* ptrie, const vector<char*>& strings, int edth)
{
	ActiveStatesManage manager;
	ActiveStates* fatherStates = manager.newActiveStates();
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
			if (child.isRLeafNode())
			{
				if (!child.getVisited())
					assert(child.getChild().node == NULL);
				child.setVisited(true);
				vector<int> SIds;
				assert(stk.top()->vnode.getDepth()+1==child.getDepth());
				similarWords(stk.top()->states, stk.top()->vnode, edth, SIds);
				outputResult(child.getRIds(), SIds, strings, edth);
				/*SIds.clear();
				for (int i=stk.size()-1,k=0; i>=0&&k<=edth/2; --i, ++k)
				{
					VTrieNode vnode = stk.at(i)->vnode.searchChild(RadixTrieNode::TERM_CHAR);
					if (k == 0) assert(vnode == child);
					if (vnode.node!=NULL && vnode.isRLeafNode())
					{
						outputResult(child.getRIds(), vnode.getRIds(), strings, edth);
					}
				}*/
				++n;
				assert(child.getChild().node == NULL);
				child = child.getNext();
				continue;
			}
			ActiveStates* activeStates = manager.newActiveStates();
			activeStates->updateFrom(stk.top(), child.getChar());
			assert(activeStates->vnode == child);
			for (int i=stk.size()-1,k=1; i>=0&&k<=ActiveStates::EDTH; --i, ++k)
			{
				stk.at(i)->states.push_back(ActiveNode(child,k));
			}
			stk.push(activeStates);
			child = child.getChild();
		}
		assert(!stk.empty());

		child = stk.pop()->vnode.getNext();
		manager.deleteActiveStates();
	}
	//assert(n == stringsR.size());
}

char* BiTrieSelfJoin::depInfo(const vector<char*>& strings)
{
	char* info = (char*)malloc(strings.size());
	HashSet<string> dep;
	memset(info, 0, strings.size());
	for (unsigned i=0; i<strings.size(); ++i)
	{
		if (dep.count(string(strings[i])))
			info[i] = 1;
		else
			dep.insert(string(strings[i]));
	}
	return info;
}
void BiTrieSelfJoin::edjoin(const vector<char*>& strings, int edth)
{
	char* dep = depInfo(strings);


	printed.clear();

	char buf[1024];
	RadixTrie* ptrie = new RadixTrie();
	// S is the first, then R is the second

	for (unsigned i=0; i<strings.size(); ++i)
	{
		int len = strlen(strings[i]);
		if (len > 1000 || len==0 ||dep[i])
			continue;
		unsigned blen = min((len+edth)/2,len);
		memcpy(buf, strings[i], blen+1);
		buf[blen] = 0;
		ptrie->insert(buf, i, 1);
	}

	for (unsigned i=0; i<strings.size(); ++i)
	{
		int len = strlen(strings[i]);
		if (len > 1000 || len==0 ||dep[i])
			continue;
		unsigned blen = len/2;
		memcpy(buf, strings[i], blen+1);
		buf[blen] = 0;
		ptrie->insert(buf, i, 0);
	}
	//ptrie->test();

	//! important
	ptrie->prepare();
	ActiveStates::EDTH = edth/2;

	edjoinPrefix(ptrie, strings, edth);
	delete ptrie;

/**************************************************************/

	ptrie = new RadixTrie();
	for (unsigned i=0; i<strings.size(); ++i)
	{
		int len = strlen(strings[i]);
		if (len > 1000 || len==0 ||dep[i])
			continue;

		unsigned blen = min((len+1+edth)/2, len);
		for (unsigned j=0; j<blen; ++j)
			buf[j] = strings[i][len-j-1];
		buf[blen] = 0;
		ptrie->insert(buf, i, 1);
	}

	for (unsigned i=0; i<strings.size(); ++i)
	{
		int len = strlen(strings[i]);
		if (len > 1000 || len==0 ||dep[i])
			continue;
		unsigned blen = (len+1)/2;
		for (unsigned j=0; j<blen; ++j)
			buf[j] = strings[i][len-j-1];
		buf[blen] = 0;
		ptrie->insert(buf, i, 0);
	}
	free(dep);
	//ptrie->test();
	//! important
	ptrie->prepare();

	ActiveStates::EDTH = edth/2;
	edjoinPrefix(ptrie, strings, edth);

	delete ptrie;

}

