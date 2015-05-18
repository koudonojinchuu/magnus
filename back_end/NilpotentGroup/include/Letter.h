// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of structure Letter
//
// This is a helper class for Free Nilpotent Groups.
//
//  Letter is an element of word: a generator together with its power,
//  i.e. an expression g^p. Generator value is always positive.
//
// Principal Author: Eugene Paderin
//
// Status: In progress
//
// Revision History:
//
// Special Notes:
//

#ifndef _LETTER_H_
#define _LETTER_H_

#include "Word.h"
#include "Generator.h"

struct Letter {

  // constructors

  Letter() : gen(1), power(0) {}
  // Default constructor makes fictive letter

  Letter(Generator g, int pow = 1) {
    if( ord(g) > 0 ) {
      gen = g; 
      power = pow; 
    }
    else { 
      gen = inv(g); 
      power = -pow; 
    }
  }

  Letter(int g, int pow = 1) {
    if( g > 0 ) { 
      gen = Generator(g); 
      power = pow; 
    }
    else { 
      gen = Generator(-g); 
      power = -pow; 
    }
  }

  // collects letters
  void collectWith (Generator g) {
    if( power == 0 ) {
      gen = abs(g);
      power = ::power(g);
    }
    else if( abs(g) == ord(gen) ) 
      power += ::power(g);
    else error("Attempt to collect two distinct letters.");
  }

  // conversion to Word
  operator Word() const {
    Word w;
    Generator g;
    int r;
    if(power > 0) {
      g = gen;
      r = power;
    } else if(power < 0) {
      g = inv(gen);
      r = -power;
    } else {
      return w;
    }
    for( ; r > 0; r--) w *= g;
    return w;
  }

  void printOn(ostream& os) const {
    os << gen;
    if(power != 1) os << "^" << power;
  }

  void invert() {  power = -power; }
  // Inverts the letter. Do not confuse with inv() global function.

  int shortLexIndex() const {
    int g = ord(gen);
    return (power > 0) ? 2*(g-1) : 2*g-1;
  }
  // Returns generator's ShortLex index. Does not work with power==0.

  // IPC write
  friend ostream& operator < (ostream& os, const Letter& let) {
    os < let.gen < let.power;
  }

  // IPC read
  friend istream& operator > (istream& is, Letter& let) {
    is > let.gen > let.power;
  }

  // Data

  Generator gen;
  int power;
};

// Global functions

inline ostream& operator<< (ostream& os, const Letter& let) {
  let.printOn(os);
  return os;
}

// inversion
inline Letter inv( const Letter& let) {
  return Letter(let.gen, -let.power); 
}

// comparison

inline bool operator == (const Letter& let1, const Letter& let2) {
  if( let1.power != let2.power ) return false;
  if( let1.power == 0 ) return true;
  return (let1.gen == let2.gen);
}

inline bool operator != (const Letter& let1, const Letter& let2) {
  return ! (let1 == let2);
}

#endif


