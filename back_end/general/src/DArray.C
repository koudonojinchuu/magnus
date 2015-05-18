// Copyright (C) 1995 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Implementations of classes DArray. 
//
// Principal Author: Alexey Myasnikov
//
// Status: Useable
//
// Usage:
//
// Discussion:
//
// Revision History:
//
// Next Implementation Steps:
//


#include "DArray.h"
#include "Integer.h"
#include "Rational.h"
#include "Int2.h"

//------------------------------------------------------------------------//
//-------------------------------- DArrayRep -----------------------------//
//------------------------------------------------------------------------//


template <class R>
DArrayRep<R>::DArrayRep(const DArrayRep& DA)
{
      width = DA.width;
      height = DA.height;
      rowBegin = 0;
      colBegin = 0;
      //@njz:
      //      theArray = new MatrixRow<R>[height](width);
      //      for( int i = 0; i < height; i++ ) {
      //	theArray[i] = DA.theArray[DA.rowBegin+i];
      //      }
      //
      theArray = new MatrixRow<R>[height];
      for( int i = 0; i < height; i++ ) {
	theArray[i] << DA.theArray[DA.rowBegin+i];
      }
      //
}

template <class R>
VectorOf<R> DArrayRep<R>::getRow(int i) const
{
    #if SAFETY > 0
       if(!inBounds(i))
          error("DArray incorrect number of row in DArrayRep::getRow(int)");
    #endif
    return theArray[rowBegin+i];
}

template <class R>
VectorOf<R> DArrayRep<R>::getCol(int i) const
{
     #if SAFETY > 0
       if(i<0 || i>= width)
          error("DArray incorrect number of col in DArrayRep::getCol(int)");
     #endif
     VectorOf<R> vec(height);

     for (int j=rowBegin;j<rowBegin+height;j++)
          vec[j-rowBegin] = theArray[j].row[colBegin+i];
     return vec;
}

template <class R>
   void DArrayRep<R>::assignCol(int i,const VectorOf<R>& vc){
     #if SAFETY > 0
      if (i<0 || i>=width)
          error("DArray incorrect number of col in DArray::assignCol(int)");
      if (height != vc.length())
          error("DArray length of col is not equal to length of Vector in"
                       " DArray::assignCol(int)");
     #endif     
     for (int j=rowBegin;j<rowBegin+height;j++)
          theArray[j].row[colBegin+i] = vc[j-rowBegin];
   }


// Methods, which changes the DArray size
template <class R>
  void DArrayRep<R>::reSize(int newHeight,int newWidth)
{
  #if SAFETY > 0
     if (newHeight<0 || newWidth < 0)
          error("DArray incorrect parameters in  DArray::reSize(int,int ");
  #endif
  if( (newWidth == 0)||(newHeight == 0) ) {
      deleteArray();
      width = newWidth;
      height = newHeight;
      rowBegin = 0;
      colBegin = 0;
      return;
  }
  MatrixRow<R>* tmpArray = theArray;

  //@njz:
  //  theArray = new MatrixRow<R>[newHeight](newWidth);
  theArray = new MatrixRow<R>[newHeight];
  MatrixRow<R> tmpRow(newWidth);
  for( int i = 0; i < newHeight; i++ ) {
    theArray[i] << tmpRow;
  }
  //

  int wRange = ( (width<newWidth) ? width : newWidth );
  int hRange = ( (height<newHeight) ? height : newHeight );
  for (int i=0;i<hRange;i++)
       for (int j=0;j<wRange;j++)
            theArray[i].row[j] = tmpArray[i].row[j];
  width = newWidth;
  height = newHeight;
  rowBegin = 0;
  colBegin = 0;
  delete [] tmpArray;
}

