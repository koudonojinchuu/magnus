// Contents: Declaration of classes GAConjProblemForORGroupSolver, GAConjProblemForORGroupConjecture
//
// Principal Author: Alexander Ushakov
//
// Status: in progress
//
// Revision History:
//

#ifndef _GACPFORORGSOLVER_H_
#define _GACPFORORGSOLVER_H_

#include "Word.h"
#include "OneRelatorGroup.h"
#include "Associations.h"
#include "RandomNumbers.h"


//---------------------------------------------------------------------------//
//----------------------- GACPforORGSolverChromosome ------------------------//
//---------------------------------------------------------------------------//


class GACPforORGSolverChromosome
{
 public:
  GACPforORGSolverChromosome( const Word& c, bool d ) : deg(d) , con(c) { }
  
  bool getDeg( ) const { return deg; }
  
 protected:
  Word con;
  // conjugator
  bool deg;
  // degree of the relator 
  // false: -1
  // true :  1
  
  friend class GACPforORGSolverGene;
};



//---------------------------------------------------------------------------//
//------------------------ GACPforORGSolverGene -----------------------------//
//---------------------------------------------------------------------------//



class GACPforORGSolverGene
{
 public:

  /////////////////////////////////////////////////////////
  //                                                     //
  //  Constructors                                       //
  //                                                     //
  /////////////////////////////////////////////////////////

  GACPforORGSolverGene( OneRelatorGroup& group , const Word& w1 , const Word& w2 );
  GACPforORGSolverGene( const GACPforORGSolverGene& g );
  GACPforORGSolverGene& operator = ( const class GACPforORGSolverGene& g );
  ~GACPforORGSolverGene( );

  
  /////////////////////////////////////////////////////////
  //                                                     //
  //  Accessors                                          //
  //                                                     //
  /////////////////////////////////////////////////////////

  double fitness( );
  bool noConj( ) const { return exp==NOONE; }

  Word getWord1( ) const { return theWord1; }
  Word getWord2( ) const { return theWord2; }


  bool getHasShorterWords( ) { bool res = hasShorterWords; hasShorterWords = false; return res; }
  bool getHasConjecture( )   const { return hasConjecture; }
  Word getConjectureWord( )  const { return conjectureWord; } 

  static Word randomWord( int gens , int wLen );
  static double proximity( const Word& W1 , const Word& W2 , int* c1=0 , int* c2=0 );
  // proximity of two words. This function invoked by fitness function only.
  // Currently the results type is integer. Return a value that show 
  // how one (cyclically) word likes to another.

  GACPforORGSolverChromosome* randomChromosome( bool deg ) const;


public:

  /////////////////////////////////////////////////////////
  //                                                     //
  //  Reproduction functions                             //
  //                                                     //
  /////////////////////////////////////////////////////////


  void check( );
  // check exp. sum of generators in two given words 
  // and make some mutation to set it equal (by the relator)

  // next functions invoke by Solver::reproduction( )

  bool mutation( );

  bool permutation( );
  // permutation of any two chromosomes in chromosomes table
  // doesn't use currently

  friend bool crossover( GACPforORGSolverGene& g1 , GACPforORGSolverGene& g2 );
  // this function changes places of two chromosomes in g1 and in g2
  // if curExp!=ANY => the degree of chromosomes must be equal


private:

  /////////////////////////////////////////////////////////
  //                                                     //
  //  Data members                                       //
  //                                                     //
  /////////////////////////////////////////////////////////

  Word theWord1;             
  // first word
  Word theWord2;             
  // second word (algorithm deals with this word), this word isn't shorter than previous one
  OneRelatorGroup& theGroup; 
  // group

  static UniformRandom r;

  double fit;                //current fitness of gene, or -1 if it doesn't exist jet
  int nChr;                  //number of chromosomes  (not using field currently)


  /////////////////////////////////////////////////////////
  //                                                     //
  //  Chromosome table and functions deals with it       //
  //                                                     //
  /////////////////////////////////////////////////////////

  //Chromosome table has got theWord2.length() columns

  void resize( unsigned pos , unsigned newsize );
  GACPforORGSolverChromosome*** chromosomes; // pointer to chromosomes table
  unsigned* lengthes;        // lengthes of each column in chromosomes table
  unsigned* sizes;           // sizes of column 
  static unsigned jumpSize;  // number of increasing columns in table

  void clear( );             
  // clear the chromosome table


  void delChr( unsigned p1, unsigned p2 );
  void addChr( GACPforORGSolverChromosome* chr , unsigned p1, unsigned p2 );
  // add and delete chromosome from the table

  enum _FindPos{ ALL=0, POSITIVE=1, NEGATIVE=2};
  void findPos( _FindPos tp, unsigned num , unsigned& p1 , unsigned& p2 ) const;

 private:

  /////////////////////////////////////////////////////////
  //                                                     //
  //  Type of gene                                       //
  //                                                     //
  /////////////////////////////////////////////////////////

