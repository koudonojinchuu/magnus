// vector.cpp


#include "rkbp.h"




/////////////////////////////////////////////////////////////////
//
//                   vector_char::vector_char
//
/////////////////////////////////////////////////////////////////

vector_char::vector_char(
  long    mxl
)
{
  max_length = mxl;
  length     = 0;

  if (mxl > 0) {
    data = (char *) fxmllc( (ulong) mxl);
    if (data == NULL) fxexit("vector_char::vector_char",1);
  }
}



/////////////////////////////////////////////////////////////////
//
//                vector_char::set_max_length
//
/////////////////////////////////////////////////////////////////

void vector_char::set_max_length(
  long   mxl
)
{
  char   *new_data;

  if (mxl > max_length) {
    if (max_length == 0)
      new_data = (char *) fxmllc( (ulong)mxl );
    else
      new_data = (char *) fxrllc( (void huge *)data, (ulong)mxl);

    if (new_data == NULL)
      fxexit("vector_char::set_max_length",1);

    data = new_data;
    max_length = mxl;
  }
}



/////////////////////////////////////////////////////////////////
//
//                   vector_char::set_data
//
/////////////////////////////////////////////////////////////////

void vector_char::set_data(
  char  *str
)
{
  long  len;

  if (str == NULL) {
    length = 0;
    return;
  }

  len = strlen(str);
  set_max_length(len+1);
  strcpy(data,str);
  length = len;
}



/////////////////////////////////////////////////////////////////
//
//                    vector_char::safe_set_entry
//
/////////////////////////////////////////////////////////////////

void vector_char::safe_set_entry(
  long   i,
  char  x
)
{
  if (i >= max_length) set_max_length(2*i+100);
  if (i >= length) length = i+1;
  ( (char *) data)[i] = x;
}



/////////////////////////////////////////////////////////////////
//
//                   vector_char::safe_get_entry
//
/////////////////////////////////////////////////////////////////

char vector_char::safe_get_entry(
  long  i
)
{
  if ( i < 0 || i >= length )
    error("Index error in vector_char::safe_get_entry");

  return data[i];
}



/////////////////////////////////////////////////////////////////
//
//                  vector_char::terminator
//
/////////////////////////////////////////////////////////////////

void vector_char::terminator(void)
{
  safe_set_entry( length, '\0');
  length--;
}



/////////////////////////////////////////////////////////////////
//
//                   vector_char::append
//
/////////////////////////////////////////////////////////////////

void vector_char::append(
  vector_char  &vec
)
{
  length = length + vec.length;
  set_max_length(length + 1);
  strcpy(data+length, vec.data);
  return;
}



/////////////////////////////////////////////////////////////////
//
//                  operator>> for vector_char
//
/////////////////////////////////////////////////////////////////

istream &operator>>(
  istream      &stream,
  vector_char  &vec
)
{
  long  i;
  char  x;

  vec.set_length(0);

  // Find the first non-whitespace character.

  for (;;) {
    if ( !stream.get(x) ) return stream;
    if ( !isspace(x) ) break;
  }

  i = 0;
  vec.safe_set_entry(i++,x);
  vec.set_length(i);

  // Read until the next whitespace character is found.

  for (;;) {
    if (! stream.get(x) ) return stream;
    if ( isspace(x) ) break;
    vec.safe_set_entry(i++,x);
    vec.set_length(i);
  }

  vec.terminator();

  return stream;
}



/////////////////////////////////////////////////////////////////
//
//                  operator<< for vector_char
//
/////////////////////////////////////////////////////////////////

ostream &operator<<(
  ostream      &stream,
  vector_char  &vec
)
{
  stream << vec.data;
  return stream;
}



/////////////////////////////////////////////////////////////////
//
//                   vector_long::vector_long
//
/////////////////////////////////////////////////////////////////

vector_long::vector_long(
  long         mxl
)
{
  dimension  = mxl;
  max_length = mxl;
  length     = 0;

  if (mxl > 0) {
    data = (long *) fxmllc( (ulong) mxl*sizeof(long) );
    if (data == NULL) fxexit("vector_long::vector_long",1);
  }

  else data = NULL;
}



/////////////////////////////////////////////////////////////////
//
//                   vector_long::set_max_length
//
/////////////////////////////////////////////////////////////////

void vector_long::set_max_length(
  long  mxl
)
{
  long   *new_data;

  if (mxl > dimension) {
    if (dimension == 0)
      new_data = (long *) fxmllc( (ulong)mxl*sizeof(long) );
    else
      new_data = (long *)
          fxrllc( (void huge *) data, (ulong)mxl*sizeof(long) );

    if (new_data == NULL)
      fxexit("vector_long::set_max_length",1);

    data = new_data;
    dimension = mxl;
  }

  max_length = mxl;

  return;
}



/////////////////////////////////////////////////////////////////
//
//                    vector_long::set_length
//
/////////////////////////////////////////////////////////////////

