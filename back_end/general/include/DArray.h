// Copyright (C) 1996 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.
//
// Contents: Definition of class DArray.
//
// Principal Author:  Alexey Myasnikov
//
// Status: Useable
//
// Usage:
//
// Revision History:
//
//


#ifndef _DARRAY_H_
#define _DARRAY_H_

#include "Vector.h"
#include "IStreamPoll.h"
#include "WordParser.h"

template <class R> class MatrixRep;
template <class R> class MatrixComputations;
template <class R> class MatrixRow;
//@db porting
template <class R> class DArrayParser;
template <class R> class MatrixCell;


//------------------------------------------------------------------------//
//------------------------------- DArrayRep ------------------------------//
//------------------------------------------------------------------------//

template <class R> class DArrayRep : public PureRep {

public:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  DArrayRep( int height, int width): theArray(NULL)
  {
   #if SAFETY > 0
    if( (width < 0)||(height < 0) )
      error("incorrect matrix size in constructor DArray( int )");
  #endif  
    makeArray(height,width);
  }

  DArrayRep( int n ): theArray(NULL) 
  {
  #if SAFETY > 0
    if( n < 0 )
      error("incorrect matrix size in constructor DArray( int )");
  #endif  
    makeArray(n,n);
  }

  DArrayRep( const DArrayRep& );

  ~DArrayRep( ) { deleteArray(); }

  DArrayRep<R>* clone( ) const { return new DArrayRep<R>(*this); }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Operators:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool operator == ( const DArrayRep& DA ) const;
  
  MatrixRow<R>& operator []( int i ) { 
      #if ( SAFETY > 0 )
    if(!inBounds(i)){
          error("DArray index out of bounds DArrayRep::operator [](int i)");
    }
          
      #endif
      return theArray[rowBegin+i];
   }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  inline int getWidth( ) const { return width; }  
    
  inline int getHeight( ) const { return height; }  

  VectorOf<R> getRow(int i) const;

  VectorOf<R> getCol(int i) const;

  void assignCol(int i,const VectorOf<R>& vc);

  // Here functions, which allows to change the size of theArray in different
  // ways
  void reSize(int newHeight,int newWidth);

  void colInsBefore(int col,int colsNum);

  void colInsAfter(int col,int colsNum);

  void rowInsBefore(int col,int colsNum);

  void rowInsAfter(int col,int colsNum);

  void colDelBefore(int col,int colsNum, bool reAlloc);

  void colDelAfter(int col,int colsNum, bool reAlloc);

  void colDelRange(int col1,int col2, bool reAlloc);

  void rowDelBefore(int row,int rowsNum, bool reAlloc);

  void rowDelAfter(int row,int rowsNum, bool reAlloc);

  void rowDelRange(int row1,int row2, bool reAlloc);

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // I/O:                                                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool readFrom( istream& istr, Chars& errMesg ) 
  {
    MatrixRow<R>* matrix;
    int matrixHeight,matrixWidth;
    
    DArrayParser<R> P(istr);
    bool result;
    result = P.parseDArray(errMesg, &matrix, matrixHeight,matrixWidth);
    if (result){
       height = matrixHeight;
       width = matrixWidth;
       rowBegin = 0;
       colBegin = 0;
       theArray = matrix;
    }else {
#if SAFETY > 0
      error("DArray reading from stream aborted in DArrayRep::readFrom =");
#endif
     };
    return result;
  }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
  // I did not want to write width for each MatrixRef in the theArray
  // that why MatrixRef does not have it's own operators < and >

  virtual void write( ostream& ostr ) const{
     ostr < height < width;
     for (int i=rowBegin; i< rowBegin+height;i++)
       for (int j=colBegin;j<colBegin+width;j++)
           ostr < theArray[i].row[j]; 
  };
 
  virtual void read( istream& istr ){
     istr > height > width;
     makeArray(height,width);
     for (int i=0; i< height;i++){
       for (int j=0;j<width;j++)
           istr > theArray[i].row[j]; 
     }
     rowBegin = 0;
     colBegin = 0;
  }
private:

  friend class MatrixRep<R>;
  friend class MatrixComputations<R>;

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Data Members:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  int width, height;
  // Sizes of matrix
   
  int rowBegin, colBegin;
  // The numbers of column and row, where the data begins.
  // It's changed in delay functions when you don't reallocate the memory.


  MatrixRow<R>* theArray;
  // Store the matrix as the array of MatrixRow, see below

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Private functions:                                                  //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////
    void makeArray( int mHeight, int mWidth);
    //inicializes theArray, and sizes    

    void deleteArray( );
    // deletes theArray

    bool inBounds(int i) const {
                if (i<0 || i>=height) {return false;}
                else return true;
    }
    //checks the height bounds, width bounds usualy are checked in MatrixRow

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Debugging stuff:                                                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

#ifdef DEBUG

    //friend int main( );

#endif

};

//------------------------------------------------------------------------//
//------------------------------- DArray  --------------------------------//
//------------------------------------------------------------------------//


