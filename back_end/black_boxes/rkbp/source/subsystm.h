// subsystm.h


enum  automaton_type  {PREFIX_AUTOMATON, SUBWORD_AUTOMATON,
                       ALPHABET_AUTOMATON};

enum index_type {NO_INDEX, AUTOMATON, RABIN_KARP, HYBRID};
enum action_type {REWRITE, OVERLAP, BACKTRACK};



/////////////////////////////////////////////////////////////////
//
//                         subsystem
//
/////////////////////////////////////////////////////////////////

class subsystem {
  class rewriting_system  *parent;
  vector_char              subsystem_file;
  rule_type                r_type;
  index_type               i_type;
  
  // External rule information
  
  vector_char      rule_file;
  _boolean_t          compressed;
  external_form    ext_form;
  
  // Internal rule information
  
  chunky_array  packed_rules;
  chunky_array  origin;
  long          free;
  sort_type     sort;
  long          n_bits;
  long          next_heur;

  // Summary rule data
  
  long       nrules;
  long       llmin;
  long       llmax;
  long       lltotal;
  long       lrmin;
  long       lrmax;
  long       lrtotal;
  long       ttlmin;
  long       ttlmax;


  // Various indexes
  
  // The Rabin-Karp index

  long          window_size;
  long          rkprime;
  long          hash_prime;
  long          neg_power;
  chunky_array  signature;
  chunky_array  hash_table;
  
  // The automaton and hybrid indexes
  
  automaton_type   a_type;
  long             nstates;
  chunky_array     table;
  long             initial_state;
  vector_long      len_start;


  // Scratch matrices used in rewriting and overlap formation.

  matrix_long      rw_aux;
  matrix_long      ov_aux;

  // Linkages on various linked lists

  subsystem   *next_subsystem;
  subsystem   *prev_subsystem;
  subsystem   *next_save;
  subsystem   *prev_save;
  
  friend class gen_extractor;
  friend class bit_extractor;
  friend class gen_inserter;

 public:
  subsystem(
    rewriting_system  *sys = NULL,
    rule_type          rtp = SUBWORD_RULE,
    index_type         itp = NO_INDEX,
    sort_type          stp = NO_SORT
  );

  void set_parent( rewriting_system *sys );

  void  set_subsystem_file( char *file )
    { subsystem_file.set_data(file); }


  void  set_next_subsystem( subsystem *subsyst )
    { next_subsystem = subsyst; }

  void set_next_save( subsystem *subsyst )
    { next_save = subsyst; }

  void set_prev_subsystem( subsystem *subsyst )
    { prev_subsystem = subsyst; }

  void set_prev_save( subsystem *subsyst )
    { prev_save = subsyst; }

  rule_type get_r_type() { return r_type; }
  index_type get_i_type() { return i_type; }

  long    get_nrules() { return nrules; }
  long    get_free() { return free; }
  long    get_llmax() { return llmax; }

  long inverse(long g) { return parent->inverse(g); }

  void average_lengths(long &avll, long &avlr);
  _boolean_t origin_in_use()
    { return i_type == RABIN_KARP || i_type == HYBRID; }

  void rule_by_origin( long &orig, packed_rule &p_rule );

  void rule_by_number( long numb, packed_rule &p_rule )
  {
	 long temp = origin.extract_long(numb);
	 rule_by_origin(temp,p_rule);
  }

  void input(istream &stream, _boolean_t prompt, _boolean_t check);
  void input_rest(istream &stream, _boolean_t prompt,
                  _boolean_t check);
  long input_rules(istream &stream, char *file,
            external_form extfrm, _boolean_t check, _boolean_t prompt);
  void add_rule(rule &rul, _boolean_t update_index, _boolean_t secret);
  void enter_rule(rule &rul, long &orig);
  
  void summary();
  subsystem *get_next_subsystem() { return next_subsystem; }
  subsystem *get_prev_subsystem() { return prev_subsystem; }
  subsystem *get_next_save() { return next_save; }
  subsystem *get_prev_save() { return prev_save; }

  void check_aux( action_type act, _boolean_t subword_only,
                  long len );

