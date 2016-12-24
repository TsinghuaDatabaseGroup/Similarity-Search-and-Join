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

#ifndef MTENTRY_H
#define MTENTRY_H

#include <string.h>
#include <stdio.h>
#ifndef MAXDOUBLE
#ifdef _WIN32	// for MAXDOUBLE
#include <float.h>
#define MAXDOUBLE DBL_MAX
#else
#include <values.h>
#endif
#endif
#include "GiST.h"
#include "MTobject.h"

class MTentry;

// MTkey is a covering region indicated by an object and its covering radii: obj, rmin, rmax
// We also throw in some standard geo operators to support various query predicates, etc.

class MTkey: public GiSTobject {
private:
	Object obj;	// object
	double rmin, rmax;	// covering radii
public:
friend class MTentry;
	// constructors, destructors, assignment, etc.
	MTkey(): rmin(0), rmax(MAXDOUBLE), distance(-maxDist()), splitted(FALSE), recomp(FALSE) { }

	MTkey(const MTkey& k): obj(k.obj), rmin(k.rmin), rmax(k.rmax), distance(k.distance), splitted(k.splitted), recomp(k.recomp) { }

	MTkey(Object& p_obj, const double p_rmin, const double p_rmax, const double p_distance=-maxDist(), const BOOL p_splitted=FALSE, const BOOL p_recomp=FALSE):
		obj(p_obj), rmin(p_rmin), rmax(p_rmax), distance(p_distance), splitted(p_splitted), recomp(p_recomp) { }

	GiSTobject *Copy() const { return new MTkey(*this); }

	~MTkey() { }
	GiSTobjid IsA() { return MTKEY_CLASS; }

	MTkey& operator = (const MTkey& k) {
		obj=k.obj;
		rmin=k.rmin;
		rmax=k.rmax;
		distance=k.distance;
		splitted=k.splitted;
		recomp=k.recomp;
		return *this;
	}

	// covering region property and comparison methods
	int operator==(const MTkey& k) {
		return ((!recomp)&&(!k.recomp)&&(obj==k.obj)&&(rmin==k.rmin)&&(rmax==k.rmax)&&(distance==k.distance));
	}

	int overlap (const MTkey& k) {
		double d=obj.distance(k.obj);

		return ((d+rmax>=k.rmin)&&(d+k.rmax>=rmin)&&(d<rmax+k.rmax));
	}

	int contained(const MTkey& k) {
		double d=obj.distance(k.obj);

		return ((d-rmax>=k.rmin)&&(d+rmax<=k.rmax));
	}

	int contains(const MTkey& k) {
		double d=obj.distance(k.obj);

		return ((d-k.rmax>=rmin)&&(d+k.rmax<=rmax));
	}

	double area() { return obj.area(rmax); }

	// expand this covering region to contain k
	MTkey *expand(const MTkey& k) {
		MTkey *result;
		double d=obj.distance(k.obj);

		result=new MTkey(obj, MIN(rmin, d), MAX(rmax, d));
		return result;
	}

	Object object() const { return obj; }
	double minradius() const { return rmin; }
	double maxradius() const { return rmax; }

	double distance;	// Here we store the distance between the entry and its parent, only for index performance, in order to minimize the number of distance computations;
	BOOL splitted;	// indicate that the node has been splitted
	BOOL recomp;	// indicate that the node has been splitted but that no Union has been performed (thus radii are in a potentially inconsistent state): this can happen after a split

	// I/O methods
#ifdef PRINTING_OBJECTS
	void Print(ostream& os) const {
		os << '"';
		os << "(" << obj << ", " << rmin << ", " << rmax << ")" << '"';
	}
#endif
};

#ifdef PRINTING_OBJECTS
inline ostream& operator<< (ostream& os, const MTkey& k) {
	k.Print(os);
	return os;
}
#endif

class MTpenalty: public GiSTpenalty {	// we have to create a sub-class in order to add the member value distance
public:
	MTpenalty(): GiSTpenalty(), distance(-maxDist()) { }

	MTpenalty(const double v, const double d): GiSTpenalty(v), distance(d) { }

	double distance;	// this is necessary since each penalty is defined by two values: the radius increase and the distance from the entry
};

int sizeofEntry();

class MTentry: public GiSTentry {	// this are the entries stored in each node
public:
	// constructors, destructors, etc.
	MTentry() { }

	MTentry(const MTkey& k, const GiSTpage p) {
		key=new MTkey(k);
		ptr=p;
	}

	MTentry(const MTentry& e): GiSTentry(e) {
		key=new MTkey(*(MTkey *)(e.key));
	}

	GiSTobjid IsA() const { return MTENTRY_CLASS; }

	GiSTobject *Copy() const { return new MTentry(*this); }

	void InitKey() { key=new MTkey; }

	int IsEqual(const GiSTobject& obj) const {
		if(obj.IsA()!=MTENTRY_CLASS) return 0;
		return ((*((MTkey *)key)==(*((MTkey *)(((const MTentry&) obj).key)))));
	}

	// cast key member to class MTkey *.  Prevents compiler warnings.
	MTkey *Key() const { return (MTkey *)key; }

	// basic GiST methods
	GiSTpenalty *Penalty(const GiSTentry &newEntry) const;
	GiSTpenalty *Penalty(const GiSTentry &newEntry, MTpenalty *minPenalty) const;	// redefined in order to optimize distance computations

	// Other methods we're required to supply
	int CompressedLength() const {
		return object().CompressedLength()+3*sizeof(double)+2*sizeof(BOOL);	// compressedEntry is: Object, rmin, rmax, distance, splitted, recomp
	}

	GiSTcompressedEntry Compress() const;
	void Decompress(const GiSTcompressedEntry entry);

	// Method for ordered domains
	int Compare(const GiSTentry& entry) const {
		assert(entry.IsA()==MTENTRY_CLASS);
		double d=Key()->distance-((MTentry &)entry).Key()->distance;
		int i=0;

		if(d>0) i=1;
		else if(d<0) i=-1;
		return i;
	}

	// I/O methods
#ifdef PRINTING_OBJECTS
	void Print(ostream& os) const {
		key->Print(os);
		os << "->" << Ptr() << " (" << Key()->distance << ")";
		if(Key()->splitted) os << " *";
		if(Key()->recomp) os << " #";
		os << endl;
	}
#endif

	// access to private members
	const MTkey& cregion() const { return *Key(); }
	Object& object() const { return Key()->obj; }
	double minradius() const { return Key()->rmin; }
	double maxradius() const { return Key()->rmax; }
	void setobject(const Object& o) { Key()->obj=o; }
	void setminradius(double d) { Key()->rmin=d; }
	void setmaxradius(double d) { Key()->rmax=d; }
};

#endif
