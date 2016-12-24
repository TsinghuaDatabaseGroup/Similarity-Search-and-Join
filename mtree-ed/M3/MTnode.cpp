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

#include <string.h>
#include "MT.h"
#include "MTpredicate.h"

double MIN_UTIL;	// minimum node utilization
pp_function PROMOTE_PART_FUNCTION;	// promotion method
pv_function PROMOTE_VOTE_FUNCTION;	// confirmed promotion method (if needed)
pp_function SECONDARY_PART_FUNCTION;	// root promotion method (can't use stored distances): used only for confirmed and MAX_UB_DIST methods
r_function RADIUS_FUNCTION;	// mM_RAD promotion method (if needed)
int NUM_CANDIDATES;	// number of candidates for sampling
s_function SPLIT_FUNCTION;	// split method

extern int IOread;

// used to quick-sort the entries in a node according to their distance from the parent
int
MTentrycmp(const void *x, const void *y)
{
	double d=(*(MTentry **)x)->Key()->distance-(*(MTentry **)y)->Key()->distance;
	int i=0;

	if(d>0) i=1;
	else if(d<0) i=-1;
	return i;
}

// used in Split to find the next nearest entry
int
FindMin(double *vec, int veclen)
{
	int i, min_i=-1;
	double min=MAXDOUBLE;

	for(i=0; i<veclen; i++)
		if(vec[i]<min) {
			min_i=i;
			min=vec[i];
		}
	return min_i;
}

GiSTobject *
MTnode::NCopy()
{
	MTnode *newnode=(MTnode *)Copy();

	if((obj==NULL)&&(Path().Level()>1)) {
		MTentry *e=Entry();

		obj=newnode->obj=&(e->object());
		delete e;
	}
	newnode->InvalidateEntries();
	return newnode;
}

#ifdef PRINTING_OBJECTS
void
MTnode::Print(ostream& os) const
{
	if(obj!=NULL) os << *obj << " ";
//	else cout << "obj NULL...\n";
	os << ((MTnode *)this)->Path() << " #Entries: " << NumEntries() << ", Level " << Level();
	if(IsLeaf()) os << "(Leaf)";
	else os << "(Internal)";
	os << ", Sibling: " << Sibling() << ", Size: " << Size() << "/" << Tree()->Store()->PageSize() << endl;
	for(int i=0; i<NumEntries(); i++) (*this)[i]->Print(os);
}
#endif

int
MTnode::IsUnderFull(const GiSTstore &store) const
{
	return ((MIN_UTIL>0)&&(Size()<(int)(store.PageSize()*MIN_UTIL)));
}

void
MTnode::InvalidateEntries()
{
	for(int i=0; i<NumEntries(); i++)
		((MTentry *)((*this)[i].Ptr()))->Key()->distance=-maxDist();
}

void
MTnode::InvalidateEntry(BOOL isNew)
{
	GiSTpath path=Path();
	if(path.Level()>1) {
		MTnode *parent=((MT *)Tree())->ParentNode((MTnode *)this), *gparent=((MT *)Tree())->ParentNode(parent);
		int i;

		for(i=0; i<parent->NumEntries(); i++) {	// search the entry between the parent's children
			MTentry *e=(MTentry *)((*parent)[i].Ptr());

			if(e->Ptr()==path.Page()) {
				if(isNew) e->Key()->distance=-maxDist();
//				else e->Key()->distance=-e->Key()->distance;
				e->Key()->splitted=TRUE;
				break;
			}
		}
		path.MakeParent();
		for(i=0; i<gparent->NumEntries(); i++) {	// search the parent entry between the grandparent's children
			MTentry *e=(MTentry *)((*gparent)[i].Ptr());

			if(e->Ptr()==path.Page()) {
				e->setmaxradius(-1);
				break;
			}
		}
		((MT *)Tree())->WriteNode(parent);	// write parent node (in inconsistent state)
		((MT *)Tree())->WriteNode(gparent);	// write gparent node (to invalidate the parent's entry)
		delete parent;
		delete gparent;
	}
}

MTentry *
MTnode::Entry() const
{
	GiSTpath path=((MTnode *)this)->Path();
	MTnode *parent=((MT *)Tree())->ParentNode((MTnode *)this);
	MTentry *returnEntry=NULL;

	for(int i=0; (i<parent->NumEntries())&&(returnEntry==NULL); i++)
		if((*parent)[i].Ptr()->Ptr()==path.Page()) returnEntry=(MTentry *)(*parent)[i].Ptr()->Copy();
	delete parent;
	return returnEntry;
}

double
MTnode::distance(int i) const
{
	MTentry *e=(MTentry *)((*this)[i].Ptr());

//	if(e->Key()->distance>=0) cout << "Distance between " << obj << " & " << e->object() << " = " << e->Key()->distance << endl;
//	else cout << "Computing distance between " << obj << " & " << e->object() << "..." << endl;
	return (e->Key()->distance<0)? ((e->Key()->distance>-maxDist())? -e->Key()->distance: obj->distance(e->object())): e->Key()->distance;
}

