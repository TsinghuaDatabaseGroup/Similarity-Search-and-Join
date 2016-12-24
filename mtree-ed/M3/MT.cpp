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

#include <stdlib.h>
#include "list.h"
#include "MT.h"
#include "MTpredicate.h"
#include "MTcursor.h"

#ifdef _WIN32	// these functions are defined under UNIX
void srandom(int seed) { srand(seed); }
int random() { return rand(); }
#endif

TruePredicate truePredicate;

int
PickRandom(int from, int to)
{
	return (random()%(to-from))+from;
}

MTnode *
MT::ParentNode(MTnode *node)
{
	GiSTpath path=node->Path();
	MTnode *parentnode;

	path.MakeParent();
	parentnode=(MTnode *)ReadNode(path);
	return parentnode;	// parentnode should be destroyed by the caller
}

void
MT::CollectStats()
{
/*#if defined(_DEBUG)
	CMemoryState msOld, msNew, msDif;
	msOld.Checkpoint();
#endif */
	GiSTpath path;
	GiSTnode *node;

	path.MakeRoot();
	node=ReadNode(path);
	if(!node->IsLeaf()) {
		TruePredicate truePredicate;
		GiSTlist<GiSTentry*> list=node->Search(truePredicate);	// retrieve all the children
		double *areas, *radii, totarea=0;
		int *n, maxn=node->Level(), totn=1, i;

		areas=new double[maxn];
		radii=new double[maxn];
		n=new int[maxn];
		for(i=0; i<maxn; i++) {
			n[i]=0;
			areas[i]=0;
			radii[i]=0;
		}
		delete node;
		while(!list.IsEmpty()) {
			GiSTentry *e=list.RemoveFront();
			GiSTlist<GiSTentry*> newlist;

			path.MakeChild(e->Ptr());
			node=ReadNode(path);
			n[node->Level()]++;
			areas[node->Level()]+=((MTkey *)e->Key())->area();
			radii[node->Level()]+=((MTkey *)e->Key())->maxradius();
			if(!node->IsLeaf()) newlist=node->Search(truePredicate);	// recurse to next level
			while(!newlist.IsEmpty()) list.Append(newlist.RemoveFront());
			path.MakeParent();
			delete e;
			delete node;
		}
		// output the results
		cout << "Level:\tPages:\tRadius:\tArea:\n";
		for(i=maxn-1; i>=0; i--) {
			totn+=n[i];
			totarea+=areas[i];
			cout << i << ":\t" << n[i] << "\t" << radii[i]/n[i] << "\t" << areas[i]/n[i] << endl;
		}
		cout << "tot:\t" << totn << "\t" << totarea << endl;
		delete []areas;
		delete []radii;
		delete []n;
	}
	else delete node;
/* #if defined(_DEBUG)
	msNew.Checkpoint();
	msDif.Difference(msOld, msNew);
	msDif.DumpStatistics();
#endif */
}

BOOL
MT::CheckNode(GiSTpath path, MTentry *e)
{
	MTnode *node=(MTnode *)ReadNode(path);
	BOOL ret=TRUE;

	for(int i=0; (i<node->NumEntries())&&ret; i++) {
	    MTentry *child=(MTentry *)(*node)[i].Ptr();

	    if((e!=NULL)&&(((child->Key()->distance+child->maxradius())>e->maxradius())||(child->object().distance(e->object())!=child->Key()->distance))) {
			cout << "Error with entry " << child << "in " << node;
			ret=FALSE;
		}
		path.MakeChild(child->Ptr());
		if(!node->IsLeaf()) ret&=CheckNode(path, child);
		path.MakeParent();
	}
	delete node;
	return ret;
}

GiSTlist<MTentry *>
MT::RangeSearch(const MTquery& query)
{
	GiSTpath path;

	path.MakeRoot();
	MTnode *root=(MTnode *)ReadNode(path);
	GiSTlist<MTentry *> list=root->RangeSearch(query);

	delete root;
	return list;
}

