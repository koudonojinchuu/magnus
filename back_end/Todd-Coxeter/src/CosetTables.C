// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contentsent: Implementations of classes CosetTables. 
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

#include "CosetTables.h"



CosetTable::CosetTable(const Word& w,CosetRelationsSet& crs,
		       VectorOf<Integer>& coset_nums,
		       bool sgTable):
  relation(w),
  numOfLetters(w.length()),
  theRelationSet(crs),
  activeOrder(0),
  relatorsChanged(false),  
  isSubgroupTable(sgTable),
  cosetNumbers(coset_nums)
{
  addCoset(1);
}

CosetTable::~CosetTable(){

}

void CosetTable::printOn( ostream& ostr) const 
{
  ostr << endl << "Number of rows :" << activeOrder << endl << "  ";
  for (int i=0;i<relation.length();i++)
    ostr << relation[i] << "   ";
  ostr << endl;
  CosetTable* tmpT = (CosetTable* )this;
  if (!tmpT->currentTable.isEmpty()){
    EasyCell<CosetRow*>* savPosition = tmpT->currentTable.getCurrent();
    for (tmpT->currentTable.setToFirst();!tmpT->currentTable.gotEnd();
	 tmpT->currentTable.next())  {
      CosetRow* tmpRow = tmpT->currentTable.getCurrent()->content();
      for (int i=0;i<numOfLetters;i++){
	if (i > tmpRow->left && i <tmpRow->right)
	  ostr << "0";
	else
	  ostr << getCosetInRow(i,*tmpRow);
	ostr << " | ";
      }
      ostr << getCosetInRow(0,*tmpRow)<<endl;
    }
  tmpT->currentTable.setCurrent(savPosition);
  }
  // cout << "Filled :" << endl;
  if (!tmpT->filedTable.isEmpty()){
    EasyCell<CosetRow*>* savPosition = tmpT->filedTable.getCurrent();
    for (tmpT->filedTable.setToFirst();!tmpT->filedTable.gotEnd();
	 tmpT->filedTable.next())  {
      CosetRow* tmpRow = tmpT->filedTable.getCurrent()->content();
      for (int i=0;i<numOfLetters;i++)
	ostr << getCosetInRow(i,*tmpRow) << " | ";
      ostr << getCosetInRow(0,*tmpRow)<<endl;
    };
    ostr << "relations :" << theRelationSet << endl;
    tmpT->filedTable.setCurrent(savPosition);
  }
}  

bool CosetTable::addCoset(int coset_number,bool first_used)
{
  // Adding a new row in the table. If this table for group relator.
  if (!isSubgroupTable || coset_number == 1){
    activeOrder++;
    if (!emptyTable.isEmpty()){
      CosetRow* tmpRow = emptyTable.getFirst()->content();
      emptyTable.deleteFirst();
      tmpRow->setCell(0,coset_number-1);
      tmpRow->left = 0;
      tmpRow->right = numOfLetters;    
      currentTable.append(tmpRow);
    }
    else{
      CosetRow* tmpRow = new CosetRow(numOfLetters);
      tmpRow->setCell(0,coset_number-1);
      currentTable.append(tmpRow);
    }
  }

  // Adding a new coset number in the first row of the table (if needed).
  if (coset_number > 1 && first_used){
    relatorsChanged = false;
    CosetRow* tmpRow = currentTable.getFirst()->content();
    if (tmpRow->right - tmpRow->left < 2)
      error("Wrong coset table");  
    tmpRow->left++;
    tmpRow->setCell(tmpRow->left,coset_number-1);
    
    // Add new relation
    if (theRelationSet.addRelation(getCosetInRow(tmpRow->left-1,*tmpRow).as_long(),
				   relation[tmpRow->left-1],
				   getCosetInRow(tmpRow->left,*tmpRow).as_long()))
      {
	relatorsChanged = true;
      }
    
  }
  if (currentTable.setToFirst())
    if (getClosed()) return true;   
  return false;
}

bool CosetTable::iterateTable()
{
  bool changed = true;
  bool retChanged = false;
  // repeate iteration, until there is no new relations 
  while (changed){
    changed = false;
    for (currentTable.setToFirst();!currentTable.gotEnd();currentTable.next())  {
      if (iterateFromLeft()){
	changed = true;
      }
      if (iterateFromRight()){
	changed = true;
      }
      if (changed)
	retChanged = true;
	
    }
  }
  return retChanged;
}

bool CosetTable::iterateFromLeft()
{
  CosetRow* row = currentTable.getCurrent()->content();
  while (row->right != row->left ){
    if (getClosed())
      return relatorsChanged;
    
    int newRelation;
    if ((newRelation = 
	 theRelationSet.getRelationNumber(getCosetInRow(row->left,*row).as_long(),
					  relation[row->left])
	 )){
      row->left++;
      row->setCell(row->left,newRelation-1);
    }
    else
      break;
  }
  return false;
}

bool CosetTable::iterateFromRight()
{
  if (currentTable.isEmpty()) return false;
  CosetRow* row = currentTable.getCurrent()->content();
  while (row->right != row->left ){
    if (getClosed())
      return relatorsChanged;
    
    int newRelation;
    if (row->right == numOfLetters)
      newRelation = 
	theRelationSet.getRelationNumber(getCosetInRow(0,*row).as_long(),
					 inv(relation[row->right-1]));
    else
      newRelation = 
	theRelationSet.getRelationNumber(getCosetInRow(row->right,*row).as_long(),
					 inv(relation[row->right-1]));
    if (newRelation){
      row->right--;
      row->setCell(row->right,newRelation-1);
    }
    else
      break;
  }
  return false;
}

bool CosetTable::getClosed()
{
  relatorsChanged = false;
  CosetRow* row = currentTable.getCurrent()->content();
  if (row->left == row->right)
    return true;
  // If all cells in the row were filled, add new relation
  // and move the row to filled table
  if (row->right - row->left == 1){
    int rightPart;
    if (row->right == numOfLetters)
      rightPart  = getCosetInRow(0,*row).as_long();
    else
      rightPart = getCosetInRow(row->right,*row).as_long();
    
    if (theRelationSet.addRelation(getCosetInRow(row->left,*row).as_long(),
				   relation[row->left],
				   rightPart)){
	relatorsChanged = true;
    }
    row->left++;
    currentTable.deleteCurrent();
    filedTable.append(row);
    return true;
  }
  return false;  
}

bool CosetTable::removeCosets(int bad)
{
  // First replace in filled table
  for (filedTable.setToFirst();!filedTable.gotEnd();filedTable.next())  {
    CosetRow* tmpRow = filedTable.getCurrent()->content();
    // Delete row if it of bad coset
    if (getCosetInRow(0,*tmpRow).as_long() == bad){
      filedTable.deleteCurrent();
      activeOrder--;
      emptyTable.append(tmpRow);
    }
  }
  // the same with current table
  for (currentTable.setToFirst();!currentTable.gotEnd();currentTable.next())  {
    CosetRow* tmpRow = currentTable.getCurrent()->content();
    if (getCosetInRow(0,*tmpRow).as_long() == bad){
      currentTable.deleteCurrent();
      activeOrder--;
      emptyTable.append(tmpRow);
    }
  }
}

Integer CosetTable::getCosetInRow(int i,CosetRow& row)const
{
  return cosetNumbers[row.getCell(i)];
}

