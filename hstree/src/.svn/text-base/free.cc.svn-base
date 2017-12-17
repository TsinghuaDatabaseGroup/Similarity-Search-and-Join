#include "hstree.h"

void deleteAll()
{

   for(int i = 0; i < maxPN; ++i)
     for(int j = 0;j <= maxDictlen; ++j)
   {
     delete[] partLen[i][j];
     delete[] partPos[i][j];
     delete[] ivLists[i][j];
   }
   for(int i = 0; i <= maxDictlen; ++i)
       delete[] partPos[maxPN][i];
   for(int i = 0; i < maxPN; ++i)
   {
       delete[] substrings[i];
       delete[] partLen[i];
       delete[] partPos[i];
       delete[] ivLists[i];
   }
   delete[] partPos[maxPN];
   /*for(int i = 0;i <= maxDictlen; ++i)
       delete[] matrix[i];
   for(int i = 0; i< N; ++i)
   {
       delete[] curmatchpos[i];
       delete[] matchinfo[i];
   }*/

   delete[] substrings;
   delete[] cfilter;
   //delete[] curmatchpos;
   delete[] matrix;
   delete[] partPos;
   delete[] partLen;
   delete[] ivLists;
   delete[] visited;
   //delete[] matchinfo;
   delete[] buckets;

   cout<<"delete finished!"<<endl;
}


