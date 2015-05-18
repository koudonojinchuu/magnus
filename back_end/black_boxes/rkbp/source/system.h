// system.h



enum alphabet_type         {GROUP, MONOID};
enum rewriting_strategy    {FROM_LEFT, GLOBAL};
enum knuth_bendix_strategy {BASIC, TWO_STEP};

enum order_type            {LNLX, WTLNLX, WRLNLX, WRWTLNLX};
                            
enum naming_convention     {PREFIX, LETTERS, NO_NAME};
enum numbering_convention  {a_b, a_A, NO_NUMB};
enum inverse_convention    {CASE, EXPONENT, NO_INV};
enum convention            {CONSTANT, NONCONSTANT};



/////////////////////////////////////////////////////////////////
//
//                       rewriting_system
//
/////////////////////////////////////////////////////////////////

class rewriting_system {
  vector_char    system_file;
  alphabet_type  a_type;
  order_type     o_type;
  _boolean_t        use_heuristics;
  long           n_gens;    // Number of generators.
  long           t_gens;    // Total number of monoid generators.
                            // The same as n_gens for monoid
                            // alphabets and 2*n_gens for group
                            // alphabets.
  long           n_bits;    // Number of bits needed to pack a
                            // generator or the inverse of a
                            // generator.
  generator     *gens;      // Array of generator information.
  gen_autn       autn;      // Facilitates parsing.
  long           n_sub;     // The number of subsystems
  long           savell;    // Max length of left sides of rules
                            // currently being saved.
  long           savelr;    // Max length of right sides of rules
                            // currently being saved.
  long           bsize;     // Current length of rewriting
                            // buffers.

  rewriting_strategy      rw_strategy;
  knuth_bendix_strategy   kb_strategy;

  _boolean_t        rewrite_verbose;   // If true, report on
                                    // rewriting.
  long           logll;
  ofstream       log_stream;

  long           n_rewrite;
  long           total_input;
  long           n_applications;
  long           total_left;
  long           noverlaps;
  long           nbalance;
  long           nsave;

  // The subsystems are described by various doubly linked lists

  subsystem     *first_subsystem;
  subsystem     *last_subsystem;
  subsystem     *first_save;
  subsystem     *last_save;
  subsystem     *broken_subword_rules;
  subsystem     *broken_prefix_rules;
  
 public:
  rewriting_system(void);

 ~rewriting_system();

  subsystem  *new_subsystem(char *str = NULL);

  void get_system_file(char *fil) { system_file.get_data(fil); }
  alphabet_type get_a_type(void) { return a_type; }

  long          get_n_gens(void) { return n_gens; }
  long          get_t_gens(void) { return t_gens; }
  long          get_n_bits(void) { return n_bits; }
  long          get_n_nsub(void) { return n_sub;  }

  _boolean_t get_use_heuristics() { return use_heuristics; }
  void set_use_heuristics(_boolean_t value)
    { use_heuristics = value; }

  void input(char * file, _boolean_t check);
  void input(istream &stream, _boolean_t prompt, _boolean_t check);

  long inverse(long g) { return gens[g].get_inv_numb(); }
  long gen_number(char *str);
  void gen_name(char *nam, long i) { gens[i].get_name(nam); }
  long parse(vector_char &vec, vector_long &wrd);
  long parse_6bit(vector_char &vec, long len, vector_long &word);
  void add_subsystem( subsystem *subsyst );
  void make_generator_index();
  long read_word( istream &stream, external_form exf,
                  vector_long &wrd);
  void summary();

  void set_save( long ll, long lr) { savell = ll; savelr = lr; }
  void set_bsize( long bsz ) { bsize = bsz; }
  long get_bsize() { return bsize; }
  long get_weight( long g ) { return gens[g].get_weight(); }
  long get_level( long g ) { return gens[g].get_level(); }
  long word_weight(vector_long &wrd);
  _boolean_t orient_rule(rule &rul);
  void balance_rule(rule &rul);

  void save_rule(rule &rul);
  void save_broken_rule(rule &rul);
  
  void set_rewrite_verbose( _boolean_t val )
    { rewrite_verbose = val; }

  void ask_rewrite_verbose(void);

  long (rewriting_system::*base_order)
            (vector_long &u, vector_long &v);

  long (rewriting_system::*compare)
            (vector_long  &u, vector_long  &v);

  long comp(vector_long &u, vector_long &v) 
    { return (this->*compare)(u,v); }

  long lnlx(vector_long &u, vector_long &v);

  long wtlnlx(vector_long &u, vector_long &v);

  long wreath(
     vector_long &u,
     vector_long &v
  );

