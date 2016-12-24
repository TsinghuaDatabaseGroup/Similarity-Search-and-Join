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

#ifdef UNIX
#include <unistd.h>
#endif
#include "MT.h"

extern double MIN_UTIL;

// find the node having the minimum number of children
// this is used in the redistributing phase of the BulkLoad algorithm
int
FindMin(int *children, int max)
{
	int j, jmin=0;

	for(j=1; j<max; j++)
		if(children[j]<children[jmin]) jmin=j;
	return jmin;
}

// return root level+1 (the height of the tree)
// this is used in the "splitting" phase of the BulkLoad algorithm
int
MT::MaxLevel() const
{
	GiSTnode *root;
	GiSTpath path;
	int i;

	path.MakeRoot();
	root=ReadNode(path);
	i=root->Level();
	delete root;
	return i+1;
}

// split this M-tree into a list of trees having height level
// this is used in the "splitting" phase of the BulkLoad algorithm
GiSTlist<char *> *
MT::SplitTree(int *ncreated, int level, GiSTlist<MTentry *> *children, char *name)
// ncreated is the number of created sub-trees,
// level is the split level for the tree,
// children is the list of the parents of each sub-tree,
// name is the root for the sub-trees names
// the return value is the list of splitted sub-trees
{
	GiSTlist<char *> *trees=new GiSTlist<char *>;	// this is the results list
	GiSTlist<MTnode *> *oldList=new GiSTlist<MTnode *>;	// this is the nodes list
	GiSTpath path;
	MTnode *node=new MTnode;	// this is because the first operation on node is a delete
	char newname[50];

	path.MakeRoot();
	oldList->Append((MTnode *)ReadNode(path));
	do {	// build the roots list
		GiSTlist<MTnode *> *newList=new GiSTlist<MTnode *>;	// this is the list of the current level nodes

		while(!oldList->IsEmpty()) {
			delete node;	// delete the old node created by ReadNode
			node=oldList->RemoveFront();	// retrieve next node to be examined
			path=node->Path();
			for(int i=0; i<node->NumEntries(); i++) {	// append all its children to the new list
				MTentry *e=(MTentry *)(*node)[i].Ptr();

				path.MakeChild(e->Ptr());
				newList->Append((MTnode *)ReadNode(path));
				path.MakeParent();
			}
		}
		delete oldList;
		oldList=newList;
	} while(node->Level()>level);	// stop if we're at the split level
	delete node;
	while(!oldList->IsEmpty()) {	// now append each sub-tree to its root
		MT *subtree=new MT;
		MTnode *newnode;

		node=oldList->RemoveFront();
		sprintf(newname, "%s.%i", name, ++(*ncreated));
		unlink(newname);	// if this M-tree already exists, delete it
		subtree->Create(newname);	// create a new M-tree
		path.MakeRoot();
		newnode=(MTnode *)subtree->ReadNode(path);	// read the root of the tree
		subtree->Append(newnode, (MTnode *)node->Copy());	// append the sub-tree of current node to the root of this M-tree
		children->Append(node->Entry());	// insert the root entry into the list
		trees->Append(strdup(newname));	// insert the new M-tree name into the list
		delete node;
		delete newnode;
		delete subtree;
	}
	delete oldList;
	return trees;
}