void vector_long::set_length(
  long  lng
)
{
  if (lng > max_length)
    error("Attempt to extend a vector_long passed max_length.");

  length = lng;
}



/////////////////////////////////////////////////////////////////
//
//                    vector_long::append
//
/////////////////////////////////////////////////////////////////

void vector_long::append(
  long   lng,
  long  *list
)
{
  long   i,j,newlen;

  newlen = length +lng;
  if (newlen > max_length) set_max_length(newlen);

  i = 0;
  j = length;
  while (i < lng) data[j++] = list[i++];
  length = newlen;
}



/////////////////////////////////////////////////////////////////
//
//                    vector_long::safe_set_entry
//
/////////////////////////////////////////////////////////////////

void vector_long::safe_set_entry(
  long   i,
  long   x
)
{
  if (i >= max_length) set_max_length(2*i+100);
  if (i >= length) length = i+1;
  ( (long *) data)[i] = x;
}



/////////////////////////////////////////////////////////////////
//
//                   vector_long::safe_get_entry
//
/////////////////////////////////////////////////////////////////

long vector_long::safe_get_entry(
  long  i
)
{
  if (i < 0 || i >= length)
    error("Index error in vector_long::safe_get_entry");

  return data[i];
}



/////////////////////////////////////////////////////////////////
//
//                   vector_long::set_data
//
/////////////////////////////////////////////////////////////////

void vector_long::set_data(
  long   len,
  long  *list
)
{
  long   i;

  set_length(len);
  for (i = 0; i < len; i++) data[i] = list[i];
}



/////////////////////////////////////////////////////////////////
//
//              vector_long_segment::segment
//
/////////////////////////////////////////////////////////////////

void vector_long_segment::segment(
  vector_long  &vec,
  long          offset,
  long          leng
)
{
  data   = vec.get_data() + offset;
  length = max_length = leng;
}



/////////////////////////////////////////////////////////////////
//
//                    vector_long::truncate
//
/////////////////////////////////////////////////////////////////

void vector_long::truncate(
  long  i
)
{
  long  j;

  if (i <= 0) return;

  j = 0;
  while (i < length) data[j++] = data[i++];
  length = j;
}



/////////////////////////////////////////////////////////////////
//
//                   vector_long::clear
//
/////////////////////////////////////////////////////////////////

void vector_long::clear(void)
{
  if (data != NULL) fxfree( (void huge *)data );
  dimension = max_length = length = 0;
}



/////////////////////////////////////////////////////////////////
//
//                   vector_long::sum
//
/////////////////////////////////////////////////////////////////

long vector_long::sum(void)
{
  long    sum,i;
  
  sum = 0;
  for ( i = 0; i < length; i++) sum += data[i];
  
  return sum;
}




/////////////////////////////////////////////////////////////////
//
//                    vector_long::print
//
/////////////////////////////////////////////////////////////////

void vector_long::print()
{
  long  len,i;
  
  len = get_length();
  
  for (i = 0; i < len; i++) cout << " " << data[i];
  cout << endl;
}



/////////////////////////////////////////////////////////////////
//
//                   matrix_long::matrix_long
//
/////////////////////////////////////////////////////////////////

matrix_long::matrix_long(
  long         mxr,
  long         ncl
)
: vector_long(mxr*ncl)
{
  num_rows = 0;
  max_rows = mxr;
  num_cols = ncl;
}



/////////////////////////////////////////////////////////////////
//
//                    matrix_long::safe_set_entry
//
/////////////////////////////////////////////////////////////////

void matrix_long::safe_set_entry(
  long   i,
  long   j,
  long   x
)
{
  if (i >= max_rows) set_max_rows(2*i+100);
  if (i >= num_rows) num_rows = i+1;
  ( (long *) data)[i*num_cols+j] = x;
}



/////////////////////////////////////////////////////////////////
//
//                   matrix_long::safe_get_entry
//
/////////////////////////////////////////////////////////////////

long matrix_long::safe_get_entry(
  long  i,
  long  j
)
{
  if (i >= num_rows)
    error("Index error in matrix_long::safe_entry");

  return ( (long *) data)[i*num_cols+j];
}



/////////////////////////////////////////////////////////////////
//
//                 operator<< for vector_long
//
/////////////////////////////////////////////////////////////////

ostream &operator<<(
  ostream &stream,
  vector_long &vec
)
{
  long   lv,i;

  lv = vec.get_length();
  stream << lv << " ";

  for (i = 0; i < lv; i++) stream << vec[i] << " ";

  return stream;
}



/////////////////////////////////////////////////////////////////
//
//                    matrix_long::print
//
/////////////////////////////////////////////////////////////////

void matrix_long::print(void)
{
  long  i,j;

  for (i = 0; i < num_rows; i++) {
    for (j = 0; j < num_cols; j++)
      cout << get_entry(i,j) << "  ";

    cout << endl;
  }
}
