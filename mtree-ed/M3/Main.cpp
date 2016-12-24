#include "../../common/time.h"

#include <string>
#include <fstream>
#include <time.h>
#include <malloc.h>
#include <stdio.h>
#ifdef UNIX
#include <unistd.h>
#include <ctype.h>
#endif

#include "MT.h"
#include "MTpredicate.h"

#define IndObjs 11648

using namespace std;

int compdists;
int IOread, IOwrite;
int objs;
int dimension=2;
double eps;

language query_language=FUZZY_STANDARD;
dist2sim hfunction=LINEAR;

extern double MIN_UTIL;
extern pp_function PROMOTE_PART_FUNCTION;
extern pv_function PROMOTE_VOTE_FUNCTION;
extern pp_function SECONDARY_PART_FUNCTION;
extern r_function RADIUS_FUNCTION;
extern int NUM_CANDIDATES;
extern s_function SPLIT_FUNCTION;

MT *gist=NULL;

long long resultnum;
long long candnum;
int debug=0;

// close the tree and delete it
void
CommandClose()
{
	if(gist) {
		delete gist;
		gist=NULL;
	}
}

// create a new tree and open it
void
CommandCreate(const char *method, const char *table)
{
	CommandClose();
	if(!strcmp(method, "mtree")) gist=new MT;
	else {
		cerr << "The only supported method is mtree.\n";
		return;
	}
	gist->Create(table);
	if(!gist->IsOpen()) {
		cout << "Error opening index.\n";
		delete gist;
		return;
	}
}

// delete the tree from disk
void
CommandDrop(const char *table)
{
	unlink(table);
}

// open the specified tree
void
CommandOpen(const char *method, const char *table)
{
	CommandClose();
	if(!strcmp(method, "mtree")) gist=new MT;
	else {
		cerr << "The only supported method is mtree.\n";
		return;
	}
	gist->Open(table);
	if(!gist->IsOpen()) {
		delete gist;
		cout << "Error opening table.\n";
		return;
	}
//	cout << "Table " << table << " opened.\n";
}

// execute a nearest neighbor query
void
CommandNearest(const TopQuery &query)
{
//	cout << "Searching " << query.Pred() << "...\n";
	MTentry **results=gist->TopSearch(query);

	for(unsigned int i=0; i<query.k; i++) {
		MTentry *e=results[i];
		cout << e;
//		cout << e->Ptr() << " ";
		delete e;
		objs++;
	}
//	cout << endl;
	delete []results;
}

// execute a range query
void
CommandSelect(MTquery& query)
{
	GiSTlist<MTentry *> list=gist->RangeSearch(query);

	while(!list.IsEmpty()) {
		MTentry *e=list.RemoveFront();
		// cout << e;
		resultnum++;
		delete e;
		objs++;
	}
/*#if defined(_DEBUG)
	CMemoryState msOld, msNew, msDif;
	msOld.Checkpoint();
#endif
#if defined(_DEBUG)
	msNew.Checkpoint();
	msDif.Difference(msOld, msNew);
	msDif.DumpStatistics();
#endif */
}

// delete the objects satisfying the predicate
void
CommandDelete(const GiSTpredicate& pred)
{
	gist->Delete(pred);
}

// insert an object in the tree
void
CommandInsert(const MTkey& key, int ptr)
{
	gist->Insert(MTentry(key, (GiSTpage)ptr));
//	cout << key << "->" << ptr << " inserted into " << table << ".\n";
}

// use the BulkLoading alogrithm to create the tree
void
CommandLoad(const char *table, MTentry **data, int n)
{
	CommandDrop(table);
	CommandCreate("mtree", table);
//	if(gist==NULL) printf("M3 NULL!!\n");
	printf("MinUtil=%f\n", MIN_UTIL);
	gist->BulkLoad(data, n, 1, "tmp");
}

// close the tree and exit
void
CommandQuit()
{
	CommandClose();
	cout << "Goodbye.\n";
//	exit(0);
}

// print the prompt
void
CommandPrompt()
{
	cout << "MTree> ";
	cout.flush();
}

// activate the debug mode (currently unavailable)
void
CommandDebug()
{
	debug=!debug;
	cout << "Debugging Output ";
	cout << (debug ? "ON\n" : "OFF\n");
}

// print the help file (currently unavailable)
void
CommandHelp()
{
	ifstream is("MTree.help");
	char c;

	while(is.get(c)) cout << c;
}

