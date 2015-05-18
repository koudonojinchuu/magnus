// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: A standalone executable which accepts a stream of relators
//           and test words; says whether the test word is trivial
//           modulo the relator.
//
// Principal Author: Dion Gildenhuys
//
// Status: Stable
//
// Usage:
//
// Discussion:
//
// Revision History:
//
// * 07/12/95 Roger commented out most output to cout
//
// Next Implementation Steps:
//


#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <new.h>

// #define INFILE
//#define VERBOSE_MODE
#define MAXSTACK 10000
#define UNIX
#define READ_LENGTHS

#ifdef READ_LENGTHS

static int len1, len2;
static int maxLen1 = 1000, maxLen2 = 3000;

#endif

enum probType { wp, mp, rp }; // word problem, membership problem
	// reduction problem
enum pinchType { none, up, down };
enum tryMoveType { out, left, right };
enum answerType { maybe, no, yes };

#ifndef UNIX

 enum { false, true };
 typedef int bool;

#endif


// ofstream outFile( "out", ios::out );

#ifdef INFILE

 ifstream inFile( "in", ios::in );

#endif


template <class TYPE>
class stack{
private:
    enum { EMPTY = -1 };        
    TYPE* s;
    int max_len;
    int top;
public:
    stack() :max_len(MAXSTACK)
	   { s = new TYPE[MAXSTACK]; if (s == 0) {
			cerr << "\nOut of Memory\n"; exit(1);} top = EMPTY; }
	stack(int size) { s = new TYPE[size]; if ( s == 0 ) {
			cerr << "\nOut of Memory\n"; exit(1);}
			max_len = size; top = EMPTY;}
    ~stack() { delete [] s; }
    void reset() { top = EMPTY;}
	void push(TYPE c) { s[++top] = c; }
	TYPE pop() { return(s[top--]); }
    int empty() const { return(top == EMPTY);}
};

template<class Type>
class ListElt{
public:
   Type   data;
   ListElt<Type>* next;
};

template<class Type>
class list{
protected:
   ListElt<Type>* h;      //head of list
public:
   list() {h=0;}    //0 denotes the empty list
   ~list() { release(); }
   list(const list& l);
   void prepend(const Type &c);        //adds to the front of the list
   void del() {     ListElt<Type>* temp =h;
                    h = h->next;
                    delete temp;}
   ListElt<Type>* first() const { return (h); }
   void release();
   void reverse();
   void print();
   void advance();
   int length() const;
   bool isTrivial() const { return ( h == 0 ); }
   list<Type>& operator=(const list<Type>& );
};

template<class Type> list<Type>& list<Type>::operator=(const list<Type>& w)
{
    if ( this == &w )
        return *this;
    ListElt<Type>* elem = w.h;

    if ( elem == 0 )
        return *this;
    if ( h != 0 )
        delete h;
    h = 0;
    while ( elem ) {
        prepend( elem -> data );        // overloaded assignment
        elem = elem -> next;
        }
    reverse();
    return *this;
    }

template<class Type> int list<Type>::length() const
{
   ListElt<Type>* temp = h;
   int count = 0;

   while (temp != 0) {
      ++count;
      temp = temp -> next;
      }

   return(count);
}



template<class Type> void list<Type>::prepend( const Type &c)
{
   ListElt<Type>* temp = new ListElt<Type>; // creates new element

   if ( temp == 0 ) {
	cerr << "\nOut of memory!\n";
	exit(1);
	}
   temp -> next = h;
   temp -> data = c; // in the case of superWord, this is the overloaded
                     // assignment operator for superGen
   h = temp;
}

template<class Type> void list<Type>::release()
{
   while (h !=0 )
          del();
}

template<class Type> void list<Type>::reverse()
{
   ListElt<Type>* pred, *succ, *elem;

   if (h == 0 ) return;
   if (h && ( ( succ = h -> next ) != NULL ) ) {
        pred = 0;
        elem = h;
        while (succ) {
           elem -> next = pred;
           pred = elem;
           elem = succ;
           succ = succ -> next;
        }
		h = elem;
        h -> next = pred;
   }
}

template<class Type> void list<Type>::advance()
{
   ListElt<Type>* temp;

   temp = h;
   h = temp -> next;
   delete temp;
}

template<class Type> list<Type>::list(const list<Type>& l)
{
   ListElt<Type>* elem = l.h;
   list<Type> temp;
   
   h = 0;
   while (elem) {
        temp.prepend( elem -> data );
        ++elem;
        }
   reverse();
}

class superGen {     // has an exponent and list of subscripts
private:
   int exponent, temp;
   ListElt<int> *subscript;
public:
   superGen() {subscript = 0; exponent = 0;}
   superGen(const superGen& s);
   ~superGen() { release(); }
   void assignExponent(int n) { exponent = n;}
   void adjoinSubscript(const int m);
   void print();
   void del() { ListElt<int>* temp = subscript;
                subscript = subscript -> next;
                delete temp;}
   ListElt<int>* firstSubscript() const { return (subscript); }
   int exp() const { return (exponent);}
   void release();
   void reverseSubscripts();
   void invert() { temp = exponent; exponent = -temp; }
   superGen& operator=(const superGen& );
   bool isTrivial() const { return ( subscript == 0 ); }
};


superGen& superGen::operator=(const superGen& s)
{
   if ( this == &s )
     return *this;
   ListElt<int>* oldElem = s.subscript;

   exponent = s.exponent;
   if ( subscript != 0 )
     delete subscript;
   subscript = 0;

   if (oldElem == 0)
        return(*this);

   while (oldElem) {
        adjoinSubscript(oldElem -> data);
        oldElem = oldElem -> next;
		}

   reverseSubscripts();
   return(*this);
}

int sameSubscripts(const superGen &a, const superGen &b)
{

   ListElt<int>* elt1, *elt2;

   elt1 = a.firstSubscript();
   elt2 = b.firstSubscript();
   if ((elt1 == 0) && (elt2 == 0))
        return(1);
   while ((elt1 != 0) && (elt2 != 0))
        {
        if ((elt1 -> data) != (elt2 -> data))
           return(0);
        elt1 = elt1 -> next;
        elt2 = elt2 -> next;
        }
   if ((elt1 == 0) && (elt2 != 0))
        return(0);
   if ((elt2 == 0) && (elt1 != 0))
        return(0);
   return(1);
}

void superGen::adjoinSubscript( const int m )
{
   ListElt<int>* temp = new ListElt<int>;

	if ( temp == 0 ) {
	cerr << "\nOut of memory!\n";
	exit(1);
	}


   temp -> next = subscript;
   temp -> data = m;
   subscript = temp;
}

void superGen::release()
{
   while (subscript != 0)
        del();
}

void superGen::reverseSubscripts()
{
   ListElt<int>* pred, *succ, *elem;

      if (subscript && ( ( succ = subscript -> next) != NULL ) ) {
        pred = 0;
        elem = subscript;
        while (succ) {
           elem -> next = pred;
           pred = elem;
           elem = succ;
           succ = succ -> next;
        }
        subscript = elem;
        subscript -> next = pred;
   }
}


superGen::superGen(const superGen& s) : exponent(s.exponent)
{ 
   ListElt<int>* oldElem = s.subscript;

   subscript = 0;
   if (oldElem == 0)
        return;

   while (oldElem) {
        adjoinSubscript(oldElem -> data);
        oldElem = oldElem -> next;
        }
   reverseSubscripts();
}


void superGen::print()
{
   ListElt<int>* temp = subscript;
   int k;

   cout << "x_" ;
   while (temp != 0) {
        k = temp -> data;
		cout << k;
        temp = temp -> next;
		if (temp) cout << ',';
   }
   cout << '^' << exponent;
}

template<> void list<char>::print()
{
   ListElt<char>* temp = h;

   while (temp !=0) { //detect end of list
		  cout << temp -> data;
          temp = temp -> next;
   }
   cout << "\n";
}

class PowerOfNamedGen {
private:
   char name;
   int power;
public:
   void assign(char c, int n){ name = c; power = n;}
   void print();
   char nme() { return(name); }
   int pwr() { return(power);}
};

void PowerOfNamedGen::print()
{
   cout << name << "^" << power;
}
 

class ListOfPowersOfNamedGens: public list<PowerOfNamedGen>
{
public:
   void print(); 
   bool userInput();
};


void ListOfPowersOfNamedGens::print()
{
   ListElt<PowerOfNamedGen>* temp = h;

   while (temp != 0) {
         (temp -> data).print();
          temp = temp -> next;
   }
   cout << "\n";
}

bool ListOfPowersOfNamedGens::userInput()
{
   PowerOfNamedGen g;
   int c;
   int n;

//   cout << "Input string:\n";

#ifdef INFILE

   c = inFile.get();

#else

   c = cin.get();

#endif

   while ( c != '\n'){
	  if ( c == EOF )
		return( false );
	  if ( ( c > 'z') || (c < 'A' ) ) {
		 cerr << "Bad input \n ";
		 exit(1);
	  }

#ifdef INFILE

	  inFile >> n;

#else

	  cin >> n;

#endif

	  g.assign( c, n );
	  prepend( g );

#ifdef INFILE

	  c = inFile.get();

#else

	  c = cin.get();

#endif

   }

   reverse();
   return( true );
}

int indexFirstCharMatch(const char c, ListOfPowersOfNamedGens* lpn)
   // return the index to assign to the generator with name c
{
   int count = 0;
   ListElt<PowerOfNamedGen>* le = lpn -> first();
   char d;

   while (le != 0) {
        d = (le -> data).nme();
        ++count;
        if (d == c) 
           break;
        le = le -> next;
        }
   return(count);
}


class superWord: public list<superGen>
{
public:
   void print();
   void invert();
   void freelyReduce();
   void adjoinSubscriptFirstGen( int n )
        { ( h -> data ).adjoinSubscript( n ); }
   void assignExponentFirstGen( int n )
        { ( h -> data ).assignExponent( n ); }
   void invertFirstGen() { ( h -> data ).invert(); }
};