// SearchMinPenalty returns where a new entry should be inserted.
// Overriden to insert the distance from the parent in the new entry.
GiSTpage
MTnode::SearchMinPenalty(const GiSTentry& newEntry) const
{
	MTentry *minEntry=NULL;
	MTpenalty *minPenalty=NULL;

	for(int i=0; i<NumEntries(); i++) {
		MTentry *e=(MTentry *)((*this)[i].Ptr());

		assert((MTnode *)e->Node()==this);
		MTpenalty *penalty=(MTpenalty *)e->Penalty(newEntry, minPenalty);	// use the alternate Penalty method in order to avoid some distance computations

		if((minEntry==NULL)||(*penalty)<(*minPenalty)) {
			minEntry=e;
			if(minPenalty) delete minPenalty;
			minPenalty=penalty;
		}
		else delete penalty;
	}
	((MTentry&)newEntry).Key()->distance=minPenalty->distance;
	delete minPenalty;
	return minEntry->Ptr();
}

void
MTnode::InsertBefore(const GiSTentry& entry, int index)
{
	int n=NumEntries();
	BOOL ordered=TRUE;

	if(index>0) ordered=((*this)[index-1]->Compare(entry)<=0);
	if(index<n) ordered=ordered&&((*this)[index]->Compare(entry)>=0);
	if(ordered) {	// yes, the position is right for this entry
		assert(index<=n);
		GiSTentry *e=(GiSTentry *)entry.Copy();

		e->SetLevel(Level());
		e->SetPosition(index);
		e->SetNode(this);
		// Move everything else over
		for(int i=n; i>index; i--) {
			GiSTentry *e=(*this)[i-1];

			e->SetPosition(i);
			(*this)[i]=e;
		}
		// Stick the entry in
		(*this)[index]=e;
		// Bump up the count
		SetNumEntries(n+1);
	}
	else Insert(entry);	// find the right place
}

// quick-sort the entries with respect to the distance from the parent
void
MTnode::Order()
{
	int i, obji=-1, n=NumEntries();
	MTentry **array=new MTentry *[n], *objEntry=NULL;

	for(i=0; i<n; i++) {
		array[i]=(MTentry *)((MTentry *)(*this)[i].Ptr())->Copy();
		if(obj==&((MTentry *)(*this)[i].Ptr())->object()) objEntry=array[i];
	}
	qsort(array, n, sizeof(MTentry *), MTentrycmp);
	while(NumEntries()>0) DeleteEntry(0);
	for(i=0; i<n; i++) {
		InsertBefore(*(array[i]), i);
		if(objEntry==array[i]) obji=i;
		delete array[i];
	}
	delete []array;
	if(obji>=0) obj=&((MTentry *)(*this)[obji].Ptr())->object();
}

GiSTnode *
MTnode::PickSplit()
{
	MTnode *rightnode;
	int leftdeletes, rightdeletes;	// number of entries to be deleted from each node
	int *leftvec=new int[NumEntries()], *rightvec=new int[NumEntries()];	// array of entries to be deleted from each node

	// promote the right node (possibly reassigning the left node);
	// the right node's page is copied from left node; 
	// we'll delete from the nodes as appropriate after the splitting phase
//	cout << "In PickSplit with node " << this << "\n";
	rightnode=PromotePart();
	// now perform the split
	Split(rightnode, leftvec, rightvec, &leftdeletes, &rightdeletes);	// complexity: O(n)
	// given the deletion vectors, do bulk deletes
	DeleteBulk(leftvec, leftdeletes);
	rightnode->DeleteBulk(rightvec, rightdeletes);
//	cout << "Nodes:\n" << this << rightnode;
	// order the entries in both nodes
	Order();
	rightnode->Order();
	delete []leftvec;
	delete []rightvec;
	// return the right node
	return rightnode;
}