// perform a check of the nodes of the tree
void
CommandCheck()
{
	GiSTpath path;
	path.MakeRoot();
	gist->CheckNode(path, NULL);
}

// print a dump of each node of the tree
void
CommandDump()
{
	GiSTpath path;
	path.MakeRoot();
#ifdef PRINTING_OBJECTS
	gist->DumpNode(cout, path);
#endif
}

// collect and print statistics on the tree
void
CommandStats()
{
	gist->CollectStats();
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		printf("usage: %s filename threshold\n", argv[0]);
		return -1;
	}
	printf("mtree-ed, ed=%s\n", argv[2]);
	log_start();

	MIN_UTIL=0.2;
	SPLIT_FUNCTION=(s_function)0;
	PROMOTE_PART_FUNCTION=(pp_function)0;
	SECONDARY_PART_FUNCTION=(pp_function)0;
	CommandDrop("graphs.M3");
	CommandCreate("mtree", "graphs.M3");

	string line;
	ifstream in(argv[1]);
	int threshold = atoi(argv[2]);

	while (getline(in, line))
	{
		Object obj(line);

		Pred pred(obj);
		SimpleQuery query(&pred, threshold);
		CommandSelect(query);

		MTkey key(obj, 0, 0);
		CommandInsert(key, 0);
	}

	cout << resultnum << endl;
	cout << candnum << endl;

	return 0;
