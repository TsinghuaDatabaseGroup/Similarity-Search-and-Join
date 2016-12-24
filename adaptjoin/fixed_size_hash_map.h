#pragma once
#include <cassert>
#include <cstring>
#include <iostream>
using namespace std;

struct HashEntry
{
	int current_id;
	int count;
};

class FixedSizeHashMap
{
private:
	HashEntry* object_id_to_count;
	int fixed_size;
	int alive_id;
public:
	FixedSizeHashMap(int fixed_size)
	{
		object_id_to_count = NULL;
		Resize(fixed_size);
	}
	FixedSizeHashMap():object_id_to_count(NULL), fixed_size(0){}
	~FixedSizeHashMap(void)
	{
		if (object_id_to_count != NULL)
			delete[] object_id_to_count;
	}
	void Resize(int new_size)
	{
		if (object_id_to_count != NULL)
			delete[] object_id_to_count;
		fixed_size = new_size;
		alive_id = 1;
		object_id_to_count = new HashEntry[fixed_size];
		memset(object_id_to_count, 0, fixed_size*sizeof(HashEntry));
	}
	int Count(int object_id)
	{
		assert(object_id<fixed_size && object_id >=0);
		return object_id_to_count[object_id].current_id == alive_id ? object_id_to_count[object_id].count : 0;
	}
	void PlusOne(int object_id)
	{
		assert(object_id<fixed_size && object_id >=0);
		if (object_id_to_count[object_id].current_id != alive_id)
		{
			object_id_to_count[object_id].current_id = alive_id;
			object_id_to_count[object_id].count = 1;
		}
		else
			++object_id_to_count[object_id].count;
	}
	void Remove(int object_id)
	{
        assert(object_id<fixed_size && object_id >=0);
		object_id_to_count[object_id].current_id = 0;
	}
	void clear()
	{
		++alive_id;
	}
};