void superWord::freelyReduce()
{

   ListElt<superGen>* genp1, *genp2, *genp3, *genp0;
   int exp1, exp3, len = length();
   stack<ListElt<superGen>*> st(len);

   genp1 = h;   // points to current generator
   genp0 = h;   // will point later to previous generator if there is one

   while (genp1 != 0){
        genp3 = genp1 -> next;  // the testing generator 
        if (genp3 == 0)
            return; // there is only one generator
        exp1 = (genp1 -> data).exp();
        while (sameSubscripts( genp1 -> data, genp3 -> data )) {
           exp3 = ( genp3 -> data ).exp();
           exp1 += exp3;
           genp2 = genp3;   // for selfdestruction
           genp3 = genp3 -> next;
           delete(genp2); //
           if (genp3 == 0)
              break;
           }
        if (exp1 != 0) {  // consolodate generators into one
                // perhaps there was no cancellation
           (genp1 -> data).assignExponent(exp1);
           genp1 -> next = genp3;
           st.push( genp0 ); // push it onto the stack
           genp0 = genp1;  // will point to previous generator
           genp1 = genp3; // advance the current generator
           }
        else {  // a sequence of generators, including genp1, cancel
           if (h == genp1) { // at the beginning of the word
              delete(h);  // cancellation at the start
			  h = genp3;   // new beginning of word
              genp0 = h;
              genp1 = h;
              }
           else { 
              delete( genp1 );   // the current generator cancelled
              genp0 -> next = genp3;   // same previous generator
              if (genp3 == 0)
                 genp1 = 0;
              else {
                 if (0 == sameSubscripts
                    ( genp0 -> data, genp3 -> data ))
                    genp1 = genp3;       // not same subscripts
                 else { 
                    genp1 = genp0;      // backtrack
                    if (h != genp1)   // the stack should be nonempty
                       {
                       genp0 = st.pop();
                       genp0 -> next = genp1;
                       }
                    } 
                 }
                 
              }
           }
        }                       
}   

void superWord::invert()
{
   ListElt<superGen>* elem;

   reverse();
   elem = h;
   while (elem != 0) {
        (elem -> data).invert();
        elem = elem -> next;
        }
}

void superWord::print()
{
   ListElt<superGen>* temp = h;

   if (temp == 0) cout << "1";
   while (temp != 0) {
        (temp -> data).print();
		cout << " ";
        temp = temp -> next;
   }
   cout << "\n";
}

int exponentSum(const superGen &g, const superWord &w)
{
   int k = 0;
   ListElt<superGen>* x;

   x = w.first(); // pointer to first generator
   while (x != 0) {
        if (sameSubscripts( x -> data, g ))
                k += (x -> data).exp();      // add the exponent of x to k
        x = x -> next;
        }
   return (k);
} 

bool appearsIn( const superGen &g, const superWord &w )
{
    bool ans = false;
    ListElt<superGen>* lesg = w.first();

    if ( lesg == 0 )
        return( ans );

    while ( !sameSubscripts( lesg -> data, g )) {
        lesg = lesg -> next;
        if ( lesg == 0 )
            break;
        }

    if ( lesg != 0 )
        ans = true;
    return(ans);
}


void removeGen( superWord &a, const superGen &g )
{
    ListElt<superGen> *olesg, *lesg;


    lesg = a.first();

    if ( lesg == 0 )
	return;

    if ( sameSubscripts( g, lesg -> data ) ) {
	a.del();	// delete the first subscript
	return;
	}

	do {
	olesg = lesg;
	lesg = lesg -> next;
	if ( lesg != 0 ) {
		if ( sameSubscripts( g, lesg -> data ) ) {
		olesg -> next = lesg -> next;
		delete lesg;
		}
		lesg = olesg -> next;
		}
	} while ( lesg != 0 );
}

bool doesNotAppearIn( const superGen &g, const superWord &w )
{
	if ( appearsIn( g, w ))
		return( false );
	else
		return( true );
}


void conditionalChoice( int &x1, int &x2, superGen &st,
	superGen &accomp, const superWord &r, const superWord &target,
	bool (*cond)( const superGen&, const superWord& ))
	// This function defines st and its exponentsum x1, perhaps also
	// accomp and its exponentsum x2. If r contains a generator g, for
	// which cond( g, target ) is satisfied (if the pointer to cond has
	// been set to zero, then the condition cond is not applied),
	// and g has zero exponentsum
	// in r, then st is defined to be the first such g, and accomp and
	// x2 are ignored; otherwise, we choose accomp to be the first
	// generator g in r, different from st, that satisfies
	// cond( r, target ), and we set x2 equal to its exponentsum.
	// The function conditionalChoice is only invoked when the function
	// pointer to cond is set to zero, or when cond( g, target ) is true
	// for at least two generators g in r.

{
	int k = 0, abk, e, abe, m;
	ListElt<superGen> *x, *y, *z;
	bool firstTime = true, foundTwo = false;
	superGen *g;

	x = r.first();

	while ( x != 0 ) {
		if ( cond ) {
			while ( !( cond( x -> data, target ))) {
				x = x -> next;
				if ( x == 0 )	// impossible the first time
					break;
				}
			}
		if ( x == 0 )
			break;
		e = exponentSum( x -> data, r );
		if ( e == 0 ) {
			st = x -> data;	// overloaded assignment
			x1 = 0;
			break;	// the search has been completed
			}
		abe = (e < 0) ? -e : e; // abe is the absolute value of e
		if ( firstTime ) {
			y = x;
			k = e;
			firstTime = false;
			}
		else {
			if ( !sameSubscripts( x -> data, y -> data )) {
				foundTwo = true; // we found y and z = x:
				z = x;
				m = e;
				}
			abk = (k < 0) ? -k : k; // abs val of k
			if ( abe < abk ) { // then switch, so that abk remains minimal
				m = k;
				z = y;
				k = e;
				y = x;
				}
			}
		x = x -> next;
		}
	if ( e == 0 )
		return;
	if ( foundTwo ) {
		st = y -> data;
		x1 = k;
		accomp = z -> data;
		x2 = m;
		}
}

void chooseStableAccomp( const superWord &r, const superWord &outsideGens,
  superGen &stable, superGen &accomp, probType &ptype,
  const superWord &target, int &x1, int &x2, bool &sits )
	// Choose the generators stable and accomp in
	// accordance with the type of problem. Assume r has length >1.
	// In the cases of the word problem or the reduction problem,
	// the choice is made by the function conditionalChoice, with
	// its parameter function pointer cond set to 0. In the case
	// of the membership problem, we first consider the case where
	// the target subgroup is cyclic. If its generator has zero
	// exponent sum in r, we choose it as stable, we put x1=0,
	// sits = true, and we don't define accomp and x2. If the
	// generator for the cyclic target subgroup has nonzero exponent
	// sum in r, we look for another generator in r with nonzero
	// exponent sum in r. If we find it, then it will be accomp,
	// x2 will be its exponent sum, the generator for the target subgroup
	// will be stable (strictly, a root of it),
	// and x1 its exponent sum. In this case sits is
	// set to true. If no generator in r, outside the cyclic target
	// subgroup has nonzero exponent sum, then we define
	// stable outside the target subgroup, appearing in r, sits=false,
	// x1=0, and we don't define accomp, x2.
{
	ListElt<superGen> *x, *y, *fog = outsideGens.first();;
	int e;

	if ( ptype == wp ) { // find the exponentsum with least abs. value
		sits = false;
		if ( fog != 0 ) {
			stable = fog -> data;	// deep copy
			x1 = 0;
			return;
			}
		conditionalChoice( x1, x2, stable, accomp, r, target, 0 );
		}

	else { // first search for gen. outside r, in the target
		x = target.first();
		while ( x ) {
			if ( !appearsIn( x -> data, r )) {
				stable = x -> data; // deep copy
				x1 = 0;
				sits = true;
				ptype = rp;
				return;
				}
			x = x -> next;
			}

		if ( 1 == target.length() ) {
			x = target.first();
			x1 = exponentSum( x -> data, r ); // it may be redefined
			if ( x1 != 0 ) {
				y = r.first();
				while( y ) {
					e = exponentSum( y -> data, r );
					if (( e != 0 ) &&
					  ( !sameSubscripts( x -> data, y -> data )))
						break;
					y = y -> next;
					}
				if ( y == 0 ) { // the one case where sits will be false
					y = r.first();
					if ( sameSubscripts( x -> data, y -> data ))
						y = y -> next;
					stable = y -> data; // deep copy
					x1 = 0;
					sits = false;
					return;
					}
				else {
					stable = x -> data; // deep copy
					accomp = y -> data; // deep copy
					x2 = e;
					sits = true; // stable in, accomp outside target
					ptype = rp;
					return;
					}
				}
			else {
				stable = x -> data;
				sits = true;
				ptype = rp;
				return;
				}
			}

		// At this point, all target gens. (more than one) appear in r
		conditionalChoice( x1, x2, stable, accomp, r, target, appearsIn );
		ptype = rp;
		sits = true;
		 // stable and accomp will be chosen in the target subgroup

		}
}

void adjoinSubscriptsByConjugation
	( const superWord &w, const superGen &conjugator, superWord &psw )
{       // w is assumed to be freely reduced
   ListElt<superGen>* x;
   int sumPowersOfConj = 0;

   x = w.first();

   while ( x != 0 ) {
		if ( sameSubscripts( x -> data, conjugator ))
		   sumPowersOfConj += ( x -> data ).exp();
        else {
		   psw.prepend( x -> data ); // prepend a copy of the superGen
           psw.adjoinSubscriptFirstGen( sumPowersOfConj );
           }
        x = x -> next;
        }               
   if (sumPowersOfConj != 0) {
	  psw.prepend( conjugator ); // prepend a copy of the conjugator
	  psw.assignExponentFirstGen( sumPowersOfConj );
	  }
   psw.reverse();
   psw.freelyReduce(); 
}


void eltTransform( const superWord &w, const superGen &g1,
  const superGen &g2, superWord &ts)
  // If g1 = a^i then it is replaced by (a.g2)^i, yielding ts from w
{
   ListElt<superGen>* x;
   int count, exp;

   x = w.first();

   while (x != 0) {
        while (( x != 0 ) && ( !sameSubscripts( x -> data, g1 ))) {
            ts.prepend( x -> data ); // prepend a copy of the superGen
            x = x -> next; 
            }
        if ( x != 0 ) {  // g1 and x are the same
            exp = (x -> data).exp();
            count = (exp < 0) ? -exp : exp; // count is abs. val. exp
            while (count != 0) {
                if (exp < 0) {
                    ts.prepend( g2 );
                    ts.invertFirstGen();
                    ts.prepend( x -> data );
                    ts.assignExponentFirstGen( -1 );
                    }
                else {
                    ts.prepend( x -> data );
                    ts.assignExponentFirstGen( 1 );
                    ts.prepend( g2 );
                    }
                --count;
                }  // insertions have been made for 1 occurence of g1
            x = x -> next;
            }           // case of x = g1 is done
        }
   ts.reverse();
}

