// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementation of classes GACPforORGSolverGene, GAConjProblemForORGroupSolver, GAConjProblemForORGroupConjecture
//
// Principal Author: Alexander Ushakov
//
// Status: in progress
//
// Revision History:
//


#include "GACPforORGSolver.h"

//#define debug_Conjecture
//#define debug_reproduction



//---------------------------------------------------------------------------//
//--------------------------- GACPforORGSolverGene --------------------------//
//---------------------------------------------------------------------------//


unsigned GACPforORGSolverGene::jumpSize = 10;
const int GACPforORGSolverGene::ANY = 65534;
const int GACPforORGSolverGene::NOONE = 65535;
UniformRandom GACPforORGSolverGene::r;


Word GACPforORGSolverGene::randomWord( int gens , int wLen )
{
  Word w;

  do{
    int g = r.rand( 0 , gens-1 ) + 1;
    g *= 2 * r.rand( 0 , 1 ) - 1;    
    w *= Word( Generator( g ) );
    w = w.freelyReduce( );
  } while( w.length( )<wLen );
  
  return w;
}



GACPforORGSolverGene::GACPforORGSolverGene( OneRelatorGroup& group , const Word& w1 , const Word& w2 ) :
  theWord1( w1 ), 
  theWord2( w2 ), 
  theGroup(group),
  curExp( 0 ),
  fit( -1 ), 
  chromosomes( new GACPforORGSolverChromosome**[ w2.length( ) ] ),
  sizes( new unsigned[ w2.length( ) ]),
  lengthes( new unsigned[ w2.length( ) ]),
  hasConjecture( false ),
  hasShorterWords( false ),
  nChr( 0 )
{ 
  exp = getExp( );
  for( unsigned i=0 ; i<w2.length( ) ; ++i ) {
    chromosomes[i] = 0;
    sizes[i] = 0;
    lengthes[i] = 0;
  }
}



void GACPforORGSolverGene::resize( unsigned pos , unsigned newsize )
{
  unsigned i;
  GACPforORGSolverChromosome** nc = new GACPforORGSolverChromosome*[newsize];
  for( i=0 ; i<sizes[pos] ; ++i ) nc[i] = chromosomes[pos][i];
  for( i=sizes[pos] ; i<newsize ; ++i ) nc[i] = 0;
  if( chromosomes[pos] )
    delete []chromosomes[pos];
  chromosomes[pos] = nc;
  sizes[pos] = newsize;

  if( nc==0 )
    error( "GACPforORGSolverGene::resize" );

}



GACPforORGSolverGene::GACPforORGSolverGene( const GACPforORGSolverGene& g ) :
  theWord1( g.theWord1 ),
  theWord2( g.theWord2 ),
  theGroup(g.theGroup),
  fit( g.fit ),
  chromosomes( new GACPforORGSolverChromosome**[ theWord2.length( ) ] ),
  sizes( new unsigned[ theWord2.length( ) ] ),
  lengthes( new unsigned[ theWord2.length( ) ] ),
  exp(g.exp),
  curExp(g.curExp),
  nChr( g.nChr ),
  hasConjecture( false ),
  hasShorterWords( false )
{ 
  for( unsigned pos=0 ; pos<theWord2.length( ) ; ++pos ) {
    
    sizes[pos] = g.sizes[pos];
    lengthes[pos] = g.lengthes[pos];
    
    if( sizes[pos]>0 )
      chromosomes[pos] = new GACPforORGSolverChromosome*[ sizes[pos] ];
    else
      chromosomes[pos] = 0;
    
    for( unsigned j=0 ; j<g.lengthes[pos]; ++j )
      if( g.chromosomes[pos][j] ) chromosomes[pos][j] = new GACPforORGSolverChromosome( *g.chromosomes[pos][j] ) ;
      else chromosomes[pos][j] = 0;
    
  }
}


//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