template <class R>
void DArrayRep<R>::colInsBefore(int col,int colsNum){
    #if SAFETY > 0
      if (col<0 || col>=width)
          error("DArray incorrect number of col in DArray::colInsBefore(int,int ");
    #endif   
    R* tmpRow;
    width +=colsNum;
    for (int index=rowBegin;index<rowBegin+height;index++){
      tmpRow = theArray[index].row;
      theArray[index].row = new R[width+colBegin];
      theArray[index].width = width;
      theArray[index].rowBegin = colBegin;
      for(int j=width-1+colBegin;j>=colBegin;j--){
	if (j >=col+colsNum+colBegin) {theArray[index].row[j] = tmpRow[j-colsNum];}
	if (j<col+colBegin) {theArray[index].row[j] = tmpRow[j];}
      }
      delete [] tmpRow;
    }
}

template <class R>
void DArrayRep<R>::colInsAfter(int col,int colsNum){
    #if SAFETY > 0
      if (col<0 || col>=width)
          error("DArray incorrect number of col in DArray::colInsAfter(int,int)");
    #endif   
    R* tmpRow;
    width +=colsNum;
    for (int index=rowBegin;index<rowBegin+height;index++){
      tmpRow = theArray[index].row;
      theArray[index].row = new R[width+colBegin];
      theArray[index].width = width;
      theArray[index].rowBegin = colBegin;
 
      for(int j=colBegin;j<colBegin+width;j++){
	if (j<=colBegin+col) {theArray[index].row[j] = tmpRow[j];}
	if (j >colBegin+col+colsNum) {theArray[index].row[j] = tmpRow[j-colsNum];}
      }
      delete [] tmpRow;
    }
}
template <class R>
void DArrayRep<R>::rowInsBefore(int row,int rowsNum){
    #if SAFETY > 0
      if (row<0 || row>=height)
          error("DArray incorrect number of row in DArray::rowInsBefore(int,int)");
    #endif   
    MatrixRow<R>* tmpArray = theArray;
    height +=rowsNum;
    //@njz:
    //    theArray = new MatrixRow<R>[height+rowBegin](width+colBegin);   
    theArray = new MatrixRow<R>[height+rowBegin];   
    MatrixRow<R> tmpRow(width+colBegin);
    for( int i = 0; i < height+rowBegin; i++ ) {
      theArray[i] << tmpRow;
    }
    //


    for (int j=height-1+rowBegin;j>=rowBegin;j--){
	if (j<row+rowBegin) {theArray[j] = tmpArray[j];}
	if (j >=row+rowsNum+rowBegin) {theArray[j] = tmpArray[j-rowsNum];}
    }
    delete [] tmpArray;
}

template <class R>
void DArrayRep<R>::rowInsAfter(int row,int rowsNum){
    #if SAFETY > 0
      if (row<0 || row>=height)
          error("DArray incorrect number of col in DArray::rowInsAfter(int,int)");
    #endif   
    MatrixRow<R>* tmpArray = theArray;
    height +=rowsNum;
    //@njz:
    //    theArray = new MatrixRow<R>[height+rowBegin](width+colBegin);
    theArray = new MatrixRow<R>[height+rowBegin]();
    MatrixRow<R> tmpRow(width+colBegin);
    for( int i = 0; i < height+rowBegin; i++ ) {
      theArray[i] << tmpRow;
    }
    //

      for(int j=rowBegin;j<rowBegin+height;j++){
	if (j<=row+rowBegin) {theArray[j] = tmpArray[j];}
	if (j >row+rowsNum+rowBegin) {theArray[j] = tmpArray[j-rowsNum];}
      }
    delete [] tmpArray;
}