// load this M-tree with n data using the BulkLoad algorithm [CP98]
void
MT::BulkLoad(MTentry **data, int n, double padFactor, char *name)
// data is an array of n entries
// padFactor is the maximum node utilization (use 1)
// name is the name of the tree
{
	int i, Size=0, totSize, addEntrySize=(sizeofEntry()? sizeof(GiSTpage): sizeof(GiSTlte)+sizeof(GiSTpage)), minSize=(int)(Store()->PageSize()*MIN_UTIL), NumEntries;	// this is the total size of entries

	if(sizeofEntry()) Size=n*(sizeof(GiSTpage)+sizeofEntry());	// (only valid if we've fixed size entries)
	else for(i=0; i<n; i++) Size+=sizeof(GiSTlte)+sizeof(GiSTpage)+data[i]->CompressedLength();
	totSize=Size+GIST_PAGE_HEADER_SIZE+sizeof(GiSTlte);
	NumEntries=(int)(Store()->PageSize()*padFactor*n)/totSize;
//	cout << "exp. size=" << totSize << endl;
	if(totSize>Store()->PageSize()) {	// we need to split the entries into several sub-trees
		GiSTlist<char *> nameslist, othernameslist;	// list of the sub-trees names
		GiSTlist<MTentry *> plist, parentslist;	// lists of the root entries of each sub-tree
		GiSTlist<int> *lists=NULL;	// list of entries for each sub-tree
		GiSTlist<double> *dists=NULL;	// list of distances for each sub-tree
		char **trees;	// array of the sub-trees names
		GiSTlist<MTnode *> *oldList=new GiSTlist<MTnode *>;
		GiSTpath path;
		MTentry ***arrays;	// array of the entries for each sub-tree
		MTentry **parents;	// array of the root entries for each sub-tree
		MTnode *node=NULL;
		GiSTlist<double *> *distm;	// distance matrix
		int s=(int)MAX(MIN(NumEntries, ceil(((float)n)/NumEntries)), NumEntries*MIN_UTIL);	// initial number of samples
		int j, nsamples, *samples=new int[s], *sizes=NULL, *ns=NULL, ncreated=0, minLevel=MAXINT, nInit, l, iters=0, MAXITER=s*s;
		char newname[50];
		BOOL *sampled=new BOOL[n];	// is this entry in the samples set?

//		cout << "NE*pF=" << NumEntries*padFactor << ", n/NE*pF=" << n/floor(NumEntries*padFactor) << ", s=" << s << endl;
		distm=(GiSTlist<double *> *)calloc(s,sizeof(GiSTlist<double *>));
		do {	// sampling phase
			iters++;
			if(iters>1) {	// this is a new sampling phase
//				cout << "Re-sampling: " << iters << "/" << MAXITER << endl;
				while(!lists[0].IsEmpty()) {
					lists[0].RemoveFront();
					dists[0].RemoveFront();
				}
				delete []lists;
				delete []dists;
				delete []sizes;
				delete []ns;
				while(!distm[0].IsEmpty()) delete []distm[0].RemoveFront();	// empty the distance list
				for(i=1; i<s; i++) distm[i].front=distm[i].rear=NULL;
			}
//			if(iters>=MAXITER) minSize=0;
			if(iters>=MAXITER) {
				cout << "Too many loops in BulkLoad!\nPlease select a lower minimum node utilization or a bigger node size.\n";
				exit(1);
			}
			nsamples=0;
			for(i=0; i<n; i++) sampled[i]=FALSE;
			// pick samples to create parents
			while(nsamples<s) {
				do i=PickRandom(0, n);
				while(sampled[i]);
				sampled[i]=TRUE;
				samples[nsamples++]=i;
			}
//			cout << "Samples:\n";
//			for(i=0; i<s; i++) cout << "\t" << i << ":\t" << data[samples[i]];
			lists=new GiSTlist<int>[s];
			dists=new GiSTlist<double>[s];
			sizes=new int[s];
			ns=new int[s];
			for(i=0; i<s; i++) {
				sizes[i]=GIST_PAGE_HEADER_SIZE+sizeof(GiSTlte);
				ns[i]=1;
			}
			for(i=0; i<s; i++) distm[i].Prepend(new double[s]);
			for(i=0; i<s; i++) {	// compute the relative distances between samples
				for(j=0; j<i; j++)
					distm[j].front->entry[i]=(distm[i].front->entry[j]=data[samples[j]]->object().distance(data[samples[i]]->object()));
				distm[i].front->entry[i]=0;
			}
			for(i=0; i<n; i++) {	// assign each entry to its nearest parent
//				cout << "Now assigning " << data[i];
				if(sampled[i]) {
					for(j=0; samples[j]!=i; j++);
					lists[j].Prepend(i);	// insert the entry in the right list
					dists[j].Prepend(0);
					sizes[j]+=addEntrySize+data[i]->CompressedLength();
//					cout << "\tAssigned (0) to " << j << ", " << data[samples[j]];
				}
				else {	// here we optimize the distance computations (like we do in the insert algorithm)
					double *dist=new double[s];
					int minindex=0;

					dist[0]=data[samples[0]]->object().distance(data[i]->object());
					for(j=1; j<s; j++) {
						BOOL cont=TRUE;

						dist[j]=-1;
						if(fabs(data[samples[j]]->Key()->distance-data[i]->Key()->distance)>dist[minindex]) continue;	// pruning for reference point (parent)
						for(int k=0; (k<j)&&cont; k++)	// pruning for reference points (other samples)
							if(dist[k]<0) continue;
							else cont=(fabs(dist[k]-distm[j].front->entry[k])<dist[minindex]);
						if(!cont) continue;
						dist[j]=data[samples[j]]->object().distance(data[i]->object());	// we have to compute this distance
						if(dist[j]<dist[minindex]) minindex=j;
					}
//					cout << "\tAssigned (" << dist[minindex] << ") to " << minindex << ", " << data[samples[minindex]];
					lists[minindex].Append(i);
					dists[minindex].Append(dist[minindex]);
					sizes[minindex]+=addEntrySize+data[i]->CompressedLength();
					ns[minindex]++;
					if(sizes[minindex]>=minSize) delete []dist;
					else distm[minindex].Append(dist);
				}
			}
			// redistribute underfilled parents
//			cout << "Underfilled parents redistribution...\n";
			while(sizes[i=FindMin(sizes, nsamples)]<minSize) {
				GiSTlist<int> list=lists[i];
				GiSTlist<double *> dlist=distm[i];

				while(!dists[i].IsEmpty()) dists[i].RemoveFront();
//				cout << "Redistributing " << i << "' set (" << sizes[i] << "/" << minSize << ")...\n";
				for(j=0; j<nsamples; j++)
					for(GiSTlistnode<double *> *lnode=distm[j].front; lnode; lnode=lnode->next) lnode->entry[i]=lnode->entry[nsamples-1];
				// substitute this set with last set 
				distm[i]=distm[nsamples-1];
				lists[i]=lists[nsamples-1];
				dists[i]=dists[nsamples-1];
				samples[i]=samples[nsamples-1];
				sizes[i]=sizes[nsamples-1];
				ns[i]=ns[nsamples-1];
				nsamples--;
				while(!list.IsEmpty()) {	// assign each entry to its nearest parent
					double *d=dlist.RemoveFront();
					int k=list.RemoveFront(), index, minindex=-1;
//					cout << "Now assigning " << data[k];

					for(index=0; (index<nsamples)&&(minindex<0); index++)	// search for a computed distance
						if(d[index]>0) minindex=index;
					if(minindex<0) {	// no distance was computed (i.e. all distances were pruned)
						d[0]=data[samples[0]]->object().distance(data[k]->object());
						minindex=0;
					}
					for(index=0; index<nsamples; index++) {
						BOOL cont=TRUE;

						if(index==minindex) continue;
						if(d[index]<0) {	// distance wasn't computed
							if(fabs(data[samples[index]]->Key()->distance-data[k]->Key()->distance)>d[minindex]) continue;	// pruning for reference point (parent)
							for(l=0; (l<index)&&cont; l++)	// pruning for reference points (other samples)
								if(d[l]<0) continue;
								else cont=(fabs(d[l]-distm[index].front->entry[l])<d[minindex]);
							if(!cont) continue;
							d[index]=data[samples[index]]->object().distance(data[k]->object());	// we have to compute this distance
						}
						if(d[index]<d[minindex]) minindex=index;
					}
//					cout << "\tAssigned (" << d[minindex] << ") to " << minindex << ", " << data[samples[minindex]];
					lists[minindex].Append(k);
					dists[minindex].Append(d[minindex]);
					sizes[minindex]+=addEntrySize+data[k]->CompressedLength();
					ns[minindex]++;
					if(sizes[minindex]>=minSize) delete []d;
					else distm[minindex].Append(d);
				}
				assert(dlist.IsEmpty());
			}
		} while(nsamples==1);	// if there's only one child, repeat the sampling phase
//		cout << "Samples:\n";
//		for(i=0; i<nsamples; i++) cout << "\t" << i << ": " << data[samples[i]];
		arrays=new MTentry **[nsamples];
		for(i=0; i<nsamples; i++) {	// convert the lists into arrays
			arrays[i]=new MTentry *[ns[i]];
			for(j=0; j<ns[i]; j++) {
				int k=lists[i].RemoveFront();

				arrays[i][j]=(MTentry *)data[k]->Copy();
				arrays[i][j]->Key()->distance=dists[i].RemoveFront();
			}
			assert(lists[i].IsEmpty());
			assert(dists[i].IsEmpty());
		}
		delete []dists;
		delete []lists;
		for(i=0; i<nsamples; i++)
			while(!distm[i].IsEmpty())
				delete [](distm[i].RemoveFront());
		free(distm);
		// build an M-tree under each parent
		nInit=nsamples;
//		cout << "Now building " << nsamples << " sub-trees...\n";
		MT *tree=new MT;

//		for(i=0; i<nsamples; i++) cout << i+1 << "' set: " << ns[i] << endl;
		for(i=0; i<nInit; i++) {
			MTnode *root;
			GiSTpath path;

			sprintf(newname, "%s.%i", name, ++ncreated);
			unlink(newname);
			tree->Create(newname);	// create the new sub-tree
//			cout << "Now building sub-tree " << newname << " on " << ns[i] << " data (exp. size=" << sizes[i] << ")...\n";
			tree->BulkLoad(arrays[i], ns[i], padFactor, newname);	// insert the data into the sub-tree
//			cout << "Tree level=" << tree->MaxLevel() << endl;
			// if the root node is underfilled, we have to split the tree
			path.MakeRoot();
			root=(MTnode *)tree->ReadNode(path);
			if(root->IsUnderFull(*Store())) {
				GiSTlist<MTentry *> *roots=new GiSTlist<MTentry *>;
				GiSTlist<char *> *list=tree->SplitTree(&ncreated, tree->MaxLevel()-1, roots, name);	// split the tree

				nsamples--;
				while(!list->IsEmpty()) {	// insert all the new trees in the sub-trees list
					MTentry *e=roots->RemoveFront();

					othernameslist.Append(list->RemoveFront());
					for(j=0; j<n; j++) if(data[j]->object()==e->object()) {	// append also the root entry to the parents list
//						cout << "parent=" << data[j];
						plist.Append(data[j]);
						j=n;
					}
					delete e;
					nsamples++;
				}
				delete list;
				delete roots;
				minLevel=MIN(minLevel, tree->MaxLevel()-1);
			}
			else {
				char *tmp=new char[50];

				strcpy(tmp, newname);
				othernameslist.Append(tmp);
				plist.Append(data[samples[i]]);
				minLevel=MIN(minLevel, tree->MaxLevel());
			}
			delete root;
			tree->Close();
			delete tree->Store();	// it was created in tree->Create()
		}
		for(i=0; i<nInit; i++)  {
			for(j=0; j<ns[i]; j++) delete arrays[i][j];
			delete []arrays[i];
		}
		delete []arrays;
		while(!plist.IsEmpty()) {	// insert the trees in the list (splitting if necessary)
			MTentry *parent=plist.RemoveFront();
			char *tmp=othernameslist.RemoveFront();

			strcpy(newname, tmp);
			delete []tmp;
			tree->Open(newname);
//			cout << ": Tree level=" << tree->MaxLevel() << " (" << minLevel << ")\n";
			if(tree->MaxLevel()>minLevel) {	// we have to split the tree to reduce its height
//			cout << "level too high!!! (min=" << minLevel << ") Splitting the tree...\n";
				GiSTlist<MTentry *> *roots=new GiSTlist<MTentry *>;
				GiSTlist<char *> *list=tree->SplitTree(&ncreated, minLevel, roots, name);	// split the tree

				nsamples--;
				while(!list->IsEmpty()) {	// insert all the new trees in the sub-trees list
					MTentry *e=roots->RemoveFront();

					nameslist.Append(list->RemoveFront());
					for(j=0; j<n; j++) if(data[j]->object()==e->object()) {	// append also the root entry to the parents list
//						cout << "parent=" << data[j];
						parentslist.Append(data[j]);
						j=n;
					}
					delete e;
					nsamples++;
				}
				delete list;
				delete roots;
			}
			else {	// simply insert the tree and its root to the lists
				char *tmp=new char[50];

				strcpy(tmp, newname);
				nameslist.Append(tmp);
//				cout << "parent=" << data[samples[i]];
				parentslist.Append(parent);
			}
			tree->Close();
			delete tree->Store();	// it was created in tree->Open()
		}
		parents=new MTentry *[nsamples];
		trees=new char *[nsamples];
		for(i=0; i<nsamples; i++) {	// convert the lists into arrays
			trees[i]=nameslist.RemoveFront();
			parents[i]=parentslist.RemoveFront();
		}
		// build the super-tree upon the parents
		sprintf(newname, "%s.0", name);
//		cout << "Now building super-tree " << newname << " on " << nsamples << " data...\n";
		BulkLoad(parents, nsamples, padFactor, newname);
		// attach each sub-tree to the leaves of the super-tree
		path.MakeRoot();
		node=(MTnode *)ReadNode(path);
		oldList->Append(node);
//		cout << "super-tree built!\n";
		l=node->Level();
		while(l>0) {	// build the leaves list for super-tree
			GiSTlist<MTnode *> *newList=new GiSTlist<MTnode *>;

			while(!oldList->IsEmpty()) {
				node=oldList->RemoveFront();
				path=node->Path();
				node->SetLevel(node->Level()+minLevel);	// update level of the upper nodes of the super-tree
				WriteNode(node);
				for(i=0; i<node->NumEntries(); i++) {
					MTentry *e=(MTentry *)(*node)[i].Ptr();

					path.MakeChild(e->Ptr());
					newList->Append((MTnode *)ReadNode(path));
					path.MakeParent();
				}
				delete node;
			}
			delete oldList;
			oldList=newList;
			l--;
		}
//		cout << "Finished " << newname << endl;
		while(!oldList->IsEmpty()) {	// attach each sub-tree to its leaf
			GiSTpath rootpath;

			rootpath.MakeRoot();
			node=oldList->RemoveFront();	// retrieve next leaf (root of sub tree)
			node->SetLevel(minLevel);	// update level of the root of the sub-tree
			path=node->Path();
			for(i=0; i<node->NumEntries(); i++) {
				MTnode *newnode=(MTnode *)CreateNode();
				MTentry *e=(MTentry *)(*node)[i].Ptr();
				GiSTpath newpath;

				path.MakeChild(Store()->Allocate());
				newnode->Path()=path;
				e->SetPtr(path.Page());
				path.MakeParent();
				for(j=0; e->object()!=parents[j]->object(); j++);	// search the tree to append
				tree->Open(trees[j]);
//				cout << "Now appending sub-tree " << trees[j] << endl;
				Append(newnode, (MTnode *)tree->ReadNode(rootpath));	// append this sub-tree to the super-tree
				tree->Close();
				delete tree->Store();	// it was created in tree->Open()
				newpath=newnode->Path();
				delete newnode;
			}
			WriteNode(node);
			delete node;
		}
		tree->Open(trees[0]);	// in order to destroy the object tree
		delete tree;
		for(i=0; i<nsamples; i++) delete []trees[i];
		delete []trees;
		// update radii of the upper nodes of the result M-tree
		path.MakeRoot();
		node=(MTnode *)ReadNode(path);
		oldList->Append(node);
		l=node->Level();
		while(l>=minLevel) {	// build the list of the nodes which radii should be recomputed
			GiSTlist<MTnode *> *newList=new GiSTlist<MTnode *>;

			while(!oldList->IsEmpty()) {

				node=oldList->RemoveFront();
				path=node->Path();
				for(i=0; i<node->NumEntries(); i++) {
					MTentry *e=(MTentry *)(*node)[i].Ptr();

					path.MakeChild(e->Ptr());
					newList->Append((MTnode *)ReadNode(path));
					path.MakeParent();
				}
				delete node;
			}
			delete oldList;
			oldList=newList;
			l--;
		}
		while(!oldList->IsEmpty()) {	// adjust the radii of the nodes
			MTnode *node=oldList->RemoveFront();

			AdjKeys(node);
			delete node;
		}
		// be tidy...
		delete oldList;
		delete []parents;
		delete []sizes;
		delete []ns;
		delete []sampled;
		delete []samples;
		for(i=0; i<=ncreated; i++) {	// delete all temporary sub-trees
			  sprintf(newname, "%s.%i", name, i);
			  unlink(newname);
		}
	}
	else {	// we can insert all the entries in a single node
		GiSTpath path;
		GiSTnode *node;

		path.MakeRoot();
		node=ReadNode(path);
		for(i=0; i<n; i++)
			node->Insert(*(data[i]));
		assert(!node->IsOverFull(*Store()));
//		cout << "real size=" << node->Size() << endl;
		WriteNode(node);
		delete node;
	}
}