int indexFromListNames( list<char> &listNames, const char c )
      //  check if c is on the list; if so, return its position 
        // from the end, else prepend it to the list and return its
		// new position from the end
{
   int len, i = 0, found = 0;
   ListElt<char>* le;

   le = listNames.first();
   while (le != 0) {
        ++i;    
        if (c == le -> data) {
           found = 1;
           break;
           }
        le = le -> next;
        }
   if (found) {
        len = listNames.length();
        return(len + 1 -i);  // position from the end
        }
   listNames.prepend(c);  // name not found; add it to the list
   return(i + 1);  // length plus one
}


void superWordFromLPNG( ListOfPowersOfNamedGens &lpn,
    list<char> &listNames, superWord& sw )
{
   superGen *s;
   PowerOfNamedGen pn;
   ListElt<PowerOfNamedGen>* le;
   int index;
   int power;
   char name;

   le = lpn.first();
   while (le != 0) {
		s = new superGen;
		if ( s == 0 ) {
		cerr << "\nOut of memory!\n";
		exit(1);
			}
		pn = le -> data;
        power = pn.pwr();
        name = pn.nme();
        index = indexFromListNames(listNames, name);
        s -> adjoinSubscript(index);
        s -> assignExponent(power);
        sw.prepend(*s);
        le = le -> next;
        }

   sw.reverse();
}


void adjoinRootOfGen( superWord &w, const superGen &g, int n)
        // replace g by g^n in the word w
{
   ListElt<superGen>* le = w.first();
   int exp;

   while (le != 0) {
        if (sameSubscripts( g, le -> data )) {
           exp = (le -> data).exp();
           exp *= n;    // multiply the exponent by n
           (le -> data).assignExponent(exp);
		   }
        le = le -> next;
        }
}


class minMaxEntry {
public:
   superGen entry;
   int small;
   int big;
   minMaxEntry& operator=(const minMaxEntry& );
};

minMaxEntry& minMaxEntry::operator=(const minMaxEntry& mm)
{

	if ( this != &mm ) {
	 small = mm.small;
	 big = mm.big;
	 entry = mm.entry;   // overloaded assignment operator
    }
	return *this;

}

void lookUpMinMaxEntries
    ( const superGen &g, const list<minMaxEntry> &lmme,
    int &min, int &max)
    // it sets min and max equal to 0 if no match is found
{
    ListElt<minMaxEntry> *lemme;
    minMaxEntry* mme;
    bool found = false;

    lemme = lmme.first();

    while ( lemme != 0) {
        mme = &( lemme -> data );
        if (sameSubscripts( mme -> entry , g )) {
            min = mme -> small;
            max = mme -> big;
            found = true;
            break;
            }
        lemme = lemme -> next;
        }
    if ( !found ) {
	min = 0;
	max = 0;
	}

}

void adjoinSubscriptsByRestrictedConjugation
  (const superWord &w, const superGen &conjugator,
   const list<minMaxEntry> &lmme, superWord &psw )
{       // w is assumed to be freely reduced
   ListElt<superGen>* x;
   int sumPowersOfConj = 0;
   int lowerBd, upperBd;
   bool done = false;

   x = w.first(); // pointer to first generator

   while ( x != 0 ) {
        if ( sameSubscripts( x -> data, conjugator )) {
           sumPowersOfConj += ( x -> data ).exp();
		   }
        else {
           lookUpMinMaxEntries( x -> data, lmme, lowerBd, upperBd);
           if (lowerBd > sumPowersOfConj) {
                psw.prepend( conjugator ); // not moved through
                psw.assignExponentFirstGen( sumPowersOfConj - lowerBd );
                psw.prepend( x -> data );
                psw.adjoinSubscriptFirstGen( lowerBd );
                sumPowersOfConj = lowerBd; // the power that is moved through
                done = true;
                }
           if (upperBd < sumPowersOfConj) {
                psw.prepend( conjugator ); // not moved through
                psw.assignExponentFirstGen( sumPowersOfConj - upperBd );
                psw.prepend( x -> data );
                psw.adjoinSubscriptFirstGen( upperBd );
                sumPowersOfConj = upperBd; // the power that is moved through
                done = true;
                }

           if (!done) {
              psw.prepend( x -> data );
              psw.adjoinSubscriptFirstGen( sumPowersOfConj );
              }
           done = false;
           }
        x = x -> next;
        }               
   if (sumPowersOfConj != 0) {
      psw.prepend( conjugator );
      psw.assignExponentFirstGen( sumPowersOfConj );
      } 
   psw.reverse();
   psw.freelyReduce();
}

class MagProb {
public:
    int childNumber;
    list<int> exponentsStableLetter;
    int leftExp, rightExp;
    probType problemType;
    pinchType pinchT;
    list<char> listNames;   // names of generators; only used at the root
    superWord relator, rewrittenRelator;
    int relatorLength;
    superWord test, rewrittenTest, outsideGens; // outside means
	// outside the relator and outside the target subgroup
    superWord gensTargetSubgp; // re. membership problem
    superWord inheritedTarget;
    superWord gensAssocSubgp; // re. HNN extension
    superGen stableLetter, accompanyingGen;
    int exps1, exps2;
        // exps1 is the least exponent sum in the relator
        // If exps1 is not zero then it, exps2 and accompanyingGen are used
        // in the rewriting process of the relator and test word;
	// and exps2 is the exponentsum of accompanyingGen in relator
    list<minMaxEntry> lmm;
      // list of minimal and maximal subscripts
    bool stableInTargetSubgp;
    MagProb();
	bool inputRelator(); // only used at the root
	void inputTest();    // only used at the root
    void rewriteRelator(); // It defines exps1, exps2, rewrittenRelator,
			   // stableLetter, accompanyingGen,
			   // stableInTargetSubgp
    void defineOutsideGens();
    void createMinMaxSubscriptTable();
    void rewriteTest(); // it requires that exps1, exps2, stableLetter,
        // accompanyingGen, rewrittenRelator, llm all be already defined
    void printRelator() { relator.print(); }
    void printStableLetter() { stableLetter.print(); }
    void printAccompanyingGen() { if ( !accompanyingGen.isTrivial() )
                                    accompanyingGen.print(); }
    void printTestWord() { test.print();}
    void printRewrittenRelator() { rewrittenRelator.print(); }
    void printRewrittenTestWord() { rewrittenTest.print(); }
    void printMinMaxSubscripts();
    void printPinchType();
    void printInfo();
    bool abTest();
};

MagProb::MagProb()
{
    childNumber = 0;
    leftExp = 0;
    rightExp = 0;
    problemType = wp;
	pinchT = none;
	stableInTargetSubgp = false;
}


bool MagProb::abTest() // the abelianized word problem. Test whether
	// the exponentsum of each generator in test and not in gensTargetSubgp
	// is divisible by the exponentsum of the same generator in relator.
{
superGen *g;
bool ans = true;
int exps1, exps2, ratio;
ListElt<superGen>* lesg;
superWord found;	// the distinct generators in test
bool ratioDefined = false;

	lesg = test.first();
	if ( lesg == 0 )
		return( true );
	g = &(lesg -> data);

	while ( lesg != 0 ) {
		if (( problemType == mp ) || ( problemType == rp )) {
			while ( appearsIn( *g , gensTargetSubgp )) {
				lesg = lesg -> next;
				if ( lesg == 0 )
					break;
				else
					g = &(lesg -> data);
				}
			}
		if ( lesg == 0 )
			break;
		if ( !appearsIn( *g, found )) {
			found.prepend( *g );
			exps1 = exponentSum( *g, test );
			exps2 = exponentSum( *g, relator );
			if (( exps2 == 0 ) && ( exps1 != 0 )) {
				ans = false;
				break;
				}
			if (( exps2 != 0 ) && ( exps1 % exps2)) {
				ans = false;
				break;
				}
			if (( exps2 != 0 ) && ( exps1 == 0 ) && ( ratioDefined )) {
				ans = false;
				break;
				}
			if (( exps1 != 0) && ( exps2 != 0 )) {
				if ( ratioDefined ) {
					if ( ratio != exps1/exps2 ) {
						ans = false;
						break;
						}
					}
				else {
					ratio = exps1/exps2;
					ratioDefined = true;
					found.release();	// start all over
					lesg = test.first();
					g = &( lesg -> data );
					continue;
					}
				}
			}
		lesg = lesg -> next;
		g = &( lesg -> data );
		}

	if (( ratioDefined ) && ( ans == true )) {
		// search for a generator with nonzero exponent sum in the
		// relator, not appearing in the target subgroup, and
		// not appearing in test
		lesg = relator.first();
		while ( lesg != 0 ) {
			g = &( lesg -> data );
			if (( problemType == mp ) || ( problemType == rp )) {
			 // skip over the generators in
					// the target subgroup
				while ( appearsIn( *g , gensTargetSubgp )) {
					lesg = lesg -> next;
					if ( lesg == 0 )
						break;
					else
						g = &(lesg -> data);
					}
				}
			if ( lesg == 0 )
				break;
			if (( 0 != exponentSum( *g, relator )) &&
			  ( !appearsIn( *g, found ))) {
				ans = false;
				break;
				}
			lesg = lesg -> next;
			}
		}


#ifdef VERBOSE_MODE
 	cout << "\n\nFor the relator\n";
 	printRelator();
	cout << "Gens. target subgp\n";
	gensTargetSubgp.print();
 	cout << "and test word\n";
 	printTestWord();
 	if ( ans == true )
 		cout << "\nabTest() succeeds\n\n";
 	else
 		cout << "\nabTest() fails\n\n";
#endif
	return( ans );
}

void MagProb::printPinchType()
{
cout << "The type of pinch for the current problem is ";
switch ( pinchT ) {
    case none:
		cout << "none";
        break;
    case up:
		cout << "up";
        break;
    case down:
		cout << "down";
        break;
    }
	cout << endl;
}