template <class R>
void DArrayRep<R>::colDelBefore(int col,int colsNum, bool reAlloc){
    #if SAFETY > 0
      if (col<0 || col>=width || col-colsNum<0)
          error("DArray incorrect number of col in DArray::colDelBefore(int,int ");
    #endif   
    width -=colsNum;
    if (!reAlloc && col-colsNum == 0){
      colBegin += col;
      for (int i=rowBegin;i<rowBegin+height;i++){
         theArray[i].width = width;
         theArray[i].rowBegin = colBegin;
      }
      return;   
    }
    R* tmpRow;
    for (int index=rowBegin;index<rowBegin+height;index++){
      tmpRow = theArray[index].row;
      if (reAlloc)
           theArray[index].row = new R[width+colBegin];
      theArray[index].width = width;
      for(int j=colBegin;j<colBegin+width;j++){
	if (j <col-colsNum+colBegin) {theArray[index].row[j] = tmpRow[j];}
	if (j>=col-colsNum+colBegin) {theArray[index].row[j] = tmpRow[j+colsNum];}
      }
     if (reAlloc) delete [] tmpRow;
    }
}

template <class R>
void DArrayRep<R>::colDelAfter(int col,int colsNum,bool reAlloc){
    #if SAFETY > 0
      if (col<0 || col>=width || col+colsNum>=width)
          error("DArray incorrect number of col in DArray::colDelAfter(int,int)");
    #endif   
    width -=colsNum;
    if (!reAlloc && col+colsNum==width-1){
      for (int i=rowBegin;i<rowBegin+height;i++){
         theArray[i].width = width;
      }
      return;
    }
    R* tmpRow;
    for (int index=rowBegin;index<rowBegin+height;index++){
      tmpRow = theArray[index].row;
      if (reAlloc) theArray[index].row = new R[colBegin+width];
      theArray[index].width = width;
 
      for(int j=colBegin;j<colBegin+width;j++){
	if (j<=colBegin+col) {theArray[index].row[j] = tmpRow[j];}
	if (j >colBegin+col) {theArray[index].row[j] = tmpRow[j+colsNum];}
      }
      if (reAlloc) delete [] tmpRow;
    }
}
template <class R>
void DArrayRep<R>::colDelRange(int col1,int col2,bool reAlloc){
    #if SAFETY > 0
      if (col1 > col2 || col1 <0 || col2 >=width)
          error("DArray incorrect parameters in DArray::colDelRange(int,int)");
    #endif   
    width -=(col2-col1+1);
    if (!reAlloc && col1==0){
      colBegin+=col2+1;
      for (int i=rowBegin;i<rowBegin+height;i++){
         theArray[i].width = width;
         theArray[i].rowBegin = colBegin;
      }
      return;
    }
    if (!reAlloc && col2==width-1){
      for (int i=rowBegin;i<rowBegin+height;i++){
         theArray[i].width = width;
      }
      return;
    }

     R* tmpRow;
    for (int index=rowBegin;index<rowBegin+height;index++){
      tmpRow = theArray[index].row;
      if (reAlloc) theArray[index].row = new R[colBegin+width];
      theArray[index].width = width;
 
      for(int j=colBegin;j<colBegin+width;j++){
	if (j<colBegin+col1) {theArray[index].row[j] = tmpRow[j];}
	if (j >=colBegin+col1) {theArray[index].row[j] = tmpRow[j+(col2-col1+1)];}
      }
      if (reAlloc) delete [] tmpRow;
    }
}

template <class R>
void DArrayRep<R>::rowDelBefore(int row,int rowsNum, bool reAlloc){
    #if SAFETY > 0
      if (row<0 || row>=height || row-rowsNum<0)
          error("DArray incorrect number of col in DArray::rowDelBefore(int,int ");
    #endif   
    height -=rowsNum;
    if (!reAlloc && row-rowsNum == 0){
      rowBegin += row;
      return;   
    }
    MatrixRow<R>* tmpArray = theArray;

    //@njz:
    //    if (reAlloc) theArray = new MatrixRow<R>[height+rowBegin](width+colBegin);
    if (reAlloc) {
      theArray = new MatrixRow<R>[height+rowBegin];
      MatrixRow<R> tmpRow(width+colBegin);
      for( int i = 0; i < height+rowBegin; i++ ) {
	theArray[i] << tmpRow;
      }
    }
    //

    for (int i=rowBegin;i<rowBegin+height;i++){
	if (i <row-rowsNum+rowBegin) {theArray[i] = tmpArray[i];}
	if (i>=row-rowsNum+rowBegin) {theArray[i] = tmpArray[i+rowsNum];}
      }
     if (reAlloc) delete [] tmpArray;
}

