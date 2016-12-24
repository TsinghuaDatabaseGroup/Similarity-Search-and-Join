#include "RadixTrie.h"

char RadixTrie::key[MAX_STRING+2];

const VTrieNode VTrieNode::searchChild(char c) const
{
	if (node->getChar(pos+1))
		return node->getChar(pos+1) == c ? VTrieNode(node, pos+1) : VTrieNode(NULL, -1);
	RadixTrieNode* childNode = node->getChild();
	while (childNode!=NULL)
	{
		if (childNode->getChar(0) == c)
			return VTrieNode(childNode, 0);
		childNode = childNode->getNext();
	}
	return VTrieNode(NULL, -1);
}
RadixTrieNode::~RadixTrieNode()
{
	if (singleBranch != NULL)
	{
		free(singleBranch);
		singleBranch = NULL;
	}
	child = next = NULL;
}
RadixTrie::RadixTrie(void)
{
	root = new RadixTrieNode();
	root->singleBranch = (char*)malloc(1);
	root->singleBranch[0] = 0;
}

RadixTrie::~RadixTrie(void)
{
	recursiveRemove(root);
}

void RadixTrie::recursiveRemove(RadixTrieNode* node)
{
	RadixTrieNode* childNode = node->child;
	while (childNode!=NULL)
	{
		RadixTrieNode* nextChildNode = childNode->next;
		recursiveRemove(childNode);
		childNode = nextChildNode;
	}
	delete node;
}


void RadixTrie::appendTERMCHAR(const char* _key)
{
	unsigned i=0;
	while (i<MAX_STRING&&_key[i]!=0) { key[i] = _key[i]; i++; }
	key[i] = RadixTrieNode::TERM_CHAR;
	key[i+1] = 0;
}

RadixTrieNode* RadixTrie::insert(const char *_key, const int id)
{
	appendTERMCHAR(_key);
	RadixTrieNode* lastNode = NULL;
	RadixTrieNode* curNode = root;
	int posInCurNode = 0;

	RadixTrieNode* insertNode = NULL;
	const char *p = key;
	bool isParent = true;
	while (*p)
	{
		char curChar = curNode->singleBranch[posInCurNode];
		// Case 1: find the branch whose first character is *p
		if (curChar == 0)
		{
			lastNode = curNode;
			curNode = curNode->child;
			isParent = true;
			while (curNode!=NULL)
			{
				if (curNode->singleBranch[0] == *p)
				{
					++p;
					posInCurNode = 1;
					break;
				}
				lastNode = curNode;
				curNode = curNode->next;
				isParent = false;
			}
			// Case 1.1 : none of branch has the same first character with *p
			if (curNode == NULL)
			{
				int keyRestLen = strlen(p);
				char* keyRest = (char*)malloc(keyRestLen+1);
				memcpy(keyRest, p, keyRestLen+1);
				assert(keyRest[keyRestLen] == 0);

				RadixTrieNode* newChildNode = new RadixTrieNode();
				newChildNode->singleBranch = keyRest;
				if (isParent)
					assert(lastNode->child == curNode);
				else
					assert(lastNode->next == curNode);
				isParent ? lastNode->child = newChildNode : lastNode->next = newChildNode;
				insertNode = newChildNode;
				break;
			}
			// Case 1.2 : curNode has the same first character with *p
		}
		else // Case 2: continue to search the branch curNode
		{
			assert(posInCurNode>0);
			if (curChar != *p) // Case 2.1: key and curNode has the different character
			{
				int keyRestLen = strlen(p);
				assert(keyRestLen>=1);
				char* keyRest = (char*)malloc(keyRestLen+1);
				memcpy(keyRest, p, keyRestLen+1);
				assert(keyRest[keyRestLen] == 0);

				int singleBranchRestLen = strlen(curNode->singleBranch+posInCurNode);
				assert(singleBranchRestLen>=1);
				char*  singleBranchRest = (char*)malloc(singleBranchRestLen+1);
				memcpy(singleBranchRest, curNode->singleBranch+posInCurNode, singleBranchRestLen+1);
				assert(singleBranchRest[singleBranchRestLen] == 0);

				char* singleBrachFirstPart = curNode->singleBranch;
				singleBrachFirstPart = (char*)realloc(singleBrachFirstPart, posInCurNode+1);
				singleBrachFirstPart[posInCurNode] = 0;

				curNode->singleBranch = singleBranchRest;

				RadixTrieNode* newChildNode = new RadixTrieNode();
				newChildNode->singleBranch = keyRest;

				RadixTrieNode* newParentNode = new RadixTrieNode();
				newParentNode->singleBranch =singleBrachFirstPart;
				if (isParent)
					assert(lastNode->child == curNode);
				else
					assert(lastNode->next == curNode);
				isParent ? lastNode->child = newParentNode : lastNode->next = newParentNode;
				newParentNode->child = curNode;
				newParentNode->next = curNode->next;
				curNode->next = newChildNode;
				insertNode = newChildNode;
				break;
			}
			else // Case 2.2: key and curNode has the same character
			{
				++posInCurNode;
				++p;
			}
		}
	}
	if (insertNode == NULL)
	{
		assert(curNode->singleBranch[posInCurNode-1] == RadixTrieNode::TERM_CHAR);
		insertNode = curNode;
	}
	if (insertNode->id ==-1)
		insertNode->id = id;
	return insertNode;
}

