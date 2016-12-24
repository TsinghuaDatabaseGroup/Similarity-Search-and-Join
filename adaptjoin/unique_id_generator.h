#pragma once
#include <string>
#include "common/type.h"
using namespace std;

class UniqueIdGenerator
{
private:
	int next_id;
	HashMap<string, int> string_to_id;
public:
	UniqueIdGenerator(void):next_id(0) {}
	int GetUniqueId(const string& str);
	int getMaxId() {return next_id-1;}
	void clear() {string_to_id.clear();}
	~UniqueIdGenerator(void) {}
};


