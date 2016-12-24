#pragma once
#include <cassert>
#include "ActiveStates.h"
class ActiveStatesManage
{
	static const int MAX_STRING = 1022;
private:
	int top;
	ActiveStates activeStatesMemory[MAX_STRING+2];
public:
	ActiveStatesManage(void):top(0){}
	ActiveStates* newActiveStates() {	assert(top<=1000);return &activeStatesMemory[top++];	}
	/*ActiveStates* newActiveStates(const VTrieNode& node) {	
		assert(top<=1000);
		activeStatesMemory[top].vnode = node; 
		return &activeStatesMemory[top++];	
	}*/
	void deleteActiveStates(){	assert(top>0);activeStatesMemory[--top].states.clear();	}
	~ActiveStatesManage(void);
};