void cyclicallyReduce( superWord &r ) {

	ListElt<superGen> *lesg1, *lesg2;
	superGen *g1, *g2;
	int x1, x2, x3;

	lesg1 = r.first();
	if ( lesg1 == 0 )
		return;
	lesg2 = lesg1 -> next;
	if ( lesg2 == 0 )
		return;
	while ( lesg2 -> next != 0 )
		lesg2 = lesg2 -> next;
	g1 = &( lesg1 -> data ); // first generator
	g2 = &( lesg2 -> data ); // last generator
	if ( !sameSubscripts( *g1, *g2 ))
		return;
	x1 = g1 -> exp();
	x2 = g2 -> exp();
	x3 = x1 + x2;
	if ( x3 != 0 )
		g2 -> assignExponent( x1 + x2 );
	else {	// remove the last generator
		r.reverse();
		r.del();
		r.reverse();
		}
	r.del(); // remove the first generator
}


bool MagProb::inputRelator()
{
	ListOfPowersOfNamedGens name;

	if ( !( name.userInput() ))
		return( false );
	superWordFromLPNG(name, listNames, relator);
	relator.freelyReduce();
	cyclicallyReduce( relator );
	return( true );
}

void MagProb::inputTest()
{
    ListOfPowersOfNamedGens name;

	name.userInput();
    superWordFromLPNG( name, listNames, test);
	test.freelyReduce();
}


void MagProb::rewriteRelator() // assume relatorLength > 1
{
    superGen g1Modified;   // modify the exponent of the stable letter
    superWord r1, r2;

	chooseStableAccomp( relator, outsideGens, stableLetter,
	accompanyingGen, problemType, gensTargetSubgp, exps1, exps2,
	stableInTargetSubgp );

    if ( !rewrittenRelator.isTrivial() )
        rewrittenRelator.release(); 
           // needed when at the root a second time

    if (exps1 == 0) 
        adjoinSubscriptsByConjugation( relator, stableLetter,
            rewrittenRelator );
    else {
           g1Modified = stableLetter;   // deep copy
           r1 = relator;        // deep copy
           adjoinRootOfGen( r1, stableLetter, exps2 );
                 // replace stableLetter by stableLetter^exps2
           g1Modified.assignExponent(-exps1);
           eltTransform( r1, accompanyingGen, g1Modified, r2);
			 // replace accompanyingGen by accompanyingGen*stableLetter^-exps1
		   r2.freelyReduce();
		   adjoinSubscriptsByConjugation
			   ( r2, g1Modified, rewrittenRelator);
		   }
	rewrittenRelator.freelyReduce();
}

void MagProb::rewriteTest()
{
    superGen g1Modified;
    superWord t1, t2;

    if ( !rewrittenTest.isTrivial() )
        rewrittenTest.release(); 
           // needed when at the root a second time

    if (exps1 == 0)
        adjoinSubscriptsByRestrictedConjugation
		  ( test, stableLetter, lmm, rewrittenTest );
    else {
        g1Modified = stableLetter;  // deep copy
        t1 = test;      // deep copy
        adjoinRootOfGen( t1, stableLetter, exps2 );
        g1Modified.assignExponent(-exps1);
        eltTransform( t1, accompanyingGen, g1Modified, t2 );
          // replace accompanyingGen by accompanyingGen*stableLetter^-exps1
        adjoinSubscriptsByRestrictedConjugation
                ( t2, g1Modified, lmm, rewrittenTest );
        }
}

void MagProb::createMinMaxSubscriptTable()
{
   ListElt<minMaxEntry> *lemme, *search;
   ListElt<superGen> *lesg;
   superGen g; // will run through the generators in relator
   superGen *h; // will run through the generators in rewrittenRelator
   int firstSubscript;
   bool found = false;
   minMaxEntry *pmme, mme;

   if ( !(lmm.isTrivial()) )
       lmm.release();
   lesg = rewrittenRelator.first();

   while (lesg != 0) {
        h = &(lesg -> data);
        firstSubscript = (h -> firstSubscript()) -> data;
        g = *h;        // a deep copy of h
        g.del(); // this advances the subscripts and creates a superGen
                        // in relator
        search = lmm.first();
        while (search != 0) { // see if we already have an entry
           pmme = &(search -> data);
           if ( sameSubscripts( pmme -> entry, g)) { // entry found
                found = true;
				if (firstSubscript < pmme -> small)
                   pmme -> small = firstSubscript; // record smaller element
                if (firstSubscript > pmme -> big)
                   pmme -> big = firstSubscript; // record bigger element
                }
           search = search -> next;
           }
        if ( found == false ) {
           mme.entry = g;   // deep copy
           mme.small = firstSubscript;
           mme.big = firstSubscript;
           lmm.prepend( mme );
           }
        found = false;
        lesg = lesg -> next;
        }       
}
                
void MagProb::printMinMaxSubscripts()
{
   ListElt<minMaxEntry> *lemme;
   minMaxEntry* mme;
    
   cout << "generator  minimum   maximum\n";
   lemme = lmm.first();
   while (lemme !=0) {
        mme = &(lemme -> data);
        (mme -> entry).print();
		cout << "         " <<
           mme -> small << "         " << mme -> big << endl;
        lemme = lemme -> next;
        }
}




int mustNotEliminate(int a, int b)
    // b is the exponent of the only generator in a relator
    // a is the exponent of this generator in the test word
{
    int absb, ret;

    if ((b == 1) || (b == -1))
        return (0);
    absb = (b < 0) ? -b : b;    // absb is the absolute value of b
    ret = a % absb;
    ret = (ret < 0) ? ret + absb : ret;
    return(ret);
}



void reduceOnceByPowerOfGen(superWord &test, const superGen &r)
    // assume that the word test is freely reduced and not trivial
{
    ListElt<superGen>* genp1, *genp0, *genp2; // current, previous and next
    int xp = r.exp();
    int testExp, residue;

    genp1 = test.first();

    if (sameSubscripts( genp1 -> data, r )) {
        testExp = (genp1 -> data).exp();
        residue = mustNotEliminate( testExp, xp );
        if (0 == residue)
            test.advance();  // kill the first generator;
        else
            (genp1 -> data).assignExponent(residue);
        }
    genp0 = genp1;
    genp1 = genp1 -> next;
	while (genp1 != 0) {
        if (sameSubscripts( genp1 -> data, r )) {
            testExp = (genp1 -> data).exp();
            residue = mustNotEliminate( testExp, xp );
            if ( residue == 0 ) { // kill genp1
                genp2 = genp1 -> next;
                genp0 -> next = genp2;
                delete(genp1);
                genp1 = genp2;
                }
            else {
                (genp1 -> data).assignExponent(residue);
                genp0 = genp1;
                genp1 = genp1 -> next;
                }
            }
        else {
            genp0 = genp1;
            genp1 = genp1 -> next;
            }
        }
    test.freelyReduce();
}

void reduceByPowerOfGen( superWord &test, const superGen &r)
{
   int l0, l2, l1;

   l1 = test.length();
   if ( l1 == 0 ) 
     return;
   do {
     reduceOnceByPowerOfGen( test, r);
     l2 = test.length();
     l0 = l1;
     l1 = l2;
   } while (( l1 != 0 ) && ( l1 < l0 ));
}

void MagProb::printInfo()
{
/*@rn
   ListElt<int>* lei = exponentsStableLetter.first();   

   cout << "\n\nChild number " << childNumber << ":\n";
   if ( relatorLength == 1 )
		cout << "The relator has length 1";
   cout << "Relator:\n";
   printRelator();
   cout << "Stable letter: ";
   printStableLetter();
   cout << endl;
   if ( !accompanyingGen.isTrivial() ) {
	 cout << "Accompanying generator: ";
     printAccompanyingGen();
   }
   else 
	 cout << "There is no accompanying generator";
   cout << endl;
   if (( relatorLength > 1 ) && ( !lmm.isTrivial() ))
            printMinMaxSubscripts();
   cout << "Test word:\n";
        printTestWord();
   if ( relatorLength > 1 ) {
		cout << "Rewritten relator:\n";
        printRewrittenRelator();
		cout << "Rewritten test word:\n";
		printRewrittenTestWord();}

   if ( !gensTargetSubgp.isTrivial() ) {
		cout << "Generators of the target subgroup:\n";
		gensTargetSubgp.print();
		}
	else
		cout << "The target subgroup is trivial\n";

   if ( !inheritedTarget.isTrivial() ) {
		cout << "Generators of the inherited target subgroup:\n";
		inheritedTarget.print();
		}
	else
		cout << "The inherited target subgroup is trivial\n";

   if ( !gensAssocSubgp.isTrivial() ) {
		cout << "Generators of the associated subgroup:\n";
		gensAssocSubgp.print();
		}
	else
		cout << "The associated subgroup is trivial\n";

   if ( lei != 0 ) {
		cout << "Exponents of the stable letter:\n";
        while ( lei ) {
			cout << lei -> data;
            if ( lei -> next )
				cout << ",";
            else
				cout << endl;
            lei = lei -> next;
            }
        }                       
        
   printPinchType();
   if ( stableInTargetSubgp )
		cout << "The stable letter is in the target subgp.\n";
   else
		cout << "The stable letter is not in the target subgp.\n";

   if ( problemType == wp )
		cout << "The problem is of type wp\n";
   if ( problemType == rp )
		cout << "The problem is of type rp\n";
   if ( problemType == mp )
		cout << "The problem is of type mp\n";
   cout << "leftExp and rightExp equal " << leftExp << ", "
     << rightExp << endl;
*/
}


class TreeEltMP{
public:
	MagProb data;

	TreeEltMP* nextSibling, *previousSibling, *parent, *firstChild;
	TreeEltMP() { nextSibling = 0; parent = 0; firstChild = 0;
				 previousSibling = 0;}

	bool input();
	void spawn();
	void defineGensTargetAssocSubgpsForChildren();
	void expand();
	TreeEltMP* firstLeaf();
	   // first leaf below given node
	TreeEltMP* nextLeaf( TreeEltMP *); // returns 0 for last leaf
	TreeEltMP* firstChildWithPotentialPinch();
	TreeEltMP* nextSiblingWithPotentialPinch();
	TreeEltMP* nextDesiredSibling();
	TreeEltMP* firstDesiredChild();
	void testFromChildren();
	bool testDecomposes();
	void adjoinInheritedToTargetsOfChildren();
	void prune();   // remove all leaves (thus creating new ones)

};

void TreeEltMP::prune()    // removes the leaves
{
	TreeEltMP* leaf, *temp, *prnt, *nxtsblng;

	leaf = firstLeaf();
	if ( leaf == 0 )
	  return;

	while ( leaf != 0 ) {
		temp = leaf;
		prnt = temp -> parent;
		nxtsblng = temp -> nextSibling;
		leaf = nextLeaf( leaf );
		prnt -> firstChild = nxtsblng;
		delete(temp);
		}
}