//@db 2.91

template <class R> class DArray;

template <class R>
ostream& operator < ( ostream& ostr, const DArray<R>& DA )
{
  DA.look()->write(ostr);
  return ostr;
}

template <class R>
istream& operator > ( istream& istr, DArray<R>& DA)
{
  DA.change()->read(istr);
  return istr;
}

//@db end 


template <class R> class DArray : public ObjectOf< DArrayRep<R> > {

public:


  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Constructors:                                                       //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  DArray( int n = 0 ) : ObjectOf<DArrayRep <R> >( new DArrayRep<R>(n) ) { }
  // Default constructor makes empty square DArray  of size n.

  DArray( int h, int w) : ObjectOf<DArrayRep <R> >( new DArrayRep<R>(h,w) ) { }
  // constructor makes DArray of size h x w.

  // Copy constructor, operator=, and destructor supplied by compiler.

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Operators:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  bool operator == ( const DArray& DA ) const {
      return *this->look() == *(DA.look());
  }

  VectorOf<R> operator []( int i ) const { return this->look()->getRow(i); }

  MatrixRow< R >& operator [] ( int i ) { return this->change()->operator [](i); }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // Accessors:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  VectorOf<R> getRow( int i ) const {
     return this->look()->getRow(i);
  }
  // Returns  i's row

  VectorOf<R> getCol( int i ) const {
     return this->look()->getCol(i);
  }
  // Returns  i's column
 
  void assignCol( int i, const VectorOf<R>& vc ){
     this->change()->assignCol(i,vc);
  }
  // Assigns i's column with vc.

 // Changes height and width. Puts data in up left corner.
  void reSize(int newHeight,int newWidth){
     this->change()->reSize(newHeight,newWidth);
  }

 // Functions below are inserts, corresponding to function names, before(after) 'col'
 // 'colsNum' columns(rows). 
  void colInsBefore(int col,int colsNum){
     this->change()->colInsBefore(col,colsNum);
  }
  void colInsAfter(int col,int colsNum){
     this->change()->colInsAfter(col,colsNum);
  }
  void rowInsBefore(int row,int rowsNum){
     this->change()->rowInsBefore(row,rowsNum);
  }
  void rowInsAfter(int row,int rowsNum){
     this->change()->rowInsAfter(row,rowsNum);
  }

 // These methods deletes number of rows(columns) before, after or between
 // the columns. If reAlloc = true - memory realocates for array, if not
 // the bounds are changed, but fisicaly memory does not realocating.
  void colDelBefore(int col,int colsNum,bool reAlloc = TRUE){
     this->change()->colDelBefore(col,colsNum,reAlloc);
  }
  void colDelAfter(int col,int colsNum, bool reAlloc =TRUE){
     this->change()->colDelAfter(col,colsNum,reAlloc);
  }
  void colDelRange(int col1,int col2, bool reAlloc =TRUE){
     this->change()->colDelRange(col1,col2,reAlloc);
  }
  void rowDelBefore(int row,int rowsNum,bool reAlloc = TRUE){
     this->change()->rowDelBefore(row,rowsNum,reAlloc);
  }
  void rowDelAfter(int row,int rowsNum, bool reAlloc =TRUE){
     this->change()->rowDelAfter(row,rowsNum,reAlloc);
  }
  void rowDelRange(int row1,int row2, bool reAlloc =TRUE){
     this->change()->rowDelRange(row1,row2,reAlloc);
  }

  int width( ) const { return this->look()->getWidth(); }  
    
  int height( ) const { return this->look()->getHeight(); }  

  // Index begins from 0, so the last column or row number differs from the 
  // height or width
  int lastCol( ) const { return this->look()->getWidth()-1; }  
    
  int lastRow( ) const { return this->look()->getHeight()-1; }  
  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // I/O:                                                                //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend IStreamPoll operator >> ( istream& istr, DArray& M )
  {
    Chars errMesg;
    
    M.change()->readFrom(istr, errMesg);
    return IStreamPoll( istr, errMesg );
  }
 
  inline friend ostream& operator << ( ostream& o, const DArray& v ) {
 
        if (v.height()==0 || v.width()==0)   {o << "| |"<<endl; return o;}

        for ( int i = 0; i < v.height(); i++ ){
              o << "| ";
              for (int j=0;j<v.width();j++)
                       o << v[i][j] << ", ";
              if (i==v.height()-1) { o << "|."<<endl;}
               else {o <<"|"<< endl;};
        }
        return o;
    }

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  // IPC tools:                                                          //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

  friend ostream& operator < <R>( ostream& ostr, const DArray& DA );
  
  friend istream& operator > <R>( istream& istr, DArray& DA);


  protected :
	DArray( DArrayRep<R>* newrep ) : ObjectOf<DArrayRep <R> >(newrep) { }

};
//-----------------------------------------------------------------------//
//---------------------- MatrixRow --------------------------------------//
//-----------------------------------------------------------------------//
// This is a private class specialy for DArray
// It was made to provide operators to work with rows and to safe acces to elements