//	cerr << "Now starting...\n";
//	malloc_stats();
	int i=1;
	char cmdLine[15];
	BOOL end=FALSE;

	compdists=IOread=IOwrite=objs=0;
	cout << "**  MTree: An M-Tree based on Generalized Search Trees\n";
	while(strcmp(cmdLine, "quit")) {
		scanf("%s", cmdLine);
		if(!strcmp(cmdLine, "drop")) {
			CommandDrop("graphs.M3");
			if(argc<5) {
				cout << "Usage is: MTree [min_util] [split_f] [promote_f] [sec_promote_f] ([vote_f] ([n_cand]|[radius_f]))\n";
				exit(-1);
			}
			MIN_UTIL=atof(argv[1]);
			SPLIT_FUNCTION=(s_function)atoi(argv[2]);
			PROMOTE_PART_FUNCTION=(pp_function)atoi(argv[3]);
			SECONDARY_PART_FUNCTION=(pp_function)atoi(argv[4]);
			if(SECONDARY_PART_FUNCTION==CONFIRMED) {
				cout << "The secondary promotion function must be an unconfirmed one\n";
				exit(-1);
			}
			if(PROMOTE_PART_FUNCTION==SAMPLING) {
				if(argc<6) {
					cout << "Usage is: MTree [min_util] [split_f] [promote_f] ([vote_f] ([n_cand]|[radius_f]))\n";
					exit(-1);
				}
				NUM_CANDIDATES=atoi(argv[5]);
			}
			if(PROMOTE_PART_FUNCTION==CONFIRMED) {
				if(argc<6) {
					cout << "Usage is: MTree [min_util] [split_f] [promote_f] [sec_promote_f] ([vote_f] ([n_cand]|[radius_f]))\n";
					exit(-1);
				}
				PROMOTE_VOTE_FUNCTION=(pv_function)atoi(argv[5]);
				if(PROMOTE_VOTE_FUNCTION==SAMPLINGV) {
					if(argc<7) {
						cout << "Usage is: MTree [min_util] [split_f] [promote_f] ([vote_f] ([n_cand]|[radius_f]))\n";
						exit(-1);
					}
					NUM_CANDIDATES=atoi(argv[6]);
				}
				else if(PROMOTE_VOTE_FUNCTION==mM_RAD) {
					if(argc<7) {
						cout << "Usage is: MTree [min_util] [split_f] [promote_f] ([vote_f] ([n_cand]|[radius_f]))\n";
						exit(-1);
					}
					RADIUS_FUNCTION=(r_function)atoi(argv[6]);
				}
			}
			switch(SPLIT_FUNCTION) {
				case G_HYPERPL:
					cout << "G_HYPL, ";
					break;
				case BAL_G_HYPERPL:
					cout << "BAL_G_HYPL, ";
					break;
				case BALANCED:
					cout << "BAL, ";
					break;
			}
			switch(PROMOTE_PART_FUNCTION) {
				case RANDOM:
					cout << "RAN_2 ";
					break;
				case MAX_UB_DIST:
					cout << "M_UB_d ";
					break;
				case SAMPLING:
					cout << "SAMP" << NUM_CANDIDATES << "_2 ";
					break;
				case MIN_RAD:
					cout << "m_R_2 ";
					break;
				case MIN_OVERLAPS:
					cout << "m_O_2 ";
					break;
				case CONFIRMED:
				switch(PROMOTE_VOTE_FUNCTION) {
					case RANDOMV:
						cout << "RAN_1 ";
						break;
					case SAMPLINGV:
						cout << "SAMP" << NUM_CANDIDATES << "_1 ";
						break;
					case MAX_LB_DIST:
						cout << "M_LB_d ";
						break;
					case mM_RAD:
						cout << "mM_";
						switch(RADIUS_FUNCTION) {
							case LB:
								cout << "m";
								break;
							case AVG:
								cout << "A";
								break;
							case UB:
								cout << "M";
								break;
						}
						cout << "_r ";
						break;
				}
				break;
			}
			switch(SECONDARY_PART_FUNCTION) {
				case RANDOM:
					cout << "(RAN_2)\n";
					break;
				case MAX_UB_DIST:
					cout << "(M_UB_d)\n";
					break;
				case SAMPLING:
					cout << "(SAMP" << NUM_CANDIDATES << "_2)\n";
					break;
				case MIN_RAD:
					cout << "(m_R_2)\n";
					break;
				case MIN_OVERLAPS:
					cout << "(m_O_2)\n";
					break;
			}
			CommandCreate("mtree", "graphs.M3");
		}
		else if(!strcmp(cmdLine, "select")) {
			Object *obj=Read();
			Pred *pred=new Pred(*obj);
			double r;

			scanf("%s", cmdLine);
			r=atof(cmdLine);

			SimpleQuery query(pred, r);

			delete obj;
			delete pred;
			if(!gist) CommandOpen("mtree", "graphs.M3");
			CommandSelect(query);
			CommandClose();
		}
		else if((!strcmp(cmdLine, "nearest"))||(!strcmp(cmdLine, "farthest"))) {
			int k;
			BOOL nearest=strcmp(cmdLine, "farthest");
			MTpred *pred;
			Object *obj=Read();

			scanf("%s", cmdLine);
			k=atoi(cmdLine);
			if(nearest) pred=new Pred(*obj);
			else {
				MTpred *npred=new Pred(*obj);

				pred=new NotPred(npred);
				delete npred;
			}
//			eps=atof(argv[1]);
			TopQuery query(pred, k);

			delete pred;
			if(!gist) CommandOpen("mtree", "graphs.M3");
			CommandNearest(query);
			CommandClose();
			delete obj;
		}
		else if(!strcmp(cmdLine, "cursor")) {
			Object *obj=Read();
			Pred pred(*obj);

			if(!gist) CommandOpen("mtree", "graphs.M3");
			MTcursor cursor(*gist, pred);

			scanf("%s", cmdLine);
			while(strcmp(cmdLine, "close")) {
				if(!strcmp(cmdLine, "next")) {
					int k;
					GiSTlist<MTentry *> list;

					scanf("%s", cmdLine);
					k=atoi(cmdLine);

//					cout << "Fetching next " << k << " entries...\n";
					for(; k>0; k--) list.Append(cursor.Next());
					while(!list.IsEmpty()) {
						MTentry *e=list.RemoveFront();
//						cout << e;
						delete e;
						objs++;
					}
				}
				scanf("%s", cmdLine);
			}
			delete obj;
			CommandClose();
		}
/*		else if(!strcmp(cmdLine, "find")) {
			int n, k, l, oldcompdists, oldIOread, oldobjs;

			scanf("%s", cmdLine);
			n=atoi(cmdLine);

			double **x=(double **)calloc(n, sizeof(double *));
			for(i=0; i<n; i++) x[i]=(double *)calloc(dimension, sizeof(double));
			MTpred **p=(MTpred **)calloc(n, sizeof(MTpred *));
			AndPred **ap=(AndPred **)calloc(n-1, sizeof(AndPred *));

			for(i=0; i<n; i++) {
				for(int j=0; j<dimension; j++) {
					scanf("%s", cmdLine);
					x[i][j]=atof(cmdLine);
				}
				if(x[i][0]>=0) {
					Object obj(x[i]);
//					cout << "obj=" << obj << endl;
					p[i]=new Pred(obj);
				}
				else {
					x[i][0]=-x[i][0];
					Object obj(x[i]);
//					cout << "obj=" << obj << endl;
					Pred *pr=new Pred(obj);
					p[i]=new NotPred(pr);
					delete pr;
				}
//				cout << "pred=" << *p[i] << endl;
			}
			if(n==2) cout << "d=" << p[1]->distance(((Pred *)p[0])->obj()) << endl;
			ap[0]=new AndPred(p[0], p[1]);
			for(i=1; i<n-1; i++) ap[i]=new AndPred(ap[i-1], p[i+1]);
//			cout << "Query: " << *ap[n-2] << endl;
			scanf("%s", cmdLine);
			k=atoi(cmdLine);
			compdists=IOread=IOwrite=0;

			TopQuery q(ap[n-2], k);

			if(!gist) CommandOpen("mtree", "graphs.M3");
			CommandNearest(q);
			cout << "Computed dists=" << compdists << "\nIO reads=" << IOread << "\nIO writes=" << IOwrite << "\nObjs=" << objs << endl;

			BOOL (*obs)[IndObjs]=new BOOL [n][IndObjs], pass=FALSE;

			l=-90;
			do {
				int j;

				for(j=0; j<IndObjs; j++)
					for(i=0; i<n; i++) obs[i][j]=FALSE;
				compdists=IOread=IOwrite=objs=0;
				l+=100;
				for(i=0; i<n; i++) {
					TopQuery qi(p[i], l);
					GiSTlist<GiSTobject *> list=gist->TopSearch(qi);

					while(!list.IsEmpty()) {
						MTentry *e=(MTentry *)list.RemoveFront();

						obs[i][e->Ptr()]=TRUE;
						delete e;
					}
				}
				for(j=0; j<IndObjs; j++) {
					BOOL check=TRUE;

					for(i=0; (i<n)&&check; i++) check=obs[i][j];
					if(check) objs++;
				}
//				cout << l << "=>" << objs << endl;
				if(objs>k) {
					pass=TRUE;
					l-=110;
					oldcompdists=compdists;
					oldIOread=IOread;
					oldobjs=objs;
				}
				if(!pass) {
					oldcompdists=compdists;
					oldIOread=IOread;
					oldobjs=objs;
				}
//				else if(objs==0) l+=90; // dangerous: could lead to infinite loops...
			} while(((objs<k)&&!pass)||((objs>k)&&pass));
			cout << l << "=>" << objs << endl;
			if(objs<k) cout << "Computed dists=" << oldcompdists << "\nIO reads=" << oldIOread << "\nObjs=" << oldobjs << endl;
			else cout << "Computed dists=" << compdists << "\nIO reads=" << IOread << "\nObjs=" << objs << endl;
			delete []obs;
			for(i=0; i<n; i++) delete x[i];
			free(x);
			for(i=0; i<n; i++) delete p[i];
			free(p);
			for(i=0; i<n-1; i++) delete ap[i];
			free(ap);
			compdists=IOread=IOwrite=objs=0;
			CommandClose();
		}
*/		else if(!strcmp(cmdLine, "check")) {
			if(!gist) CommandOpen("mtree", "graphs.M3");
			CommandCheck();
			CommandClose();
		}
		else if(!strcmp(cmdLine, "dump")) {
			if(!gist) CommandOpen("mtree", "graphs.M3");
			CommandDump();
			CommandClose();
		}
		else if(!strcmp(cmdLine, "stats")) {
			if(!gist) CommandOpen("mtree", "graphs.M3");
			CommandStats();
			CommandClose();
		}
		else if(!strcmp(cmdLine, "add")) {
			if(!gist) CommandOpen("mtree", "graphs.M3");
			scanf("%s", cmdLine);
			i=atoi(cmdLine);
			if(argc<5) {
				cout << "Usage is: MTree [min_util] [split_f] [promote_f] [sec_promote_f] ([vote_f] ([n_cand]|[radius_f]))\n";
				exit(-1);
			}
			MIN_UTIL=atof(argv[1]);
			SPLIT_FUNCTION=(s_function)atoi(argv[2]);
			PROMOTE_PART_FUNCTION=(pp_function)atoi(argv[3]);
			SECONDARY_PART_FUNCTION=(pp_function)atoi(argv[4]);
			if(SECONDARY_PART_FUNCTION==CONFIRMED) {
				cout << "The secondary promotion function must be an unconfirmed one\n";
				exit(-1);
			}
			if(PROMOTE_PART_FUNCTION==SAMPLING) {
				if(argc<6) {
					cout << "Usage is: MTree [min_util] [split_f] [promote_f] ([vote_f] ([n_cand]|[radius_f]))\n";
					exit(-1);
				}
				NUM_CANDIDATES=atoi(argv[5]);
			}
			if(PROMOTE_PART_FUNCTION==CONFIRMED) {
				if(argc<6) {
					cout << "Usage is: MTree [min_util] [split_f] [promote_f] [sec_promote_f] ([vote_f] ([n_cand]|[radius_f]))\n";
					exit(-1);
				}
				PROMOTE_VOTE_FUNCTION=(pv_function)atoi(argv[5]);
				if(PROMOTE_VOTE_FUNCTION==SAMPLINGV) {
					if(argc<7) {
						cout << "Usage is: MTree [min_util] [split_f] [promote_f] ([vote_f] ([n_cand]|[radius_f]))\n";
						exit(-1);
					}
					NUM_CANDIDATES=atoi(argv[6]);
				}
				else if(PROMOTE_VOTE_FUNCTION==mM_RAD) {
					if(argc<7) {
						cout << "Usage is: MTree [min_util] [split_f] [promote_f] ([vote_f] ([n_cand]|[radius_f]))\n";
						exit(-1);
					}
					RADIUS_FUNCTION=(r_function)atoi(argv[6]);
				}
			}
			switch(SPLIT_FUNCTION) {
				case G_HYPERPL:
					cout << "G_HYPL, ";
					break;
				case BAL_G_HYPERPL:
					cout << "BAL_G_HYPL, ";
					break;
				case BALANCED:
					cout << "BAL, ";
					break;
			}
			switch(PROMOTE_PART_FUNCTION) {
				case RANDOM:
					cout << "RAN_2 ";
					break;
				case MAX_UB_DIST:
					cout << "M_UB_d ";
					break;
				case SAMPLING:
					cout << "SAMP" << NUM_CANDIDATES << "_2 ";
					break;
				case MIN_RAD:
					cout << "m_R_2 ";
					break;
				case MIN_OVERLAPS:
					cout << "m_O_2 ";
					break;
				case CONFIRMED:
					switch(PROMOTE_VOTE_FUNCTION) {
						case RANDOMV:
							cout << "RAN_1 ";
							break;
						case SAMPLINGV:
							cout << "SAMP" << NUM_CANDIDATES << "_1 ";
							break;
						case MAX_LB_DIST:
							cout << "M_LB_d ";
							break;
						case mM_RAD:
							cout << "mM_";
							switch(RADIUS_FUNCTION) {
								case LB:
									cout << "m";
									break;
								case AVG:
									cout << "A";
									break;
								case UB:
									cout << "M";
									break;
							}
							cout << "_r ";
							break;
					}
					break;
			}
			switch(SECONDARY_PART_FUNCTION) {
				case RANDOM:
					cout << "(RAN_2)\n";
					break;
				case MAX_UB_DIST:
					cout << "(M_UB_d)\n";
					break;
				case SAMPLING:
					cout << "(SAMP" << NUM_CANDIDATES << "_2)\n";
					break;
				case MIN_RAD:
					cout << "(m_R_2)\n";
					break;
				case MIN_OVERLAPS:
					cout << "(m_O_2)\n";
					break;
			}
		}
		else if(!strcmp(cmdLine, "insert")) {
			Object *obj=Read();

			if(!gist) CommandOpen("mtree", "graphs.M3");
			CommandInsert(MTkey(*obj, 0, 0), i++);
			delete obj;
		}
		else if(!strcmp(cmdLine, "load")) {
			MTentry **entries;
			int n;

			if(argc<2) {
				cout << "Usage is: MTree [min_util]\n";
				exit(-1);
			}
			MIN_UTIL=atof(argv[1]);
			i=0;
			scanf("%s", cmdLine);
			n=atoi(cmdLine);
			entries=new MTentry*[n];
			for(i=0; i<n; i++) {
				Object *obj=Read();

				entries[i]=new MTentry(MTkey(*obj, 0, 0), i);
				delete obj;
			}
			CommandLoad("graphs.M3", entries, n);
			for(i=0; i<n; i++) delete entries[i];
			delete []entries;
		}
	}
	cout << "Computed dists=" << compdists << "\nIO reads=" << IOread << "\nIO writes=" << IOwrite << "\nObjs=" << objs << endl;
	CommandQuit();
//	cerr << "Now exiting...\n";
//	malloc_stats();
}