void maxIncreaseDecrease( TreeEltMP *pte, int &inc, int &dec )
	// gives a bound in inc on the number of pinches that can increase and
	// a bound in dec on the number of pinches that can decrease the
	// first subscript. Assume that the subscripts are within the bounds.
	// Assume at least two subscripts.

{
   MagProb *prob = &( pte -> data );
   superWord *w = &( prob -> test );
   TreeEltMP *ppte = pte -> parent;	// by assumption nonzero
   list<minMaxEntry> *lmme = &(( ppte -> data ).lmm );
   ListElt<minMaxEntry> *lemme;
   int incr, decr, difference, firstSubscript;
   int max, min;
   bool matchFound = false;
   ListElt<superGen> *lesg, *firstSG;
   superGen* g, h, *currentEntry; // h will be g without the first subscript
   minMaxEntry* mme;

   lemme = lmme -> first();
   firstSG = w -> first();
   lesg = firstSG;

   while (lemme != 0) {
		mme = &(lemme -> data);
		currentEntry = &(mme -> entry);
		max = mme -> big;
		min = mme -> small;
		while (lesg != 0) {  // search for a match of currentEntry with h
		   g = &(lesg -> data);
		   firstSubscript = (g -> firstSubscript()) -> data;
		   h = *g; // local deep copy
		   h.del();  // this removes the first subscript
		   if (sameSubscripts( *currentEntry, h)) {
				if (matchFound == false) {
				   matchFound = true;
				   incr = max - firstSubscript;
				   decr = firstSubscript - min;
				   }
				else {
				   if (max - firstSubscript < incr)
						incr = max - firstSubscript;
				   if (firstSubscript - min < decr)
						decr = firstSubscript - min;
				   }
				}
		   lesg = lesg -> next;
		   }
		lesg = firstSG;
		lemme = lemme -> next;
		}
   dec = ( decr < 0 ) ? 0 : decr; // to be changed: decr should not be < 0
   inc = ( incr < 0 ) ? 0 : incr;
}

TreeEltMP* TreeEltMP::firstChildWithPotentialPinch()
{
    MagProb *prob;
    TreeEltMP* pte = firstChild;

    while ( pte ) {
	prob = &( pte -> data );
	if ( prob -> pinchT != none )
	    break;
        pte = pte -> nextSibling;
        }
    return( pte );
}

TreeEltMP* TreeEltMP::firstDesiredChild()
{
	MagProb *prob = &data;
	ListElt<int> *lei;
    probType pt = prob -> problemType;
    bool sits = prob -> stableInTargetSubgp;

    if ( firstChild == 0 )
        return( 0 );

	if ( pt == mp ) {
        if ( sits ) 
			return( firstChild );
        else {
            if ( firstChild -> nextSibling == 0 ) {
                lei = ( prob -> exponentsStableLetter ).first();
                if ( lei -> data == 0 ) {
                    lei = lei -> next;
                    if ( lei -> data == 0 )
                        return( firstChild );
                    }
                }   
		    }
	    }

    if (( pt == wp ) && ( firstChild -> nextSibling == 0 ))
        return( firstChild );
	return( firstChildWithPotentialPinch() );

}

TreeEltMP* TreeEltMP::nextSiblingWithPotentialPinch()
{
    TreeEltMP* pte = nextSibling;
    MagProb *prob;

    while ( pte ) {
		prob = &( pte -> data );
		if ( prob -> pinchT != none )
			break;
		pte = pte -> nextSibling;
		}
	return( pte );
}

TreeEltMP* TreeEltMP::nextDesiredSibling()
{
    MagProb *pprob;

    if ( parent )
		pprob = &( parent -> data );
	else
		return( 0 );

	if (( pprob -> problemType == mp ) && ( pprob -> stableInTargetSubgp ))
		return( nextSibling );
	return( nextSiblingWithPotentialPinch() );
}



void conjugatesFromSubscripts( superWord &w, const superGen &conjugator,
    superWord &nw )     // it reverses the order
{
	int subscript;
    ListElt<superGen> *lesg;
    superGen *g, h;
    ListElt<int> *lei;

    lesg = w.first();
    if ( lesg == 0 )
        return;

    while ( lesg ) {
        g = &( lesg -> data );
        lei = g -> firstSubscript();
        subscript = lei -> data;
        g -> del();     // remove first subscript
        if ( subscript == 0 )
            nw.prepend( *g );
        else {
            h = conjugator; // deep local copy
            h.assignExponent( subscript );
            nw.prepend( h );
            nw.prepend( *g );
            h.assignExponent( -subscript );
            nw.prepend( h );
            }
        lesg = lesg -> next;
        }

}

void suppressRootOfGen( superWord &w, const superGen &g, int n)
        // replace g^n by g in the word w (assume divisibility)
{
   ListElt<superGen>* le = w.first();
   int exp;

   while (le != 0) {
        if (sameSubscripts( g, le -> data )) {
           exp = (le -> data).exp();
           exp /= n;    // divide the exponent by n
           (le -> data).assignExponent(exp);
           }
        le = le -> next;
        }
}


void TreeEltMP::testFromChildren() //
{

    TreeEltMP *cpte;
    MagProb *childProb;
    ListElt<superGen> *lesg;
    superGen *g, conj;
    superWord t1, t2;
    list<int>* pesl = &( data.exponentsStableLetter );
    ListElt<int>* lei;
    int exp;

    ( data.test ).release();	// release existing memory
    cpte = firstChild;

    lei = pesl -> first();
    while ( lei ) {
	 exp = lei -> data;
	 if ( exp ) {
	     conj = data.stableLetter;	// deep copy
	     conj.assignExponent( exp );
	     t1.prepend( conj );
	     }
	 if ( cpte ) {
	     childProb = &( cpte -> data );
 	     conjugatesFromSubscripts( childProb -> test,
	     data.stableLetter, t1 );
	     cpte = cpte -> nextSibling;
	     }
	 lei = lei -> next;
	 }

     t1.freelyReduce();
     t1.reverse();

     if ( ( data.accompanyingGen ).isTrivial() )
	 data.test = t1;     // deep copy
     else {
	 conj = data.stableLetter;  // deep copy
	 conj.assignExponent( data.exps1 );
	 eltTransform( t1, data.accompanyingGen, conj, t2 );
	 t2.freelyReduce();
	 suppressRootOfGen( t2, data.stableLetter, data.exps2 );
	 data.test = t2;    // deep copy
	 }

}


void TreeEltMP::defineGensTargetAssocSubgpsForChildren()
{

    ListElt<int>* lei;
    TreeEltMP* temp;
    MagProb *mpm;
    ListElt<minMaxEntry> *lemme;
    minMaxEntry *mme;
    superGen g;
    ListElt<superGen> *lesg;
    int count, cn, fsd, rex;
    superWord target, assoc;
    ListElt<int> *fs;
	bool sits = data.stableInTargetSubgp;

	target = data.gensTargetSubgp;  // deep copy

	if ( sits )
		removeGen( target, data.stableLetter );
	else {
		lesg = target.first();
		if ( lesg ) {
			temp = firstChild; // perhaps treated specially
			mpm = &( temp -> data );
			if ( mpm -> leftExp == 0 ) {
				g = lesg -> data; // deep copy
				g.adjoinSubscript( 0 );
				( mpm -> gensTargetSubgp ).prepend( g ); // cyclic subgp
				( mpm -> inheritedTarget ).prepend( g );
				mpm -> problemType = mp;
				}
			}
		}

	if ( sits )
		temp = firstChild;
	else
		temp = firstChildWithPotentialPinch();

	while ( temp != 0 ) {

	  mpm = &( temp -> data );

	  lesg = ( mpm -> test ).first();
	  while ( lesg ) {
		g = lesg -> data;  // deep copy
		g.del(); // remove first subscript
		if (( appearsIn( g, target )) && ( !appearsIn( lesg -> data,
		  mpm -> inheritedTarget )))
			( mpm -> inheritedTarget ).prepend( lesg -> data );
		lesg = lesg -> next;
		}

	  lemme = data.lmm.first();    // assume nonempty
	  while ( lemme != 0 ) {
		mme = &( lemme -> data );
		g = mme -> entry;	// deep copy
		for ( count = mme -> small; count <= mme -> big; ++count ) {
			if (( sits ) && ( appearsIn( g, target ))) {
				g.adjoinSubscript( count );
				if ( appearsIn( g, data.rewrittenRelator )) {
				  if ( !appearsIn( g, mpm -> inheritedTarget ))
					( mpm -> inheritedTarget ).prepend( g );
				  }
				g.del();	// remove subscript
				}
			if (( count < mme -> big ) && ((( sits )
			  && ( mpm -> rightExp < 0 )) || ( mpm -> pinchT == up ))) {
				  g.adjoinSubscript( count );
				  if ( !appearsIn( g, mpm -> gensTargetSubgp ))
					( mpm -> gensTargetSubgp ).prepend( g );
				  if ( !appearsIn( g, mpm -> gensAssocSubgp ))
					( mpm -> gensAssocSubgp ).prepend( g );
				  g.del();	// remove subscript
				  }
			if (( count > mme -> small ) && ((( sits )
			  && ( mpm -> rightExp > 0 )) || ( mpm -> pinchT == down ))) {
				  g.adjoinSubscript( count );
				  if ( !appearsIn( g, mpm -> gensTargetSubgp ))
					( mpm -> gensTargetSubgp ).prepend( g );
				  if ( !appearsIn( g, mpm -> gensAssocSubgp ))
					( mpm -> gensAssocSubgp ).prepend( g );
				  g.del();	// remove subscript
				  }
			}   // the for loop

		lemme = lemme -> next;
		}

	if ( sits ) {
		if (( mpm -> gensTargetSubgp ).isTrivial() )
			mpm -> problemType = wp;
		else 
			mpm -> problemType = mp; // might be reset to rp
		temp = temp -> nextSibling;
		}
	else {
		if (( mpm -> gensTargetSubgp ).isTrivial() )
			mpm -> problemType = wp;
		else
			mpm -> problemType = mp;
		temp = temp -> nextSiblingWithPotentialPinch();
		}
	}
}


