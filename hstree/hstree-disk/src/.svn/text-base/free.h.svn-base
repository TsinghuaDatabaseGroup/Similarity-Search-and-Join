/*
 * free.h
 *
 *  Created on: 2014-12-11
 *      Author: wangjin
 */

#ifndef FREE_H_
#define FREE_H_
#include "util.h"
#include "index.h"

void freeAll()
{
	for(int i = 0; i <= maxDictlen; ++i)
	     for(int j = 0;j < maxLevel; ++j)
	   {
	     delete[] partLen[i][j];
	     delete[] partPos[i][j];
	   }
	for(int i = 0; i < maxLevel;++i)
		delete [] substrings[i];
	   for(int i = 0; i <= maxDictlen; ++i)
	   {
	       delete[] partLen[i];
	       delete[] partPos[i];
	   }

	   for(int i = 0;i <= maxDictlen; ++i)
	       delete[] matrix[i];

	   delete[] substrings;
	   delete[] cfilter;
	   delete[] matrix;
	   delete[] partPos;
	   delete[] partLen;
	   delete[] visited;
	   //delete[] matchinfo;
	   //delete[] curmatchpos;

	  cout<<" delete finished!"<<endl;
}


#endif /* FREE_H_ */

/*   for(int i = 0; i< N; ++i)
   {
       delete[] curmatchpos[i];
       delete[] matchinfo[i];
   }*/
