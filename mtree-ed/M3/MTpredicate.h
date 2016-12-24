/*********************************************************************
*                                                                    *
* Copyright (c) 1997,1998, 1999                                      *
* Multimedia DB Group and DEIS - CSITE-CNR,                          *
* University of Bologna, Bologna, ITALY.                             *
*                                                                    *
* All Rights Reserved.                                               *
*                                                                    *
* Permission to use, copy, and distribute this software and its      *
* documentation for NON-COMMERCIAL purposes and without fee is       *
* hereby granted provided  that this copyright notice appears in     *
* all copies.                                                        *
*                                                                    *
* THE AUTHORS MAKE NO REPRESENTATIONS OR WARRANTIES ABOUT THE        *
* SUITABILITY OF THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING  *
* BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY,      *
* FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT. THE AUTHOR  *
* SHALL NOT BE LIABLE FOR ANY DAMAGES SUFFERED BY LICENSEE AS A      *
* RESULT OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS    *
* DERIVATIVES.                                                       *
*                                                                    *
*********************************************************************/

#ifndef MTPREDICATE_H
#define MTPREDICATE_H

#include "MTobject.h"

//class GiSTentry;

typedef enum {
	FUZZY_STANDARD,
	FUZZY_ALGEBRAIC
} language;

typedef enum {
	LINEAR,
	EXPONENTIAL,
	DISTR
} dist2sim;

extern language query_language;

double Dist2Sim(double dist);
double Sim2Dist(double sim);

class MTpred: public GiSTobject {	// the base class for predicates
public:
	virtual GiSTobjid IsA() { return MTPREDICATE_CLASS; }
	virtual double distance(const Object &obj) const=0;
	virtual ~MTpred() {}

#ifdef PRINTING_OBJECTS
	virtual void Print(ostream& os) const=0;
#endif
};

class Pred: public MTpred {	// a simple predicate
public:
	// constructors, destructors, etc.
	Pred(const Object &obj): object(obj) {}
	Pred(const Pred& p) : object(p.object) {}
	GiSTobject *Copy() const { return new Pred(*this); }

	// virtual methods
	GiSTobjid IsA() { return MTPREDICATE_CLASS; }
	double distance(const Object &obj) const { 
		return object.distance(obj);
	}

	// access to private methods
	Object obj() const { return object; }

#ifdef PRINTING_OBJECTS
	void Print(ostream& os) const {
		os << object;
	}
#endif

private:
	Object object;
};

class AndPred: public MTpred {	// a conjunction of two predicates
public:
	// constructors, destructors, etc.
	AndPred(const MTpred *p1, const MTpred *p2): pred1((MTpred *)p1->Copy()), pred2((MTpred *)p2->Copy()) {}
	GiSTobject *Copy() const {
		return new AndPred(pred1, pred2);
	}
	~AndPred() {
		delete pred1;
		delete pred2;
	}

	// virtual methods
	GiSTobjid IsA() { return MTPREDICATE_AND_CLASS; }
	double distance(const Object &obj) const {
		switch(query_language) {
			case FUZZY_STANDARD: {
				double s1=Dist2Sim(pred1->distance(obj)), s2=Dist2Sim(pred2->distance(obj));

				return Sim2Dist(MIN(s1, s2));
			}
			case FUZZY_ALGEBRAIC:	return Sim2Dist(Dist2Sim(pred1->distance(obj))*Dist2Sim(pred2->distance(obj)));
			default:	return maxDist();
		}
	}

	// access to private members
	MTpred *Pred1() const { return pred1; }
	MTpred *Pred2() const { return pred2; }

#ifdef PRINTING_OBJECTS
	void Print(ostream& os) const {
		os << "(" << *pred1 << "&&" << *pred2 << ")";
	}
#endif

private:
	MTpred *pred1, *pred2;
};

class OrPred: public MTpred {	// a disjunction of two predicates
public:
	// constructors, destructors, etc.
	OrPred(const MTpred *p1, const MTpred *p2): pred1((MTpred *)p1->Copy()), pred2((MTpred *)p2->Copy()) {}
	GiSTobject *Copy() const {
		return new OrPred(pred1, pred2);
	}
	~OrPred() {
		delete pred1;
		delete pred2;
	}

	// virtual methods
	GiSTobjid IsA() { return MTPREDICATE_OR_CLASS; }
	double distance(const Object &obj) const { 
		switch(query_language) {
			case FUZZY_STANDARD: {
				double s1=Dist2Sim(pred1->distance(obj)), s2=Dist2Sim(pred2->distance(obj));

				return Sim2Dist(MAX(s1, s2));
			}
			case FUZZY_ALGEBRAIC: {
				double s1=Dist2Sim(pred1->distance(obj)), s2=Dist2Sim(pred2->distance(obj));

				return Sim2Dist(s1+s2-s1*s2);
			}
			default:	return maxDist();
		}
	}

	// access to private members
	MTpred *Pred1() const { return pred1; }
	MTpred *Pred2() const { return pred2; }

#ifdef PRINTING_OBJECTS
	void Print(ostream& os) const {
		os << "(" << *pred1 << "||" << *pred2 << ")";
	}
#endif

private:
	MTpred *pred1, *pred2;
};

class NotPred: public MTpred {	// a negated predicate
public:
	// constructors, destructors, etc.
	NotPred(const MTpred *p): pred((MTpred *)p->Copy()) {}
	GiSTobject *Copy() const {
		return new NotPred(pred);
	}
	~NotPred() { delete pred; }