MTentry **
MT::TopSearch(const TopQuery& query)
{
	GiSTpath path;	// path for retrieving root node
	MTnode *node;	// next node to be examined
	MTentry **results=new MTentry*[query.k];	// the results list (ordered for increasing distances)
	orderedlist<dst *> queue(comparedst);	// list for ordering the nodes
	SimpleQuery q(query.Pred(), maxDist(), TRUE);
	double *dists=new double[query.k];	// array containing the NN distances
	int i;

	for (i=0; i<query.k; i++) dists[i]=maxDist();	// initialization of the NN-distances array
	path.MakeRoot();
	node=(MTnode *)ReadNode(path);	// retrieve the root node
	while(node!=NULL) {	// examine current node
		double oldDist=q.Grade();

		// search the first children to be examined
//		cout << "Examining node " << node->Path() << endl;
		for(unsigned int i=0; i<node->NumEntries(); i++) {	// for each entry in the current node
			MTentry *e=(MTentry *)((*node)[i].Ptr());

			q.SetGrade(oldDist);
//			cout << "range=" << q.Radius() << ", oldDist=" << q.Grade() << ": Checking " << e;
			if(q.Consistent(*e)) {	// check if the entry is consistent with the query
//				cout << "grade=" << q.Grade() << endl;
				if(e->IsLeaf()) {	// object qualifies
					if(dists[query.k-1]<maxDist()) delete results[query.k-1];	// delete last element of the results list

					MTentry *key=(MTentry *)e->Copy();
					int j=0;

					key->setminradius(0);
					key->setmaxradius(q.Grade());	// we insert the actual distance from the query object as the key radius
					// insert dist in the results list (sorting for incr. distance)
					// could be improved using binary search
					while(dists[j]<q.Grade()) j++;
//					cout << "\tInserted (" << q.Grade() << "<" << dists[query.k-1] << ") into list in position " << j << endl;
					for(int l=query.k-1; l>j; l--) {	// shift up results array
						results[l]=results[l-1];
						dists[l]=dists[l-1];
					}
					results[j]=key;
					dists[j]=q.Grade();
					q.SetRadius(dists[query.k-1]);
				}
				else {	// we have to insert the child node in the priority queue
					GiSTpath path=node->Path();
					double dmin=q.Grade()-e->maxradius(), dmax=q.Grade()+e->maxradius();	// these are lower- and upper-bounds on the distances of the descendants of the entry from the query object

					if(dmin<0) dmin=0;
					// insert the node in the stack (sorting for decr. level and incr. LB on distance)
					// could be improved using binary search
					path.MakeChild(e->Ptr());
					dst *d=new dst(path, dmin, q.Grade());

					queue.Insert(d);
//					cout << "\tInserted (" << dmin << "<" << dists[query.k-1] << ") into list\n";
				}
			}
			else {
//				cout << "\tPruned (" << q.Grade() << ">" << dists[query.k-1] << ") from the list\n";
				// if we're in a "true" leaf node (i.e. not in the root) and last entry was pruned, we can safely prune all other entries too
//				if(e->IsLeaf()&&(!e->Node()->Path().IsRoot())&&(fabs(e->Key()->distance-oldDist)>q.Grade()))) break;
			}
		}
		// the next entry to be chosen is that whose distance from the parent is most similar to the distance between the parent and the query object
		delete node;	// delete examined node
		if(!queue.IsEmpty()) {	// retrieve next node to be examined
			dst *d=queue.RemoveFirst();

			if(d->Bound()>=dists[query.k-1]) {	// terminate the search
				while(!queue.IsEmpty())
					delete queue.RemoveFirst();
				node=NULL;
			}
			else {
				node=(MTnode *)ReadNode(d->Path());
				q.SetGrade(d->Dist());
			}
			delete d;
		}
		else node=NULL;	// terminate the search
	}
	delete []dists;
	return results;
}