//@njz
//double GACPforORGSolverGene::proximity( const Word& W1 , const Word& W2 , int* c1=0 , int* c2=0 )
double GACPforORGSolverGene::proximity( const Word& W1 , const Word& W2 , int* c1 , int* c2 )
//
{
  Word w1 = W1.inverse( ).freelyReduce( );
  Word w2 = W2.freelyReduce( );
  
  double res = 0;
  
  for( unsigned i=0 ; i<w1.length( ) ; i++ , w1 = w1.cyclicallyPermute( 1 ) ) {
    for( unsigned j=0 ; j<w2.length( ) ; j++ , w2 = w2.cyclicallyPermute( 1 ) ) {
      Word w3 = w1 * w2;
      w3 = w3.freelyReduce( );
      double len = w3.length( );
      if( (!i && !j) || len<res ) {
	if( c1 && c2 ) {
	  *c1 = i;
	  *c2 = j;
	}
	res = len;
      }
    }
  }
  
  if( w1.length()==0 || w2.length()==0 ) {
    Word w3 = w1 * w2;
    w3 = w3.freelyReduce( );
    res = w3.length( );
    if( c1 && c2 ) {
      *c1 = 0; 
      *c2 = 0;
    }
  }
  
  return res;
}


//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


double GACPforORGSolverGene::fitness( )
{
  if( fit==-1 ) {
    
    Word newWord = theWord2;
    Word relator = theGroup.relator();

    for( int pos=theWord2.length()-1 ; pos>=0 ; pos-- ) {
      for( unsigned c=0 ; c<lengthes[pos] ; ++c ) {
	GACPforORGSolverChromosome *chr = chromosomes[pos][c];
	Word w1;
	if( chr->deg ) w1 = chr->con * relator * chr->con.inverse( );
	else w1 = chr->con * relator.inverse( ) * chr->con.inverse( );
	GAConjProblemForORGroupSolver::insert( newWord , w1 , pos );
      }
    }

    newWord = GAConjProblemForORGroupSolver::greedyReduce( theGroup , newWord );

    //if new word shorter than second word
    if( newWord.length( )<theWord2.length( ) ) {
      
      hasShorterWords = true;

      //if newWord shorter than first word, change their places      
      if( newWord.length()<theWord1.length() ) {
	*this = GACPforORGSolverGene( theGroup , newWord , theWord1 );
	// here you needn't to check gene, because this function was invoked in Solver::tournament()
	// and if flag hasShorterWords sets, it invoke toStart() func., that resets all genes
	// check( );
	fit = proximity( theWord1 , theWord2 );
      }
      else {
	*this = GACPforORGSolverGene( theGroup , theWord1 , newWord );
	// here you needn't to check gene, because this function was invoked in Solver::tournament()
	// and if flag hasShorterWords sets, it invoke toStart() func., that resets all genes
	// check( );
	fit = proximity( theWord1 , theWord2 );
      }
    }	
    else {
      int c1 , c2;
      fit = proximity( theWord1 , newWord , &c1 , &c2 );
      
      //int L = theWord2.length( )-theWord1.length();
      //L>=0 always
      
      //There's two way
      //if reduced more than half of words, I do conjecture that other two parts are equal
      //if obtained a pair like ( w , w x ), I do conjecture that x==1
      if( fit/(theWord2.length( )+theWord1.length()) < 0.5 && theWord2.length( )+theWord1.length()>10 &&
	  //prev. condition (line) may be changed by next condition (line), prev. condition stronger than next one
	  //fit<=L  &&  
	  fit!=0  &&  
	  theWord1.length( )>3 ) 
	{
	  Word conjWord1;
	  Word conjWord2 = ( theWord1.inverse( ).cyclicallyPermute(c1) * newWord.cyclicallyPermute(c2) );
	  conjWord2 = conjWord2.freelyReduce();

	  hasConjecture = true;
	  conjectureWord = conjWord2;
	}
    }
    
  }
  return fit;
}


//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


GACPforORGSolverGene::~GACPforORGSolverGene( )
{
  clear( );
}


//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


void GACPforORGSolverGene::clear( )
{
  for( unsigned pos=0 ; pos<theWord2.length( ) ; ++pos ) {
    for( unsigned j=0 ; j<lengthes[pos] ; ++j )
      delete chromosomes[pos][j];
    delete[] chromosomes[pos];
  }
  
  delete []chromosomes;
  delete []lengthes;
  delete []sizes;
}


//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


