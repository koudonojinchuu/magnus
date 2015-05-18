// Copyright (C) 1994 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of the TietzeTrekker class.
//
// Principal Author: Roger Needham
//
// Status: experimental
//
// Revision History:
//
// * 5/27/95 Roger replaced the use of MAGNUS_HOME with MagnusHome.
//

#include "TietzeTrekker.h"
#include "MagnusHome.h"


TietzeTrekker::TietzeTrekker(const VectorOf<Chars>& genNames,
									  const SetOf<Word>& relators
									  ) :
  TietzeTrek(Chars("cd ")
				 + MagnusHome::magnusHome()
				 + "/back_end/black_boxes/TietzeTrek; ./TietzeTrek"
				 )
{
  initialize(genNames, relators);
}



TietzeTrekker::TietzeTrekker(const FPGroup& G) :
  TietzeTrek(Chars("cd ")
				 + MagnusHome::magnusHome()
				 + "/back_end/black_boxes/TietzeTrek; ./TietzeTrek"
				 )
{
  initialize( G.namesOfGenerators(), G.getRelators() );
}



void TietzeTrekker::initialize(const VectorOf<Chars>& genNames,
										 const SetOf<Word>& relators)
{
  // Catch degenerate case.

  if ( relators.cardinality() == 0 ) {
	 if ( genNames.length() == 0 ) {
		flags = TRIVIAL + CYCLIC + FINITE + FREE;
		order = 1;
		rank = 0;
	 }
	 else if ( genNames.length() > 1 ) {
		flags = HAS_FREE_FACTOR + FREE;
		order = INFINITE;
		rank = genNames.length();
	 }
	 else {
		flags = CYCLIC + HAS_FREE_FACTOR + FREE;
		order = INFINITE;
		rank = 1;
	 }
	 status = 1;
	 return;
	 //@rn tell TietzeTrek to quit
  }

  flags = 0;

  if ( genNames.length() > 51 ) {
	 warn("TietzeTrek can't handle more than 51 generators.");
	 status = 0;
	 return;
  }

//@rn  status = TietzeTrek.sanityCheck("loading string resources");
  status = TietzeTrek.start("loading string resources");
  if ( !status ) {
	 warn("back_end/black_boxes/TietzeTrek/TietzeTrek"
			"\nfailed startup check");
	 return;
  }

  TietzeTrek.toCommand() << "n\n";  // We don' need no steenkin' instruciones!

  SetIterator<Word> I( relators );

  // For each generator g which does not appear in a relator, we must
  // pipe out a relator g Z g^-1, and finally a relator Z. The generator
  // name Z is reserved for this.

  int any_bad_p = 0;
  for( int g = genNames.length(); g > 0; --g ) {
	 int occurs_p = 0;
	 while ( !I.done() ) {
		if ( Word(I.value()).numberOfOccurrences(g) != 0 ) {
		  occurs_p = 1;
		  break;
		}
		I.next();
	 }
	 I.reset();
	 if ( !occurs_p ) {
		any_bad_p = 1;
		printGenerator(TietzeTrek.toCommand(), g);
		TietzeTrek.toCommand() << 'Z';
		printGenerator(TietzeTrek.toCommand(), -g);
		TietzeTrek.toCommand() << ",\n";
	 }
  }
  if ( any_bad_p ) {
	 TietzeTrek.toCommand() << "Z,\n";
  }

  // Pipe out relators. Check as we go for any longer than 60; if so,
  // print a warning and bail out.

  while ( !I.done() ) {
	 if ( Word(I.value()).length() > 60 ) {
		warn("TietzeTrek can't handle relators longer than 60.");
		status = 0;
		return;
	 }
	 printWord(TietzeTrek.toCommand(), I.value());
//	 printWord(cerr, I.value());                     //@rn for debugging
	 I.next();
	 if ( !I.done() ) TietzeTrek.toCommand() << ",\n";
	 else TietzeTrek.toCommand() << "\n!\n";
  }
  TietzeTrek.toCommand().flush();

  // Make sure TietzeTrek is speaking to us.

  char buf[128];
  TietzeTrek.fromCommand().getline(buf, 128);
  // If everything is ok, this will suck in "#1 Input presentation:".
  //@rn Does it actually get something else? Investigate.
  buf[13] = '\0';
  if ( !strcmp(buf, "invalid input") ) {
	 warn("problem passing relators to TietzeTrek");
	 status = 0;
  }
}