template <class R>
void DArrayRep<R>::rowDelAfter(int row,int rowsNum,bool reAlloc){
    #if SAFETY > 0
      if (row<0 || row>=width || row+rowsNum>=height)
          error("DArray incorrect number of col in DArray::rowDelAfter(int,int)");
    #endif   
    height -=rowsNum;
   if (!reAlloc && row+rowsNum==width-1)
                    return;

    MatrixRow<R>* tmpArray = theArray;

    //@njz: a bug is also found: row should be rowBegin,
    //    if (reAlloc) theArray = new MatrixRow<R>[height+row](width+colBegin);
    if (reAlloc) {
      theArray = new MatrixRow<R>[height+rowBegin];
      MatrixRow<R> tmpRow(width+colBegin);
      for( int i = 0; i < height+rowBegin; i++ ) {
	theArray[i] << tmpRow;
      }
    }
    //

    for(int j=rowBegin;j<height+rowBegin;j++){
	if (j<=rowBegin+row) {theArray[j] = tmpArray[j];}
	if (j >rowBegin+row) {theArray[j] = tmpArray[j+rowsNum];}
      }
    if (reAlloc) delete [] tmpArray;
}

template <class R>
void DArrayRep<R>::rowDelRange(int row1,int row2,bool reAlloc){
    
    #if SAFETY > 0
      if (row1>row2 || row1<0 || row2>=height)
          error("DArray incorrect number of row in DArray::rowDelRange(int,int)");
    #endif   
    height -=(row2-row1+1);

    if (!reAlloc && row1==0){
      rowBegin+=row2+1;
        return;
    }

    if (!reAlloc && row2==height-1)
           return;

    MatrixRow<R>* tmpArray = theArray;

    //@njz:
    //    if (reAlloc) theArray = new MatrixRow<R>[height+rowBegin](width+colBegin);
    if (reAlloc) {
      theArray = new MatrixRow<R>[height+rowBegin];
      MatrixRow<R> tmpRow(width+colBegin);
      for( int i = 0; i < height+rowBegin; i++ ) {
	theArray[i] << tmpRow;
      }
    }
    //

    for(int j=rowBegin;j<rowBegin+height;j++){
	if (j<row1+rowBegin) {theArray[j] = tmpArray[j];}
	if (j >=rowBegin+row1) {theArray[j] = tmpArray[j+(row2-row1+1)];}
      }
    if (reAlloc) delete [] tmpArray;
}

template <class R>
bool DArrayRep<R>::operator == ( const DArrayRep& DA ) const
{
  if( (width != DA.width)||(height != DA.height) ) return false;
  for (int i=0;i<height;i++)
     for(int j=0;j<width;j++)
       if(theArray[rowBegin+i].row[colBegin+j] != DA.theArray[DA.rowBegin+i].row[DA.colBegin+j]) return false;
  return true;
}


template <class R>
void DArrayRep<R>::makeArray(int mHeight, int mWidth )
{
  width  = mWidth;
  height = mHeight;
  rowBegin = 0;
  colBegin = 0;
  if( (width == 0)||(height == 0) ) return;

  //@njz:
  //      theArray = new MatrixRow<R>[height](width);
  theArray = new MatrixRow<R>[height];
  MatrixRow<R> tmpRow(width);
  for( int i = 0; i < height; i++ ) {
    theArray[i] << tmpRow;
  }
  //
}


template <class R>
void DArrayRep<R>::deleteArray()
{
  if( (width == 0)||(height == 0) ) return;
  delete [] theArray;
}

//------------------------------------------------------------------------//
//-------------------------------- DArrayParser --------------------------//
//------------------------------------------------------------------------//