void TreeEltMP::expand()
	// assume that relator, test, parent, relatorLength have been defined
	// assume that relatorLength > 1
    // define exps1, stableLetter, accompanyingGen,
    // exps2 if accompanyingGen is not trivial, rewrittenRelator, lmm,
    // rewrittenTest
{
    data.rewriteRelator();
        // it defines stableLetter, its exponent sum exps1, accompanyingGen
        // if accompanyingGen is not trivial it defines its exponentsum exps2
        // it defines rewrittenRelator
    data.createMinMaxSubscriptTable(); // it defines lmm
    data.rewriteTest();  // the test word is not the same among siblings
}




TreeEltMP* TreeEltMP::firstLeaf()
{
    TreeEltMP *temp2 = this;

    if ( temp2 == 0 ) return(0);
    if ( temp2 -> firstChild == 0 )
      return( 0 );
    while ( temp2 -> firstChild != 0 )
        temp2 = temp2 -> firstChild;
    return (temp2);
}

void TreeEltMP::adjoinInheritedToTargetsOfChildren()
{
	TreeEltMP *temp = firstChild;
	MagProb *prob;
	ListElt<superGen>* lesg;
	superGen *g;

	while (temp ) {
		prob = &( temp -> data );
		lesg = ( prob -> inheritedTarget ).first();
		if ( lesg )
			prob -> problemType = mp; // it may have been wp before
		while ( lesg ) {
			g = &( lesg -> data );
			if ( !appearsIn( *g, prob -> gensTargetSubgp ))
				( prob -> gensTargetSubgp ).prepend( *g );
			lesg = lesg -> next;
			}
		temp = temp -> nextSibling;
		}
}



class treeMP{
private:
	friend class treeIterator;
    TreeEltMP* h;  // the root
public:
    treeMP() { h = 0; }
    treeMP(TreeEltMP& te): h( &te ) {}	// planting the root te
    void plantRoot( TreeEltMP *temp ) { h = temp; }
    ~treeMP();
    TreeEltMP* root() { return (h); }
    void pruneBackToRoot();

};

TreeEltMP* TreeEltMP::nextLeaf( TreeEltMP *leaf1 )
    // returns 0 if we are at the last leaf
{
    TreeEltMP *temp2 = leaf1;

    if ( temp2 == 0 ) return(0);
    if ( parent == 0 ) return(0); // the root does not have a next leaf

    while (( temp2 -> nextSibling == 0 ) && ( temp2 != this ))  
         temp2 = temp2 -> parent;
    if (temp2 == this ) 
      return(0); // there are no more leaves
    temp2 = temp2 -> nextSibling;    // it must be nonzero
    if ( temp2 -> firstChild )
      temp2 = temp2 -> firstLeaf();
    return(temp2);
}


void treeMP::pruneBackToRoot()
{
	while ( h -> firstChild != 0 ) h -> prune();
}


treeMP::~treeMP()
{
	pruneBackToRoot();
	delete h;
	h = 0;

}

void TreeEltMP::spawn() // assume that rewrittenRelator has length > 1
// Assume that rewrittenTest is not a power of stableLetter
// It creates firstChild and all its siblings
// It creates a new relator and a new test in each child,
// It defines the list exponentsStableLetter
// Assume that rewrittenTest is nontrivial
{
   ListElt<superGen>* lesg; // will run down the word rewrittenTest
   superGen* g;  // g is the data in lesg
   MagProb* mpm, *ompm;
   superWord* newTest;
   TreeEltMP* temp, *oldTemp;
   superGen* stableL;
   int expStableL;
   ListElt<int>* lei;
   bool mustStartNewTest = true;
   bool startWithStable = false;
   int zero = 0;

   if ( !data.exponentsStableLetter.isTrivial() )
		data.exponentsStableLetter.release();
		   // needed when at the root a second time
   oldTemp = 0; // will signal first time
   stableL = &( data.stableLetter );

   lesg = ( data.rewrittenTest ).first();

		// We will break up rewrittenTest into subwords - the test words
		// of the children

   while (lesg != 0) {

	 g = &(lesg -> data);

     if ( sameSubscripts( *g, *stableL ) ) {

	expStableL = g -> exp();
	data.exponentsStableLetter.prepend( expStableL );
	mustStartNewTest = true;
	if ( oldTemp != 0 ) { // define pinchT
	  ompm = &( oldTemp -> data );
	  ( ompm -> test ).reverse();
	  ompm -> rightExp = expStableL;
	  if (( ompm -> leftExp < 0 ) && ( ompm -> rightExp > 0 ))
		  ompm -> pinchT = down;
	  else {
		if (( ompm -> leftExp > 0 ) && ( ompm -> rightExp < 0 ))
		  ompm -> pinchT = up;
		else
          ompm -> pinchT = none;
	    }
          }
	else // old Temp is 0
	  startWithStable = true;
	}

      else {  // g is not the stable letter

	if ( mustStartNewTest ) {

	  mustStartNewTest = false;
	  temp = new TreeEltMP;
	  if ( temp == 0 ) {
	    cerr << "\nOut of memory!\n";
	    exit(1);
	    }

	  temp -> parent = this;
	  mpm = &(temp -> data);

	  if ( oldTemp == 0 ) { // signals first time
	    mpm -> childNumber = 1;
	    firstChild = temp;
	    if ( !startWithStable ) {
	      data.exponentsStableLetter.prepend( zero );
	      mpm -> leftExp = 0;
	      }
	    else
	      mpm -> leftExp = expStableL;
	    }

	  else {  // oldTemp is the previous child
	    mpm -> childNumber = (( oldTemp -> data ).childNumber ) + 1;
	    oldTemp -> nextSibling = temp;
	    temp -> previousSibling = oldTemp;
	    mpm -> leftExp = expStableL;
		}

	  oldTemp = temp;
	  mpm -> relator = data.rewrittenRelator;    // deep copy
	  mpm -> relatorLength = ( data.rewrittenRelator ).length();
	  newTest = &( mpm -> test );
	  } // end of mustStartNewTest

	newTest -> prepend( *g );

	if ( lesg -> next == 0 ) {
	  data.exponentsStableLetter.prepend( zero );
	  mpm -> rightExp = 0;
	  mpm -> pinchT = none;
	  ( mpm -> test ).reverse();
	  }

	} // end of the case where g is not the stable letter

      lesg = lesg -> next;
      }

   data.exponentsStableLetter.reverse();

}

bool TreeEltMP::input()
{

#ifdef READ_LENGTHS

char c;

#endif

	data.childNumber = 0;
	data.leftExp = 0;
	data.rightExp = 0;
	data.problemType = wp;
	data.pinchT = none;
	data.stableInTargetSubgp = false;

#ifdef READ_LENGTHS

#ifdef INFILE

    if ( !( inFile >> len1 ))
      return( false );
    c = inFile.get();

#else

    cin >> len1;
    c = cin.get();

#endif

    if ( c != '\n' ) {
//@rn        cerr << "\nBad Input!\n";
	exit( 1 );
        }

#endif

	if ( !( data.inputRelator() ))
		return( false ); // define relator
	data.relatorLength = data.relator.length();  // define its length

#ifdef READ_LENGTHS

#ifdef INFILE

    inFile >> len2;
    c = inFile.get();

#else

    cin >> len2;
    c = cin.get();

#endif

    if ( c != '\n' ) {
//@rn        cerr << "\nBad Input!\n";
	exit( 1 );
        }

#endif

	data.inputTest();    // define test
	return( true );
}

void MagProb::defineOutsideGens()
    // generators that appear in the test word and not in r or target
{
	ListElt<superGen> *lesg = test.first();
	superGen *g;

	while ( lesg != 0 ) {
		g = &( lesg -> data );
		if ( ( !appearsIn( *g, relator )) &&
		    ( !appearsIn( *g, gensTargetSubgp )))
			outsideGens.prepend( *g );
		lesg = lesg -> next;
		}
	}

void amalgCycles( const superWord &r, superWord &t )
{
	ListElt<superGen> *lesg = r.first();
	superGen *g1, *g2, *g3;
	int x1, x2, x3, olt, nlt, x;

	g1 = &( lesg -> data );
	x1 = g1 -> exp();
	lesg = lesg -> next;
	g2 = &( lesg -> data );
	x2 = g2 -> exp();
	nlt = t.length();
	do {
		olt = nlt;
		lesg = t.first();
		while ( lesg != 0 ) {
			if ( sameSubscripts( *g2, lesg -> data )) {
				x = ( lesg -> data ).exp();
				if ( x % x2 == 0 ) { // then x is divisible by x2
					lesg -> data = *g1;	// deep copy
					( lesg -> data ).assignExponent( -x1*x/x2 );
					}
				}
			lesg = lesg -> next;
			}
		t.freelyReduce();
		nlt = t.length();
		if ( nlt == olt ) { // then no reduction has occurred
			lesg = t.first();
			while ( lesg != 0 ) {
				if ( sameSubscripts( *g1, lesg -> data )) {
					x = ( lesg -> data ).exp();
					if ( x % x1 == 0 ) { // then x is divisible by x1
						lesg -> data = *g2;	// deep copy
						( lesg -> data ).assignExponent( -x2*x/x1 );
						}
					}
				lesg = lesg -> next;
				}
			t.freelyReduce();
			nlt = t.length();
			}
		} while (( nlt < olt ) && ( nlt > 1));

}

void doAmalgCycles( MagProb *prob )
{
    ListElt< superGen > *lesg, *lesh;
    superGen *g, *h;
    int x1, x2, x3;


    amalgCycles( prob -> relator, prob -> test );
	// the relator defines a free product of a free group with
	// an amalgamated product of 2 cycles; prob -> test will have
	// been reduced by this function

#ifdef VERBOSE_MODE
	    cout << "\n\nAmalgamated product of two cycles:";
	    cout << "\nReduced test word:\n";
	    prob -> printTestWord();
	    cout << "Relator:\n";
	    prob -> printRelator();
		cout << "Generators for the target subgroup:\n";
	    ( prob -> gensTargetSubgp ).print();
	    cout << endl;
#endif

	    lesg = ( prob -> test ).first();
	    while ( lesg != 0 ) {
		g = &( lesg -> data );
		if (( !appearsIn( *g, prob -> gensTargetSubgp )) &&
		  ( appearsIn ( *g, prob -> relator ))) {
		    lesh = ( prob -> relator ).first();
		    h = &( lesh -> data );
		    if ( sameSubscripts( *g, *h )) {
			lesh = lesh -> next;
			h = &( lesh -> data );
			} // g,h are now the generators in r
		    x1 = g -> exp();
		    x2 = exponentSum( *g, prob -> relator );
		    if ( x1 % x2 ) {
		       // then g cannot be written as a power of h
			lesg = lesg -> next;
			continue; // with next generator in prob->test
			}
		    else {
			x3 = exponentSum( *h, prob -> relator );
			h -> assignExponent( -x3*x1/x2 );
			lesg -> data = *h; // deep copy
			break;
			}
		    }
		    lesg = lesg -> next;
		}

	    ( prob -> test ).freelyReduce();


#ifdef VERBOSE_MODE
	    cout << "\nThe test word becomes:\n";
		    prob -> printTestWord();
#endif

}