	// virtual methods
	GiSTobjid IsA() { return MTPREDICATE_NOT_CLASS; }
	double distance(const Object &obj) const { 
		return Sim2Dist(1-Dist2Sim(pred->distance(obj)));
	}

	// access to private members
	MTpred *Pred() const { return pred; }

#ifdef PRINTING_OBJECTS
	void Print(ostream& os) const {
		os << "(!" << *pred << ")";
	}
#endif

private:
	MTpred *pred;
};

// for the queries, the conjunction corresponds to intersection, the disjunction to union, and the negation to difference

class MTquery: public GiSTobject {	// the base class for queries
public:
	// constructors, destructors, etc.
	MTquery() : grade(0), isOpen(FALSE) {}
	MTquery(const double g, const BOOL o=FALSE) : grade(g), isOpen(o) {}
	virtual ~MTquery() {}

	// pure virtual methods
	virtual int Consistent(const GiSTentry& entry)=0;
	virtual int NonConsistent(const GiSTentry& entry)=0;

	// access to private members
	double Grade() const { return grade; }
	void SetGrade(double p_grade) { grade=p_grade; }

protected:
	double grade;
	BOOL isOpen;
};

class SimpleQuery : public MTquery {	// a simple query
public:
	// constructors, destructors, etc.
	SimpleQuery(const MTpred *p, const double r, const BOOL o=FALSE) : MTquery(0,o), pred((MTpred *)p->Copy()), radius(r) {}
	SimpleQuery(const SimpleQuery& q) : MTquery(q.grade, q.isOpen), pred((MTpred *)q.pred->Copy()), radius(q.radius) {}
	GiSTobject *Copy() const {
		return new SimpleQuery(*this);
	}
	~SimpleQuery() { delete pred; }

	// basic consistency methods
	int Consistent(const GiSTentry& entry);
	int NonConsistent(const GiSTentry& entry);

	// access to private members
	double Radius() const { return radius; }
	void SetRadius(double p_radius) { radius=p_radius; }

private:
	MTpred *pred;
	double radius;
};

class AndQuery : public MTquery {	// the conjunction of two queries
public:
	// constructors, destructors, etc.
	AndQuery(const MTquery *q1, const MTquery *q2): q1((MTquery *)q1->Copy()), q2((MTquery *)q2->Copy()) {}
	GiSTobject *Copy() const {
		return new AndQuery(q1, q2);
	}
	~AndQuery() {
		delete q1;
		delete q2;
	}

	// basic consistency methods
	int Consistent(const GiSTentry& entry) {
		int response=q1->Consistent(entry)&&q2->Consistent(entry);

		grade=MAX(q1->Grade(), q2->Grade());
		return response;
	}
	int NonConsistent(const GiSTentry& entry) {
		int response=q1->NonConsistent(entry)||q2->NonConsistent(entry);

		grade=MAX(q1->Grade(), q2->Grade());	// or what?
		return response;
	}

#ifdef PRINTING_OBJECTS
	void Print(ostream& os) const {
		os << "(" << *q1 << " and " << *q2 << ")";
	}
#endif

private:
	MTquery *q1, *q2;
};

class OrQuery : public MTquery {	// the disjunction of two queries
public:
	// constructors, destructors, etc.
	OrQuery(const MTquery *q1, const MTquery *q2): q1((MTquery *)q1->Copy()), q2((MTquery *)q2->Copy()) {}
	GiSTobject *Copy() const {
		return new OrQuery(q1, q2);
	}
	~OrQuery() {
		delete q1;
		delete q2;
	}

	// basic consistency methods
	int Consistent(const GiSTentry& entry) {
		int response=q1->Consistent(entry)||q2->Consistent(entry);

		grade=MIN(q1->Grade(), q2->Grade());
		return response;
	}
	int NonConsistent(const GiSTentry& entry) {
		int response=q1->NonConsistent(entry)&&q2->NonConsistent(entry);

		grade=MIN(q1->Grade(), q2->Grade());	// or what?
		return response;
	}

#ifdef PRINTING_OBJECTS
	void Print(ostream& os) const {
		os << "(" << *q1 << " or " << *q2 << ")";
	}
#endif

private:
	MTquery *q1, *q2;
};

class NotQuery: public MTquery {	// the negation of a query
public:
	// constructors, destructors, etc.
	NotQuery(const MTquery *q): q((MTquery *)q->Copy()) {}
	GiSTobject *Copy() const { return new NotQuery(q); }
	~NotQuery() { delete q; }

	// basic consistency methods
	int Consistent(const GiSTentry& entry) {
		int response=q->NonConsistent(entry);

		grade=q->Grade();
		return response;
	}
	int NonConsistent(const GiSTentry& entry) {
		int response=q->Consistent(entry);

		grade=q->Grade();	// or what?
		return response;
	}

#ifdef PRINTING_OBJECTS
	void Print(ostream& os) const {
		os << "(not " << *q << ")";
	}
#endif

private:
	MTquery *q;
};

class TopQuery: public GiSTobject {	// a simple k-NN query
public:
	// constructors, destructors, etc.
	TopQuery(const MTpred *p, const int n) : pred((MTpred *)p->Copy()), k(n) {}
	TopQuery(const TopQuery& q) : pred((MTpred *)q.pred->Copy()), k(q.k) {}
	GiSTobject *Copy() const { return new TopQuery(*this); }
	~TopQuery() { delete pred; }

	// access to private members
	const MTpred *Pred() const { return pred; }

	int k;
private:
	MTpred *pred;
};

#endif
