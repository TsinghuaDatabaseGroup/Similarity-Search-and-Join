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

#ifndef MTNODE_H
#define MTNODE_H

#include "MTentry.h"

class MTquery;

class MTnode : public GiSTnode {
//	long wasted_space[100];	// only for debug purposes
public:
	// constructors, destructors, etc.
	MTnode(): GiSTnode(), obj(NULL) { };

	MTnode(const MTnode& node): GiSTnode(node), obj(node.obj) { };

	GiSTobjid IsA() const { return MTNODE_CLASS; }

	GiSTobject *Copy() const { return new MTnode(*this); }

	GiSTobject *NCopy();	// copy the node invalidating its entries (useful in Split)

	void Print(ostream& os) const;

	// search methods
	GiSTpage SearchMinPenalty(const GiSTentry& entry) const;	// overload of virtual method
	GiSTlist<MTentry *> RangeSearch(const MTquery& query);	// range search optimized for distance computations

	// insertion and deletion
	void InsertBefore(const GiSTentry& entry, int index);	// overload of virtual method in order to insert the entry in the right place

	// two of the basic GiST methods 
	GiSTnode *PickSplit();
	GiSTentry* Union() const;

	// support functions for methods above
	MTnode *PromotePart();	// object promotion
	MTnode *PromoteVote();	// object confirmed promotion
	int *PickCandidates();	// pick a sample of children objects
	void Split(MTnode *node, int *leftvec, int *rightvec, int *leftdeletes, int *rightdeletes);	// perform the split between the parents

	// used (in split and) in deletion
	int IsUnderFull(const GiSTstore &store) const;

	// required support methods
	GiSTentry *CreateEntry() const { return new MTentry; }
	int FixedLength() const { return sizeofEntry(); }

	void InvalidateEntry(BOOL isNew);	// invalidate the distance from its parent
	void InvalidateEntries();	// invalidate the distance of its children
	void Order();	// order the children with respect to the distance from their parent
	void mMRadius(MTentry *e) const;	// compute the min & MAX radii for this node storing them in e
	MTentry *Entry() const;	// retrieve the entry representing this node
	double distance(int i) const;	// useful to avoid computation of known distances

	Object *obj;	// object copied from the node's entry, useful for Union()
};

#endif