  bool hasConjecture;
  // this variable sets if algoritm obtained words with a big reduce
  Word conjectureWord;
  // result of the big reduce, it means just if variable "hasConjecture" sets
  // two word W1 and W2 with a big reduce - it means (in this version of algorithm) 
  // length(W1*W2) < ( length(W1)+length(W2) ) / 2

  bool hasShorterWords;
  // if algorithm obtained shorter words, it sets to true

  /////////////////////////////////////////////////////////
  //                                                     //
  //  Exponential sum of degree of chromosomes in table  //
  //                                                     //
  /////////////////////////////////////////////////////////

  int exp;
  int curExp;
  // exponetial sum of the RELATOR
  // these members must be equal
  
  int getExp( );
  // compute a number (exp), exponetial sum of RELATOR must be equal with
  
  static const int ANY;
  static const int NOONE;
  //const values for curExp
  //ANY => it means any number (exp may be equal to any integer value)
  //NOONE => it impossible to set exp to curExp and words are not conjugates

  //friend class GAConjProblemForORGroupSolver;
};



//---------------------------------------------------------------------------//
//------------------- GAConjProblemForORGroupSolver -------------------------//
//---------------------------------------------------------------------------//



class GAConjProblemForORGroupSolver
{
 public:

  /////////////////////////////////////////////////////////////////
  //                                                             //
  // Constructors:                                               //
  //                                                             //
  /////////////////////////////////////////////////////////////////

  GAConjProblemForORGroupSolver( const OneRelatorGroup& group , const Word& W1 , const Word& W2 , bool createFile = true , bool cp = true );

  ~GAConjProblemForORGroupSolver( );

  GAConjProblemForORGroupSolver operator = ( const GAConjProblemForORGroupSolver& solver ) ;
  // hidden

 protected:

  GAConjProblemForORGroupSolver( const OneRelatorGroup& group , const Word& W1 , const Word& W2 , File* f );
  // this constructor used for conjectures only

 public:

  

  //////////////////////////////////////////////////////////////////
  //                                                              //
  // Accessors:                                                   //
  //                                                              //
  //////////////////////////////////////////////////////////////////

  bool isConj( );

  int getNumberOfIterations( ) const { return theIter1; }
  // get total number of iterations done by Solver::isConj and internal conjectures

  Chars getFileName( ) const { 
    if( !file )
      error( "GAConjProblemForORGroupSolver::getFileName" );
    return file->getFileName(); 
  }


  //////////////////////////////////////////////////////////////////
  //                                                              //
  //  Internal functions                                          //
  //                                                              //
  //////////////////////////////////////////////////////////////////

 public:

  static int rnd1( int max );  
  static int roulette( double d1 , double d2 );
  static int roulette( int num , double* d);
  
  static Word greedyReduce( const OneRelatorGroup& group , const Word& word );
  static bool oneGreedyReduce( const OneRelatorGroup& group , Word& w );
  static void insert( Word& dst , const Word& src , int pos);

 protected:


  void checkImprovementTime( );

  int reproduction( );
  bool tournament( GACPforORGSolverGene& gene );
  int selectGene( ) const;


  // I invoke this function when get more shorter words, in the constructor 
  // and if there is so long time since last improvement
  // It resets pair of words
  void toStart( const Word& W1 , const Word& W2 );

 protected:

  //////////////////////////////////////////////////////////////////
  //                                                              //
  //  Data members                                                //
  //                                                              //
  //////////////////////////////////////////////////////////////////

  static double prob[2][3];
  //two sets of probability of mutation, permutation and crossover

  bool conjProblem;
  // it it sets then it solves conjugacy problem
  // else solves word problem

  int lastImprovement;
  //
 
  int fitnessRate;

  int theIter1, theIter2;
  // number of iterations

  double bestFit;
  // best fitness value have ever obtained 

  File* file;
  bool deleteFile;
  // session file

  //////////////////////////////////////////////////////////////////
  //                                                              //
  //  Checked words                                               //
  //                                                              //
  //////////////////////////////////////////////////////////////////

  AssociationsOf< Word , int > checkedWords;


  //////////////////////////////////////////////////////////////////
  //                                                              //
  //  Genes                                                       //
  //                                                              //
  //////////////////////////////////////////////////////////////////

  int numGenes;
  GACPforORGSolverGene** genes;
  GACPforORGSolverGene *newGene[2];


  //////////////////////////////////////////////////////////////////
  //                                                              //
  //  Algebraic objects                                           //
  //                                                              //
  //////////////////////////////////////////////////////////////////

  OneRelatorGroup theGroup;
  Word theWord1;
  Word theWord2;
  
  static const int NOCONJ;
};


//---------------------------------------------------------------------------//
//----------------- GAConjProblemForORGroupConjecture -----------------------//
//---------------------------------------------------------------------------//


class GAConjProblemForORGroupConjecture : private GAConjProblemForORGroupSolver
{
 private:
  GAConjProblemForORGroupConjecture( const OneRelatorGroup& group , const Word& W , File* f ) : 
    GAConjProblemForORGroupSolver( group , Word( ) , W , f ) { }

  bool isConj( int maxIter , int& doneIter );

  friend class GAConjProblemForORGroupSolver;
};


#endif
