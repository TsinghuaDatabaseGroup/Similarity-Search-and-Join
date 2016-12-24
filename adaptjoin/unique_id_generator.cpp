#include "unique_id_generator.h"

int UniqueIdGenerator::GetUniqueId(const string& s)
{
	if (string_to_id.count(s))
		return string_to_id[s];
	else
	{
		string_to_id.insert(HashMap<string, int>::value_type(s,next_id));
		return next_id++;
	}
}