GACPforORGSolverGene& GACPforORGSolverGene::operator = ( const class GACPforORGSolverGene& g )
{
  clear( );
  
  theWord1 = g.theWord1;
  theWord2 = g.theWord2;
  
  nChr = g.nChr;
  exp = g.exp;
  curExp = g.curExp;
  fit = g.fit;
  lengthes = new unsigned [ theWord2.length( ) ]; 
  sizes = new unsigned [ theWord2.length( ) ];
  chromosomes = new GACPforORGSolverChromosome**[ theWord2.length( ) ];
  
  for( unsigned pos=0 ; pos<theWord2.length( ) ; ++pos ) {
    
    sizes[pos] = g.sizes[pos];
    lengthes[pos] = g.lengthes[pos];
    
    if( sizes[pos]>0 )
      chromosomes[pos] = new GACPforORGSolverChromosome*[ sizes[pos] ];
    else
      chromosomes[pos] = 0;
    
    for( unsigned j=0 ; j<g.lengthes[pos]; ++j )
      if( g.chromosomes[pos][j] ) chromosomes[pos][j] = new GACPforORGSolverChromosome( *g.chromosomes[pos][j] ) ;
      else chromosomes[pos][j] = 0;
  }

  return *this;
}


//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


GACPforORGSolverChromosome* GACPforORGSolverGene::randomChromosome( bool deg ) const 
{
  int cLen = 0;
  while( GAConjProblemForORGroupSolver::roulette( 1 , 3 ) ) 
    ++cLen;
  
  Word c = randomWord( theGroup.numberOfGenerators( ), cLen );
  
  return new GACPforORGSolverChromosome( c , deg );
}


//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


void GACPforORGSolverGene::check( )
{
  // add chromosomes if (exp != curExp) 
  // such gene isn't accessible

  if( curExp!=exp && exp!=ANY && exp!=NOONE ) {
    for( int i=0 ; i<abs(exp-curExp) ; ++i) {
      GACPforORGSolverChromosome* chr = randomChromosome( exp<curExp );
      int pos = GAConjProblemForORGroupSolver::rnd1( theWord2.length( ) );
      if( lengthes[pos]==sizes[pos] )
	resize( pos , sizes[pos]+jumpSize );
      chromosomes[pos][lengthes[pos]++] = chr;
      nChr++;
    }
    curExp = exp;
    fit = -1;
    return;
  }
}


//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


bool GACPforORGSolverGene::mutation( )
{
  //1. add two new chromosomes to Gene or delete two chromosomes if accessible
  switch( GAConjProblemForORGroupSolver::roulette( 1 , 1 ) ) {
  case 0: {
    bool deg = ( GAConjProblemForORGroupSolver::rnd1( 2 )==0);
    
    //insert random chromosome in gene
    {
      GACPforORGSolverChromosome *chr = randomChromosome( deg );
      int pos = GAConjProblemForORGroupSolver::rnd1( theWord2.length( ) );
      addChr( chr , pos , GAConjProblemForORGroupSolver::rnd1( lengthes[pos] ) );
    }
    
    fit = -1;
    if( exp==ANY ) return true;
    
    //insert second random chromosome (with other degree) in gene
    {
      GACPforORGSolverChromosome *chr = randomChromosome( !deg );
      int pos = GAConjProblemForORGroupSolver::rnd1( theWord2.length( ) );
      addChr( chr , pos , GAConjProblemForORGroupSolver::rnd1( lengthes[pos] ) );
    }
  }
  break;
  
  // Delete chromosome from gene
  case 1 : {
    unsigned pos, pos2;
    unsigned d1=0, d2=0;
    for( pos=0 ; pos<theWord2.length() ; ++pos )
      for( pos2=0 ; pos2<lengthes[pos]  ; ++pos2 )
	if( chromosomes[pos][pos2]->getDeg( ) ) ++d1;
					else ++d2;
    
    if( exp==ANY ) {
      //delete chromosome of any degree
      if( d1+d2==0 ) return false;
      findPos( ALL , GAConjProblemForORGroupSolver::rnd1(d1+d2) , pos , pos2 );
      delChr( pos , pos2 );
    }
    else {
      if( d1==0 || d2==0 )
	return false;
      findPos( POSITIVE , GAConjProblemForORGroupSolver::rnd1(d1) , pos , pos2 );
      delChr( pos , pos2 );
      findPos( NEGATIVE , GAConjProblemForORGroupSolver::rnd1(d2) , pos , pos2 );
      delChr( pos , pos2 );
    }
    fit = -1;
  }
  break;
  }
  
  return true;
}