MTnode *
MTnode::PromotePart()
{
	MTnode *newnode;

	switch(PROMOTE_PART_FUNCTION) {
		case RANDOM: {	// complexity: constant
			int i, j;

			// pick two *different* random entries
//			cout << "Random promotion: ";
			i=PickRandom(0, NumEntries());
			do j=PickRandom(0, NumEntries());
			while (j==i);
			if(((MTentry *)(*this)[j].Ptr())->Key()->distance==0) {
				int k=i; i=j; j=k;	// if we chose the parent entry, put it in the left node
			}
//			cout << "Entries " << (*this)[i].Ptr() << " & " << (*this)[j].Ptr() << " chosen.\n";
			newnode=(MTnode *)NCopy();
			// re-assign the nodes' object
			newnode->obj=&((MTentry *)((*newnode)[j].Ptr()))->object();
			obj=&((MTentry *)((*this)[i].Ptr()))->object();
			if(((MTentry *)(*this)[i].Ptr())->Key()->distance>0) {	// if the parent object wasn't confirmed, invalidate also the parent
				InvalidateEntry(TRUE);
				InvalidateEntries();
			}
			else InvalidateEntry(FALSE);	// else, invalidate only the node's radii
			break;
		}
		case CONFIRMED: {	// complexity: determined by the confirmed promotion algorithm
			int i;
			BOOL isRoot=TRUE;

//			cout << "Confirmed promotion: ";
//			for(i=0; (i<NumEntries())&&(isRoot); i++) isRoot=(((MTentry *)((*this)[i].Ptr()))->Key()->distance==-MAXDIST);
			isRoot=(((MTentry *)((*this)[0].Ptr()))->Key()->distance==-maxDist());	// we have ordered entries
			if(isRoot) {	// if we're splitting the root we have to use a policy that doesn't use stored distances
				PROMOTE_PART_FUNCTION=SECONDARY_PART_FUNCTION;	
				newnode=PromotePart();
				PROMOTE_PART_FUNCTION=CONFIRMED;
			}
			else {
				int index=-1;

				for(i=0; (i<NumEntries())&&(index<0); i++) if(((MTentry *)((*this)[i].Ptr()))->Key()->distance==0) index=i;
				obj=&((MTentry *)((*this)[index].Ptr()))->object();
				// now choose the right node parent
				newnode=PromoteVote();
			}
			InvalidateEntry(FALSE);
			break;
		}
		case MAX_UB_DIST: {	// complexity: constant
			double maxdist=-1, maxdist2;
			int i, maxcand1, maxcand2;
			BOOL isRoot=TRUE;

//			cout << "Largest max dist promotion:\n";
//			for(i=0; (i<NumEntries())&&(isRoot); i++) isRoot=(((MTentry *)((*this)[i].Ptr()))->Key()->distance==-MAXDIST);
			isRoot=(((MTentry *)((*this)[0].Ptr()))->Key()->distance==-maxDist());	// we have ordered entries
			if(isRoot) {	// if we're splitting the root we have to use a policy that doesn't use stored distances
				PROMOTE_PART_FUNCTION=SECONDARY_PART_FUNCTION;	
				newnode=PromotePart();
				PROMOTE_PART_FUNCTION=CONFIRMED;
			}
			else
				if(Tree()->IsOrdered()) {	// if the tree is ordered we can choose the last two elements
					maxcand1=NumEntries()-1;
					maxcand2=NumEntries()-2;
				}	// the following code should be unreachable
				else	// otherwise we have to search the two objects which are farthest from the parent
					for (i=0; i<NumEntries(); i++) {
						MTentry *e=(MTentry *)((*this)[i].Ptr());

						if (e->Key()->distance>maxdist) {
							maxdist2=maxdist;
							maxdist=e->Key()->distance;
							maxcand2=maxcand1;
							maxcand1=i;
						}
						else if (e->Key()->distance>maxdist2) {
							maxdist2=e->Key()->distance;
							maxcand2=i;
						}
					}
//			cout << "Entries " << (*this)[maxcand1].Ptr() << " & " << (*this)[maxcand2].Ptr() << " chosen.\n";
			// for sure the parent isn't confirmed (unless we have a binary tree...)
			obj=&((MTentry *)((*this)[maxcand1].Ptr()))->object();
			InvalidateEntry(TRUE);
			InvalidateEntries();
			newnode=(MTnode *)NCopy();
			newnode->obj=&((MTentry *)((*newnode)[maxcand2].Ptr()))->object();
			break;
		}
		case SAMPLING: {	// complexity: O(kn) distance computations
//			cout << "Sampling: ";
			int *vec=PickCandidates(), i, j, min1, min2, bestld, bestrd, *bestlv=new int[NumEntries()], *bestrv=new int[NumEntries()];
			double minvalue=MAXDOUBLE, sec_minvalue=MAXDOUBLE, **distances=new double*[MIN(NUM_CANDIDATES, NumEntries())];	// distance matrix

			// initialize distance matrix
			for(i=0; i<MIN(NUM_CANDIDATES, NumEntries()); i++) {
				distances[i]=new double[NumEntries()];
				for(j=0; j<NumEntries(); j++) distances[i][j]=-maxDist();
			}
			for(i=0; i<MIN(NUM_CANDIDATES, NumEntries()); i++)
				if(((MTentry *)((*this)[vec[i]].Ptr()))->Key()->distance==0) {
					for(j=0; j<NumEntries(); j++) distances[i][j]=((MTentry *)((*this)[j].Ptr()))->Key()->distance;
					break;
				}
			for(i=0; i<MIN(NUM_CANDIDATES, NumEntries()); i++) distances[i][vec[i]]=0;
			// find the candidates with minimum radius
			for(i=1; i<MIN(NUM_CANDIDATES, NumEntries()); i++)
				for (j=0; j<i; j++) {
					MTentry *e1=new MTentry, *e2=new MTentry;
					MTnode *node1=(MTnode *)NCopy(), *node2=(MTnode *)NCopy();
					double value, sec_value;
					int leftdeletes, rightdeletes, *leftvec=new int[NumEntries()], *rightvec=new int[NumEntries()], k;

					for(k=0; k<NumEntries(); k++) {
						((MTentry *)((*node1)[k].Ptr()))->Key()->distance=distances[i][k];
						((MTentry *)((*node2)[k].Ptr()))->Key()->distance=distances[j][k];
					}
					node1->obj=&((MTentry *)((*this)[vec[i]].Ptr()))->object();
					node2->obj=&((MTentry *)((*this)[vec[j]].Ptr()))->object();
					// perform the split
					node1->Split(node2, leftvec, rightvec, &leftdeletes, &rightdeletes);
					for(k=0; k<NumEntries(); k++) {
						distances[i][k]=((MTentry *)((*node1)[k].Ptr()))->Key()->distance;
						distances[j][k]=((MTentry *)((*node2)[k].Ptr()))->Key()->distance;
					}
					// given the deletion vectors, do bulk deletes
					node1->DeleteBulk(leftvec, leftdeletes);
					node2->DeleteBulk(rightvec, rightdeletes);
					e1->InitKey();
					e2->InitKey();
					e1->setobject(*node1->obj);
					e2->setobject(*node2->obj);
					e1->setmaxradius(0);
					e2->setmaxradius(0);
					e1->setminradius(MAXDOUBLE);
					e2->setminradius(MAXDOUBLE);
					// compute the radii
					node1->mMRadius(e1);
					node2->mMRadius(e2);
					// check the result
					value=MAX(e1->maxradius(), e2->maxradius());	// this is minMAX_RADII
					sec_value=MIN(e1->maxradius(), e2->maxradius());
					if((value<minvalue)||((value==minvalue)&&(sec_value<sec_minvalue))) {
						int index;

						minvalue=value;
						sec_minvalue=sec_value;
						bestld=leftdeletes;
						bestrd=rightdeletes;
						for(index=0; index<leftdeletes; index++) bestlv[index]=leftvec[index];
						for(index=0; index<rightdeletes; index++) bestrv[index]=rightvec[index];
						min1=i;
						min2=j;
					}
					// be tidy
					delete []leftvec;
					delete []rightvec;
					delete node1;
					delete node2;
					delete e1;
					delete e2;
				}
//			cout << "Entries " << (*this)[vec[min1]].Ptr() << " & " << (*this)[vec[min2]].Ptr() << " chosen.\n";
			if(((MTentry *)(*this)[vec[min2]].Ptr())->Key()->distance>0) newnode=(MTnode *)NCopy();
			else newnode=(MTnode *)Copy();
			newnode->obj=&((MTentry *)((*newnode)[vec[min2]].Ptr()))->object();
			obj=&((MTentry *)((*this)[vec[min1]].Ptr()))->object();
			if(((MTentry *)(*this)[vec[min1]].Ptr())->Key()->distance>0) {	// if the parent object wasn't confirmed, invalidate also the parent
				InvalidateEntry(TRUE);
				InvalidateEntries();
			}
			else InvalidateEntry(FALSE);	// else, invalidate only the node's radii
			for(i=0; i<NumEntries(); i++) {
				((MTentry *)((*this)[i].Ptr()))->Key()->distance=distances[min1][i];
				((MTentry *)((*newnode)[i].Ptr()))->Key()->distance=distances[min2][i];
			}
			delete []bestlv;
			delete []bestrv;
			for(i=0; i<MIN(NUM_CANDIDATES, NumEntries()); i++) delete []distances[i];
			delete []distances;
			break;
		}
		case MIN_RAD:
		case MIN_OVERLAPS: {	// complexity: O(n^2) distance computations
			int min1, min2, i, j, bestld, bestrd, *bestlv=new int[NumEntries()], *bestrv=new int[NumEntries()];
			double minvalue=MAXDOUBLE, sec_minvalue=MAXDOUBLE, **distances=new double *[NumEntries()];	// distance matrix

			// initialize distance matrix
			for(i=0; i<NumEntries(); i++) {
				distances[i]=new double[NumEntries()];
				for(j=0; j<NumEntries(); j++) distances[i][j]=-maxDist();
			}
			for(i=0; i<NumEntries(); i++)
				if(((MTentry *)((*this)[i].Ptr()))->Key()->distance==0) {
					for(j=0; j<NumEntries(); j++) {
						distances[i][j]=((MTentry *)((*this)[j].Ptr()))->Key()->distance;
						distances[j][i]=distances[i][j];
					}
					break;
				}
			for(i=0; i<NumEntries(); i++) distances[i][i]=0;
//			if(PROMOTE_PART_FUNCTION==MIN_RADII) cout << "Min radii promotion: ";
//			else cout << "Min overlaps promotion: ";
			for (i=1; i<NumEntries(); i++)
				for (j=0; j<i; j++) {
					MTentry *e1=new MTentry, *e2=new MTentry;
					MTnode *node1=(MTnode *)NCopy(), *node2=(MTnode *)NCopy();
					double value, sec_value;
					int leftdeletes, rightdeletes, *leftvec=new int[NumEntries()], *rightvec=new int[NumEntries()], k;

					for(k=0; k<NumEntries(); k++) {
						((MTentry *)((*node1)[k].Ptr()))->Key()->distance=distances[i][k];
						((MTentry *)((*node2)[k].Ptr()))->Key()->distance=distances[j][k];
					}
					node1->obj=&((MTentry *)((*this)[i].Ptr()))->object();
					node2->obj=&((MTentry *)((*this)[j].Ptr()))->object();
					// perform the split
					node1->Split(node2, leftvec, rightvec, &leftdeletes, &rightdeletes);
					for(k=0; k<NumEntries(); k++) {
						distances[i][k]=((MTentry *)((*node1)[k].Ptr()))->Key()->distance;
						distances[j][k]=((MTentry *)((*node2)[k].Ptr()))->Key()->distance;
						distances[k][i]=distances[i][k];
						distances[k][j]=distances[j][k];
					}
					// given the deletion vectors, do bulk deletes
					node1->DeleteBulk(leftvec, leftdeletes);
					node2->DeleteBulk(rightvec, rightdeletes);
					e1->InitKey();
					e2->InitKey();
					e1->setobject(*node1->obj);
					e2->setobject(*node2->obj);
					e1->setmaxradius(0);
					e2->setmaxradius(0);
					e1->setminradius(MAXDOUBLE);
					e2->setminradius(MAXDOUBLE);
					// compute the radii
					node1->mMRadius(e1);
					node2->mMRadius(e2);
					// check the result
					if(PROMOTE_PART_FUNCTION==MIN_RAD) {
						value=MAX(e1->maxradius(), e2->maxradius());	// this is minMAX_RADII
						sec_value=MIN(e1->maxradius(), e2->maxradius());
					}
					else value=e1->maxradius()+e2->maxradius()-distances[i][j];
					if((value<minvalue)||((value==minvalue)&&(sec_value<sec_minvalue))) {
						int index;

						minvalue=value;
						sec_minvalue=sec_value;
						bestld=leftdeletes;
						bestrd=rightdeletes;
						for(index=0; index<leftdeletes; index++) bestlv[index]=leftvec[index];
						for(index=0; index<rightdeletes; index++) bestrv[index]=rightvec[index];
						min1=i;
						min2=j;
					}
					// be tidy
					delete []leftvec;
					delete []rightvec;
					delete node1;
					delete node2;
					delete e1;
					delete e2;
				}
//			cout << "Entries " << (*this)[min1].Ptr() << " & " << (*this)[min2].Ptr() << " chosen.\n";
			if(((MTentry *)(*this)[min2].Ptr())->Key()->distance>0) newnode=(MTnode *)NCopy();
			else newnode=(MTnode *)Copy();
			newnode->obj=&((MTentry *)((*newnode)[min2].Ptr()))->object();
			obj=&((MTentry *)((*this)[min1].Ptr()))->object();
			if(((MTentry *)(*this)[min1].Ptr())->Key()->distance>0) {	// if the parent object wasn't confirmed, invalidate also the parent
				InvalidateEntry(TRUE);
				InvalidateEntries();
			}
			else InvalidateEntry(FALSE);	// else, invalidate only the node's radii
			for(i=0; i<NumEntries(); i++) {
				((MTentry *)((*this)[i].Ptr()))->Key()->distance=distances[min1][i];
				((MTentry *)((*newnode)[i].Ptr()))->Key()->distance=distances[min2][i];
			}
			delete bestlv;
			delete bestrv;
			for(i=0; i<NumEntries(); i++) delete []distances[i];
			delete []distances;
			break;
		}
	}
	return newnode;
}