bool inSubgp( const superWord &test, const superWord &gensSubgp )
{
	ListElt<superGen> *lesg = test.first();
	superGen *g;

	while ( lesg ) {
	g = &( lesg -> data );
	if ( !appearsIn( *g, gensSubgp ))
		return( false );
	lesg = lesg -> next;
	}
	return( true );
}


class treeIterator {
private:
	treeMP *pt; // associated treeMP
	TreeEltMP *pte;  // current node being visited
	answerType answer;
	bool answerTold;
public:
	treeIterator(treeMP& t): pt( &t ), pte( t.h ), answer( maybe ),
		answerTold( false ) {}
	// create an iterator, with associated tree t, visiting the root
	TreeEltMP *nextSibling() { return pte -> nextSibling; }
	TreeEltMP *previousSibling() { return pte -> previousSibling; }
	TreeEltMP *parent() { return pte -> parent; }
	TreeEltMP *firstChild() { return pte -> firstChild; }
	TreeEltMP *currentNode() { return pte; }
	bool atRoot() { if ( !parent() ) return( true );
		else return ( false );}
	void printCurrentNode();
	void genProb();
	void tellYes() { cout << "Yes!\n"; }
	void tellNo() { cout << "No!\n"; }
	void tellAnswer() { if ( answer == yes ) tellYes();
		else tellNo(); answerTold = true; }
	bool replied() { if ( answerTold ) return( true );
		else return( false ); }
	void setAnswerForShortRel();
	void resetAnswerNode();
	void pruneBackToCurrentNode();


};

void treeIterator::pruneBackToCurrentNode() {

	while ( pte -> firstChild != 0 ) pte -> prune();
}


void treeIterator::printCurrentNode()
{
	MagProb *prob;

	if ( pte == 0 )
	return;

	prob  = &( pte -> data );
	prob -> printInfo();

}


void cycPermuteToBeginWith( superWord& w, const superGen& g1 )
	// conjugate so as to put g1 in first position
{
	ListElt<superGen> *lesg;
	superGen *g;
	superWord w1, w2, w3;

	lesg = w.first();
	g = &( lesg -> data );
	if ( sameSubscripts( lesg -> data, g1 ))
		return;

	while ( !sameSubscripts( lesg -> data, g1 )) {
		w1.prepend( *g );
		lesg = lesg -> next;
		if ( lesg == 0 )
			return; // then g1 is absent from w
		g = &( lesg -> data );
		}

	while (lesg ) {
		g = &( lesg -> data );
		w2.prepend( *g );
		lesg = lesg -> next;
		}

	lesg = w1.first();
	while ( lesg ) {
		g = &( lesg -> data );
		w3.prepend( *g );
		lesg = lesg -> next;
		}

	lesg = w2.first();
	while ( lesg ) {
		g = &( lesg -> data );
		w3.prepend( *g );
		lesg = lesg -> next;
		}


	w = w3;
}


bool testDecomp( const superWord &tau, const superWord *assoc,
	const superWord *inherited, superWord &tpart, superWord &apart ) {

	superWord tau_reversed;  // deep copy
	ListElt< superGen > *lesg = tau.first();
	superGen *g;

	while ( lesg ) {
		g = &( lesg -> data );
		tau_reversed.prepend( *g );
		lesg = lesg -> next;
		}

	lesg = tau_reversed.first();
	while ( lesg ) {
		g = &( lesg -> data);
//		if (( appearsIn( *g, *assoc )) && 
//		    ( !appearsIn( *g, *inherited ))) {
		if ( appearsIn( *g, *assoc )) {
			apart.prepend( *g );
			lesg = lesg -> next;
			}
		else {
		  tpart.prepend( *g );
		  lesg = lesg -> next;
		  while ( lesg ) {
		    g = &( lesg -> data );
		    if ( !appearsIn( *g, *inherited ))
		      return( false );
		  else 
		    tpart.prepend( *g );
		  lesg = lesg -> next;
		  }
		}
	}

	// If we get here, then the test word decomposes

	return( true );

}



bool TreeEltMP::testDecomposes()

	// We assume that there is a parent with problem type mp.
	// If the test word decomposes, then the test word of the
	// parent will be modified, as well as the test word of the
	// next sibling (we assume that there is a next sibling).

{
	MagProb *prob = &data;
	int re = prob -> rightExp;
	ListElt< superGen > *lesg = ( prob -> test ).first();
	superGen *g, h;
	superWord *assoc = &( prob -> gensAssocSubgp );
	superWord *inherited = &( prob -> inheritedTarget );
	superWord tpart, apart; // factors of the target subgroup
	TreeEltMP* npte = nextSibling;
	MagProb *nprob;
	TreeEltMP* ppte = parent;
	MagProb *pprob = &( ppte -> data );
	superWord *nextTest;
	TreeEltMP* temp, *temp1;
	MagProb *mpm, *mpm1;
	ListElt<int> *lei;
	int fs, chn, exp, count;
	list< int > newExps;

	if ( assoc -> isTrivial() )
	  return( true );

  #ifdef VERBOSE_MODE

	cout << "\nArrived in testDecomposes()\n" << flush;

  #endif

	if ( npte ) {
		nprob = &( npte -> data );
		nextTest =  &( nprob -> test );
		}

	if ( !testDecomp( prob -> test, assoc, inherited, tpart, apart ))
		return( false );

	if ( apart.isTrivial() )
		return( true );

	if ( tpart.isTrivial() )
		( prob -> test ).release(); // kill it
	else
		prob -> test = tpart;   // deep copy

	if ( re > 0 ) {
		lesg = apart.first();   // decrement subscripts
		while ( lesg ) {
			g = &( lesg -> data );
			lei = g -> firstSubscript();
			fs = lei -> data;
			--fs;
			g -> del();
			g -> adjoinSubscript( fs );
			lesg = lesg -> next;
			}
		}

	if (( re > 1 ) || (( re == 1 ) && ( npte == 0 ))) {
	 // then we must insert a new child

		temp = new TreeEltMP;
		if ( temp == 0 ) {
			cerr << "\nOut of memory!\n";
			exit(1);
			}

		temp -> parent = ppte;
		temp -> previousSibling = this;
		nextSibling = temp;
		temp -> nextSibling = npte;

		mpm = &(temp -> data);

		mpm -> rightExp = re - 1;
		mpm -> leftExp = 1;
		prob -> rightExp = 1;
		if ( npte )
			nprob -> leftExp = mpm -> rightExp;

		chn = prob -> childNumber;
		mpm -> childNumber = chn + 1;

		lei = ( pprob -> exponentsStableLetter ).first();
		count = 0;
		while ( lei ) {
			exp = lei -> data;
			if ( count != chn )
				newExps.prepend( exp );
			else {
				newExps.prepend( 1 );
				newExps.prepend( exp -1 );
				}
			++count;
			lei = lei -> next;
			}
		( pprob -> exponentsStableLetter ).release();
		lei = newExps.first();
		while ( lei ) {
			exp = lei -> data;
			( pprob -> exponentsStableLetter ).prepend( exp );
			lei = lei -> next;
			}
		temp1 = npte;
		while ( temp1 ) {
			mpm1 = &( temp1 -> data );
			++( mpm1 -> childNumber );
			temp1 = temp1 -> nextSibling;
		    }
		mpm -> pinchT = none;
		mpm -> relator = prob -> relator;   // deep copy
		mpm -> test = apart;	// deep copy
		mpm -> relatorLength = prob -> relatorLength;
		if ( re > 0 ) {
		  mpm -> gensAssocSubgp = prob -> gensAssocSubgp; // deep copy
		  mpm -> gensTargetSubgp = prob -> gensAssocSubgp; //deep copy
		}
		lesg = apart.first();
		while ( lesg ) {
		  g = &( lesg -> data );
		  h = *g; // deep copy
		  h.del();
		  if ( appearsIn( h, pprob -> gensTargetSubgp )) {
		      if ( !appearsIn( *g, mpm -> inheritedTarget )) 
			( mpm -> inheritedTarget ).prepend( *g );
		      if ( !appearsIn( *g, mpm -> gensTargetSubgp ))
		        ( mpm -> gensTargetSubgp ).prepend( *g );
		      }
		  lesg = lesg -> next;
		}
		if (( mpm -> gensTargetSubgp ).isTrivial() )
		  mpm -> problemType = wp;
		else
		  mpm -> problemType = mp;

	    }

	if ( re < 0 ) {
		lesg = apart.first();   // increment subscripts
		while ( lesg ) {
			g = &( lesg -> data );
			lei = g -> firstSubscript();
			fs = lei -> data;
			++fs;
			g -> del();
			g -> adjoinSubscript( fs );
			lesg = lesg -> next;
			}
		}

	if (( re < -1 ) || (( re == -1 ) && ( npte == 0 ))) {
	 // then we must insert a new child

		temp = new TreeEltMP;
		if ( temp == 0 ) {
		    cerr << "\nOut of memory!\n";
			exit(1);
		    }

		temp -> parent = ppte;
		temp -> previousSibling = this;
		nextSibling = temp;
		temp -> nextSibling = npte;

		mpm = &(temp -> data);

		mpm -> rightExp = re + 1;
		mpm -> leftExp = -1;
		prob -> rightExp = -1;
		if ( npte )
			nprob -> leftExp = mpm -> rightExp;

		chn = prob -> childNumber;
		mpm -> childNumber = chn + 1;

		lei = ( pprob -> exponentsStableLetter ).first();
		count = 0;
		while ( lei ) {
			exp = lei -> data;
			if ( count != chn )
				newExps.prepend( exp );
			else {
				newExps.prepend( -1 );
				newExps.prepend( exp + 1 );
				}
			++count;
			lei = lei -> next;
			}
		( pprob -> exponentsStableLetter ).release();
		lei = newExps.first();
		while ( lei ) {
			exp = lei -> data;
			( pprob -> exponentsStableLetter ).prepend( exp );
			lei = lei -> next;
			}

		temp1 = npte;
		while ( temp1 ) {
			mpm1 = &( temp1 -> data );
			++( mpm1 -> childNumber );
			temp1 = temp1 -> nextSibling;
			}

		mpm -> pinchT = none;
		mpm -> relator = prob -> relator;   // deep copy
		mpm -> test = apart;	// deep copy
		if ( re < 0 ) {
		  mpm -> gensAssocSubgp = prob -> gensAssocSubgp; // deep copy
		  mpm -> gensTargetSubgp = prob -> gensAssocSubgp; //deep copy
		}

		mpm -> relatorLength = prob -> relatorLength;
		mpm -> gensAssocSubgp = prob -> gensAssocSubgp; // deep copy

		lesg = apart.first();
		while ( lesg ) {
		  g = &( lesg -> data );
		  h = *g; // deep copy
		  h.del();
		  if ( appearsIn( h, pprob -> gensTargetSubgp )) {
		      if ( !appearsIn( *g, mpm -> inheritedTarget )) 
			( mpm -> inheritedTarget ).prepend( *g );
		      if ( !appearsIn( *g, mpm -> gensTargetSubgp ))
		        ( mpm -> gensTargetSubgp ).prepend( *g );
		      }
		  lesg = lesg -> next;
		}
		if (( mpm -> gensTargetSubgp ).isTrivial() )
		  mpm -> problemType = wp;
		else
		  mpm -> problemType = mp;


	}

	if (( re == 1 ) || ( re == -1 )) {
	// move generators; extend the inheritedTarget and target of mpm
        apart.reverse();
	    lesg = apart.first();
	    while ( lesg ) {
		    g = &( lesg -> data );
		    nextTest -> prepend( *g );
		    h = *g; // deep copy
		    h.del();
		    if ( appearsIn( h, pprob -> gensTargetSubgp )) {
		      if ( !appearsIn( *g, nprob -> inheritedTarget )) 
			( nprob -> inheritedTarget ).prepend( *g );
		      if ( !appearsIn( *g, nprob -> gensTargetSubgp ))
		        ( nprob -> gensTargetSubgp ).prepend( *g );
		      }
		    lesg = lesg -> next;
		    }
	    }

	return( true );

}