//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


bool GACPforORGSolverGene::permutation( )
{
  int count = 0;
  for( unsigned pos=0 ; pos<theWord2.length() ; ++pos )
    count += lengthes[pos];
  if( count<2 ) return false;
  
  int ch1 = GAConjProblemForORGroupSolver::rnd1( count );
  int ch2 = GAConjProblemForORGroupSolver::rnd1( count-1 );
  if( ch2>=ch1 )
    ch2++;
  
  unsigned p11, p12, p21, p22;
  findPos( ALL , ch1 , p11 , p12 );
  findPos( ALL , ch2 , p21 , p22 );
  
  GACPforORGSolverChromosome* tmp = chromosomes[p11][p12];
  chromosomes[p11][p12] = chromosomes[p21][p22];
  chromosomes[p21][p22] = tmp;
  
  fit = -1;
  return true;
}


//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


bool crossover( GACPforORGSolverGene& g1 , GACPforORGSolverGene& g2 )
{
  unsigned gp1 = 0;
  unsigned gn1 = 0;
  unsigned gp2 = 0;
  unsigned gn2 = 0;
  
  unsigned pos, pos2;
  
  for( pos=0 ; pos<g1.theWord2.length() ; ++pos ) 
    for( pos2=0 ; pos2<g1.lengthes[pos] ; ++pos2 ) 
      if( g1.chromosomes[pos][pos2]->getDeg( ) ) ++gp1;
      else ++gn1;
  
  for( pos=0 ; pos<g2.theWord2.length() ; ++pos ) 
    for( pos2=0 ; pos2<g2.lengthes[pos] ; ++pos2 ) 
      if( g2.chromosomes[pos][pos2]->getDeg( ) ) ++gp2;
      else ++gn2;
  
  unsigned gp = ( gp1<gp2 ? gp1 : gp2 );
  unsigned gn = ( gn1<gn2 ? gn1 : gn2 );
  if( gn==0 && gp==0 )
    return false;
  
  if( GAConjProblemForORGroupSolver::roulette( gp , gn ) ) {
    //swap two negative chromosomes
    unsigned num1 = GAConjProblemForORGroupSolver::rnd1( gn1 );
    unsigned num2 = GAConjProblemForORGroupSolver::rnd1( gn2 );
    unsigned p11, p12, p21, p22;
    g1.findPos( GACPforORGSolverGene::NEGATIVE , num1 , p11 , p12 );
    g2.findPos( GACPforORGSolverGene::NEGATIVE , num2 , p21 , p22 );
    GACPforORGSolverChromosome* tmp = g1.chromosomes[p11][p12];
    g1.chromosomes[p11][p12] = g2.chromosomes[p21][p22];
    g2.chromosomes[p21][p22] = tmp;
  }
  else {
    //swap two positive chromosomes
    unsigned num1 = GAConjProblemForORGroupSolver::rnd1( gp1 );
    unsigned num2 = GAConjProblemForORGroupSolver::rnd1( gp2 );
    unsigned p11, p12, p21, p22;
    g1.findPos( GACPforORGSolverGene::POSITIVE , num1 , p11 , p12 );
    g2.findPos( GACPforORGSolverGene::POSITIVE , num2 , p21 , p22 );
    GACPforORGSolverChromosome* tmp = g1.chromosomes[p11][p12];
    g1.chromosomes[p11][p12] = g2.chromosomes[p21][p22];
    g2.chromosomes[p21][p22] = tmp;
  }

  g1.fit = -1;
  g2.fit = -1;
  return true;
}


//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


void GACPforORGSolverGene::findPos( _FindPos tp, unsigned num , unsigned& p1 , unsigned& p2 ) const
{
  for( unsigned pos=0 ; pos<theWord2.length() ; ++pos ) {
    for( unsigned pos2=0 ; pos2<lengthes[pos]  ; ++pos2 ) {
      
      if( tp==ALL 
	  || ( tp==POSITIVE && chromosomes[pos][pos2]->getDeg( ) ) 
	  || ( tp==NEGATIVE && !chromosomes[pos][pos2]->getDeg( ) ) ) {
	if( num==0 ) {
	  p1 = pos;
	  p2 = pos2;
	  return;
	}
	--num;
      }
    }
  }
		
  error( "Internal error: GACPforORGSolverGene::findPos" );
}

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