MTnode *
MTnode::PromoteVote()
{
	MTnode *newnode=(MTnode *)NCopy();
	int i;

	switch(PROMOTE_VOTE_FUNCTION) {
		case RANDOMV: {	// complexity: constant
//			cout << "Random voting: ";
			// pick a random entry (different from the parent)
			do i=PickRandom(0, NumEntries());
			while(((MTentry *)(*this)[i].Ptr())->Key()->distance==0);
//			cout << "Entry " << (*this)[i].Ptr() << " chosen.\n";
			newnode->obj=&((MTentry *)((*newnode)[i].Ptr()))->object();
			break;
		}
		case SAMPLINGV: {	// complexity: O(kn) distance computations
//			cout << "Sampling voting: ";
			int *vec=PickCandidates(), bestcand, bestld, bestrd, *bestlv=new int[NumEntries()], *bestrv=new int[NumEntries()];
			double minvalue=MAXDOUBLE, sec_minvalue=MAXDOUBLE, **distances=new double *[MIN(NUM_CANDIDATES, NumEntries())];	// distance matrix

			// find the candidate with minimum radius
			for (i=0; i<MIN(NUM_CANDIDATES, NumEntries()); i++) {
				MTentry *cand=(MTentry *)((*this)[vec[i]].Ptr()), *e1=new MTentry, *e2=new MTentry;
				MTnode *node1=(MTnode *)Copy(), *node2=(MTnode *)NCopy();
				double value, sec_value;
				int leftdeletes, rightdeletes, *leftvec=new int[NumEntries()], *rightvec=new int[NumEntries()], j;

//				cout << "Entry " << cand;
				// initialize distance matrix
				distances[i]=new double[NumEntries()];
				for (j=0; j<NumEntries(); j++) distances[i][j]=((vec[i]==j)? 0: cand->object().distance(((MTentry *)((*this)[j].Ptr()))->object()));
				for(j=0; j<NumEntries(); j++) ((MTentry *)((*node2)[j].Ptr()))->Key()->distance=distances[i][j];
				node1->obj=obj;
				node2->obj=&((MTentry *)((*this)[vec[i]].Ptr()))->object();
				// perform the split
				node1->Split(node2, leftvec, rightvec, &leftdeletes, &rightdeletes);
				// given the deletion vectors, do bulk deletes
				node1->DeleteBulk(leftvec, leftdeletes);
				node2->DeleteBulk(rightvec, rightdeletes);
				e1->InitKey();
				e2->InitKey();
				e1->setobject(*node1->obj);
				e2->setobject(*node2->obj);
				e1->setmaxradius(0);
				e2->setmaxradius(0);
				e1->setminradius(MAXDOUBLE);
				e2->setminradius(MAXDOUBLE);
				// compute the radii
				node1->mMRadius(e1);
				node2->mMRadius(e2);
				// check the result
				value=MAX(e1->maxradius(), e2->maxradius());	// this is minMAX_RADII
				sec_value=MIN(e1->maxradius(), e2->maxradius());
				if((value<minvalue)||((value==minvalue)&&(sec_value<sec_minvalue))) {
					int index;

					minvalue=value;
					sec_minvalue=sec_value;
					bestld=leftdeletes;
					bestrd=rightdeletes;
					for(index=0; index<leftdeletes; index++) bestlv[index]=leftvec[index];
					for(index=0; index<rightdeletes; index++) bestrv[index]=rightvec[index];
					bestcand=i;
				}
				// be tidy
				delete e1;
				delete e2;
				delete node1;
				delete node2;
				delete []leftvec;
				delete []rightvec;
			}
//			cout << "Entry " << (*this)[vec[bestcand]].Ptr() << " chosen.\n";
			newnode->obj=&((MTentry *)((*newnode)[vec[bestcand]].Ptr()))->object();
			// update the distance of the children from the new parent
			for (i=0; i<NumEntries(); i++)
				((MTentry *)((*newnode)[i].Ptr()))->Key()->distance=distances[bestcand][i];
			for (i=0; i<MIN(NUM_CANDIDATES, NumEntries()); i++) delete []distances[i];
			delete []distances;
			delete []vec;
			delete []bestlv;
			delete []bestrv;
			break;
		}
		case MAX_LB_DIST: {	// complexity: constant
			double maxdist=-1;
			int maxcand;

//			cout << "Largest min dist voting:\n";
			if(Tree()->IsOrdered()) maxcand=NumEntries()-1;	// if the tree is ordered we can choose the last element
			else	// otherwise we have to search the object which is farthest from the parent
				for (i=0; i<NumEntries(); i++) {
					MTentry *e=(MTentry *)((*this)[i].Ptr());

					if (e->Key()->distance>maxdist) {
						maxdist=e->Key()->distance;
						maxcand=i;
					}
				}
//			cout << "Entry " << (*this)[maxcand].Ptr() << " chosen.\n";
			newnode->obj=&((MTentry *)((*newnode)[maxcand].Ptr()))->object();
			break;
		}
		case mM_RAD: { // complexity: constant
			double minradius=MAXDOUBLE;
			int bestcand;

//			cout << "Best radius voting:\n";
			for (i=0; i<NumEntries(); i++) {
				MTentry *cand=(MTentry *)((*this)[i].Ptr());
				double radius=0;

				if(cand->Key()->distance==0) continue;
				for (int j=0; j<NumEntries(); j++) {
					MTentry *e=(MTentry *)((*this)[j].Ptr());
					double dmin, dmax;

					if (i==j) continue;
					dmin=fabs(cand->Key()->distance-e->Key()->distance);
					dmax=cand->Key()->distance+e->Key()->distance;
					switch (RADIUS_FUNCTION) {
						case LB:
							radius=MAX(radius, dmin);
							break;
						case AVG:
							radius=MAX(radius, (dmin+dmax)/2);
							break;
						case UB:
							radius=MAX(radius, dmax);
							break;
					}
				}
				if (radius<minradius) {
					bestcand=i;
					minradius=radius;
				}
			}
//			cout << "Entry " << (*this)[bestcand].Ptr() << " chosen.\n";
			newnode->obj=&((MTentry *)((*newnode)[bestcand].Ptr()))->object();
			break;
		}
	}
	return newnode;
}

