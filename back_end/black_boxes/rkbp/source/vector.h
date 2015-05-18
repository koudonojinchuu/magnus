// vector.h

/////////////////////////////////////////////////////////////////
//
//                      vector_char
//
/////////////////////////////////////////////////////////////////

class vector_char {
 protected:
  long        max_length;
  long        length;
  char huge  *data;

 public:
  vector_char(
    long  mxl = 100
  );

 ~vector_char() { if (data != NULL) fxfree(data); data = NULL; };

  long  get_length(void) { return length; }
  void  set_max_length( long mxl );

  void  set_length( long lng)
    { set_max_length(lng); length = lng; }

  void  set_data( char *str);
  char  operator[]( long i ) { return data[i]; }
  char  safe_get_entry( long i);
  void  set_entry( long i, char x) { data[i] = x; }
  void  safe_set_entry( long i, char x);
  void  get_data(char *dest) { strcpy(dest,(char *)data); }
  char  last() { return data[length-1]; }
  void  terminator();
  void  append( vector_char &vec );

  _boolean_t compare_data(char *str)
    { if (0 == strcmp(str,(char*)data) ) return TRUE;
      else return FALSE;}
      
  void open(fstream &stream) { stream.open(data,ios::out); }

  friend ostream &operator<<(ostream &stream, vector_char &vec);
  friend istream &operator>>(istream &stream, vector_char &vec);
};



// Other related functions.

istream &operator>>(istream &stream, vector_char &vec);




/////////////////////////////////////////////////////////////////
//
//                      vector_long
//
/////////////////////////////////////////////////////////////////

class vector_long {
 protected:
  long        dimension;
  long        max_length;
  long        length;
  long huge  *data;

 public:
  vector_long(
    long  mxl = 100     // 100 is arbitrary!
  );

  ~vector_long()
     {if (data != NULL) fxfree((void huge*)data); data = NULL; }

  long  get_length(void) { return length; }
  long  get_max_length(void) { return max_length; }
  long huge *get_data() { return data; }
  void  set_max_length( long mxl );
  void  set_data( long len, long *list);
  _boolean_t nonnull() { if (data != NULL) return TRUE;
                      else return FALSE; }

  void set_null() { data = NULL; }

  void  set_length( long lng );

  void  set_copy( vector_long &vec)
    { set_data(vec.length,vec.data); }

  void append( long  lng, long *list );

  void append( vector_long &vec )
    { append(vec.length,vec.data); }

  long  operator[]( long i ) { return data[i]; }
  long  safe_get_entry( long i);
  void  set_entry( long i, long x) { data[i] = x; }
  void  safe_set_entry( long i, long x);
  void  truncate(long i);
  void  clear();
  long  sum();
  void  print();
};



/////////////////////////////////////////////////////////////////
//
//                     vector_long_segment
//
/////////////////////////////////////////////////////////////////

class vector_long_segment : public vector_long {
 public:
  vector_long_segment() : vector_long(0) {;}
 ~vector_long_segment() { data = NULL; }

  void segment( vector_long &vec, long offset, long leng );
};



/////////////////////////////////////////////////////////////////
//
//                      matrix_long
//
/////////////////////////////////////////////////////////////////

class matrix_long : public vector_long {
 protected:
  long   max_rows;
  long   num_rows;
  long   num_cols;

 public:
  matrix_long(
    long        mxr = 0,
    long        ncl = 0
  );

  long  get_num_rows() { return num_rows; }
  long  get_num_cols() { return num_cols; }

  void  set_max_rows( long mxr )
    { set_max_length(mxr*num_cols ); max_rows = mxr; }
  
  void  set_num_rows( long nrw ) { num_rows = nrw; }

  long  get_entry( long i, long j )
      { return data[i*num_cols+j]; }

  long  safe_get_entry( long i, long j);

  void  set_entry( long i, long j, long x)
      { vector_long::set_entry(i*num_cols+j,x); }

  void  safe_set_entry( long i, long j, long x);
  void  print();
};




// Other functions.

ostream &operator<<(ostream &stream, vector_long &vec);
