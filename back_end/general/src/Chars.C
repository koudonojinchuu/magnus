//   Copyright (C) 1994 The New York Group Theory Cooperative
//   See magnus/doc/COPYRIGHT for the full notice.
 
// Contents: Implementation of Chars and CharsRep classes
//
// Principal Author: Roger Needham
//
// Status: Useable.
//
// Revision History:
//
// * 01/96 Dmitry B. implemented IPC tools.
//


#include "Chars.h"
#include <stdio.h>

//------------------------------------------------------------------------//
//---------------------------- CharsRep ----------------------------------//
//------------------------------------------------------------------------//


CharsRep::CharsRep( )
{
  length = 0;
  theChars = special;
  *theChars = 0;
}


CharsRep::CharsRep( const char* s )
{
  length = strlen(s);
  theChars = strdup(s);     //@rn Oops!
  if ( theChars == NULL )
	 error("out of memory in CharsRep::CharsRep(const char*)");
}


CharsRep::CharsRep( char c )
{
  length = 1;
  theChars = special;
  *theChars = c;
  *(theChars + 1) = 0;
}

CharsRep::CharsRep(int num) {
  char buffer[20];
  sprintf(buffer, "%d", num);
  theChars = strdup(buffer);
  length = strlen(theChars);
}

CharsRep::CharsRep( const CharsRep& CR )
{
  length = CR.length;
  if ( length < 2 ) {
	 theChars = special;
	 strcpy(special, CR.theChars);
  } else {
	 theChars = strdup(CR.theChars);
	 if ( theChars == NULL )
		error("out of memory in CharsRep::CharsRep(const CharsRep&)");
  }
}


CharsRep::~CharsRep( )
{
  if ( length > 1 )
	 delete theChars;
}


char& CharsRep::ref(int i) 
{
#if ( SAFETY > 0 )
  if ( i < 0 || i >= len() ) error("Chars index out of bounds"
				   " in char& CharsRep::ref(int)");
#endif
  return theChars[i];
}
  
char CharsRep::val(int i) const 
{
#if ( SAFETY > 0 )
  if ( i < 0 || i >= len() ) error("Chars index out of bounds"
				   " in char CharsRep::val(int) const");
#endif
  return theChars[i];
}

void CharsRep::concat( const char* s, int len )
{
  char* temp = theChars;
  theChars = new char[length + len + 1];
  if ( theChars == NULL )
	 error("out of memory in CharsRep::concat(const char*, int)");
  strcpy(theChars, temp);
  if ( length > 1 )
	 delete [] temp;
  strcpy(theChars + length, (char*)s);
  length += len;
}

inline static char* expandBuffer( char* oldBuffer, int oldSize, int newSize )
{
  char *newBuffer = new char[ newSize ];
  memcpy( newBuffer, oldBuffer, oldSize );
  delete oldBuffer;
  return newBuffer;
}

char* getLine( istream& istr )
{
  //@njz: 'ipfx' obsoleted
  //  if (!istr.ipfx(0) || (!(istr.flags() & ios::skipws) && !ws(istr))) {
  //    // Redundant if using GNU iostreams.
  //    istr.clear(ios::failbit|istr.rdstate()); 
  //    char *str = new char[0];
  //    *str = '\0';
  //    return str;
  //  }
  //

  int ch;
  int bufsize = 4;
  int used = 0;
  char *buf = new char[ bufsize ];
  streambuf *sb = istr.rdbuf();

  //@njz: detecting the starting of a string with a non-space char.
  bool starting = true;
  //

  while ((ch = sb->sbumpc()) != EOF) {

    //@njz:
    //    if (isspace(ch)) {
    //      break;
    //    }
    if (isspace(ch)) {
      if (starting)
	continue;
      else
	break;
    }

    starting = false;
    //

    if( used == bufsize ) {
      buf = expandBuffer( buf, used, bufsize *= 2 );
    }
    buf[ used++ ] = ch;
  }
  buf = expandBuffer( buf, used, used+1 );
  buf[used] = '\0';

  //@njz
  //  int new_state = istr.rdstate();
  ios::iostate new_state = istr.rdstate();
  //
  if (used == 0) new_state |= ios::failbit;
  if (ch == EOF) new_state |= ios::eofbit;
  istr.clear(new_state);

  return buf;
}

