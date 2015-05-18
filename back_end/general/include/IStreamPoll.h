// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Definition of istream helper class IStreamPoll.
//
//   IStreamPoll is a simple helper class to construct intermediate
//   results from input operations, which are then automatically
//   converted depending on usage.

// Principal Author: Stephane Collart
//
// Usage:
//
//   Suppose a class A admits an
//      istr& operator >> ( istream& i, A& a );
//   with the purpose of reading in a value for a. The operator can be
//   made to return a temporary of type IStreamPoll instead
//      IStreamPoll >> ( istream& i, A& a ) {
//         // read operations; a = ...; and so on as before
//         Chars return_msg; // the message to pass back
//         if (ok) return_msg = "";
//         else return_msg = "failure to read value in istream& i >> A& a";
//         return IStreamPoll(i,msg);
//      }
//   The return value is thus a temporary intermediate value of type
//   IStreamPoll, which is automatically converted depending on how
//   you use it. It can become
//   - an int, to do
//           A a[20]; int i = 0;
//           while ( i < 20 && cin >> a[i]) i++;
//   - a Chars, to poll an error message
//           A a;
//           Chars errMesg = cin >> a;
//   - an istream, to concatenate in the usual manner
//           A a,b;
//           cin >> a >> b;
//   
// Status: Useable.
//
// Restrictions:
//
// * the conversion to int from IStreamPoll relies for its semantic
//   correctness on the assumption that a read operation was succesful
//   if and only if the return message is the empty string
//
// Extensions:
//
// * if desired, one could add an explicit flag
//      bool successflag;
//   to IStreamPoll, and a constructor
//      IStreamPoll(istream&,bool status,Chars errmsg);
//   to uncouple the status of the read operation from the contents of
//   the message string
//
// Revision History:
//

#ifndef _READRESULT_H_
#define _READRESULT_H_


#include "Chars.h"


class IStreamPoll {
  
public :
  
  IStreamPoll( istream& is, Chars em = "" ) :
    istr(is), errMesg(em) { }
  // em must be passed by value to take actual arguments which
  // are local to an exiting function
  
  typedef istream& IstreamRef;
  // @stc what is this for?
  
  operator int( ) { return errMesg == Chars(""); }
  // to get a result condition: while (cin >> group);

  operator Chars( ) { return errMesg; }
  // to retrieve error message: Chars result = (cin >> group);
  
  operator IstreamRef( ) { return istr; }
  // to convert back to istream&
  
  
private :
  
  istream& istr;
  
  Chars errMesg;

};

#endif
