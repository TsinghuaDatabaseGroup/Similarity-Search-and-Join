/*********************************************************************
*                                                                    *
* Copyright (c) 1997,1998, 1999                                      *
* Multimedia DB Group and DEIS - CSITE-CNR,                          *
* University of Bologna, Bologna, ITALY.                             *
*                                                                    *
* All Rights Reserved.                                               *
*                                                                    *
* Permission to use, copy, and distribute this software and its      *
* documentation for NON-COMMERCIAL purposes and without fee is       *
* hereby granted provided  that this copyright notice appears in     *
* all copies.                                                        *
*                                                                    *
* THE AUTHORS MAKE NO REPRESENTATIONS OR WARRANTIES ABOUT THE        *
* SUITABILITY OF THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING  *
* BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY,      *
* FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT. THE AUTHOR  *
* SHALL NOT BE LIABLE FOR ANY DAMAGES SUFFERED BY LICENSEE AS A      *
* RESULT OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS    *
* DERIVATIVES.                                                       *
*                                                                    *
*********************************************************************/

#ifndef MTCURSOR_H
#define MTCURSOR_H

#ifndef MAXDOUBLE
#ifdef _WIN32   // for MAXDOUBLE
#include <float.h>
#define MAXDOUBLE DBL_MAX
#else
#include <values.h>
#endif
#endif
#include "GiST.h"
#include "list.h"

class MT;
class MTentry;
class MTpred;

class dst {	// this class is used in the NN-search algorithm for the priority queue
public:
	dst(): bound(0), d(MAXDOUBLE) {}
	dst(GiSTpath p, double LB, double dist): path(p), bound(LB), d(dist) {}

	double Bound() const { return bound; }
	double Dist() const { return d; }
	GiSTpath Path() const { return path; }
private:
	double bound;
	double d;
	GiSTpath path;
};

int comparedst(dst *a, dst *b);
int comparedst(dst& a, dst& b);

class MTcursor {
public:
	MTcursor(const MT& tree, const MTpred& query);
	MTentry *Next();
	BOOL IsReady() const;
	double Bound() const;
	~MTcursor();

	void FetchNode();

private:
	const MT& tree;
	orderedlist<MTentry *> results;
	orderedlist<dst *> queue;
	MTpred *query;
};

#endif
