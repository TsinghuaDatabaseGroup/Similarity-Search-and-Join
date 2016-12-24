#include "common/time.h"
#include "adapt_join.h"
#include "GlobFunc.h"

int main(int argc, char* argv[])
{
	// ./adapt-all j filename threshold
	// ./adapt-all ed method filename threshold q
	if (argc != 4 && argc != 6) return -1;
	if (argv[1][0] == 'e' && argc != 6) return -1;
	if (argv[1][0] != 'e' && argc != 4) return -1;
	log_start();
	if (argv[1][0] == 'e')
	{
		string gramtype = string(argv[2]);
		AlgorithmChoice algo;
		if (gramtype == "gram")
		{
			algo = GRAM;
		} else if (gramtype == "indexgram")
		{
			algo = INDEXGRAM;
		} else if (gramtype == "indexchunk")
		{
			algo = INDEXCHUNK;
		} else
		{
			return -2;
		}
		int q = atoi(argv[5]);
		int edth = atoi(argv[4]);
		char* filename = argv[3];
		vector<string> string_set;
		loadStringSet(filename, string_set);
		AdaptJoin adapt_join;
		adapt_join.InitializeParameters(edth, q, algo);
		adapt_join.PerformEdjoin(string_set);
	} else
	{
		double theta = atof(argv[3]);
		char* filename = argv[2];

		AdaptJoin adapt_join;
		adapt_join.InitializeParameters(argv[1], theta);
		adapt_join.PerformSimjoin(filename);
	}
	return 0;
}

