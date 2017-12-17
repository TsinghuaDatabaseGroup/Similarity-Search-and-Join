/*
 * search.h
 *
 *  Created on: 2014-12-11
 *      Author: wangjin
 */

#ifndef SEARCH_H_
#define SEARCH_H_
#include "util.h"

void preprocess(const string & query, int k, bool tag);
void htopkSearch(const string& path, const string& query, int k);
void hsimSearch(const string& path, const string& query, int tau);

#endif /* SEARCH_H_ */
