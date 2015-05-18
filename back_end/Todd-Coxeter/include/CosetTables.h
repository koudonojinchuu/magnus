// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of class CosetTable, CosetRow.
//
// Principal Author: Alexey Myasnikov
//
// Status: Useable
//
// Usage:
//
// Revision History:
//
//


#ifndef _COSET_TABLES_H_
#define _COSET_TABLES_H_

#include "Word.h"
#include "CosetRelations.h"
#include "EasyList.h"
#include "Integer.h"

// Internal class to keep rows of Coset Tables.
class CosetRow {
public:
  CosetRow(int length):
    row(new int[length]),
    left(0),
    right(length)
  { 
    if (row == NULL)
      error ("Memory exhausted in Todd Coxeter algorithm");
     //    for (int i=0;i<length;i++)
     //        row[i] = 0;
  }
  
  ~CosetRow(){ delete[] row;}
  CosetRow(const CosetRow&);
  bool operator == (const CosetRow& cr);
  int getCell (int i)const {return row[i];}
  void setCell(int i,int index) {row[i]= index;}
  CosetRow operator = (const CosetRow& cr);

  int left;
  // Position of last filled cell in a row from the left
  int right;
  // Position of first filled cell in a row from the right
private:
  void printOn(ostream& ostr=cout){
    cout << "left-"<<left<<"; right-"<<right<<endl;
  }
  int* row;
};

// --------------------- CosetTable --------------------//
// Internal class of Todd-Coxeter

class CosetTable {
public:
  CosetTable(const Word& w,CosetRelationsSet& crs,
	     VectorOf<Integer>& coset_nums,
	     bool sgTable = false);
 
  // sgTable true if table represents subgroup generator.
  ~CosetTable();
  //////////////////////////////////////////////////////////////////////// 
  //                                                                   //
  // Methods                                                           //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////

  bool complete() const {return !filedTable.isEmpty() && 
			   currentTable.isEmpty();}
  // True if table is complete, i.e. All rows filled.
  bool addCoset(int coset_number,bool first_used = false);
  // Add a new coset to the table. If first_used filled with
  // coset_number first empty from the left cell. If the table
  // represents group relator, adds new row to the table.
  bool removeCosets(int bad);
  // Raplaces all entries equals to 'bad' with 'good' numbers.
  bool iterateTable();
  // Fills table with coset numbers.
  
  int numberOfRows() const {return activeOrder;}
  // returns current number of rows in the table.
  // It is equal to number of rows in filled and current tables

  //////////////////////////////////////////////////////////////////////// 
  //                                                                   //
  // Operators                                                         //
  //                                                                   //
  ///////////////////////////////////////////////////////////////////////
  
  friend ostream& operator << ( ostream& ostr, const CosetTable& ct )
  {
    ct.printOn(ostr);
    return ostr;
  }
private:

  CosetTable(const CosetTable&);
  void printOn( ostream& ostr=cout)const;  
  bool iterateFromRight();
  // Fills table with coset numbers from the left.
  bool iterateFromLeft();
  // Fills table with coset numbers from the right.

  bool getClosed( );
  // Checks if current row in the current table was filled
  // moves it to the filled table and adds relations if needed
  Integer getCosetInRow(int i,CosetRow& row)const;

  int numOfLetters;
  // Length of 'relation'
  Word relation;
  // Word for which the table was bilt
  EasyList<CosetRow*> currentTable;
  // Table with rows,which was  not filled yet.
  EasyList<CosetRow*> filedTable;
  // Table with  filled rows.
  EasyList<CosetRow*> emptyTable;
  // Table with rows freed after conflicts.
  // I keep them for avoid extra operations with memory
  CosetRelationsSet& theRelationSet;
  // Reference to the set of relations. It suppose to be
  // common for all tables. 
  bool relatorsChanged;
  // true if relation was added to the relation set,
  // during last iteration
  int activeOrder;
  // Keeps current number of rows
  bool isSubgroupTable;
  // true if 'relation' is a subgroup generator
  VectorOf<Integer>& cosetNumbers;
};
#endif