RadixTrieNode* RadixTrie::searchKey(const char *_key)
{
	appendTERMCHAR(_key);

	RadixTrieNode* curNode = root;
	int posInCurNode = 0;

	const char* p = key;
	while (*p)
	{
		char curChar = curNode->singleBranch[posInCurNode];
		if (curChar == 0)
		{
			curNode = curNode->child;
			while (curNode!=NULL)
			{
				if (curNode->singleBranch[0] == *p)
				{
					++p;
					posInCurNode = 1;
					break;
				}
				curNode = curNode->next;
			}
		}
		else
		{
			assert(posInCurNode>0);
			if (curChar != *p)
				return NULL;
			++p;
			++posInCurNode;
		}
	}
	assert(*p==0);
	return curNode;
}

void RadixTrie::computeLength()
{
	computeLength(root, 0);
}

void RadixTrie::computeLength(RadixTrieNode* node, int depth)
{
	assert(node != NULL);
	int minLength=1<<30, maxLength = -1;
	int newDepth = depth+node->getNumOfNodes();

	RadixTrieNode* childNode = node->child;
	if (childNode == NULL)
	{
		minLength = maxLength = newDepth;
	}
	while (childNode != NULL)
	{
		computeLength(childNode, newDepth);
		if (childNode->getMinLength() < minLength)
			minLength = childNode->getMinLength();
		if (childNode->getMaxLength() > maxLength)
			maxLength = childNode->getMaxLength();
		childNode = childNode->next;
	}
	node->setMinLength(minLength);
	node->setMaxLength(maxLength);
}
void RadixTrie::test(RadixTrieNode* node)
{
	assert(node != NULL);
	static int nc = 0;
	static int space = 0;
	static int nn = 0;
	bool flag = (node == root);

	++nc;
	unsigned l = 0;
	for (l=0; node->singleBranch[l]; ++l)
	{
		if (node->singleBranch[l] == RadixTrieNode::TERM_CHAR)
		{
			assert(node->singleBranch[l+1] == 0);
		}
	}
	if (node->id != -1)
	{
		assert(l>=1 && node->singleBranch[l-1] == RadixTrieNode::TERM_CHAR);
		++nn;
	}
	if (node->singleBranch[l-1] == RadixTrieNode::TERM_CHAR)
		assert(node->child == NULL);
	assert(l == strlen(node->singleBranch));
	assert(sizeof(*node) == 20);
	assert(sizeof(RadixTrieNode) == 20);
	space += sizeof(*node)+l;
	node = node->child;
	if (node != NULL && !flag)
		assert(node->next != NULL);
	while (node != NULL)
	{
		test(node);
		node = node->next;
	}
	if (flag)
	{
		printf("Num of Node : %d\n", nc);
		printf("Num of Leaf :  %d\n", nn);
		printf("Space Size :  %d\n", space);
	}
}
void RadixTrie::test()
{
	test(root);
}
