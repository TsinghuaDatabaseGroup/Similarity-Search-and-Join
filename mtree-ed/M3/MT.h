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

#ifndef MT_H
#define MT_H

#include "GiST.h"
#include "GiSTentry.h"
#include "MTfile.h"
#include "MTnode.h"
#include "MTcursor.h"
#ifdef _WIN32   // for MAXDOUBLE
#include <float.h>
#include <limits.h>
#define MAXDOUBLE DBL_MAX
#define MINDOUBLE DBL_MIN
#define MAXINT INT_MAX
#else   // under UNIX these constants are defined in values.h
#include <values.h>
#endif

class MTkey;
class TopQuery;
class MTcursor;

typedef enum {
    MIN_R_INCR=0,
    MIN_OVERLAP=1,
    MIXED=2
} tb_function;  // tie breaking functions

typedef enum {
    RANDOM=0,
    CONFIRMED=1,
    MAX_UB_DIST=2,
    MIN_RAD=3,
    MIN_OVERLAPS=4,
    SAMPLING=5,
} pp_function;  // promotion functions

typedef enum {
    RANDOMV=0,
    SAMPLINGV=1,
    MAX_LB_DIST=2,
    mM_RAD=3
} pv_function;  // confirmed promotion functions

typedef enum {
    LB=0,
    AVG=1,
    UB=2
} r_function;   // min_MAX_radius functions

typedef enum {
    G_HYPERPL=0,
    BAL_G_HYPERPL=1,
    BALANCED=2
} s_function;   // split functions

class MT : public GiST {
public:
    // optional, for debugging support
    GiSTobjid IsA() { return MT_CLASS; }

    int IsOrdered() const { return 1; } // the children of each node are ordered on increasing distances from the parent
    int MaxLevel() const;   // height of the M-tree
    GiSTlist<MTentry *> RangeSearch(const MTquery& query);  // range search
    MTentry **TopSearch(const TopQuery& query); // generic search algorithm: k-NN search for a TopQuery
    MTnode *ParentNode(MTnode *node);   // return the parent of node (the caller should delete the object)
    void BulkLoad(MTentry **data, int n, double padFactor, char *name); // bulk-loading algorithm
    void CollectStats();    // level-stats collecting function: total number of pages, average radius and average covered volume (if applicable)
    BOOL CheckNode(GiSTpath path, MTentry *e);  // debug function: check for node consistency

    // friend methods and functions
    friend void MTnode::InvalidateEntry(BOOL isNew);
    friend GiSTlist<MTentry *> MTnode::RangeSearch(const MTquery& query);
    friend void MTcursor::FetchNode();

protected:
    // Required members
    GiSTnode  *CreateNode()  const { return new MTnode; }
    GiSTstore *CreateStore() const { return new MTfile; }

    // Service methods for Bulk Load
    GiSTlist<char *> *SplitTree(int *ncreated, int level, GiSTlist<MTentry *> *children, char *name);   // split this M-tree into a list of M-trees whose names are returned
    void Append(MTnode *to, MTnode *from);  // append the sub-tree of node from to node to
private:
    void AdjKeys(GiSTnode *node);   // slightly different from AdjustKeys (necessary to keep track of the distance from the parent)
};

int PickRandom(int from, int to);   // pick a determined number of samples in the range [from, to[

#endif