void GACPforORGSolverGene::delChr( unsigned p1, unsigned p2 )
{
  if( p1>theWord2.length( ) || p2>=lengthes[p1] )
    error( "Internal error: GACPforORGSolverGene::delChr" );
  
  delete chromosomes[p1][p2];
  for( unsigned t=p2 ; t<lengthes[p1]-1 ; ++t )
    chromosomes[p1][t] = chromosomes[p1][t+1];
  chromosomes[p1][--lengthes[p1]] = 0;
  
  --nChr;
}

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

void GACPforORGSolverGene::addChr( GACPforORGSolverChromosome* chr , unsigned p1, unsigned p2 )
{
  if( p1>theWord2.length( ) || p2>lengthes[p1] )
    error( "Internal error: GACPforORGSolverGene::addChr" );
  
  if( lengthes[p1]==sizes[p1] )
    resize( p1 , sizes[p1]+jumpSize );
  
  for( unsigned j=lengthes[p1] ; j>p2 ; --j )
    chromosomes[p1][j] = chromosomes[p1][j-1];
  chromosomes[p1][p2] = chr;
  lengthes[p1]++;
  
  ++nChr;
}

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

int GACPforORGSolverGene::getExp( )
{
  unsigned i;
  unsigned nr = theGroup.numberOfGenerators( );

  Word w1 = theWord1;
  Word w2 = theWord2;

  Word rel = theGroup.relator( );
  int *e = new int[nr];
  int *er = new int[nr];
  
  for( i=0 ; i<nr ; ++i ) er[i] = e[i] = 0;
  
  for( i=0 ; i<w1.length( ) ; ++i )  { int g = ord( w1[i] );  e[ abs(g)-1 ]  -= g/abs(g); }
  for( i=0 ; i<w2.length( ) ; ++i )  { int g = ord( w2[i] );  e[ abs(g)-1 ]  += g/abs(g); }
  for( i=0 ; i<rel.length( ) ; ++i ) { int g = ord( rel[i] ); er[ abs(g)-1 ] += g/abs(g); }
  
  int res = ANY;
  for( i=0 ; i<nr ; ++i ) if( er[i]!=0 ) {
    res = e[i]/er[i];
    break;
  }
  
  for( i=0 ; i<nr ; ++i ) if( res*er[i]!=e[i] ) {
    res = NOONE;
    break;
  }

  delete []e;
  delete []er;

  return res;
}


//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


//reset the word2 and clear chromosomes table
/*
void GACPforORGSolverGene::reset( const Word& word )
{
  clear( );
  
  theWord2 = word;
  
  nChr = 0;
  fit = -1;
  lengthes = new unsigned [ theWord2.length( ) ]; 
  sizes = new unsigned [ theWord2.length( ) ];
  chromosomes = new GACPforORGSolverChromosome**[ theWord2.length( ) ];
  
  for( unsigned pos=0 ; pos<theWord2.length( ) ; ++pos ) {
    
    sizes[pos] = 0;
    lengthes[pos] = 0;
    
    if( sizes[pos]>0 )
      chromosomes[pos] = new GACPforORGSolverChromosome*[ sizes[pos] ];
    else
      chromosomes[pos] = 0;
  }
  
}
*/


//---------------------------------------------------------------------------//
//------------------- GAConjProblemForORGroupSolver -------------------------//
//---------------------------------------------------------------------------//



double    GAConjProblemForORGroupSolver::prob[2][3] = { { 1 , 0 , 0 } , { 0.3 , 0 , 0.7 } };
const int GAConjProblemForORGroupSolver::NOCONJ = 65535;


//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


GAConjProblemForORGroupSolver::GAConjProblemForORGroupSolver( const OneRelatorGroup& group , const Word& W1 , const Word& W2 , File* f ) : 
  theGroup( group ),
  file( f ),
  deleteFile( false ),
  fitnessRate( 0 ),
  numGenes( 50 )
{
  genes = new GACPforORGSolverGene*[numGenes];
  for( int i=0 ; i<numGenes ; ++i ) genes[i] = 0;
  
  // create two helper genes, they are used for reproduction
  newGene[0] = new GACPforORGSolverGene( theGroup , Word() , Word() );
  newGene[1] = new GACPforORGSolverGene( theGroup , Word() , Word() );

  toStart( W1 , W2 );
}


