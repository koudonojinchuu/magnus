// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contentsent: Implementations of classes CosetEnumerator. 
//
// Principal Author:  Alexei Myasnikov
//
// Status: In development
//
// Usage:
//
// Discussion:
//
// Revision History:
//
// Next Implementation Steps:
//

#include "CosetEnumerator.h"
#include "AbelianGroup.h"
#include "AbelianInfinitenessProblem.h"
#include "conversions.h"

PermutationRepresentation::PermutationRepresentation(const PermutationRepresentation& p)
{
    numberOfGens = p.numberOfGens;
    representatives = p.representatives;
    permTable = new ListOf< VectorOf<int> >[numberOfGens];
    for (int i=0;i<numberOfGens;i++)
      permTable[i] = p.permTable[i];
}
  
PermutationRepresentation& PermutationRepresentation::operator = ( const PermutationRepresentation& p)
{
    if (permTable)
      delete [] permTable;
    numberOfGens = p.numberOfGens;
    representatives = p.representatives;
    permTable = new ListOf< VectorOf<int> >[numberOfGens];
    for (int i=0;i<numberOfGens;i++)
      permTable[i] = p.permTable[i];
}
  
void PermutationRepresentation::printOn(const VectorOf<Chars>& n, ostream& ostr)const
{
  if (n.length() != numberOfGens)
    error("void PermutationRepresentation::printOn(const VectorOf<Chars>& n, ostream& ostr)const : wrong Names.");
  bool isIdentity;
  for (int i=0;i<numberOfGens;i++){
    ostr << n[i]<< "=";
    isIdentity = true;
    ListIterator<ListOf<VectorOf<int> > > I( permTable[i]);
    for (I.reset();!I.done();I.next()){
      const VectorOf<int> v = I.value();
      ostr << "(";
      for (int j=0;j<v.length();j++ )
	ostr << v[j] << " ";
      ostr << ")";
      isIdentity = false;
    }
    if (isIdentity)
      ostr << "1";
    ostr << endl << endl;    
  }
}

void PermutationRepresentation::addCycle(int g,const VectorOf<int>& v)
{
  if (g <0 || g >= numberOfGens)
    error("void addCycle(int g,const VectorOf<int>& v):" 
	  " wrong parameter");
  permTable[g].append(v);
}

Word PermutationRepresentation::representativeOf(const Word& w)const
{
  int wLen = w.length();
  int searchCoset = 1;
  for ( int i=0; i<wLen; i++ ){
    searchCoset = searchNext(w[i],searchCoset);
  }
  return representatives[searchCoset-1];
}
int PermutationRepresentation::searchNext(const Generator& g,int coset) const
{
  if (abs(g)-1 < 0 || abs(g)-1 >= numberOfGens)
    error("void searchNext(Generator ,int coset):" 
	  " wrong parameter");

  bool negative = false;
  if (ord(g) < 0)
    negative = true;
  const ListOf<VectorOf<int> >& l = permTable[abs(g)-1];
  ListIterator< ListOf<VectorOf<int> > > I(l);
  
  int returnCoset = coset;

  for (I.reset();!I.done();I.next()){
    VectorOf<int> v = I.value();
    int cIndex = v.indexOf( coset );
    if (cIndex >= 0){
      if (negative){

	if (cIndex == 0)
	  returnCoset = v[v.length()-1];
	else
	  returnCoset = v[cIndex-1];
	break;

      } else {

	if (cIndex == v.length()-1)
	  returnCoset = v[0];
	else
	  returnCoset = v[cIndex+1];
	break;

      }
    } // if (cIndex >= 0)
  }// while
  return returnCoset;
}

CosetEnumerator::CosetEnumerator(const FPGroup& group)
  :  theGroup(group),
     theOrder(0),
     theRelationSet(group.numberOfGenerators()),
     cosetNumbers(100),
     haveFastSolution(false),
     numberOfTables(0),
     thePermutationRepresentation(group.numberOfGenerators())
{
  // If infinite group, then enumerationis usles,
  // and order is infinite. This check is fast  
  if (infinite())   {
    haveFastSolution = true;
    return;
  }
  VectorOf<Word> tmpV(theGroup.getRelators().cardinality());
  numberOfGroupRelators = 0;
  SetIterator<Word> I(theGroup.getRelators());

  // check the relator for empty words
  for (I.reset();!I.done();I.next()){
    if ((tmpV[numberOfGroupRelators] = I.value().freelyReduce()).length())
      numberOfGroupRelators++;
  }
  // if group is finite and norelatorsit suppose to be trivial
  if (numberOfGroupRelators == 0){
    theOrder = 1;
    haveFastSolution = true;
    return;    
  }
  cosetNumbers[0] = 1;
  // Build tables
  //@njz
  //  tables = new (CosetTable*)[numberOfGroupRelators];
  tables = new CosetTable*[numberOfGroupRelators];
  //
  for (int i=0;i<numberOfGroupRelators;i++)
    tables[i] = new CosetTable(tmpV[i],theRelationSet,cosetNumbers);
  numberOfTables = numberOfGroupRelators;
}