void TietzeTrekker::printWord(ostream& ostr, const Word& w)
{
  // We have to
  // 1) translate all generator names to [a-zA-Z],
  // 2) split lines to TietzeTrek so that none is longer than 255,

  int len = w.length();
  for( int i = 0; i < len; i++ ) {
	 printGenerator(ostr, ord(w[i]));
	 if ( (i % 64) == 0 ) ostr << endl;
  }
}


void TietzeTrekker::printGenerator(ostream& ostr, int g)
{
  int ag = abs(g);
  if ( ag < 27 )
	 ostr << char('a' + ag - 1);
  else
	 ostr << char('A' + ag - 27);
  if ( g < 0 ) ostr << "-1";
}


Bool TietzeTrekker::getFactoid(Bool queuePresentations, long giveUpLimit)
{
  if ( status == 0 ) return FALSE;

  char buf[128];  // Adequate for input we expect.

  while ( --giveUpLimit > 0 ) {
  
	 TietzeTrek.fromCommand().getline(buf, 128);

	 // Leave this line in to see what we get from TietzeTrek.
	 // cerr << "+ " << buf << endl;

	 // Does buf contain a factoid or a presentation header?

	 if ( *buf == '*' ) {      // We have a factoid, or frankish bibble.
		
		buf[11] = '\0';         // Safe.

		if ( !strcmp(buf, "*** TRIVIAL") ) {
		  flags |= TRIVIAL + CYCLIC + FINITE + FREE;
		  order = 1;
		  rank = 0;
		  return TRUE;
		}
		if ( !strcmp(buf, "*** CYCLIC ") ) { flags |= CYCLIC; return TRUE; }
		if ( !strcmp(buf, "*** NON-TRI") ) {
		  flags |= HAS_FREE_FACTOR;
		  order = 0;
		  return TRUE;
		}
		
		if ( !strcmp(buf, "*** FINITE ") ) {
		  flags |= FINITE;
		  order = atoi(buf + 26);
		  if ( order == 1 ) {
			 flags |= TRIVIAL + FREE;
			 rank = 0;
		  }
		  return TRUE;
		}
		
		if ( !strcmp(buf, "*** FREE of") ) {
		  flags |= FREE;
		  rank = atoi(buf + 17);
		  if ( rank == 0 ) {
			 order = 1;
			 flags |= TRIVIAL + CYCLIC + FINITE;
		  }
		  else flags |= HAS_FREE_FACTOR;
		  return TRUE;
		}
		// Keep looking or give up.
	 }

	 else if ( *buf == '#' ) {  // We have a presentation.
		// Ignore the ancestry info for now.
		if ( queuePresentations ) {
		  FPGroup G;
		  Chars errMesg = TietzeTrek.fromCommand() >> G;
        //@rn check errMesg
		  presentations.append(G);
		}
		// If we're not queueing presentations, the lines they occupy are ignored.
	 }

	 // Else skip whatever buf holds.
  } // while

  return FALSE;
}


int TietzeTrekker::getOrder( )
{
  if ( flags & FINITE ) { return order; }
  if ( flags & ( FREE | HAS_FREE_FACTOR ) ) { return 0; }
  return -1;
}


int TietzeTrekker::getRank( )
{
  if ( flags & FREE ) { return rank; }
  error("TietzeTrekker::getRank called when group is not known to be free");
}


FPGroup TietzeTrekker::getPresentation( )
{
  while ( presentations.length() == 0 ) getFactoid(TRUE);
  // We're sure to get a presentation.

  FPGroup result = presentations[0];
  presentations.removeElementOfIndex(0);
  return result;
  //@rn need ListOf::pop
}
