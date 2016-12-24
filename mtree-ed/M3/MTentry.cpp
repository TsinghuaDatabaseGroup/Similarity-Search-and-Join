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

#include "MTentry.h"
#include "MT.h"

tb_function TB_FUNCTION=MIN_R_INCR;

int sizeofEntry() {
	if(sizeofObject()) return(sizeofObject()+3*sizeof(double)+2*sizeof(BOOL));	// compressedEntry is: Object, rmin, rmax, distance, splitted, recomp
	return 0;
}

GiSTpenalty * 
MTentry::Penalty(const GiSTentry &newEntry) const
{
	double retval;
	assert(newEntry.IsA()==MTENTRY_CLASS);
	const MTentry& e=(const MTentry &)newEntry;
	double dist=object().distance(e.object())+e.maxradius();

//	printf("Distance between %s & %s=%f\n", object().name, e.object().name, dist);
	// return area enlargement
	switch(TB_FUNCTION) {
		case MIN_R_INCR: {
			retval=dist-maxradius();
			if(retval<0) retval=-maxDist()+dist;
			// if the entry can fit in (more than) a node without enlarging its radius, we assign it to its nearest node
			break;
		}
		case MIN_OVERLAP: {
			retval=0;
			for (int i=0; i<Node()->NumEntries(); i++) {	// compute total overlap for this node
				const MTentry& sibling=(const MTentry &)*(*(Node()))[i].Ptr();
				double cost=sibling.maxradius()+dist-object().distance(sibling.object());

				retval+=MAX(cost, 0);
			}
			break;
		}
	}
//	cout << "Penalty for " << newEntry << " in " << this << " = " << retval << " (distance = " << dist << ")" << endl;
	return new MTpenalty(retval, dist);
}

GiSTpenalty * 
MTentry::Penalty(const GiSTentry &newEntry, MTpenalty *minPenalty) const
// in this case we can avoid to compute some distances by using some stored information:
// minPenalty (the minimum penalty achieved so far),
// newEntry.key->distance (the distance of the entry from the parent of this entry, stored in the SearchMinPenalty method),
// and maxradius.
{
	double retval;
	assert(newEntry.IsA()==MTENTRY_CLASS);
	const MTentry& e=(const MTentry &)newEntry;
	double dist;

//	printf("Distance between %s & %s=%f\n", object().name, e.object().name, dist);
	// return area enlargement
	switch(TB_FUNCTION) {
		case MIN_R_INCR: {
			if(((MTkey *)newEntry.Key())->distance>0) {	// in this case is possible to prune some entries using triangular inequality
				double distPen=fabs(((MTkey *)newEntry.Key())->distance-Key()->distance)-maxradius();
				MTpenalty *tmpPen;

				if(distPen>=0) tmpPen=new MTpenalty(distPen, 0);
				else tmpPen=new MTpenalty(distPen+maxradius()-maxDist(), 0);
				if((minPenalty!=NULL)&&!((*tmpPen)<(*minPenalty))) {
					delete tmpPen;
					return new MTpenalty(MAXDOUBLE, 0);	// ... and we avoid to compute this distance
				}
				delete tmpPen;
			}
			dist=object().distance(e.object())+e.maxradius();	// compute the distance
			retval=dist-maxradius();
			if(retval<0) retval=-maxDist()+dist;
			// if the entry can fit in (more than) a node without enlarging its radius, we assign it to its nearest node
			break;
		}
		case MIN_OVERLAP: {
			retval=0;
			dist=object().distance(e.object())+e.maxradius();
			for (int i=0; i<Node()->NumEntries(); i++) {	// compute total overlap for this node
				const MTentry& sibling=(const MTentry &)*(*(Node()))[i].Ptr();
				double cost=sibling.maxradius()+dist-object().distance(sibling.object());

				retval+=MAX(cost, 0);
			}
			break;
		}
	}
//	cout << "Penalty for " << newEntry << " in " << this << " = " << retval << " (distance = " << dist << ")" << endl;
	return new MTpenalty(retval, dist);
}

GiSTcompressedEntry
MTentry::Compress() const
{
	GiSTcompressedEntry compressedEntry;
	double rmin=((MTkey *)key)->minradius(), rmax=((MTkey *)key)->maxradius(), dist=((MTkey *)key)->distance;
	int sizeofObject=((MTkey *)key)->object().CompressedLength();
	BOOL splitted=((MTkey *)key)->splitted, recomp=((MTkey *)key)->recomp;

	compressedEntry.key=new char[CompressedLength()];
	((MTkey *)key)->object().Compress(compressedEntry.key);	// compress the object
	memcpy(compressedEntry.key+sizeofObject, &rmin, sizeof(double));
	memcpy(compressedEntry.key+sizeofObject+sizeof(double), &rmax, sizeof(double));
	memcpy(compressedEntry.key+sizeofObject+2*sizeof(double), &dist, sizeof(double));
	memcpy(compressedEntry.key+sizeofObject+3*sizeof(double), &splitted, sizeof(BOOL));
	memcpy(compressedEntry.key+sizeofObject+3*sizeof(double)+sizeof(BOOL), &recomp, sizeof(BOOL));
//	Object obj(compressedEntry.key);	// decompress the object
	compressedEntry.keyLen=CompressedLength();
	compressedEntry.ptr=ptr;
	return compressedEntry;
}

void
MTentry::Decompress(const GiSTcompressedEntry entry)
{
	Object obj(entry.key);	// decompress the object
	double rmin, rmax, dist;
	int sizeofObject=obj.CompressedLength();
	BOOL splitted, recomp;

	memcpy(&rmin, entry.key+sizeofObject, sizeof(double));
	memcpy(&rmax, entry.key+sizeofObject+sizeof(double), sizeof(double));
	memcpy(&dist, entry.key+sizeofObject+2*sizeof(double), sizeof(double));
	memcpy(&dist, entry.key+sizeofObject+2*sizeof(double), sizeof(double));
	memcpy(&splitted, entry.key+sizeofObject+3*sizeof(double), sizeof(BOOL));
	memcpy(&recomp, entry.key+sizeofObject+3*sizeof(double)+sizeof(BOOL), sizeof(BOOL));
	key=new MTkey(obj, rmin, rmax, dist, splitted, recomp);
	ptr=entry.ptr;
}
