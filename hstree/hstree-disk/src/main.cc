/*
 * main.cc
 *
 *  Created on: 2015-1-16
 *      Author: wangjin
 */

#include "util.h"
#include "storage.h"
#include "index.h"
#include "search.h"
#include "free.h"
#include<sys/time.h>
#include<iostream>
#include<fstream>
using namespace std;

int main(int argc, char** argv){
	timeval tstart, tend;
	    readData(argv[3]);
	    readQuery(argv[4]);
	    sortData();
	    float totalTime = 0;
	    cout<<" minlen: "<<minDictlen<<" maxlen: "<<maxDictlen<<" number of strings: "<<N<<endl;
	     int k = atoi(argv[2]);
	   //  printIndex();
	     initial();
	     string datapath = argv[3];
	     gettimeofday(&tstart, NULL);
	     createIndex(datapath);
	     gettimeofday(&tend, NULL);
	     string indexpath = datapath + ".idx";
	     fprintf(stderr, "#     Index Construction Time : %.3fs\n", tend.tv_sec - tstart.tv_sec +
	        (tend.tv_usec - tstart.tv_usec) * 1.0 / CLOCKS_PER_SEC);
	     string opt = argv[1];
	     if( opt == "-k")
	     {
	    	 printf("Perform Top-k Similarity Search:\n");
	     for(int i = 0; i < static_cast<int>(queries.size()); ++i)
	        {
	          preprocess(queries[i], k, 1);
	          fprintf(stderr, "QUERY %d : %s\n",i, queries[i].c_str());
	          gettimeofday(&tstart, NULL);
	          htopkSearch(indexpath, queries[i], k);
	          gettimeofday(&tend, NULL);
	          totalTime += (tend.tv_sec - tstart.tv_sec +
	        (tend.tv_usec - tstart.tv_usec) * 1.0 / CLOCKS_PER_SEC);
	          while(!results.empty())
	          {
	        	  q_entry ent = results.top();
	        	  printf("%s \t %d\n",dict[ent.id].c_str(),ent.dist);
	        	  results.pop();
	          }
	          printf("#################################################\n");
	        }
	     }
	     else if (opt == "-r")
	     {
	    	 printf("Perform Threshold-based Similarity Search:\n");
	    	 simres = 0;
	         for(int i = 0; i < static_cast<int>(queries.size()); ++i)
	         {
	        	 	 preprocess(queries[i],k,0);
	        	 	//fprintf(stderr, "QUERY %d : %s\n",i, queries[i].c_str());
	        	 	          gettimeofday(&tstart, NULL);
	        	 	          hsimSearch(indexpath, queries[i], k);
	        	 	          gettimeofday(&tend, NULL);
	        	 	          totalTime += (tend.tv_sec - tstart.tv_sec +
	        	 	        (tend.tv_usec - tstart.tv_usec) * 1.0 / CLOCKS_PER_SEC);

	         }
	         cout<<"Number of results: "<<simres<<endl;
	     }
	     else
	     {
	    	 printf("Error! The first argument should be -r or -k !\n");
	     }
         //cout<<"#     Number of I/O:  " <<(datavisited/PGSZ)<<endl;
	     fprintf(stderr, "#     Total Query Time : %.3fs\n", totalTime);
	    freeAll();
    return 0;
}