CosetEnumerator::CosetEnumerator(const FPGroup& group,
				 const VectorOf<Word>& subgroup)
  : theGroup(group),
    theRelationSet(group.numberOfGenerators()),
    theOrder(0),
    haveFastSolution(false),
    numberOfTables(0),
    cosetNumbers(100),
    theSubgroup(subgroup),
    thePermutationRepresentation(group.numberOfGenerators())
{
  // If group is trivial, than subgroup is trivial???
  bool in = infinite();
  
  if (in){
    haveFastSolution = true;
    return;    
  }
  
  if (!in && theGroup.getRelators().cardinality() == 0)   {
    haveFastSolution = true;
    theOrder = 1;
    return;
  }
  
  numberOfGroupRelators = 0;
  // Make tables of group relators.
  SetIterator<Word> I(theGroup.getRelators());
  VectorOf<Word> tmpV(theGroup.getRelators().cardinality());
  for (I.reset();!I.done();I.next()){
    if ((tmpV[numberOfGroupRelators] = I.value().freelyReduce()).length())
      numberOfGroupRelators++;
  }

  numberOfTables = numberOfGroupRelators;
  VectorOf<Word> tmpSGV(theSubgroup.length());
  for (int i=0;i<theSubgroup.length();i++){
    if ((tmpSGV[numberOfTables-numberOfGroupRelators] = 
	 theSubgroup[i].freelyReduce()).length())
      numberOfTables++;
  }
  // If empty subgroup
  if (numberOfTables == numberOfGroupRelators ){
    haveFastSolution = true;
    theOrder = 1;
    return;
  }
  cosetNumbers[0] = 1;
  // Make tables of subgroup generators.
  //@njz
  //  tables = new (CosetTable*)[numberOfTables];
  tables = new CosetTable*[numberOfTables];
  //
  for (int i=0;i<numberOfTables;i++){
    if (i < numberOfGroupRelators )
      tables[i] = new CosetTable(tmpV[i],theRelationSet,cosetNumbers);
    else
      tables[i] = new CosetTable(tmpSGV[i-numberOfGroupRelators],
				 theRelationSet,cosetNumbers,true);
  }
}

CosetEnumerator::~CosetEnumerator()
{
  if (numberOfTables > 0){
    for (int i=0;i<numberOfTables;i++ )
      delete tables[i];
    delete [] tables;
  }
}

bool CosetEnumerator::addCoset(int coset_num)
{
  // Add a new coset to all tables
  int indexOfNotComplete = 0;
  for (int i=0;i<numberOfTables && !indexOfNotComplete;i++ ){
    // start to fill first not complete table 
    if (!tables[i]->complete()){
      tables[i]->addCoset(coset_num,true);
      indexOfNotComplete = i+1;
    }
  }
  if (!indexOfNotComplete) return false;
  for (int i=0;i<numberOfTables;i++ )
    if (i!=indexOfNotComplete-1)
      tables[i]->addCoset(coset_num);
  return true;
}
void  CosetEnumerator::iterateTables( )
{
  bool needsContinue = true;
  // iterate tables, untill new coset is needed
  while (needsContinue){
    needsContinue = false;
    for (int i=0;i<numberOfTables;i++ )
      if ( tables[i]->iterateTable())
	needsContinue = true;
  }
}
void  CosetEnumerator::removeCollisions( )
{    
  while (theRelationSet.numberOfCollisions()){
    int good,bad;
    
    if(theRelationSet.collision(good,bad)){
      
      // remove collisions from tables
      for (int i=0;i<numberOfTables;i++)
	tables[i]->removeCosets(bad);
      
      for(int i=0;i<cosetNumbers.length();i++)
	if (cosetNumbers[i]==bad)
	  cosetNumbers[i]=good;

      // replace bad cosets in relations
      theRelationSet.replaceCosets(good,bad);
      theRelationSet.removeCollision(bad);    }
    else
      error ("Empty collisions set");
  }
}

