// array.h


/*
   This file defines the following struct's and classes:

     rule_reference
     word_reference
     packed_rule
     gen_extractor
     bit_extractor
     gen_inserter

*/



extern long continued_string(istream &stream,
                        vector_char &buff, vector_char &buff1);



/////////////////////////////////////////////////////////////////
//
//                      rule_reference
//
/////////////////////////////////////////////////////////////////

struct rule_reference {
  class subsystem  *subsyst;
  long        orig;
};



/////////////////////////////////////////////////////////////////
//
//                       word_reference
//
/////////////////////////////////////////////////////////////////

struct word_reference {
  class subsystem  *subsyst;
  long              length;
  long              origin;
  long              bit_pos;

  word_reference() {subsyst=NULL; length = origin = bit_pos = 0;}

  void unpack( vector_long &vec, long offset);
};



/////////////////////////////////////////////////////////////////
//
//                         packed_rule
//
/////////////////////////////////////////////////////////////////

class packed_rule {
 public:
  word_reference    left;
  word_reference    right;
  _boolean_t           redundant;
  long              offset;      // In some word, the length of
                                 // the prefix having this left
                                 // side as a suffix.
                                 
  packed_rule() {redundant=FALSE; offset = 0;}

  _boolean_t is_subword(vector_long &word);
};




/////////////////////////////////////////////////////////////////
//
//                       gen_extractor
//
/////////////////////////////////////////////////////////////////

class gen_extractor {
  subsystem     *subsyst;
  long           n_bits;
  long           orig;
  long           pos;
  ulong          w;
  long           wbits;

 public:
  gen_extractor( subsystem *subsyst );

  void reset( long org, long ps );
  long next();  
};



/////////////////////////////////////////////////////////////////
//
//                       bit_extractor
//
/////////////////////////////////////////////////////////////////

class bit_extractor {
  subsystem     *subsyst;
  long           n_bits;
  long           orig;
  ulong          w;
  long           wbits;

 public:
  bit_extractor( subsystem *subsyst );

  void reset( long orig, long first_bit );
  long next();  
};



/////////////////////////////////////////////////////////////////
//
//                       gen_inserter
//
/////////////////////////////////////////////////////////////////

class gen_inserter {
  subsystem     *subsyst;
  long           n_bits;
  long           orig;
  ulong          w;
  ulong          z;
  long           wbits;
  long           zbits;

 public:
  gen_inserter( subsystem *sbsst);

  void reset(long org);
  void insert(long g);
  void close(long &org);  
};
