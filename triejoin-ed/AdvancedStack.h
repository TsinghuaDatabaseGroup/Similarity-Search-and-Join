#pragma once
#include <stack>
#include <cassert>
class ActiveStates;

class AdvancedStack
{
	static const int MAX_STRING = 1022;
private:
	int ptop;
	ActiveStates* activeStatesStack[MAX_STRING+2];
public:
	AdvancedStack(void):ptop(0){}
	ActiveStates* top() {return activeStatesStack[ptop-1];}
	ActiveStates* pop() {return activeStatesStack[--ptop];}
	void push(ActiveStates* pActiveStates) { activeStatesStack[ptop++]=pActiveStates;}
	bool empty() {return ptop==0;}
	unsigned int size() { return ptop; }
	ActiveStates* at(int k) { assert(k<ptop);return activeStatesStack[k];}
	~AdvancedStack(void);
};