int  CosetEnumerator::enumerate( )
{
  if (haveFastSolution) return theOrder;
  cosetNumbers[1] = 2;
  int i = 2;
  while(addCoset(i)){
    iterateTables();
    removeCollisions();
    i++;
    if (i>cosetNumbers.length()){
      VectorOf<Integer> tmpV(cosetNumbers.length()+100,
			     cosetNumbers);
      cosetNumbers = tmpV;
    }
    cosetNumbers[i-1] = i;
  }
  // It's just to be sure,that all was right
  for (int k=0;k<numberOfGroupRelators-1;k++)
    if (tables[k]->numberOfRows()!=tables[k+1]->numberOfRows())
      error("Orders in tables are different!");
  theOrder = tables[0]->numberOfRows();
  makeRepresentatives();
  makePermutations();
  return theOrder;
}

void CosetEnumerator::makeRepresentatives(){
  if (theOrder == 0)
    error("void makeRepresentatives() : cosets have to be enumerated first.");
  
  int numOfGens = theGroup.numberOfGenerators();

  // i'th representative == cosets[i]'th repres. * generators[i]
  // first representative is always 1, so if theOrder == 1, we have only 1
  // in the list of representatives
  VectorOf<int> cosets(theOrder-1);
  for (int i=0;i<theOrder-1;i++)
    cosets[i]=0;
  VectorOf<Generator> generators(theOrder-1);

  cosets2repres.insert(1,0);

  int nonExists = 0;
  for (int j=2;j<=theOrder+nonExists;j++){
    bool cosetFound = false;
    for (int i=0;i<numOfGens*2;i++){
      Generator g = theRelationSet.genOfRels( i);
      int rel = theRelationSet.getRelationNumber(j,g);
      if (rel != 0 && (rel < cosets[j-2-nonExists] || cosets[j-2-nonExists] ==0)){
	cosets [j-2-nonExists] = rel;
	generators[j-2-nonExists] = inv(g);
	cosetFound = true;
      }	
    }
    if (cosetFound)
      cosets2repres.insert(j,j-1-nonExists);
    else
      nonExists++;
  }
  schreierRepres = VectorOf<Word>(theOrder);
  schreierRepres[0]=Word();
  
  for (int i=1;i<theOrder;i++){
    int* rep ;
    if ((rep = cosets2repres.search(cosets[i-1]))!=NULL)
      schreierRepres[i] = schreierRepres[*rep]*generators[i-1];
  }
}

void CosetEnumerator::makePermutations() {
  if (theOrder == 0)
    error("void makePermutations() : cosets have to be enumerated first.");
  
  int numOfGens = theGroup.numberOfGenerators();

  thePermutationRepresentation.setRepresentatives(schreierRepresentatives());

  for (int i=0;i<numOfGens;i++){
    BTreeIterator<int,int> I(theRelationSet.relationsOf(Generator(i+1)));
    BTree<int,int> tmpTree;
    for (I.reset();!I.done();I.next())
      tmpTree.insert(I.getKey(),I.getValue());
    
    for (int j=1;j<=theOrder;j++){
      int currentPoint = j;

      VectorOf<int> v;
      while (true){
	int* next = tmpTree.search(currentPoint);
	if (next == NULL)
	  break;
	int n = *next;
	tmpTree.remove(currentPoint);
	if (n == currentPoint){
	  break;
	}
	int* repres = cosets2repres.search(currentPoint);
	if (repres == NULL)
	  error("void CosetEnumerator::makePermutations: " 
		"could not find a representative");

	v.append(*repres+1);
	tmpTree.remove(currentPoint);
	currentPoint = n;
      }

      if (v.length()>0)
	thePermutationRepresentation.addCycle(i,v);	
    }
    cout << endl;
  }
  return;
}

bool CosetEnumerator::infinite()const
{
  if (theSubgroup.length()==0){

    AbelianInfinitenessProblem theProb( theGroup );
    theProb.startComputation();
    while ( !theProb.continueComputation() ) { }

    return theProb.isInfinite();

  } else {

    FPGroup G( theGroup );
    SetOf<Word> S;
    int len = theSubgroup.length();
    for( int i = 0; i < len; ++i )
      S |= theSubgroup[i];
    G.addRelators(S);
    
    AbelianGroup ag( G );
    ag.computeCyclicDecomposition();
    
    return ag.isInfinite();   

  }
}























