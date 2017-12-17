#include "hstree.h"

#include <algorithm>
#include <cmath>
#include <string.h>
#include <string>
#include <vector>
#include <set>
#include <cstdlib>
#include <sys/time.h>
using namespace std;


int main(int argc, char** argv)
{
    timeval tstart, tend;
    readData(argv[3]);
    readQuery(argv[4]);
    float totalTime = 0;
    cout<<" minlen: "<<minDictlen<<" maxlen: "<<maxDictlen<<" number of strings: "<<N<<endl;
     int k = atoi(argv[2]);
   //  printIndex();
     sortData();
     initial();
     gettimeofday(&tstart, NULL);
     createIndex();
     gettimeofday(&tend, NULL);
     fprintf(stderr, "#     Index Construction Time : %.3fs\n", tend.tv_sec - tstart.tv_sec +
        (tend.tv_usec - tstart.tv_usec) * 1.0 / CLOCKS_PER_SEC);
     string opt = argv[1];
     if( opt == "-k")
     {
    	 printf("Perform Top-k Similarity Search:\n");
     for(int i = 0; i < static_cast<int>(queries.size()); ++i)
        {
          pretopk(queries[i], k);
          fprintf(stderr, "QUERY %d : %s\n",i, queries[i].c_str());
          gettimeofday(&tstart, NULL);
          hstopk(queries[i], k);
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
       printf("#     Number of verifications : %d\n", cands);
     }
     else if (opt == "-r")
     {
    	 printf("Perform Threshold-based Similarity Search:\n");
    	 simres = 0;
         for(int i = 0; i <  static_cast<int>(queries.size()); ++i)
         {
        	 	 presim(queries[i],k);
        	 	//fprintf(stderr, "QUERY %d : %s\n",i, queries[i].c_str());
        	 	          gettimeofday(&tstart, NULL);
        	 	          hsearch(queries[i], k);
        	 	          gettimeofday(&tend, NULL);
        	 	          totalTime += (tend.tv_sec - tstart.tv_sec +
        	 	        (tend.tv_usec - tstart.tv_usec) * 1.0 / CLOCKS_PER_SEC);
         }
         printf("#   Number of results: %d\n",simres);
     }
     else
     {
    	 printf("Error! The first argument should be -r or -k !\n");
     }
     fprintf(stderr, "#     Total Search Time : %.3fs\n", totalTime);
    deleteAll();
    return 0;
}