template <class R>
VectorOf<R> DArrayParser<R>::parseMatrixRow( Chars& errMesg )
{
  VectorOf<R> result; 

  if ( curToken == INIT ) getToken();

  if ( curToken !=  LSQUARE ) {
    parseError("expected a '[' here");
    errMesg = parseErrorMessage;
    return result;
  }

  while( true ) {

    R elem;
    istr >> elem;

    getToken();
    
    result.append(elem);
    
    if ( curToken ==  RSQUARE ) {
      getToken();
      errMesg = "";
      return result;
    }
    
    if ( curToken !=  COMMA ) {
      parseError("expected a ',' here");
      errMesg = parseErrorMessage;
      return result;
    }
  }
}


template <class R>
void DArrayParser<R>::deleteRows( MatrixCell<R>* rows )
{
 MatrixCell<R>* tmpRows;
 if (!rows) return;
 while (rows){
    tmpRows = rows;
    rows = tmpRows->nextCell;
    delete tmpRows;
 }
}


template <class R>
bool DArrayParser<R>::parseDArray( Chars& errMesg, MatrixRow<R> **M, int& MHeight,int& MWidth )
{
  int numOfRows = 0;
  
  MatrixCell<R>* lastRowOfM; 
  MatrixCell<R>* rowOfM;// It's  a list of MatrixRows
  
  if ( curToken == INIT ) getToken();
  
  if ( curToken !=  LSQUARE ) {
    parseError("expected a '[' here");
    errMesg = parseErrorMessage;
    return false;
  }

  getToken();
  bool bFirstIteration = true;
  
  while( true ) {
    VectorOf<R> row = parseMatrixRow(errMesg);
    if( errMesg.length() > 0 ) {
      if( !bFirstIteration )
	  deleteRows(rowOfM);
      return false;
    }
    
    if( bFirstIteration ) {
      bFirstIteration = false;
      MWidth = row.length();
      rowOfM = new MatrixCell<R>(row);
      lastRowOfM = rowOfM;
    }
    else {
      if( row.length() != MWidth ) {
	parseError("incorrect length of this row");
	errMesg = parseErrorMessage;
        deleteRows(rowOfM);
	return false;
      }
      lastRowOfM->nextCell = new MatrixCell<R>(row);
      lastRowOfM = lastRowOfM->nextCell;
    }

    numOfRows++;

    if( curToken == RSQUARE ){// The parse iz, finished
           int i=0;
           MatrixCell<R>* tmpRows;
           MHeight = numOfRows;
           MWidth = rowOfM->content.width; 
           if  (!rowOfM) return false;

	   //@njz:
	   //           (*M) = new MatrixRow<R>[numOfRows](MWidth); // Create the DArray
	   //           while (rowOfM){
	   //                  (*M)[i] = rowOfM->content;
	   //                  i++;
	   //                  tmpRows = rowOfM;
	   //                  rowOfM = tmpRows->nextCell;
	   //                  delete tmpRows;
	   //            }
           (*M) = new MatrixRow<R>[numOfRows]; // Create the DArray
           while (rowOfM){
	     
	     (*M)[i] << rowOfM->content;
	     i++;
	     tmpRows = rowOfM;
	     rowOfM = tmpRows->nextCell;
	     delete tmpRows;
           }
	   //

           delete lastRowOfM;
           return true;
    }
    else
      if( curToken == COMMA )
	getToken();
      else {
	parseError("expected ',' or ']' here");
	errMesg = parseErrorMessage;
        deleteRows(rowOfM);
	return false;
      }
  }
}



template class DArrayParser<Integer>;
template class DArrayParser<Rational>;
template class DArrayRep<Rational>;
template class DArrayRep<Integer>;
template class DArrayParser<double>;
template class DArrayRep<double>;

#include "../../Polynomial/include/Polynomial.h"
template class DArrayParser< Polynomial<Integer> >;
template class DArrayRep< Polynomial<Integer> >;
template class DArrayParser< Polynomial<Int2> >;
template class DArrayRep< Polynomial<Int2> >;
