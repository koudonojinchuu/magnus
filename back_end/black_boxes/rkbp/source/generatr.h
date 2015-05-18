// generatr.h

// This file defines the classes generator and gen_autn.


/////////////////////////////////////////////////////////////////
//
//                          generator
//
/////////////////////////////////////////////////////////////////

class generator {
  vector_char  name;     // The external name of the generator.
  long         inv_numb; // The number of the generator defining
                         // the inverse, or -1 if no such gen.
  long         weight;
  long         level;
  _boolean_t      invertible; // TRUE if generator invertible.
  vector_long  inverse;    // Word giving inverse.
  
  friend class rewriting_system;

 public:

  generator(char *nam = NULL);

  void set_name(char *nam) { name.set_data(nam); }
  void set_inv_numb(long inn) { inv_numb = inn; }
  void set_weight(long wgt) { weight = wgt; }
  void set_level(long lvl) { level = lvl; }
  void set_inverse( vector_long &inv ) { inverse.set_copy(inv); }

  void set_one( long g)
   { inverse.set_data(1,&g); }

  void set_invertible(_boolean_t flg) {invertible = flg;}

  void set(
    char        *nam,
    long         inn,
    long         wgt,
    long         lvl,
    vector_long  inv
  );

  _boolean_t compare_name(char *str)
    { return name.compare_data(str);}


  long get_length() { return name.get_length(); }
  long gen_number( char *str);
  void get_name( char * dest ) { name.get_data(dest); }
  long get_inv_numb() { return inv_numb; }
  _boolean_t get_invertible() { return invertible; }
  long get_weight() { return weight; }
  long get_level()  { return level; }
  long get_inv_length() { return inverse.get_length(); }
  long get_inv_term(long i) {return inverse[i]; }
  void print_name(ostream &stream) { stream << name; }
};



/////////////////////////////////////////////////////////////////
//
//                         gen_autn
//
/////////////////////////////////////////////////////////////////

/*
  Objects of the class gen_autn are used to parse strings of
  ASCII characters into concatenations of external names of
  generators.  A bit string is assigned to a character string
  by concatenating the reversals of the low-order CHAR_BIT-1
  bits of the characters.  The states of the automaton are
  substrings of the strings associated with the external
  names of the generators.  If a state corresponds to the
  external name of a generator, the number of that generator
  is put in the third column for that state.
*/


class gen_autn : public matrix_long {
 public:
  gen_autn( long mxr = 20);
  
  void enter_string(char *str, long flag);
};



// end generatr.h