//@njz
//GAConjProblemForORGroupSolver::GAConjProblemForORGroupSolver( const OneRelatorGroup& group , const Word& W1 , const Word& W2 , bool createFile = true , bool cp = true ) : 
GAConjProblemForORGroupSolver::GAConjProblemForORGroupSolver( const OneRelatorGroup& group , const Word& W1 , const Word& W2 , bool createFile , bool cp ) : 
//
  theGroup( group ),
  conjProblem( cp ),
  fitnessRate( 0 )
{
  if( createFile ) { 
    file = new File;
    deleteFile = true;
  }
  else {
    file = 0;
    deleteFile = false;
  }

  numGenes = 50;
  genes = new GACPforORGSolverGene*[numGenes];
  for( int i=0 ; i<numGenes ; ++i ) genes[i] = 0;
  
  //create two genes
  newGene[0] = new GACPforORGSolverGene( theGroup , Word() , Word() );
  newGene[1] = new GACPforORGSolverGene( theGroup , Word() , Word() );

  if( file )
    if( conjProblem )
      *file << "This genetic algorithm tries to determine whether given words are conjugate." << endl << endl;
    else
      *file << "This genetic algorithm tries to determine whether the given word is trivial." << endl << endl;

  toStart( W1 , W2 );
}


//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


void GAConjProblemForORGroupSolver::toStart( const Word& W1 , const Word& W2 )
{
  theWord1 = W1;
  theWord2 = W2;

  lastImprovement = 0;

  if( file ) {
    *file << "Run algorithm on words ";
    theGroup.printWord( *file , W1 );
    *file << " and ";
    theGroup.printWord( *file , W2 );
    *file << "." << endl << endl;
  }

  //1. try to reduce words in the given group (by the greedy algorithm)
  Word w1 = greedyReduce( theGroup , theWord1 );
  Word w2 = greedyReduce( theGroup , theWord2 );

  //2. if words are reducible, note about
  if( file && ( w1.length()<W1.length() || w2.length()<W2.length() ) ) {
    *file << "The length of at least one of the words can be reduced. "
	  << "We are going to continue to work with the shorter words: ";
    theGroup.printWord( *file , w1 );
    *file << " and ";
    theGroup.printWord( *file , w2 );
    *file << "." << endl << endl;
  }
  
  //3. first word must be shorter than second one
  if( w1.length()>w2.length() ) {
    Word tmp = w1;
    w1 = w2;
    w2 = tmp;
  }

  //4. remove all existing genes
  for( int i=0 ; i<numGenes ; ++i ) {
    if( genes[i] ) delete genes[i];
    genes[i] = 0;
  }

  //5. set first gene ( just one gene in the population at this time )
  genes[0] = new GACPforORGSolverGene( theGroup , w1 , w2 );
  
  //6. fast check
  genes[0]->check( );
  if( GACPforORGSolverGene( theGroup , w1 , w2 ).noConj( ) ) {
    if( file )
      if( conjProblem )
	*file << "Words are not conjugate because they are not conjugate in the abelian quotient." << endl << endl;
      else
	*file << "The word is not trivial because it is not trivial in the abelian quotient." << endl << endl;
    bestFit = NOCONJ;
    return;
  }

  //7. compute fitness of the first gene
  bool start;
  bestFit = genes[0]->fitness( );

  //8. if we got even more shorter words, then go to start with these
  if( genes[0]->getHasShorterWords( ) )
    toStart( genes[0]->getWord1( ) , genes[0]->getWord2( ) );
  else {
    if( file )
      *file << "Fitness = " << bestFit << endl << endl;
  }
}

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


GAConjProblemForORGroupSolver::~GAConjProblemForORGroupSolver()
{
  for( int i=0 ; i<numGenes ; ++i ) 
    if( genes[i] ) 
      delete genes[i];
  
  delete []genes;
  delete newGene[0];
  delete newGene[1];

  if( deleteFile )
    delete file;
}


//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