void treeIterator::setAnswerForShortRel()
{
	MagProb *prob = &( pte -> data );
	int relLen= prob -> relatorLength;
	probType pt = prob -> problemType;
	ListElt< superGen > *lesg;
	TreeEltMP* nds = pte -> nextDesiredSibling();
	TreeEltMP* p = parent();
	bool spits;

    if ( p )
        spits = ( p -> data ).stableInTargetSubgp;

	if ( relLen == 1 ) {
		lesg = ( prob -> relator ).first();
		reduceByPowerOfGen( prob -> test, lesg -> data );
		}

	if ( relLen == 2 ) {
		doAmalgCycles( prob );
		}


#ifdef VERBOSE_MODE
	cout << "\nThe test word reduces to:\n";
		prob -> printTestWord();
#endif

	if (( prob -> test ).isTrivial() )
		answer = yes;

	else {

		if ( pt == wp )
			answer = no;
		else { // then pt must be mp
			if ( inSubgp( prob -> test, prob -> gensTargetSubgp )) {
				if ( nds ) {
					if ( spits ) {
						if ( pte -> testDecomposes() ) // sits is true
							answer = yes;
						else
							answer = no;
						}
					else
					   answer = yes; //
					}
				else
					answer = yes;
				}
			else
				answer = no;
			}
		} // end of case of nontrivial test word
}

void treeIterator::resetAnswerNode() 
  // the answer is yes for rp if the test word is reduced
  // the answer is yes for mp if the test word belongs to the target subgp
  // Do as little as possible
{
	MagProb *prob = &( pte -> data );
	int relatorLength;
	ListElt< superGen > *lesg;
	TreeEltMP* p = parent();
    TreeEltMP *fdc;
	probType tpp;
	probType pt = prob -> problemType;
	bool sits;
	bool spits;
	TreeEltMP* ns = nextSibling();
	TreeEltMP* nds = pte -> nextDesiredSibling();

	if ( p ) {
		tpp = ( p -> data ).problemType;
        spits = ( p -> data ).stableInTargetSubgp;
		}

	switch( answer ) {

	case no:

		if ( atRoot() ) {
			tellAnswer();
			return;
			}

		switch( tpp ) {
			case wp:
				if ( nds ) {
					pte = nds;
					answer = maybe;
					}
				else {
					pte = p;
					answer = no;
					}
				return;
			case rp:
				if ( nds ) {
					pte = nds;
					answer = maybe;
					}
				else {
					pte = p;
					( pte -> data ).problemType = mp;
					pte -> adjoinInheritedToTargetsOfChildren();
					pte = pte -> firstChild;
					answer = maybe;  // reduction is complete
					}
				return;
			case mp:
				if ( spits ) {
					pte = p;
					pte -> testFromChildren();
					answer = no;
					}
				else {
					if ( nds ) {
						pte = nds;
						answer = maybe;
						}
					else {
						pte = p;
						pte -> testFromChildren();
						answer = no;
						}
					}
				return;
			}  // end of switch( tpp )

		break;

	case yes:

		if ( atRoot() ) {
			tellAnswer();
			return;
			}

		switch( tpp ) {
			case wp:
			case rp:
				pte = p;
				pte -> testFromChildren();
				answer = maybe;
				return;
			case mp:
				if (( ns ) && ( spits )) {
					if ( pte -> testDecomposes() ) {

						pte = nextSibling(); // nextSibling may have changed
						answer = maybe;
						}
					else
						answer = no;
					}
				else {
					if ( !spits ) {
						if (( prob -> leftExp == 0 ) &&
						  ( prob -> rightExp == 0 )) {
							pte = p;
							pte -> testFromChildren();
							answer = yes;
							}
						else {
							pte = p;
							pte -> testFromChildren();
							answer = maybe;
							}
						}
					else { // last child
						pte = p;
						pte -> testFromChildren();
						answer = yes;
						}
					}
				return;
			}

		break;

	case maybe:

		if (( prob -> test ).isTrivial() ) {
			answer = yes;
			return;
			}

		if ( !prob -> abTest() ) {
			answer = no;
			return;
			}

		switch( pt ) {

			case mp:
			  if ( inSubgp( prob -> test, prob -> gensTargetSubgp )) {
				  answer = yes;
				  return;
				  }
			  prob -> defineOutsideGens();
			  pte -> expand();
			  sits = prob -> stableInTargetSubgp;
			  if ( !sits ) {
				lesg = ( prob -> gensTargetSubgp ).first();
				cycPermuteToBeginWith( prob -> relator,lesg -> data );
				}
			  pruneBackToCurrentNode();
			  pte -> spawn();
			  pte -> defineGensTargetAssocSubgpsForChildren();
				// the problemType may have changed to rp
			  fdc = pte -> firstDesiredChild();
			  if ( fdc ) {
				  pte = fdc;
				  return;
				  }
			  else {
				  if ( prob -> problemType == rp ) {
					  prob -> problemType = mp;
					  pte -> adjoinInheritedToTargetsOfChildren();
					  fdc = pte -> firstDesiredChild();
					  pte = fdc;
					  }
				  else
					  answer = no;
				  return;
				  }

			case rp: // we can only get here by coming up after reduction
			  if ( inSubgp( prob -> test, prob -> gensTargetSubgp )) {
				  answer = yes;
				  return;
				  }

                          prob -> defineOutsideGens();
			  pte -> expand();
			  pruneBackToCurrentNode();
			  pte -> spawn();
			  pte -> defineGensTargetAssocSubgpsForChildren();
			  fdc = pte -> firstDesiredChild();
			  if ( fdc )
				  pte = fdc;
			  else {
				  prob -> problemType = mp;
				  pte -> adjoinInheritedToTargetsOfChildren();
				  pte = pte -> firstChild;
				  }
			  break;
			case wp:
			  prob -> defineOutsideGens();
			  pte -> expand();
			  pruneBackToCurrentNode();
			  pte -> spawn();
			  pte -> defineGensTargetAssocSubgpsForChildren();
			  fdc = pte -> firstDesiredChild();
			  if ( fdc )
				  pte = fdc;
			  else
				  answer = no;

			} // end of switch( pt )

		}    // end of switch( answer )

}

void treeIterator::genProb() // treats the problem at the currentNode.
{
	MagProb *prob = &( pte -> data );
	bool beenAtRoot = false;
	int relLen = prob -> relatorLength;

  #ifdef VERBOSE_MODE

	TreeEltMP *ppte = pte -> parent;
	TreeEltMP *npte = pte -> nextSibling;
	cout << "\nAt the beginning of genProb() the answer and node are:\n";
	switch( answer ) {
	case no: cout << "\nNo!"; break;
	case yes: cout << "\nYes!"; break;
	case maybe: cout << "\nMaybe!"; break;
	}
	printCurrentNode();
	if ( ppte ) {
		cout << "\nand the parent node is:";
		( ppte -> data ).printInfo();
		}

  #endif

	if ( atRoot() ) {
		if (( prob -> test ).isTrivial()) {
			answer = yes;
			tellAnswer();
			return;
			}
		if ((( prob -> relator ).isTrivial() ) ||
		  ( answer == no )) {
			answer = no;
			tellAnswer();
			return;
			}
		if ( beenAtRoot )
			pt -> pruneBackToRoot();
		beenAtRoot = true;
		prob -> problemType = wp;
		answer = maybe;
		}

	if ( relLen <= 2 )
		setAnswerForShortRel();
	resetAnswerNode();

}


void doTree( TreeEltMP& root )
{
	treeMP probTree( root );
	treeIterator ti( probTree );

	do {
	    ti.genProb(); // at the current node (initially the root)
	    } while ( !ti.replied() );

}


bool treatRelatorTestWord()
{
	TreeEltMP root;

	if ( root.input() ) {

#ifdef READ_LENGTHS

//@rn        cout << "\nThe relator and test word have lengths " <<
//@rn            len1 << ", " << len2 << endl;

        if (( len1 > maxLen1 ) || ( len2 > maxLen2 )) 
            return( true );


#endif

		doTree( root );
		return( true );
		}
	else
		return( false );

}

main()
{
	int pairCount = 0;

	do {
		++pairCount;
//@rn		cout << "Pair number " << pairCount << ": ";
		} while ( treatRelatorTestWord() );

//@rn	cout << "Done!\n";
	return( 0 );
}

