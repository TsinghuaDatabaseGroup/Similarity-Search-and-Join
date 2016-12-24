#include "Verification.h"



Verification::~Verification(void)
{
}


int Verification::levenshtein_distance(const char *s, const char *t)
{
	// d is a table with m+1 rows and n+1 columns
	static int d[_MAX_STRING_LEN + 1][_MAX_STRING_LEN + 1];
	int m = strlen(s);
	int n = strlen(t);

	for (int i = 0; i <= m; ++i)
		d[i][0] = i;
	for (int j = 0; j <= n; ++j)
		d[0][j] = j;

	for (int i = 1; i <= m; ++i)
		for (int j = 1; j <= n; ++j) {
			int cost;

			cost = s[i-1] == t[j-1] ? 0 : 1;

			d[i][j] = d[i-1][j] < d[i][j-1]? d[i-1][j] + 1 : d[i][j-1] + 1;
			d[i][j] = d[i][j] < d[i-1][j-1] + cost ?
				d[i][j] : d[i-1][j-1] + cost;
		}

		return d[m][n];
}

int Verification::verifyEd(const char* s, const char* t)
{
	//cout << s << " " << t << endl;
	int lens = strlen(s);
	int lent = strlen(t);
	if (!lens || !lent)
		return -1;
	if (lens>lent&&lens-lent>ed)
		return -1;
	if (lent>lens&&lent-lens>ed)
		return -1;
	uchar* d1 = matrix1; //'previous' cost array, horizontally
	uchar* d2 = matrix2; // cost array, horizontally
	uchar* _d;  //placeholder to assist in swapping d1 and d2
	memset(d2, -1, lent+1);
	for (int i=0; i<=lent; ++i)
	{
		d1[i] = i;
	}
	//cout << endl;
	for (int i=1; i<=lens; ++i)
	{
		int begin_j = max(i-ed,1);
		int end_j = min(i+ed,lent);
		d2[begin_j-1] = i;
		bool flag = false;
		for (int j=begin_j; j<=end_j; ++j)
		{
			d2[j] = min( min(d1[j],d2[j-1])+1 ,  d1[j-1]+(s[i-1]==t[j-1]?0:1) );
			if (d2[j]<=ed)
				flag = true;
		}
		_d = d1;
		d1 = d2;
		d2 = _d;
		if (!flag)
			return -1;
	}
	if (d1[lent] <= ed)
		return d1[lent];
	else
		return -1;


}