bool GAConjProblemForORGroupSolver::isConj( )
{
  if( bestFit==NOCONJ ) {
    if( conjProblem )
      *file << "Given words are not conjugate." << endl;
    else 
      *file << "The word is not trivial." << endl;
    return false;
  }

  theIter1 = 1 ;
  theIter2 = 0;

  for( ; bestFit!=0 ; ++theIter1, ++theIter2, ++lastImprovement ) {
    
    if( theIter1 % 100 == 0 && file ) 
      *file << "Generation " << theIter1 << ", best fitness " << bestFit << endl << endl;

    int newGenes = reproduction( );
    for( int g=0 ; g<newGenes ; ++g )
      if( tournament( *newGene[g] ) )
	break;

    checkImprovementTime( );
  }
  
  if( conjProblem )
    *file << "Given words are conjugate." << endl;
  else
    *file << "The given word is trivial." << endl;

  return true;
}


//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


void GAConjProblemForORGroupSolver::checkImprovementTime( )
{
  if( lastImprovement >= 40*numGenes ) {
    int g = selectGene( );
    if( file )
      *file << "The algorithm does not seem to be making any progress. "
	    << "Restarting the algorithm." << endl << endl;
    toStart( genes[g]->getWord1( ) , genes[g]->getWord2( ) );
    theIter2 = 0;
    lastImprovement = 0;
    ++fitnessRate;
  }
}


//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


int GAConjProblemForORGroupSolver::selectGene( ) const
{
  int result;
  double *p = new double[numGenes];

  for( int t=0 ; t<numGenes ; ++t )
    if( genes[t] ) p[t] = 1/genes[t]->fitness( );
    else p[t] = 0;

  result = roulette( numGenes , p );
  delete []p;

  return result;
}


//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


bool GAConjProblemForORGroupSolver::tournament( GACPforORGSolverGene &gene )
{
  double newFit = gene.fitness( );

  // number of last iterations with conjecture
  // I do next iteration through 1000 iterations
  static int it = theIter1;

  if( newFit==0 ) {
    bestFit = 0;
    lastImprovement = 0;
    if( file )
    *file << "Generation " << theIter1 << ", best fitness " << bestFit << endl << endl;
    return false;
  }

  if( gene.getHasShorterWords( ) ) {
    if( file )
      *file << "Given words can be shortened." << endl << endl;
    toStart( gene.getWord1( ) , gene.getWord2( ) );
    theIter2 = 0;
    return true;
  }

  //Conjecture
  if( gene.getHasConjecture( ) && theIter1 - it>1000 ) {
    Word conjWord = gene.getConjectureWord( );

    int nCheck = 1;
    if( checkedWords.bound(conjWord) )
      nCheck = checkedWords.valueOf( conjWord )+1;

    if( theIter1 - it > 1000*nCheck ) {
      checkedWords.bind( conjWord , nCheck );
      
      if( file ) {
	*file << "The algorithm will try to check whether ";
	theGroup.printWord( *file , conjWord );
	*file << " is trivial.";
	*file << endl << endl;
      }

      GAConjProblemForORGroupConjecture conjecture( theGroup , conjWord , file );
      bool conjResult = conjecture.isConj( 1000 , theIter1 );
      it = theIter1;

      if( conjResult ) {
	bestFit = 0;
	lastImprovement = 0;
	if( file ) {
	  *file << "The word is trivial." << endl << endl;
	  *file << "Generation " << theIter1 << ", best fitness " << bestFit << endl << endl;
	}
	return false;
      }

      if( file ) 
	*file << "The algorithm couldn't determine whether the word was trivial." << endl << endl;    
    }
  }
  
  // get random chromosome and compare it with the new one by roulette wheel
  int g = rnd1( numGenes );
  
  if( newFit<bestFit ) {
    delete genes[g];
    genes[g] = new GACPforORGSolverGene( gene );
    bestFit = newFit;
    lastImprovement = 0;
    if( file )
      *file << "Generation " << theIter1 << ", best fitness " << bestFit << endl << endl;
    return false;
  }
  
  if( !genes[g] )
    genes[g] = new GACPforORGSolverGene( gene );
  else {
    double prob1 = newFit, prob2 = genes[g]->fitness( );
    double min = ( prob1<prob2 ? prob1 : prob2 );
    if( min>5 ) { 
      prob1 -= min - 5;
      prob2 -= min - 5;
    }
    prob1 += fitnessRate;
    prob2 += fitnessRate;

    if( (roulette( prob1 , prob2 )==1 ) ) {
      delete genes[g];
      genes[g] = new GACPforORGSolverGene( gene );
    }
  }
  return false;
}