  void unpack(word_reference wrd, vector_long &vec, long offset);
  void unpack(packed_rule &rul, rule &rul1);

  void append( subsystem *subsyst );
  
  void incremental_test(
    action_type    act,
    _boolean_t        subword_only,
    vector_long   &output,
    rule_reference exclude,
    packed_rule   &rul
  );

  void automaton_incremental(
    vector_long    &output,
    rule_reference  exclude,
    matrix_long    &aux,
    packed_rule    &rul
  );

  void identity_incremental(
    vector_long    &output,
    rule_reference  exclude,
    matrix_long    &aux,
    packed_rule    &rul
  );

  void rabin_karp_incremental(
    vector_long    &output,
    rule_reference  exclude,
    matrix_long    &aux,
    packed_rule    &rul
  );

  void hybrid_incremental(
    vector_long    &output,
    rule_reference  exclude,
    matrix_long    &aux,
    packed_rule    &rul
  );

  packed_rule global_test(
    _boolean_t        subword_only,
    vector_long   &output,
    rule_reference exclude
  );
    
  packed_rule automaton_global(
    vector_long   &output,
    rule_reference exclude
  );
    
  packed_rule identity_global(
    vector_long   &output,
    rule_reference exclude
  );
    
  packed_rule rabin_karp_global(
    vector_long   &output,
    rule_reference exclude
  );
    
  packed_rule hybrid_global(
    vector_long   &output,
    rule_reference exclude
  );
    
  _boolean_t cummulative_test(vector_long &word,rule_type rtp);
  _boolean_t rabin_karp_cummulative(vector_long &word,
                                 rule_type rtp);
  _boolean_t automaton_cummulative(vector_long &word,
                                rule_type rtp);
  _boolean_t hybrid_cummulative(vector_long &word, rule_type rtp);
  
  long overlap_test(vector_long &word, long len,
         packed_rule &p_rule);
  
  long rabin_karp_overlap(vector_long &word, long len,
         packed_rule &p_rule);
         
  long hybrid_overlap(vector_long &word, long len,
         packed_rule &p_rule);
         
  long automaton_overlap(vector_long &word, long len,
         packed_rule &p_rule);

  void reset_statistics();
  void statistics(long ll, long lr);

  void statistics(packed_rule p_rule)
    { statistics(p_rule.left.length, p_rule.right.length); }

  void statistics(rule &rul)
    {statistics(rul.left.get_length(), rul.right.get_length() );}

  void make_origin();
  void make_index();
  
  _boolean_t reduce(void);
  
  _boolean_t heuristics(void);

  long get_window() { return window_size; }

  long compute_signature( vector_long &word);
  void make_rabin_karp(long num, long win);
  void rehash_rabin_karp();
  void rabin_karp_insert( long i );
  long rabin_karp_search( vector_long &word, long sig );
  
  void make_prefix_automaton();
  void make_subword_automaton();
  long state_length( long state );
  
  void make_subword_hybrid();
  void make_prefix_hybrid();

  long binary_search(long bot,long top,long bitnum,long &bot0,
                     long &top0,long &bot1,long &top1 );
  
  long hybrid_search( vector_long &word, long &bot, long &top,
                      long start_bit );

  void prefix_data( long flag );
  void prefix_insert( long &orig );
  
  void sort_rules( sort_type stp );
  
  void left_bit_list( packed_rule p_rule, vector_char &list);
  void set_len_start( long max );
  void reset_len_start();
  void hybrid_first_pass();
  void write( fstream &stream, char *file_name, long num );

  void subsystem::unpack(
    long          length,
    long          origin,
    long          bit_pos,
    vector_long  &wrd,
    long          offset
  );

  long jump( long ll, long lr )
    { return (n_bits*(ll+lr) + _LONG_BIT - 1)/_LONG_BIT; }

  long jump( packed_rule p_rule )
    { return jump(p_rule.left.length,p_rule.right.length) + 2; }

  void right_start( long ll, long &org, long &pos) {
    long  len;
    len = ll*n_bits;
    org = len/_LONG_BIT;
    pos = len - org*_LONG_BIT;
  }

  void details();
};