template <class R> class MatrixRow {

public:
   
   R& operator [] (int i) {   
    #if SAFETY > 0
         if (i < 0 || i >=width)
             error("DArray index out of bounds in MatrixRow::operator [] (int i)");
    #endif
    return row[rowBegin+i];
   }

   MatrixRow<R>& operator = (const MatrixRow<R>& mr){
     #if SAFETY > 0
         if (width != mr.width)
             error("DArray not equal length of rows in MatrixRow::operator ="
                                    "(const MatrixRow<R>&");
    #endif
    for (int i=0;i<width;i++)
         row[i+rowBegin] = mr.row[i+mr.rowBegin];
    return *this;
   }

   //@njz:
   MatrixRow<R>& operator << (const MatrixRow<R>& mr){
#if SAFETY > 0
     if (width != 0)
       error("Only empty MatrixRow is supposed to be initialized with this << operator method");
#endif
     width = mr.width;
     row = new R[width];
     for (int i=0;i<width;i++)
       row[i+rowBegin] = mr.row[i+mr.rowBegin];
     return *this;
   }
   //


   // Row of DArray is supposed to work with VectorOf, so we have
   // operators = and  operator VectorOf<R>()
   MatrixRow<R>& operator = (const VectorOf<R>& vc){
     #if SAFETY > 0
         if (width != vc.length())
             error("DArray not equal length of vector and row in MatrixRow::operator ="
                                    "(const VectorOf<R>&");
    #endif
    for (int i=rowBegin;i<rowBegin+width;i++)
         row[i] = vc[i-rowBegin];
    return *this;
   }

   operator VectorOf<R>(){
      VectorOf<R> tmpVc(width);
      for (int i=rowBegin;i<rowBegin+width;i++)
           tmpVc[i-rowBegin]=row[i];
      return tmpVc;
   }
  
   int length() const { return width; }
private:

   friend class DArrayParser<R>;
   friend class DArrayRep<R>;
   friend class MatrixCell<R>;

   // Constructors, are hide because of private acces   

   //@njz: added for faciliating creation of lists of MatrixRow
   MatrixRow() : width(0) {
     rowBegin = 0;
   }
   //

   MatrixRow(const int& len) : width(len) {
       if (len==0) return;
       row = new R[len];
       rowBegin = 0;
   }
   MatrixRow(const MatrixRow<R>& mr) {
      width = mr.width;
      row = new R[width];
      rowBegin = 0;
      for (int i=0;i<width;i++)
             row[i] = mr.row[i];
   }
   MatrixRow(const VectorOf<R>& vc) {
      width = vc.length();
      row = new R[width];
      rowBegin = 0;
      for (int i=0;i<width;i++)
             row[i] = vc[i];
   }

  ~MatrixRow() { delete [] row;}

   R* row;

   // Actualy width is needed only because we have to check the equality
   // of length between rows and vectors. rowBegin points to the first
   // row number, if there were deletions without reallocation of memory
   int width, rowBegin;
};

//-------------------------------------------------------------------------------//
//------------------------------- DArrayParser ----------------------------------//
//-------------------------------------------------------------------------------//


// The DArray must have the form like 
// [
//   [ x11, x12, x13 ],
//   [ x21, x22, x23 ],
//   [ x31, x32, x33 ]
// ]
// The elements of DArray must have operator and default constructor

template <class R> class DArrayParser : public WordParser {

public:

  //////////////////////////////////////////////////////////////
  //                                                          //
  // Constructors:                                            //
  //                                                          //
  //////////////////////////////////////////////////////////////

  DArrayParser(istream &istr) : WordParser(istr) { }
  // Initialize the parser with the istream from which to read.

  // Destructor supplied by compiler.

  //////////////////////////////////////////////////////////////
  //                                                          //
  // Parser function:                                         //
  //                                                          //
  //////////////////////////////////////////////////////////////
    
  bool parseDArray( Chars& errMesg, MatrixRow<R> **M, int& MHeight,int& MWidth );

protected:
  VectorOf<R> parseMatrixRow( Chars& errMesg );

private:

  //////////////////////////////////////////////////////////////
  //                                                          //
  // Private functions:                                       //
  //                                                          //
  //////////////////////////////////////////////////////////////

  void deleteRows(  MatrixCell<R>* rows);

  /////////////////////////////////////////////////////////////////////////
  //                                                                     //
  //  Debugging stuff:                                                   //
  //                                                                     //
  /////////////////////////////////////////////////////////////////////////

#ifdef DEBUG

  // friend int main( );

#endif

};
// This class is a presentation of cell in list of MatrixRows.
// It's used in parser.
template <class R> class MatrixCell{
 private:
   friend class DArrayParser<R>;
   MatrixCell(const MatrixRow<R>& mr):content(mr),nextCell(NULL){}
   MatrixRow<R> content;
   MatrixCell* nextCell;
};
#endif










