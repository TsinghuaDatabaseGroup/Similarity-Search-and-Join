#ifndef MTOBJECT_H
#define MTOBJECT_H

extern int compdists;
extern int dimension;

#ifndef MIN
#define MIN(x, y) ((x<y)? (x): (y))
#define MAX(x, y) ((x>y)? (x): (y))
#endif

#include <cstdio>
#include <cstring>
#include <iostream>
#include <algorithm>
#include "GiSTdefs.h"
using std::string;
using std::cin;
using std::min;

class Object : public GiSTobject	// the DB object class
{
public:
	string s;	// the coords
	Object() { };	// default constructor (needed)
	Object(const Object& obj) { s = obj.s; }	// copy constructor (needed)
	Object(const string& p_s) { s = p_s; }	// member constructor (needed)
	Object(const char *key) { s = key; }	// member constructor (needed)
	~Object() { }	// destructor
	double area(double r) {return 0;}
	Object& operator=(const Object& obj) { s = obj.s; return *this; }	// assignment operator (needed)
	int operator==(const Object& obj) {return s==obj.s?1:0;}	// equality operator (needed)
	int operator!=(const Object& obj) { return s!=obj.s?1:0; }	// inequality operator (needed)
	double distance(const Object& other) const;	// distance method (needed)
	int CompressedLength() const {return s.length();}	// return the compressed size of this object
	void Compress(char *key) { memcpy(key, s.c_str(), s.length()); }	// object compression
#ifdef PRINTING_OBJECTS
	void Print(ostream& os) const;
#endif
};

int sizeofObject();
double maxDist();	// return the maximum value for the distance between two objects
Object *Read(FILE *fp);	// read an object from a file
Object *Read();	// read an object from standard input
#endif