int *
MTnode::PickCandidates()
{
	int max_ind=MIN(NUM_CANDIDATES, NumEntries()), *vec=new int[max_ind], i;
	BOOL *used=new BOOL[NumEntries()];

	for(i=0; i<NumEntries(); i++) used[i]=(((MTentry *)(*this)[i].Ptr())->Key()->distance==0);
	// insert in vec the indices of the candidates for promotion
	for(i=0; i<max_ind; i++) {
		int j;

		do j=PickRandom(0, NumEntries());
		while(used[j]);
		vec[i]=j;
		used[j]=TRUE;
	}
	return vec;
}

void
MTnode::Split(MTnode *node, int *leftvec, int *rightvec, int *leftdeletes, int *rightdeletes)
{
	int i;

	*rightdeletes=0;
	*leftdeletes=0;
//	cout << "Now splitting between:\n";
//	cout << obj << " & " << node->obj << endl;
	switch(SPLIT_FUNCTION) {
		case G_HYPERPL: {
			int numentries=NumEntries();
			double *rightdistances=new double[numentries], *leftdistances=new double[numentries];

			for(i=0; i<numentries; i++) {
				leftdistances[i]=distance(i);
				rightdistances[i]=node->distance(i);
			}
			while((*rightdeletes<numentries*MIN_UTIL)&&(*leftdeletes<numentries*MIN_UTIL)) {	// balance entries up to minimum utilization (assigning to each node its remaining nearest entry)
				i=FindMin(leftdistances, numentries);
				((MTentry *)(*this)[i].Ptr())->Key()->distance=leftdistances[i];
				((MTentry *)(*node)[i].Ptr())->Key()->distance=rightdistances[i];
//				cout << (*this)[i].Ptr() << " (" << leftdistances[i] << ", " << rightdistances[i] << ") to the left\n";
				rightvec[(*rightdeletes)++]=i;
				rightdistances[i]=MAXDOUBLE;
				leftdistances[i]=MAXDOUBLE;
				i=FindMin(rightdistances, numentries);
				if(i>=0) {
					((MTentry *)(*node)[i].Ptr())->Key()->distance=rightdistances[i];
					((MTentry *)(*this)[i].Ptr())->Key()->distance=leftdistances[i];
//					cout << (*node)[i].Ptr() << " (" << leftdistances[i] << ", " << rightdistances[i] << ") to the right\n";
					leftvec[(*leftdeletes)++]=i;
					rightdistances[i]=MAXDOUBLE;
					leftdistances[i]=MAXDOUBLE;
				}
			}
			for(i=0; i<numentries; i++) {	// then perform the hyperplane split (assigning each entry to its nearest node)
				if(rightdistances[i]==MAXDOUBLE) continue;
//				((MTentry *)(*this)[i].Ptr())->Key()->distance=distance(i)+((MTentry *)(*this)[i].Ptr())->maxradius();
//				((MTentry *)(*node)[i].Ptr())->Key()->distance=node->distance(i)+((MTentry *)(*node)[i].Ptr())->maxradius();
				((MTentry *)(*this)[i].Ptr())->Key()->distance=leftdistances[i];
				((MTentry *)(*node)[i].Ptr())->Key()->distance=rightdistances[i];
				if (((MTentry *)(*this)[i].Ptr())->Key()->distance<((MTentry *)(*node)[i].Ptr())->Key()->distance) {
//					cout << (*this)[i].Ptr() << " (" << ((MTentry *)(*this)[i].Ptr())->Key()->distance << ", " << ((MTentry *)(*node)[i].Ptr())->Key()->distance << ") to the left\n";
					rightvec[(*rightdeletes)++]=i;
				}
				else {
//					cout << (*node)[i].Ptr() << " (" << ((MTentry *)(*this)[i].Ptr())->Key()->distance << ", " << ((MTentry *)(*node)[i].Ptr())->Key()->distance << ") to the right\n";
					leftvec[(*leftdeletes)++]=i;
				}
			}
			delete []rightdistances;
			delete []leftdistances;
			break;
		}
		case BAL_G_HYPERPL: {
			int numentries=NumEntries(), j;

			for(i=0; i<NumEntries(); i++) {	// assign the parents' entries
				if(obj==&((MTentry *)((*this)[i].Ptr()))->object()) {
//					cout << (*this)[i].Ptr() << " to the left\n";
					((MTentry *)(*this)[i].Ptr())->Key()->distance=0;
					rightvec[(*rightdeletes)++]=i;
				}
				if(node->obj==&((MTentry *)((*node)[i].Ptr()))->object()) {
//					cout << (*node)[i].Ptr() << " to the right\n";
					((MTentry *)(*node)[i].Ptr())->Key()->distance=0;
					leftvec[(*leftdeletes)++]=i;
				}
			}
			for(i=0; (*rightdeletes<(numentries+1)/2)&&(*leftdeletes<(numentries+1)/2); i++) {	// perform the hyperplane split up to a node utilization of the 50%
				if((obj!=&((MTentry *)((*this)[i].Ptr()))->object())&&(node->obj!=&((MTentry *)((*node)[i].Ptr()))->object())) {	// the parents' entries were already assigned
					((MTentry *)(*this)[i].Ptr())->Key()->distance=distance(i);
					((MTentry *)(*node)[i].Ptr())->Key()->distance=node->distance(i);
					if (((MTentry *)(*this)[i].Ptr())->Key()->distance<((MTentry *)(*node)[i].Ptr())->Key()->distance) {
//						cout << (*this)[i].Ptr() << " (" << ((MTentry *)(*this)[i].Ptr())->Key()->distance << ", " << ((MTentry *)(*node)[i].Ptr())->Key()->distance << ") to the left\n";
						rightvec[(*rightdeletes)++]=i;
					}
					else {
//						cout << (*node)[i].Ptr() << " (" << ((MTentry *)(*this)[i].Ptr())->Key()->distance << ", " << ((MTentry *)(*node)[i].Ptr())->Key()->distance << ") to the right\n";
						leftvec[(*leftdeletes)++]=i;
					}
				}
			}
			// then balance the remaining entries
			for(j=*rightdeletes; j<numentries/2; j++, i++)
				if((obj!=&((MTentry *)((*this)[i].Ptr()))->object())&&(node->obj!=&((MTentry *)((*node)[i].Ptr()))->object())) {	// the parents' entries were already assigned
					((MTentry *)(*this)[i].Ptr())->Key()->distance=distance(i);
					((MTentry *)(*node)[i].Ptr())->Key()->distance=-maxDist();
//					cout << (*this)[i].Ptr() << " (" << ((MTentry *)(*this)[i].Ptr())->Key()->distance << ") to the left\n";
					rightvec[(*rightdeletes)++]=i;
				}
				else j--;
			for(j=*leftdeletes; j<numentries/2; j++, i++)
				if((obj!=&((MTentry *)((*this)[i].Ptr()))->object())&&(node->obj!=&((MTentry *)((*node)[i].Ptr()))->object())) {	// the parents' entries were already assigned
					((MTentry *)(*node)[i].Ptr())->Key()->distance=node->distance(i);
					((MTentry *)(*this)[i].Ptr())->Key()->distance=-maxDist();
//					cout << (*node)[i].Ptr() << " (" << ((MTentry *)(*node)[i].Ptr())->Key()->distance << ") to the right\n";
					leftvec[(*leftdeletes)++]=i;
				}
				else j--;
			break;
		}
		case BALANCED: {	// assign iteratively to each node its remaining nearest entry
			int numentries=NumEntries();
			double *rightdistances=new double[numentries], *leftdistances=new double[numentries];

			for(i=0; i<numentries; i++) {
				leftdistances[i]=distance(i);
				rightdistances[i]=node->distance(i);
			}
			while((*rightdeletes<(numentries+1)/2)&&(*leftdeletes<(numentries+1)/2)) {
				i=FindMin(leftdistances, numentries);
				((MTentry *)(*this)[i].Ptr())->Key()->distance=leftdistances[i];
				((MTentry *)(*node)[i].Ptr())->Key()->distance=rightdistances[i];
//				cout << (*this)[i].Ptr() << " (" << leftdistances[i] << ", " << rightdistances[i] << ") to the left\n";
				rightvec[(*rightdeletes)++]=i;
				rightdistances[i]=MAXDOUBLE;
				leftdistances[i]=MAXDOUBLE;
				i=FindMin(rightdistances, numentries);
				if(i>=0) {
					((MTentry *)(*node)[i].Ptr())->Key()->distance=rightdistances[i];
					((MTentry *)(*this)[i].Ptr())->Key()->distance=leftdistances[i];
//					cout << (*node)[i].Ptr() << " (" << leftdistances[i] << ", " << rightdistances[i] << ") to the right\n";
					leftvec[(*leftdeletes)++]=i;
					rightdistances[i]=MAXDOUBLE;
					leftdistances[i]=MAXDOUBLE;
				}
			}
			delete []rightdistances;
			delete []leftdistances;
			break;
		}
	}
}