void CharsRep::readFrom(istream& istr)
{
  // Using class String here should be fairly harmless, since it
  // precisely duplicates the semantics in use up to now.
  // Of course, it will fail if the caller is trying to read
  // a whitespace-delimited string longer than 2^15, but that seems
  // especially unlikely.

  if ( length > 1 )
    delete theChars;

  char *str = getLine( istr );

  //@njz: debug
  cerr << "getLine() obtains : " << str << endl;
  //

  length = strlen( str );

  if ( length < 2 ) {
    strcpy(special, str);
    theChars = special;
    delete str;
  } else {
    theChars = str; 
    if ( theChars == NULL )
      error("out of memory in CharsRep::readFrom(istream&)");
  }
}

  
void CharsRep::write( ostream& ostr ) const
{
  ostr < length;
  if( !length) return;
  
  for( int i = 0; i < length; ++i )
    if( theChars[i] != ' ' )
      ostr << theChars[i];
    else
      ostr << '\377';
  ostr << " ";

/*  
  ostr << length << " ";
  for( int i = 0; i < 2; ++i )
    ostr << special[i] << " ";
  if( length > 1) 
    for( i = 0; i < length; ++i )
      ostr << theChars[i] << " ";
*/
}

void CharsRep::read( istream& istr )
{
  int len;
  istr > len;
  if( !len ) {
    length = 0;
    theChars = special;
    *theChars = 0;
    return;
  }
  
  readFrom(istr);
  for( int i = 0; i < length; ++i )
    if( theChars[i] == '\377' )
      theChars[i] = ' ';
/*
  if( length > 1 )
    delete [] theChars;
  
  istr >> length;
  for( int i = 0; i < 2; ++i )
    istr >> special[i];
  if( length > 1) {
    theChars = new char[length+1];
    for( int i = 0; i < length; ++i )
      istr >> theChars[i];
    theChars[length] = 0;
  }
  else
    theChars = special;
*/  
}


//------------------------------------------------------------------------//
//------------------------------ Chars -----------------------------------//
//------------------------------------------------------------------------//


Chars Chars::operator + (const Chars& C) const
{
  Chars result = *this;
  result += C;
  return result;
}


Chars Chars::operator + (const char* s) const
{
  Chars result = *this;
  result += s;
  return result;
}


Chars Chars::operator + (char c) const
{
  Chars result = *this;
  result += c;
  return result;
}


Chars& Chars::operator += (const Chars& C)
{
  change()->concat( C.look()->getChars(), C.length() );
  return *this;
}


Chars& Chars::operator += (const char* s)
{
  change()->concat( s, strlen(s) );
  return *this;
}


Chars& Chars::operator += (char c)
{
  char special[2];
  special[0] = c;
  special[1] = 0;
  change()->concat( special, 1 );
  return *this; 
}
//========================================================================
//                    Global scope functions
//========================================================================


Chars ordinalPostfix(int num) {
  if( num % 10 == 1 && num % 100 != 11 ) return "st";
  else if( num % 10 == 2 && num % 100 != 12 ) return "nd";
  else if( num % 10 == 3 && num % 100 != 13 ) return "rd";
  else return "th";
}
// returns ordinal postfix for given number, i.e. "st", "nd", "rd" or "th"


static const char escape_char      = 'a';
static const char escaped_escape   = 'a';
static const char escaped_space    = 'b';
static const char escaped_lcbrace  = 'c';
static const char escaped_rcbrace  = 'd';

Chars encodeSpaces( const Chars& s )
{
  const int slen = s.length();
  char *buf = new char[ 2 * slen + 3 ];

  char *pos = buf;

  *pos++ = '{';
  for(int i = 0; i < slen; ++i ) {
    switch( s[i] ) {
    case escape_char:
      *pos++ = escape_char;
      *pos++ = escaped_escape;
      break;
    case ' ':
      *pos++ = escape_char;
      *pos++ = escaped_space;
      break;
    case '{':
      *pos++ = escape_char;
      *pos++ = escaped_lcbrace;
      break;
    case '}':
      *pos++ = escape_char;
      *pos++ = escaped_rcbrace;
      break;
    default:
      *pos++ = s[i];
    }
  }
  *pos++ = '}';
  *pos   = '\0';

  Chars result(buf);
  delete buf;

  return result;
}


Chars decodeSpaces( const Chars& s )
{
  const int slen = s.length();
  char *res = new char[slen-1];
  char *pstr = res;

#if SAFETY > 0
  if( s[0] != '{' || s[slen-1] != '}' ) {
    error("decodeSpaces(const Chars&): malformed string.");
  }
#endif

  for( int i = 1; i < slen-1 ; ++i, ++pstr ) {
    if( s[i] != escape_char ) {
      *pstr = s[i];
    }
    else {
      switch( s[++i] ) {
      case escaped_escape:  *pstr = escape_char; break;
      case escaped_space:   *pstr = ' ';         break;
      case escaped_lcbrace: *pstr = '{';         break;
      case escaped_rcbrace: *pstr = '}';         break;
      default:
	error(Chars("Unexpected escaped char '") + s[i] + "' in '" + s + "'");
      }
    }
  }
  *pstr = '\0';

  Chars result(res);
  delete res;
  return result;
}

