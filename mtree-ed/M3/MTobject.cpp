#include <cstdio>
#include "MTobject.h"

extern long long candnum;

int sizeofObject() {return 0;}
double Object::distance(const Object& other) const
{
	candnum++;
	const string& s1 = s;
	const string& s2 = other.s;
  unsigned i, iCrt, iPre, j;
  unsigned
    n = s1.length(),
    m = s2.length();

  if (n == 0)
    return m;
  if (m == 0)
    return n;

  unsigned d[2][m + 1];

  for (j = 0; j <= m; j++)
    d[0][j] = j;

  iCrt = 1;
  iPre = 0;
  for (i = 1; i <= n; i++) {
    d[iCrt][0] = i;
    for (j = 1; j <= m; j++)
      d[iCrt][j] = min(min(d[iPre][j] + 1, d[iCrt][j - 1] + 1),
                       d[iPre][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1));
    iPre = !iPre;
    iCrt = !iCrt;
  }

  return d[iPre][m];
}
Object *Read()
{
	string s;
	cin >> s;
	return new Object(s);
}

Object *Read(FILE *fp)
{
string s;
cerr << "error" << endl;
	return new Object(s);
}


double maxDist()
{
	return 100;
}
#ifdef PRINTING_OBJECTS
void Object::Print(ostream& os) const
{
	os << s << " ";
}
#endif