GiSTentry *
MTnode::Union() const
{
	GiSTpath path=((MTnode *)this)->Path();
	MTentry *u=new MTentry;
	Object *o=NULL;

	u->InitKey();
	if(obj==NULL) {	// retrieve the node's object
		MTentry *e=Entry();

		((MTnode *)this)->obj=(o=new Object(e->object()));
		delete e;
	}
	if(path.Level()>1) {	// if we aren't in the root...
		MTnode *parent=((MT *)Tree())->ParentNode((MTnode *)this);
		MTentry *e=Entry();

		if(e!=NULL) {	// copy the entry
			u->Key()->distance=e->Key()->distance;
			if(e->Key()->splitted) u->Key()->recomp=TRUE;
			delete e;
		}
		if(u->Key()->distance<0) {	// compute the distance from the parent
			MTentry *fe=parent->Entry();

			if(u->Key()->distance==-maxDist()) u->Key()->distance=obj->distance(fe->object());
			u->Key()->recomp=TRUE;
			delete fe;
		}
		delete parent;
	}
	u->setobject(*obj);
	u->setmaxradius(0);
	u->setminradius(MAXDOUBLE);
	mMRadius(u);	// compute the radii
	if(o!=NULL) delete o;
	((MTnode *)this)->obj=NULL;
	return u;
}

