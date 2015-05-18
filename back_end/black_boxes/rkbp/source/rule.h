// rule.h


enum rule_type {PREFIX_RULE, SUBWORD_RULE};

enum external_form {NAMES, LEN_NAMES, LEN_NUMBERS, LEN_6BIT,
                    BINARY};

enum sort_type {NO_SORT, LEN, LEX, LENLEX, LENLENLEXLEX,
                LENLEXLENLEX};




/////////////////////////////////////////////////////////////////
//
//                          rule
//
/////////////////////////////////////////////////////////////////

class rule {
 public:
  rule_type     r_type;
  vector_long   left;
  vector_long   right;

  rule(
    rule_type   rtp = SUBWORD_RULE,
    long        mxl = 100,
    long        mxr = 100
  );

  rule(
    rule_type     rtp,
    vector_long   lft,
    vector_long   rgt
  );

  void set_max_length(long mxl)
   { left.set_max_length(mxl); right.set_max_length(mxl); }

  void set_r_type( rule_type rtp ) { r_type = rtp; }

  void switch_sides();
  
  void print() { left.print(); right.print(); }
};


extern ostream &operator<<(ostream &stream, rule &rul);