  long wrlnlx(vector_long &u, vector_long &v)
    { base_order = &rewriting_system::lnlx; return wreath(u, v); }

  long wrwtlnlx(vector_long &u, vector_long &v)
    { base_order = &rewriting_system::wtlnlx; return wreath(u, v); }

  void level_decomposition(vector_long &u, vector_long &v,
    vector_long &ubuff, vector_long &vbuff);

  void level_sequence(
    vector_long  &u,
    long          lc,
    long          lu,
    vector_long  &ubuff
  );

  void set_o_type(order_type otp);

  void ( rewriting_system::*pRewrite)(
           _boolean_t         subword_only,
           vector_long    &input,
           vector_long    &output,
           rule_reference  exclude,
           _boolean_t        &changed,
           _boolean_t        &reduced
  );
  
  void rewrite(rule &rul, rule_reference exclude,
       _boolean_t &changed, _boolean_t &reduced);

  void rewrite_from_left(
           _boolean_t         subword_only,
           vector_long    &input,
           vector_long    &output,
           rule_reference  exclude,
           _boolean_t        &changed,
           _boolean_t        &reduced
  );

  void rewrite_global(
           _boolean_t         subword_only,
           vector_long    &input,
           vector_long    &output,
           rule_reference  exclude,
           _boolean_t        &changed,
           _boolean_t        &reduced
  );

  void set_rw_strategy(rewriting_strategy rws)
    { rw_strategy = rws;
      if (rws == FROM_LEFT) pRewrite = &rewriting_system::rewrite_from_left;
      else pRewrite = &rewriting_system::rewrite_global; }
  
  _boolean_t (rewriting_system::*pKnuth_Bendix)(
    long      maxlen,
    long     &num_iter,
    long      prevlen,
    double    factor
  );

  _boolean_t kb_basic(
    long      maxlen,
    long     &num_iter,
    long      prevlen,
    double    factor
  );
  
  _boolean_t kb_two_step(
    long      maxlen,
    long     &num_iter,
    long      prevlen,
    double    factor
  );
  
  _boolean_t kb_random_two_step(
    long      maxlen,
    rule_type rtype,
    long      ntrials
  );
  
  void set_kb_strategy(knuth_bendix_strategy kbs)
    { kb_strategy = kbs;
      if (kbs == BASIC) pKnuth_Bendix = &rewriting_system::kb_basic;
      else pKnuth_Bendix = &rewriting_system::kb_two_step; }

  void initialize_save_list(
    rule_type  rtp,
    long       enm,
    long       ell,
    long       elr
  );

  _boolean_t reduce_all_subsystems(
    rule_type  rtp
  );

  void heuristics(
    rule  &rul
  );

  void heuristics(void);

  _boolean_t reduce_save_list(void);

  void append_save_list(subsystem *subsyst);

  void overlap(
    subsystem  *subsyst1,
    subsystem  *subsyst2,
    long        start_orig,
    long        minlen,
    long        maxlen,
    long        maxsave,
    subsystem **avoid,
    long        navoid,
    long       &neworig,
    long       &nnew,
    _boolean_t    &successful
  );

  void random_overlap(
    subsystem  *subsyst1,
    subsystem  *subsyst2,
    long        maxlen,
    subsystem **avoid,
    long        navoid
  );

  void remove_subsystem( subsystem *subsyst );

  long compare_packed_rules( sort_type stp, packed_rule v_rule,
                            packed_rule w_rule);

  long compare_lex(word_reference u_ref, word_reference v_ref);
  void write(char *file_name);
  
  void print_word(
    ostream       &stream,
    external_form  ext_form,
    vector_long   &word
  );
  
  void print_inverse( fstream &stream, generator &g )
    { print_word( stream, NAMES, g.inverse); }
  
  void print_counts();

  _boolean_t check_rule( rule &rul ) {
    if ( rul.left.get_length() <= savell &&
         rul.right.get_length() <= savelr   )
      return TRUE;

    else return FALSE;
  }

  void save_inverses();

  void details( long  i );

  long probe(
    int     seed,
    _boolean_t subword_only,
    long    ntrials,
    long    depth
  );

  void rewrite_rules(void);

  void add_powers( long exponent, long maxlen );

  void add_random_powers(
    int  seed,
    long ntrials,
    long exponent,
    long maxlen
  );

  void rewrite_words(void);

  void rewrite_words(
    istream         &instream,
    external_form   ext,
    _boolean_t      subword_only,
    ostream         &outstream
  );

  void log_on(long maxlen, char *file);
  void log_off(void);
};



extern void set_hybrid_factor(double factor);
