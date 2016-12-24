#ifndef _HELP_H_
#define _HELP_H_

#include <string>
#include <algorithm>
#include <type_traits>
#include <cstdlib>

template <typename T>
void alloc2d(T **&p, int m, int n)
{
	p = new T*[m];
	T *space = new T[m * n];
	for (int k = 0; k < m; k++)
	{
		p[k] = &(space[k * n]);
	}
}

template <typename T>
void destroy2d(T **p)
{
	delete[] p[0];
	delete[] p;
}

template <typename T>
typename std::enable_if<std::is_arithmetic<T>::value, T>::type
min3(T a, T b, T c)
{
	return std::min(a, std::min(b, c));
}

template <typename T>
typename std::enable_if<std::is_arithmetic<T>::value, T>::type
max3(T a, T b, T c)
{
	return std::max(a, std::max(b, c));
}

inline int edit_distance(const std::string &s1, const std::string &s2, int THRESHOLD,
		int xpos = 0, int ypos = 0, int xlen = -1, int ylen = -1)
{
	if (xlen == -1) xlen = s1.length() - xpos;
	if (ylen == -1) ylen = s2.length() - ypos;
	if (xlen > ylen + THRESHOLD || ylen > xlen + THRESHOLD) return THRESHOLD + 1;
	if (xlen == 0) return ylen;

	int matrix[xlen + 1][2 * THRESHOLD + 1];
	for (int k = 0; k <= THRESHOLD; k++) matrix[0][THRESHOLD + k] = k;

	int right = (THRESHOLD + (ylen - xlen)) / 2;
	int left = (THRESHOLD - (ylen - xlen)) / 2;
	for (int i = 1; i <= xlen; i++)
	{
		bool valid = false;
		if (i <= left)
		{
			matrix[i][THRESHOLD - i] = i;
			valid = true;
		}
		for (int j = (i - left >= 1 ? i - left : 1);
				j <= (i + right <= ylen ? i + right : ylen); j++)
		{
			if (s1[xpos + i - 1] == s2[ypos + j - 1])
				matrix[i][j - i + THRESHOLD] = matrix[i - 1][j - i + THRESHOLD];
			else
				matrix[i][j - i + THRESHOLD] = min3(matrix[i - 1][j - i + THRESHOLD],
						j - 1 >= i - left ? matrix[i][j - i + THRESHOLD - 1] : THRESHOLD,
						j + 1 <= i + right ? matrix[i - 1][j - i + THRESHOLD + 1] : THRESHOLD) + 1;
			if (abs(xlen - ylen - i + j) + matrix[i][j - i + THRESHOLD] <= THRESHOLD) valid = true;
		}
		if (!valid) return THRESHOLD + 1;
	}
	return matrix[xlen][ylen - xlen + THRESHOLD];
}

inline int DJB_hash(const char *str, int len, int extra)
{
	unsigned hash = 5381;

	for (int k = 0; k < len; k++)
	{
		hash += (hash << 5) + str[k];
	}

	for (int k = 0; k < len; k++)
	{
		hash += (hash << 5) + '$';
	}

	return (hash & 0x7FFFFFFF);
}

inline int DJB_hash(const char *str, int len)
{
	unsigned hash = 5381;

	for (int k = 0; k < len; k++)
	{
		hash += (hash << 5) + str[k];
	}

	return (hash & 0x7FFFFFFF);
}

inline int DJB_hash(const char *str)
{
	unsigned hash = 5381;

	while (*str)
	{
		hash += (hash << 5) + (*str++);
	}

	return (hash & 0x7FFFFFFF);
}

#endif

