// rule.cpp

#include "rkbp.h"



/////////////////////////////////////////////////////////////////
//
//                      rule::rule
//
/////////////////////////////////////////////////////////////////

// rule constructor.

rule::rule(
  rule_type   rtp,
  long        mxl,
  long        mxr
)
: left (mxl),
  right(mxr)
{
  r_type = rtp;
}



rule::rule(
  rule_type     rtp,
  vector_long   lft,
  vector_long   rgt
)
{
  r_type = rtp;
  left   = lft;
  right  = rgt;
}



/////////////////////////////////////////////////////////////////
//
//                       rule::switch_sides
//
/////////////////////////////////////////////////////////////////

void rule::switch_sides(void)
{
  vector_long  temp(0);

  temp = left;
  left = right;
  right = temp;

  // To keep from freeing memory in original left vector.

  temp.set_null();
}



/////////////////////////////////////////////////////////////////
//
//                  operator<< for rule
//
/////////////////////////////////////////////////////////////////

ostream &operator<<(
  ostream &stream,
  rule     &rul
)
{
  stream << rul.left;
  stream << rul.right;

  return stream;
}
