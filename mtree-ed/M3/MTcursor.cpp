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

#include "MTcursor.h"
#include "MT.h"
#include "MTpredicate.h"

class MTnode;

inline int
sign(double x)
{
	if(x>0) return 1;
	if(x<0) return -1;
	return 0;
}

inline int comparedst(dst *a, dst *b) { return sign(a->Bound()-b->Bound()); }
inline int comparedst(dst& a, dst& b) { return sign(a.Bound()-b.Bound()); }
inline int compareentry(MTentry *a, MTentry *b) { return sign(a->maxradius()-b->maxradius()); }

MTcursor::MTcursor(const MT& tree, const MTpred& query): queue(comparedst), results(compareentry), tree(tree)
{
	GiSTpath path;
	dst *d;

	path.MakeRoot();
	d=new dst(path, 0, MAXDOUBLE);
	this->query=(MTpred *)query.Copy();
	queue.Insert(d);
}

BOOL
MTcursor::IsReady() const
{
	if(queue.IsEmpty()) return (!results.IsEmpty());
	else if(results.IsEmpty()) return FALSE;
	else return (queue.First()->Bound()>=results.First()->maxradius());
}

double
MTcursor::Bound() const
{
	if(queue.IsEmpty())
		if(results.IsEmpty()) return maxDist();
		else return(results.First()->maxradius());
	else if(results.IsEmpty()) return(queue.First()->Bound());
	else return MIN(queue.First()->Bound(), results.First()->maxradius());
}

void
MTcursor::FetchNode()
{
	if(queue.IsEmpty()) return;

	int i, iprev, inext;	// these are the previous and the next entries to be examined
	dst *d=queue.RemoveFirst();
	MTnode *node=(MTnode *)tree.ReadNode(d->Path());	// retrieve next node to be examined

//	cout << "Read node " << node->Path() << ": " << d->Bound() << endl;
	// search the first children to be examined
	for(unsigned int i=0; i<node->NumEntries(); i++) {	// for each entry in the current node
		MTentry *e=(MTentry *)((*node)[i].Ptr());
		double dist=query->distance(e->object());

		if(!e->IsLeaf()) {	// insert the child node in the priority queue
			dst *dnew;
			GiSTpath path=node->Path();
			double dmin=dist-e->maxradius(), dmax=dist+e->maxradius();	// these are lower- and upper-bounds on the distances of the descendants of the entry from the query object

			if(dmin<0) dmin=0;
			path.MakeChild(e->Ptr());
			dnew=new dst(path, dmin, dist);
			queue.Insert(dnew);
		}
		else {	// insert the entry in the result list
			MTentry *key=(MTentry *)e->Copy();

			key->setminradius(0);
			key->setmaxradius(dist);	// we insert the actual distance from the query object as the key radius
			results.Insert(key);
//			cout << "Inserted (" << dist << ") into list\n";
		}
	}
	delete node;	// delete examined node
	delete d;
}

MTentry *
MTcursor::Next()
{
	while(!IsReady()) FetchNode();
	if(!results.IsEmpty()) return results.RemoveFirst();
	else return NULL;
}

MTcursor::~MTcursor()
{
	delete query;
	while(!results.IsEmpty()) delete results.RemoveFirst();
	while(!queue.IsEmpty()) delete queue.RemoveFirst();
}
