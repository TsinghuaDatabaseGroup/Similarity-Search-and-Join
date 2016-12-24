#ifndef _TYPE_H_
#define _TYPE_H_

#include <unordered_set>
#include <unordered_map>
#include <set>
#include <map>

template<typename T, typename H = std::hash<T>>
using HashSet = std::unordered_set<T, H>;

template<typename K, typename V, typename H = std::hash<K>>
using HashMap = std::unordered_map<K, V, H>;

template<typename T>
using Set = std::set<T>;

template<typename K, typename V>
using Map = std::map<K, V>;

#ifdef UNDEF_TYPES

#define dense_hash_map void
#define dense_hash_set void
#define unordered_map void
#define unordered_set void
#define map void
#define set void

#endif

#endif

