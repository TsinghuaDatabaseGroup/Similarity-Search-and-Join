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

#include "MT.h"
#include "MTpredicate.h"
#include "MTentry.h"

extern dist2sim hfunction;
extern double distr[1001];

double Dist2Sim(double dist) {
	switch(hfunction) {
		case LINEAR: return 1-dist/maxDist();
		case EXPONENTIAL: return exp(-dist);
//		case DISTR: return distr[(int)(dist*1001/maxDist())];
		default: return 0;
	}
}

double Sim2Dist(double sim) {
	switch(hfunction) {
		case LINEAR: return maxDist()*(1-sim);
		case EXPONENTIAL: return -log(sim);
/*		case DISTR: {
			for(int i=0; distr[i]>=sim; i++);
			return i*maxDist()/1001;
		} */
		default: return maxDist();
	}
}

int 
SimpleQuery::Consistent(const GiSTentry& entry)
{
	assert(entry.IsA()==MTENTRY_CLASS);
//	cout << "Evaluating " << entry;
	if((grade==0)||(fabs(grade-((MTentry &)entry).Key()->distance)<=radius+((MTentry &)entry).maxradius())) {	// prune for reference point
		grade=pred->distance(((MTentry &)entry).object());
		return(grade<=radius+((MTentry &)entry).maxradius());
	}
	else return FALSE;
}

int 
SimpleQuery::NonConsistent(const GiSTentry& entry)
{
	assert(entry.IsA()==MTENTRY_CLASS);
//	cout << "Evaluating " << entry;
	if((grade==0)||(fabs(grade-((MTentry &)entry).Key()->distance)>radius-((MTentry &)entry).maxradius())) {	// prune for reference point
		grade=pred->distance(((MTentry &)entry).object());
		return(grade>radius-((MTentry &)entry).maxradius());
	}
	else return FALSE;
}
