// generatr.cpp

#include "rkbp.h"


/////////////////////////////////////////////////////////////////
//
//                  generator::generator
//
/////////////////////////////////////////////////////////////////

generator::generator(
  char  *nam
)
: name(10),
inverse(10)
{
  name.set_data(nam);

  invertible = FALSE;
  inv_numb = -1;
  weight = level = 0;
}



/////////////////////////////////////////////////////////////////
//
//                       generator::set
//
/////////////////////////////////////////////////////////////////

void generator::set(
  char *nam,
  long  inn,
  long  wgt,
  long  lvl,
  vector_long  inv
)
{
  name.set_data(nam);

  inv_numb  = inn;
  weight    = wgt;
  level     = lvl;
  inverse   = inv;
}



/////////////////////////////////////////////////////////////////
//
//                    gen_autn::gen_autn
//
/////////////////////////////////////////////////////////////////

gen_autn::gen_autn(
  long  mxr
)
: matrix_long(mxr,3)
{
  safe_set_entry(0,0,LONG_MAX);
  safe_set_entry(0,1,LONG_MAX);
  safe_set_entry(0,2,LONG_MAX);
}



/////////////////////////////////////////////////////////////////
//
//                     gen_autn::enter_string
//
/////////////////////////////////////////////////////////////////

void gen_autn::enter_string(
  char  *str,
  long   flag
)
{
  long    state,len,i,k,x,bit,entry;

  state = 0;
  len   = strlen(str);

  for (i = 0; i < len; i++) {
    x = str[i];

    for (k = CHAR_BIT-2; k >= 0; k--) {
      bit = (x>>k)&1;
      entry = get_entry(state,bit);

      if (entry == LONG_MAX) {
        entry = num_rows;
        safe_set_entry(entry,0,LONG_MAX);
        safe_set_entry(entry,1,LONG_MAX);
        safe_set_entry(entry,2,LONG_MAX);
        set_entry(state,bit,entry);
      }

      state = entry;
    }
  }

  if ( get_entry(state,2) != LONG_MAX)
    error("Two generators have the same name.");

  set_entry(state,2,flag);
}


// end generatr.C