//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


int GAConjProblemForORGroupSolver::reproduction( )
{
  int pr = ( theIter2<2*numGenes ? 0 : 1 );
  switch( roulette( 3 , prob[pr] ) ) {
  case 0: 
    {
      int g = selectGene( );
      *newGene[0] = *genes[g];
      if( newGene[0]->mutation( ) ) return 1;
    }
    break;
  case 1:
    {
      int g = selectGene( );
      *newGene[0] = *genes[g];
      if( newGene[0]->permutation( ) ) return 1;
    }
    break;
  case 2:
    {
      int g1 = selectGene( );
      int g2;
      {
	GACPforORGSolverGene *_tmp = genes[g1];
	genes[g1] = 0;
	g2 = selectGene( );
	genes[g1] = _tmp;
      }
      *newGene[0] = *genes[g1];
      *newGene[1] = *genes[g2];
      if( crossover( *newGene[0] , *newGene[1] ) ) return 2;
    }
    break;
  }
  return 0;
}


//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


Word GAConjProblemForORGroupSolver::greedyReduce( const OneRelatorGroup& group , const Word& word )
{
  Word w = word;
  w = w.cyclicallyReduce( );
  while( oneGreedyReduce( group , w ) );

  return w;
}


//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


bool GAConjProblemForORGroupSolver::oneGreedyReduce( const OneRelatorGroup& group , Word& w )
{
  for( int z=0 ; z<2 ; ++z ) {
    Word relator = ( z ? group.relator( ) : group.relator( ).inverse( ) );
    for( unsigned y=0 ; y<w.length( ) ; ++y ) {
      Word w1 = w;
      insert( w1 , relator , y );
      w1 =  w1.cyclicallyReduce( );
      if( w1.length() < w.length() ) {
	w = w1;
	return true;
      }
    }
  }
  
  return false;
}


//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


void GAConjProblemForORGroupSolver::insert( Word& dst , const Word& src , int pos)
{
  if( dst.length()<pos )
    error( "Internal error: bad parameter 'pos' of insert" );

  dst = dst.initialSegment( pos ) * src * dst.terminalSegment( dst.length()-pos );
}


//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

int GAConjProblemForORGroupSolver::rnd1( int max )
{
  static UniformRandom r;
  if( max<1 ) return 0;
  return r.rand( 0 , max-1 );
}


//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


int GAConjProblemForORGroupSolver::roulette( double d1 , double d2 )
{
  double d[2] = {d1 , d2};
  return roulette( 2 , d );
}


//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


int GAConjProblemForORGroupSolver::roulette( int num , double* d)
{
  int i;
  int max  = 100;
  int part = rnd1( max );
  double sum = 0 , s = 0;
  for( i=0 ; i<num ; ++i ) sum += d[i];
  for( i=0 ; i<num ; ++i ) {
    s += d[i];
    if( part <= s*max/sum && d[i]!=0 )
      return i;
  }
  return num-1;
}


//---------------------------------------------------------------------------//
//------------------- GAConjProblemForORGroupConjecture ---------------------//
//---------------------------------------------------------------------------//



bool GAConjProblemForORGroupConjecture::isConj( int maxIter , int& doneIter )
{
  if( bestFit==NOCONJ )
    return false;

  theIter1 = doneIter;
  theIter2 = 0;

  for( ; bestFit!=0 && theIter1<=maxIter+doneIter ; ++theIter1, ++theIter2, ++lastImprovement ) {
    
    if( theIter1 % 100 == 0 && file ) 
      *file << "Generation " << theIter1 << ", best fitness " << bestFit << endl << endl;

    int imp = lastImprovement;

    int newGenes = reproduction( );
    for( int g=0 ; g<newGenes ; ++g )
      if( tournament( *newGene[g] ) )
	break;

    // if there was an improvement
    if( imp!=lastImprovement && lastImprovement==0 ) 
      maxIter += theIter2;
    
  }

  doneIter = theIter1;
  
  if( bestFit==0 ) return true;
  return false;  
}


