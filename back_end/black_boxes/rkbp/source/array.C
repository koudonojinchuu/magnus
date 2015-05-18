// array.cpp


#include "rkbp.h"

ulong  mask[33] = {0,
                   01UL,03UL,07UL,
                   017UL,037UL,077UL,
                   0177UL,0377UL,0777UL,
                   01777UL,03777UL,07777UL,
                   017777UL,037777UL,077777UL,
                   0177777UL,0377777UL,0777777UL,
                   01777777UL,03777777UL,07777777UL,
                   017777777UL,037777777UL,077777777UL,
                   0177777777UL,0377777777UL,
                   0777777777UL,01777777777UL,
                   03777777777UL,0777777777UL,
                   017777777777UL,03777777777UL};




/////////////////////////////////////////////////////////////////
//
//                   continued_string
//
/////////////////////////////////////////////////////////////////

long continued_string(
  istream  &stream,
  vector_char  &buff,
  vector_char  &buff1
)
{
  if (!(stream >> buff) ) goto input_error;

  while ( '&' == buff.last() ) {
    buff.set_length( buff.get_length()-1 );
    if (!(stream >> buff1) ) goto input_error;
    buff.append(buff1);
  }

  return 0;

 input_error:
  if (0 == buff.get_length() ) return 1;
  return 2;  
}



/////////////////////////////////////////////////////////////////
//
//                 gen_extractor::gen_extractor
//
/////////////////////////////////////////////////////////////////

gen_extractor::gen_extractor(
  subsystem  *sbsst
)
: subsyst(sbsst),
  n_bits(subsyst->n_bits),
  orig(0),
  pos(0),
  w(0),
  wbits(0)
{
  ;
}



/////////////////////////////////////////////////////////////////
//
//                  gen_extractor::reset
//
/////////////////////////////////////////////////////////////////

void gen_extractor::reset(
  long  org,
  long  ps
)
{
  orig  = org;
  w     = subsyst->packed_rules.extract_ulong(orig++);
  wbits = _LONG_BIT - ps;
}



////////////////////////////////////////////////////////////////
//
//                     gen_extractor::next
//
/////////////////////////////////////////////////////////////////

long gen_extractor::next(void)
{
  long        zbits,diff;
  ulong       z;
  void huge  *pointer;

  zbits = z = 0;
  
  while (zbits < n_bits) {
    if (wbits == 0) {
      w = subsyst->packed_rules.extract_ulong(orig++);
      wbits = _LONG_BIT;
    }

    diff = n_bits - zbits;

    if (wbits > diff) {
      z = (z<<diff) | (w>>(wbits-diff))&mask[diff];
      zbits = n_bits;
      wbits -= diff;
    }

    else {
      z = (z<<wbits) | (w&mask[wbits]);
      zbits += wbits;
      wbits  = 0;
    }
  }

  return z;
}



/////////////////////////////////////////////////////////////////
//
//                      packed_rule::is_subword
//
/////////////////////////////////////////////////////////////////

_boolean_t packed_rule::is_subword(
  vector_long  &word
)
{
  long            ll,i,j;
  gen_extractor   g_ext(left.subsyst);
  
  ll = left.length;
  if (ll > offset) return FALSE;
  
  g_ext.reset(left.origin,0);
  
  for (i = 0, j = offset-ll; i < ll; i++)
    if (g_ext.next() != word[j++]) return FALSE;
  
  return TRUE;
}



/////////////////////////////////////////////////////////////////
//
//                 bit_extractor::bit_extractor
//
/////////////////////////////////////////////////////////////////

bit_extractor::bit_extractor(
  subsystem  *sbsst
)
: subsyst(sbsst),
  n_bits(sbsst->n_bits),
  orig(0),
  w(0),
  wbits(0)
{
  ;
}



/////////////////////////////////////////////////////////////////
//
//                  bit_extractor::reset
//
/////////////////////////////////////////////////////////////////

void bit_extractor::reset(
  long  org,
  long  first_bit
)
{
  long   i;

  i = first_bit/_LONG_BIT;
  orig = org + i;
  w = subsyst->packed_rules.extract_ulong(orig++);
    
  wbits = (i+1)*_LONG_BIT - first_bit;      
}



////////////////////////////////////////////////////////////////
//
//                     bit_extractor::next
//
/////////////////////////////////////////////////////////////////

long bit_extractor::next(void)
{
  if (wbits == 0) {
    w = subsyst->packed_rules.extract_ulong(orig++);
    wbits = _LONG_BIT;
  }
  
  wbits--;
  
  return (w>>wbits)&1;
}



/////////////////////////////////////////////////////////////////
//
//                 gen_inserter::gen_inserter
//
/////////////////////////////////////////////////////////////////

gen_inserter::gen_inserter(
  subsystem  *sbsst
)
: subsyst(sbsst),
  n_bits(subsyst->n_bits),
  orig(0),
  w(0),
  z(0),
  wbits(0),
  zbits(0) 
{
  ;
}



/////////////////////////////////////////////////////////////////
//
//                  gen_inserter::reset
//
/////////////////////////////////////////////////////////////////

void gen_inserter::reset(
  long  org
)
{
  orig  = org;
  zbits = wbits = w = z = 0;
}



////////////////////////////////////////////////////////////////
//
//                     gen_inserter::insert
//
/////////////////////////////////////////////////////////////////

void gen_inserter::insert(
  long  g
)
{
  long        diff,old_orig;
  void huge  *pointer;

  z     = g;
  zbits = n_bits;
    
  while (zbits > 0) {
    if (wbits == _LONG_BIT) {
      subsyst->packed_rules.insert_ulong(orig++,w);
      wbits = w = 0;
    }

    diff = _LONG_BIT - wbits;

    if (zbits >= diff) {
      w = (w<<diff) | (z>>(zbits-diff))&mask[diff];
      zbits -= diff;
      wbits  = _LONG_BIT;
    }

    else {
      w = (w<<zbits) | (z&mask[zbits]);
      wbits += zbits;
      zbits  = 0;
    }
  }
}



/////////////////////////////////////////////////////////////////
//
//                  gen_inserter::close
//
/////////////////////////////////////////////////////////////////

void gen_inserter::close(long &org)
{
  if (wbits > 0) {
    w       = w<<(_LONG_BIT-wbits);
    subsyst->packed_rules.insert_ulong(orig++,w);
  }
  
  org = orig;
  return;
}



/////////////////////////////////////////////////////////////////
//
//               word_reference::unpack
//
/////////////////////////////////////////////////////////////////

void word_reference::unpack(
  vector_long  &vec,
  long          offset
)
{
  subsyst->unpack(*this,vec,offset);
}