void
MTnode::mMRadius(MTentry *e) const
{
	for (int i=0; i<NumEntries(); i++) {
		MTentry *mte=(MTentry *)(*this)[i].Ptr();

		mte->Key()->recomp=FALSE;
		if (mte->Key()->distance<0) {	// this code should be unreachable
			cout << "Computing distance with " << mte << "??????????????????????????????????????????????\n";	// this code should be unreachable
			mte->Key()->distance=obj->distance(mte->object());
		}
		if (mte->Key()->distance+mte->maxradius()>e->maxradius()) e->setmaxradius(mte->Key()->distance+mte->maxradius());
		if (MAX(mte->Key()->distance-mte->maxradius(), 0)<e->minradius()) e->setminradius(MAX(mte->Key()->distance-mte->maxradius(), 0));
	}
}

GiSTlist<MTentry *>
MTnode::RangeSearch(const MTquery &query)
{
	GiSTlist<MTentry *> result;

	if(IsLeaf())
		for(int i=0; i<NumEntries(); i++) {
			MTentry *e=(MTentry *)(*this)[i].Ptr()->Copy();
			MTquery *q=(MTquery *)query.Copy();

			if(q->Consistent(*e)) {	// object qualifies
				e->setmaxradius(q->Grade());
				result.Append(e);
			}
			else delete e;
			delete q;
		}
	else
		for(int i=0; i<NumEntries(); i++) {
			MTentry *e=(MTentry *)(*this)[i].Ptr();
			MTquery *q=(MTquery *)query.Copy();

			if(q->Consistent(*e)) {	// sub-tree not excluded
				GiSTpath childpath=Path();
				MTnode *child;
				GiSTlist<MTentry *>list;

				childpath.MakeChild(e->Ptr());
				child=(MTnode *)((MT *)Tree())->ReadNode(childpath);
				list=child->RangeSearch(*q);	// recurse the search
				while(!list.IsEmpty()) result.Append(list.RemoveFront());
				delete child;
			}
			delete q;
		}
	return result;
}