// append the sub-tree rooted at from to the node to
// this is used in the "append" phase of the BulkLoad algorithm
void
MT::Append(MTnode *to, MTnode *from)
{
	GiSTlist<MTnode *> *oldList=new GiSTlist<MTnode *>;	// list of the nodes to append
	GiSTlist<GiSTpath> pathList;
	MT *fromtree=(MT *)from->Tree();
	MTnode *node=new MTnode, *newnode;

//	cout << "Appending " << from;
	oldList->Append(from);
	pathList.Append(to->Path());
	do {
		GiSTlist<MTnode *> *newList=new GiSTlist<MTnode *>;

		while(!oldList->IsEmpty()) {
			GiSTpath newpath=pathList.RemoveFront();

			delete node;
			node=oldList->RemoveFront();
			newnode=(MTnode *)ReadNode(newpath);
//			cout << "Inserting " << node->NumEntries() << " entries:\n";
			for(int i=0; i<node->NumEntries(); i++) {
				MTentry *e=(MTentry *)(*node)[i].Ptr()->Copy();

				if(node->Level()>0) {	// if node isn't a leaf, we've to allocate its children
					GiSTpath nodepath=node->Path();
					MTnode *childnode=(MTnode *)CreateNode(), *fromnode;

					nodepath.MakeChild(e->Ptr());
					fromnode=(MTnode *)fromtree->ReadNode(nodepath);
					newList->Append(fromnode);
					e->SetPtr(Store()->Allocate());
					newpath.MakeChild(e->Ptr());
					childnode->Path()=newpath;
					childnode->SetTree(this);
					WriteNode(childnode);	// write the empty node
					pathList.Append(newpath);
					newpath.MakeParent();
					nodepath.MakeParent();
					delete childnode;
				}
				newnode->Insert(*e);
//				cout << "\tInserted " << e;
				delete e;
			}
			newnode->SetLevel(node->Level());
			WriteNode(newnode); // write the node
//			cout << "Created " << newnode;
			delete newnode;
		}
		delete oldList;
		oldList=newList;
	} while(node->Level()>0);	// until we reach the leaves' level
//	cout << node;
	delete node;
	delete oldList;
}

// adjust the keys of node
// this is used during the final phase of the BulkLoad algorithm
void
MT::AdjKeys(GiSTnode *node)
{
	GiSTnode *P;
	GiSTpath parent_path=node->Path();
	GiSTentry *entry, *actual;

	if(node->Path().IsRoot()) return;
	parent_path.MakeParent();
	P=ReadNode(parent_path);
	entry=P->SearchPtr(node->Path().Page());
	assert(entry!=NULL);
	actual=node->Union();
	actual->SetPtr(node->Path().Page());
	((MTkey *)actual->Key())->distance=((MTkey *)entry->Key())->distance;	// necessary to keep track of the distance from the parent
	if(!entry->IsEqual(*actual)) {	// replace this entry
		int pos=entry->Position();

		P->DeleteEntry(pos);
		P->Insert(*actual);
/*		if(P->IsOverFull(*Store())) {	// this code should be unnecessary (if we have fixed length entries)
		  GiSTpage page=node->Path().Page();

		  Split(&P, *actual);
		  node->Path()=P->Path();
		  node->Path().MakeChild(page);
		}
		else {
		  WriteNode(P);
		  AdjKeys(P);
		}	*/
		WriteNode(P);
		AdjKeys(P);
	}
	delete P;
	delete actual;
	delete entry;
}
