// subsystm.cpp


#include "rkbp.h"

extern  double  hybrid_factor;
extern  ulong   mask[33];
extern  long    print_level;



#define check(s) if ( 0 != strcmp(str,s) ) goto invalid_input

#define LABS(x)   ((x) >= 0 ? (x) : (-x))

// The following are used in hybrid_incremental and
// hybrid_cummulative.

#define  TRACE   1
#define  SEARCH  2


// The following hides a feature being developed for version
// 1.30.

#define  IDENTITY  17


/////////////////////////////////////////////////////////////////
//
//                   subsystem::subsystem
//
/////////////////////////////////////////////////////////////////

subsystem::subsystem(
  rewriting_system  *sys,
  rule_type          rtp,
  index_type         itp,
  sort_type          stp
)
: packed_rules(),
  window_size(0),
  rkprime(0),
  hash_prime(0),
  neg_power(0),
  signature(),
  hash_table(),
  table(),
  len_start(0),
  ov_aux(0,3),
  rw_aux(0,3)
{
  parent         = sys;
  r_type         = rtp;
  i_type         = itp;
  a_type         = SUBWORD_AUTOMATON;

  next_heur      = 0;  
  initial_state  = 0;

  compressed = FALSE;
  ext_form   = LEN_NAMES;
  sort       = stp;

  if (sys != NULL) n_bits = sys->get_n_bits();
  else n_bits = 0;

  reset_statistics();

  next_subsystem = NULL;
  prev_subsystem = NULL;
  next_save      = NULL;
  prev_save      = NULL;
}



/////////////////////////////////////////////////////////////////
//
//                    subsystem::set_parent
//
/////////////////////////////////////////////////////////////////

void subsystem::set_parent(
  rewriting_system  *sys
)
{
  parent = sys;
  n_bits = parent->get_n_bits();
}



/////////////////////////////////////////////////////////////////
//
//                    subsystem::reset_statistics
//
/////////////////////////////////////////////////////////////////

void subsystem::reset_statistics(void)
{
  nrules  = 0;
  free    = 0;
  llmin   = LONG_MAX;
  llmax   = 0;
  lltotal = 0;
  lrmin   = LONG_MAX;
  lrmax   = 0;
  lrtotal = 0;
  ttlmin  = LONG_MAX;
  ttlmax  = 0;
}



/////////////////////////////////////////////////////////////////
//
//                        subsystem::statistics
//
/////////////////////////////////////////////////////////////////

void subsystem::statistics(
  long  ll,
  long  lr
)
{
  long  ttl;

  nrules++;

  llmin    = min(ll,llmin);
  llmax    = max(ll,llmax);
  lltotal += ll;

  lrmin    = min(lr,lrmin);
  lrmax    = max(lr,lrmax);
  lrtotal += lr;

  ttl    = ll + lr;
  ttlmin = min(ttl,ttlmin);
  ttlmax = max(ttl,ttlmax);
}



/////////////////////////////////////////////////////////////////
//
//                subsystem::average_lengths
//
/////////////////////////////////////////////////////////////////

void subsystem::average_lengths(
  long  &avll,
  long  &avlr
)
{
  if (nrules == 0) avll = avlr = 0;

  else {
    avll = (lltotal + nrules - 1)/nrules;
    avlr = (lrtotal + nrules - 1)/nrules;
  }
}



/////////////////////////////////////////////////////////////////
//
//                   subsystem::rule_by_origin
//
/////////////////////////////////////////////////////////////////

void subsystem::rule_by_origin(
  long         &orig,
  packed_rule  &p_rule
)
{
  long         ll,lr,org,pos;
  _boolean_t      red;
  ulong        ent;

  p_rule.redundant    = FALSE;
  ent                 = packed_rules.extract_ulong(orig++);

  if ( ent>>(_LONG_BIT-1) != 0) {
    p_rule.left.length  = ll = (ent<<1)>>1;
    p_rule.redundant    = TRUE;
  }

  else p_rule.left.length = ll = ent;

  p_rule.left.origin  = orig+1;
  p_rule.left.bit_pos = 0;
  p_rule.left.subsyst = this;
  
  right_start(ll,org,pos);


  lr                   = packed_rules.extract_ulong(orig++);
  p_rule.right.length  = lr;
  p_rule.right.origin  = orig + org;
  p_rule.right.bit_pos = pos;
  p_rule.right.subsyst = this;
  
  orig += jump(ll,lr);
  p_rule.offset = 0;
}



/////////////////////////////////////////////////////////////////
//
//                   subsystem::input
//
/////////////////////////////////////////////////////////////////

void subsystem::input(
  istream &strm,
  _boolean_t  prompt,
  _boolean_t  check
)
{
  long      exists;
  _boolean_t   echo;
  char      str[500];
  ifstream  fstream;

  echo = get_echo();

  // Get the name of the subsystem file. ///////////////////

  if (prompt)
    do {  
      cout <<
        "\nEnter name of subsystem file (or \"none\"):  ";
      scan_string(strm,str);

      exists = (0 == strcmp(str,"none") || file_exists(str) );

      if (!exists) {
        cout << "file does not exist." << endl;
        if (echo) exit(1);
      }
    } while (!exists);

  else {
    strm >> str;

    if (0 != strcmp(str,"none") && !file_exists(str) )
      error("subsystem file does not exist.");
  }

  subsystem_file.set_data(str);

  if (0 == strcmp(str,"none") ) input_rest(strm,prompt,check);

  else {
    fstream.open(str);
    prompt = FALSE;
    input_rest(fstream,prompt,check);
  }

  make_index();
}



/////////////////////////////////////////////////////////////////
//
//                  subsystem::input_rest
//
/////////////////////////////////////////////////////////////////

void subsystem::input_rest(
  istream  &stream,
  _boolean_t   prompt,
  _boolean_t   check
)
{
  long      exists;
  char      str[500];
  _boolean_t   echo;
  ifstream  fstrm;

  echo = get_echo();

  // Get the type of subsystem. ///////////////////////////

 get_rule_type:
  if (prompt) {
    cout << "\nEnter rule_type (prefix or subword):  ";
    scan_string(stream,str);
  }

  else {
    stream >> str;
    check("subsystem");
    stream >> str;
    check("rule_type:");
    stream >> str;
  }

  if      (0 == strcmp(str,"prefix") )   r_type = PREFIX_RULE;
  else if (0 == strcmp(str,"subword") )  r_type = SUBWORD_RULE;
  //else if (0 == strcmp(str,"identity") ) r_type = IDENTITY;

  else {
    cout << "Invalid rule_type" << endl;
    if (prompt && !echo) goto get_rule_type;
    exit(1);
  }


  // Get the type of index. ////////////////////////////////

 get_index_type:
  if (prompt) {
    cout << 
     "\nEnter index_type " <<
     "(automaton, rabin_karp, hybrid, or none):  ";

    scan_string(stream,str);
  }

  else {
    stream >> str;
    check("index_type:");
    stream >> str;
  }

  if      (0 == strcmp(str,"automaton") )  i_type = AUTOMATON;
  else if (0 == strcmp(str,"rabin_karp") ) i_type = RABIN_KARP;
  else if (0 == strcmp(str,"hybrid") )     i_type = HYBRID;
  else if (0 == strcmp(str,"none") )       i_type = NO_INDEX;

  else {
    cout << "Invalid index_type" << endl;
    if (prompt && !echo) goto get_index_type;
    exit(1);
  }

  if (r_type == IDENTITY && i_type != NO_INDEX) {
//@rn    cout << "No index used with identities!" << cout;
	 cout << "No index used with identities!" << endl;
    i_type = NO_INDEX;
  }

  //  Get external form. ///////////////////////////////

 get_external:
  if (prompt) {
    cout <<
      "\nEnter external format\n" <<
      "(names, len_names, len_numbers, or len_6bit):  ";

    scan_string(stream,str);
  }

  else {
    stream >> str;
    check("external:");
    stream >> str;
  }

  if      (0 == strcmp(str,"names") )
    ext_form = NAMES;

  else if (0 == strcmp(str,"len_names") )
    ext_form = LEN_NAMES;

  else if (0 == strcmp(str,"len_numbers") )
    ext_form = LEN_NUMBERS;

  else if (0 == strcmp(str,"len_6bit") )
    ext_form = LEN_6BIT;

  else {
    cout << "Invalid external form" << endl;
    if (prompt && !echo) goto get_external;
    exit(1);
  }

  // Is the external rule file compressed? ///////////////

 get_compressed:
  if (prompt) {
    cout << "\nIs the rule file compressed (yes or no):  ";
    scan_string(stream,str);
  }

  else {
    stream >> str;
    check("compressed:");
    stream >> str;
  }

  if (0 == strcmp(str,"yes") ) compressed = TRUE;
  else if (0 == strcmp(str,"no") ) compressed = FALSE;

  else {
    cout << "Invalid value for compressed" << endl;
    if (prompt && !echo) goto get_compressed;
    exit(1);
  }

  // Get the name of the rule file. ///////////////////

 get_rule_file:
  if (prompt) {
    cout <<
      "\nEnter name of rule file (or \"none\"):  ";

    scan_string(stream,str);
  }

  else {
    stream >> str;
    check("rule_file:");
    stream >> str;
  }

  exists = (0 == strcmp(str,"none") ||
            (int)compressed && (int)uncompress_file(str) ||
            file_exists(str) );

  if (!exists) {
    cout << "rule file does not exist." << endl;
    if (prompt && !echo) goto get_rule_file;
    exit(1);
  }

  rule_file.set_data(str);

  if (0 == strcmp(str,"none") ) {
    cout << "\nSignal end of rules with a left side of length 0."
         << endl;

    if (!input_rules(stream,"xxxxxx.tmp",
                     ext_form,FALSE,prompt) ) {
      reset_statistics();
      fstrm.open("xxxxxx.tmp");
      (void) input_rules(fstrm,NULL,LEN_NUMBERS,check,FALSE);
      fstrm.close();
    }

    remove("xxxxxx.tmp");
  }

  else {
    fstrm.open(str);

    if (!input_rules(fstrm,"",ext_form,FALSE,FALSE) ) {
      fstrm.close();
      reset_statistics();
      fstrm.open(str);
      (void) input_rules(fstrm,NULL,ext_form,check,FALSE);
      fstrm.close();
    }

    else fstrm.close();

    if (compressed) compress_file(str);
  }

  return;

 invalid_input:
  cout <<
    "Invalid input" << str << " in subsystem::input_rest" <<
    endl;
  exit(1);
}



/////////////////////////////////////////////////////////////////
//
//                 subsystem::input_rules
//
/////////////////////////////////////////////////////////////////

long subsystem::input_rules(
  istream        &stream,
  char           *file,
  external_form   extfrm,
  _boolean_t         check,
  _boolean_t         prompt
)
{
  long                      total,code;
  rule                      rul;
  rule_reference            exclude = {NULL, 0};
  _boolean_t                   reduced,changed,echo;
  enum {ENTER, SKIM, OUT}   flag;
  ofstream  fstrm;

  echo = get_echo();

  rul.r_type = r_type;
  rul.set_max_length(parent->get_bsize() );

  if      ( file == NULL)       flag = ENTER;
  else if ( 0 == strlen(file) ) flag = SKIM;

  else {
    fstrm.open(file);
    flag = OUT;
  }

  if (check && i_type != NO_INDEX) {
    parent->initialize_save_list(r_type,1000,5,5);
    if (r_type == SUBWORD_RULE) parent->save_inverses();
  }

  for (;;) {
    code =  parent->read_word(stream,extfrm,rul.left);
    if (1 == code) break;
    if (2 == code) goto input_error;

    if (prompt && echo)  {
      parent->print_word(cout,extfrm,rul.left);
      cout << " ";
    }

    if (0 == rul.left.get_length() ) {
      if (prompt && echo) cout << endl;
      break;
    }

    code = parent->read_word(stream,extfrm,rul.right);

    if (1 == code)
      error("Incomplete rule encountered on input.");

    if (2 == code) goto input_error;

    if (prompt && echo) {
      parent->print_word(cout,extfrm,rul.right);
      cout << endl;
    }

    if (flag == ENTER) {
      if (check && i_type != NO_INDEX) {
        parent->rewrite(rul,exclude,changed,reduced);
        if (parent->orient_rule(rul) ) continue;

        if (!reduced) {
          cout << "Unable to reduce an input rule" << endl;
          exit(1);
        }

        parent->balance_rule(rul);
        parent->save_rule(rul);
      }

      else add_rule(rul,FALSE,FALSE);
    }
    
    else {
      statistics(rul);

      if (flag == OUT) {
        fstrm << rul;
        fstrm << endl;
      }
    }

    continue;

   input_error:
    cout << "Input error reading rule" << endl;
    if (flag != OUT || !prompt || echo) exit(1);
    cout << "Reenter both sides of rule:  ";
  }

  if (check && i_type != NO_INDEX) {
    parent->reduce_save_list();
    parent->append_save_list(this);
  }

  return 0;
}



/////////////////////////////////////////////////////////////////
//
//                   subsystem::enter_rule
//
/////////////////////////////////////////////////////////////////

void subsystem::enter_rule(
  rule     &rul,
  long     &orig
)
{
  long   i,ll,lr;
  gen_inserter  g_ins(this);

  ll = rul.left.get_length();
  lr = rul.right.get_length();
  packed_rules.insert_ulong(orig++,(ulong)ll);
  packed_rules.insert_ulong(orig++,(ulong)lr);
  g_ins.reset(orig);
  
  for (i = 0; i < ll; i++) g_ins.insert(rul.left[i]);
  for (i = 0; i < lr; i++) g_ins.insert(rul.right[i]);
  g_ins.close(orig);
}



/////////////////////////////////////////////////////////////////
//
//                    subsystem::add_rule
//
/////////////////////////////////////////////////////////////////

void subsystem::add_rule(
  rule         &rul,
  _boolean_t       update_index,
  _boolean_t       secret
)
{
  long  orig,mxsig,lorig,lnew,pos,newfree;

  orig = free;

  if (!secret && print_level >= 3) {
    cout << "Adding rule to subsystem at " << (long)this << "."
         << "  Origin is " << orig << "." << endl;
    rul.print();
  }

  newfree = free +
    2 + jump(rul.left.get_length(),rul.right.get_length() );

  if (!secret) free = newfree;

  while ( packed_rules.get_max_index() < newfree-1 )
    packed_rules.extend();
    
  while (origin.get_max_index() < nrules) origin.extend();

  origin.insert_long(nrules,orig);

  pos = 0;
  enter_rule(rul,orig);
  if (!secret) statistics(rul);
  
  if (!update_index) return;
  
  if (NO_INDEX == i_type) return;

  if (i_type != RABIN_KARP) make_index();
  
  else {
    while (nrules-1 > signature.get_max_index() )
      signature.extend();
  
    signature.insert_long(nrules-1,compute_signature(rul.left) );

    if ((10*hash_prime-100) < 13*nrules) rehash_rabin_karp();
    rabin_karp_insert(nrules-1);    
  }
}




/////////////////////////////////////////////////////////////////
//
//                    subsystem::summary
//
/////////////////////////////////////////////////////////////////

void subsystem::summary(void)
{
  cout << "\nrule_type:  ";

  if (IDENTITY == r_type) cout << "identity,  ";
  else if (r_type == SUBWORD_RULE) cout << "subword,  ";
  else cout << "prefix,  ";

  cout << "index_type:  ";

  if      (i_type == AUTOMATON)  cout << "automaton";
  else if (i_type == RABIN_KARP) cout << "rabin-karp";
  else if (i_type == HYBRID)     cout << "hybrid";
  else if (i_type == NO_INDEX)   cout << "none";

  cout << endl;

  cout << "nrules = " << nrules << endl;
  cout << "llmin  = " << llmin  << ",  llmax = " << llmax <<
          ",  lltotal = " << lltotal << endl;
  cout << "lrmin  = " << lrmin  << ",  lrmax = " << lrmax <<
          ",  lrtotal = " << lrtotal << endl;
  cout << "\n";
}



/////////////////////////////////////////////////////////////////
//
//                subsystem::unpack
//
/////////////////////////////////////////////////////////////////

void subsystem::unpack(
  word_reference   wrd,
  vector_long     &vec,
  long             offset
)
{
  unpack(wrd.length,wrd.origin,wrd.bit_pos,vec,offset);
}



void subsystem::unpack(
  packed_rule  &rul,
  rule         &rul1
)
{
  unpack(rul.left, rul1.left, 0);
  rul1.left.set_length(rul.left.length);

  unpack(rul.right, rul1.right, 0);
  rul1.right.set_length(rul.right.length);

  rul1.r_type = r_type;
}



/////////////////////////////////////////////////////////////////
//
//                subsystem::incremental_test
//
/////////////////////////////////////////////////////////////////

void subsystem::incremental_test(
  action_type     act,
  _boolean_t         subword_only,
  vector_long    &output,
  rule_reference  exclude,
  packed_rule    &rul
)
{
  rul.left.subsyst = rul.right.subsyst = NULL;

  if (nrules <= 0) return;

  if (r_type == PREFIX_RULE && subword_only) return;

  if (act == REWRITE) {
    if (i_type == AUTOMATON)
      automaton_incremental(output,exclude,rw_aux,rul);
    else if (i_type == RABIN_KARP)
      rabin_karp_incremental(output,exclude,rw_aux,rul);
    else if (i_type == HYBRID)
      hybrid_incremental(output,exclude,rw_aux,rul);
    else if (r_type == IDENTITY)
      identity_incremental(output,exclude,rw_aux,rul);
  }

  else {
    if (i_type == AUTOMATON)
      automaton_incremental(output,exclude,ov_aux,rul);
    else if (i_type == RABIN_KARP)
      rabin_karp_incremental(output,exclude,ov_aux,rul);
    else if (i_type == HYBRID)
      hybrid_incremental(output,exclude,ov_aux,rul);
    else if (r_type == IDENTITY)
      identity_incremental(output,exclude,ov_aux,rul);
  }

  return;
}



/////////////////////////////////////////////////////////////////
//
//              subsystem::identity_incremental
//
/////////////////////////////////////////////////////////////////

void subsystem::identity_incremental(
  vector_long    &output,
  rule_reference  exclude,  // This argument is ignored!
  matrix_long    &aux,      // So is this one!
  packed_rule    &rul
)
{
  long                 len,t_gens,n_gens,orig,i,j,k,x,y,z,lx,lxa;
  long                 pos,st,ii,lls,lrs,ll,lr,maxlen,m,cmp;
  _boolean_t              group,back;
  vector_long_segment  s_vec;
  rule                 rul1,rul2;
  packed_rule          p_rule;

  rul.left.subsyst = rul.right.subsyst = NULL;
  rul1.set_max_length(ttlmax);

  len    = output.get_length();
  maxlen = output.get_max_length();
  t_gens = parent->get_t_gens();
  n_gens = parent->get_n_gens();
  group  = n_gens < t_gens;

  vector_long  start(t_gens);
  vector_long  length(t_gens);
  vector_long  count(t_gens);

  start.set_max_length(t_gens);
  start.set_length(t_gens);
  length.set_max_length(t_gens);
  length.set_length(t_gens);
  count.set_max_length(t_gens);
  count.set_length(t_gens);

  // Look at each identity in turn!

  orig = 0;

  for (i = 0; i < nrules; i++) {
    rule_by_origin(orig,p_rule);
    ll = p_rule.left.length;
    if (ll > len) continue;

    unpack(p_rule,rul1);

    for (j = 0; j < t_gens; j++) {
      start.set_entry(j,LONG_MAX);
      count.set_entry(j,0);
    }

    for (j = 0; j < ll; j++) {
      x = rul1.left[j];

      if (group) x = min(x,inverse(x) );
      count.set_entry(x,count[x]+1);
    }

    lls  = ll;
    pos  = len;
    k    = ll - 1;
    back = FALSE;

    while (k < ll) {
      if (k < 0) {
        // A match has been found with the left side.

        s_vec.segment(output,len-lls,lls);

        lr  = rul1.right.get_length();
        lrs = 0;

        for (j = 0; j < lr; j++) {
          x = rul1.right[j];

          if (LONG_MAX == start[x])
            error("Right side of identity has new generator");

          lrs += LABS(length[x]);
        }

        if (len + lrs - lls > maxlen) {
          k    = 0;
          back = TRUE;
          continue;
        }

        rul2.set_max_length(max(lls,lrs) );
        rul2.left.set_length(lls);
        rul2.right.set_length(lrs);

        for (j = 0; j < lls; j++) rul2.left.set_entry(j,s_vec[j]);
        m = 0;

        for (j = 0; j < lr; j++) {
          x   = rul1.right[j];
          st  = start[x];
          lx  = length[x];
          lxa = LABS(lx);

          if (lx > 0)
            for (ii = 0; ii < lx; ii++)
              rul2.right.set_entry(m++,output[st+ii]);

          else {
            st += lxa - 1;

            for (ii = 0; ii < lxa; ii++)
              rul2.right.set_entry(m++,inverse(output[st-ii]) );
          } 
        }

        cmp = parent->comp(rul2.left,rul2.right);

        if (1 != cmp) {
          k = 0;
          back = TRUE;
          continue;
        }

        add_rule(rul2,FALSE,TRUE);  // Rule is "secret".
        orig = free;   // First argument to rule_by_origin is
                       // modified.
        rule_by_origin(orig,rul);
        return;
      }

      // The end of rul1.left has not yet been reached.

      x  = rul1.left[k];
      st = start[x];

      if (group) {
        y = inverse(x);
        z = min(x,y);
      }

      else z = x;

      if (LONG_MAX == st) {
        // The image of x is not currently defined, so we we
        // can't be in a backtrack phase.  Define the image
        // of x to be the next generator in output.

        pos--;
        start.set_entry(x,pos);
        length.set_entry(x,1);

        if (group) {
          start.set_entry(y,pos);
          length.set_entry(y,-1);
        }

        k--;
      }

      else {
        // The image of x is currently defined.

        lx  = length[x];
        lxa = LABS(lx);
        if (!back) pos -= lxa;

        if (st == pos) {
          // The current position is the definition of x.

          if (lls + count[z] > len) {
            // We can't make the image of x any longer.

            start.set_entry(x,LONG_MAX);
            if (group) start.set_entry(y,LONG_MAX);
            k++;
            pos += lx;
            lls -= (lx-1)*count[z];
            back = TRUE;
          }

          else {
            // Increase the length of the image of x.

            pos--;
            start.set_entry(x,pos);
            length.set_entry(x,++lx);

            if (group) {
              start.set_entry(y,pos);
              length.set_entry(y,-lx);
            }

            lls += count[z];
            back = FALSE;
            k--;
          }
        }

        else {
          // This is not the defining position for x.

          if (back) {
            k++;
            pos += lxa;
            continue;
          }

          // Check whether the image of x matches here.

          if (lx > 0) {
            for (j = 0; j < lx; j++)
              if (output[pos+j] != output[st+j]) {
                back = TRUE;
                break;
              }
          }

          else {
            lx = -lx;
            st = st + lx - 1;

            for (j = 0; j < lx; j++)
              if (output[pos+j] != inverse(output[st-j]) ) {
                back = TRUE;
                break;
              }
          }

          if (back) {
            k++;
            pos += lxa;
          }

          else k--;
        }  // End this is not definition of x.
      }    // End x is defined.
    }      // End backtrack loop.
  }        // End loop over rules.
}



/////////////////////////////////////////////////////////////////
//
//              subsystem::automaton_incremental
//
/////////////////////////////////////////////////////////////////

void subsystem::automaton_incremental(
  vector_long    &output,
  rule_reference  exclude,
  matrix_long    &aux,
  packed_rule    &rul
)
{
  long         len,state,x,k,entry,orig;

  rul.left.subsyst = rul.right.subsyst = NULL;
  len   = output.get_length();

  if (len <= 0) return;
  state = aux.get_entry(len-1,0);

  if (state == LONG_MAX) {
    aux.set_entry(len,0,LONG_MAX);
    return;
  }

  x     = output[len-1];

  for (k = n_bits-1; k >= 0; k--) {
    entry = table.extract_long2(state,(x>>k)&1);

    if (entry == LONG_MAX) {
      aux.set_entry(len,0,LONG_MAX);
      return;
    }

    if (k > 0 && state < 0)
      error("Automaton error in automaton_incremental.");
      
    state = entry;
  }

  if (state >= 0)
    aux.set_entry(len,0,state);

  else {
    orig = -state - 1;
    
    if (exclude.subsyst != this || exclude.orig != orig) {
      rule_by_origin(orig,rul);
      rul.offset = len;
      aux.set_entry(len,0,LONG_MAX);
    }
  }

  return;
}



/////////////////////////////////////////////////////////////////
//
//              subsystem::rabin_karp_incremental
//
/////////////////////////////////////////////////////////////////

void subsystem::rabin_karp_incremental(
  vector_long    &output,
  rule_reference  exclude,
  matrix_long    &aux,
  packed_rule    &rul
)
{
  long         len,sig,y,k,i,ll,orig;

  rul.left.subsyst = rul.right.subsyst = NULL;
  len   = output.get_length();

  if (len <= 0) return;
  sig = aux.get_entry(len-1,0);

  if (len > window_size) {
    y = output[len-window_size-1];
    sig += zp_prod(neg_power,y,rkprime);
    if (sig >= rkprime) sig -= rkprime;
  }

  for (k = 0; k < n_bits; k++) {
    sig <<= 1;
    if (sig >= rkprime) sig -= rkprime;
  }

  sig += output[len-1];
  if (sig >= rkprime) sig -= rkprime;

  aux.set_entry(len,0,sig);
  k = zp_prod(sig,128,rkprime)%hash_prime;

  for (;;) {
    i = hash_table.extract_long(k);
    if (i == LONG_MAX) break;

    if (sig == signature.extract_long(i) ) {
      orig = origin.extract_long(i);

      if (this != exclude.subsyst ||
          orig != exclude.orig      ) {
        rule_by_origin(orig,rul);
        rul.offset = len; //  Is this right?!!!!
        ll = rul.left.length;

        if (!rul.redundant &&
            (r_type == SUBWORD_RULE && ll <= len ||
             r_type == PREFIX_RULE  && ll == len   ) &&
            rul.is_subword(output)                      ) break;

        rul.left.subsyst = rul.right.subsyst = NULL;
      }
    }

    if (++k >= hash_prime) k = 0;
  }

  return;
}



/////////////////////////////////////////////////////////////////
//
//              subsystem::hybrid_incremental
//
/////////////////////////////////////////////////////////////////

void subsystem::hybrid_incremental(
  vector_long    &output,
  rule_reference  exclude,
  matrix_long    &aux,
  packed_rule    &rul
)
{
  long                 len,state,x,k,bot,top,phase,jj,ls;
  long                 suffix,bitnum,orig,ll,old_orig;
  vector_long_segment  s_vec;

  rul.left.subsyst = rul.right.subsyst = NULL;
  len   = output.get_length();

  if (len <= 0) return;
  state = aux.get_entry(len-1,0);

  if (state == LONG_MAX) goto impossible;

  if ( state >= 0) {
    jj = len - 1;
    goto trace;
  }

  else {
    ls = -state;
    suffix = len - ls;
    bot    = aux.get_entry(len-1,1);
    top    = aux.get_entry(len-1,2);
    bitnum = (ls-1)*n_bits;
    goto search;
  }

 trace:
  phase = TRACE;
  
  for ( ; jj < len; jj++) {
    x = output[jj];

    for (k = n_bits-1; k >= 0; k--) {
      state = table.extract_long2(state,(x>>k)&1);
      if (state == LONG_MAX) goto impossible;
      bot = table.extract_long2(state,0);

      if (bot < 0) {
        bot    = -bot - 1;
        top    = -table.extract_long2(state,1) - 1;
        bitnum = state_length(state);
        suffix = jj - (bitnum-1)/n_bits;
        ls     = len - suffix;
        goto search;
      }
    }
  }

  goto finish;

 search:
  phase = SEARCH;
  
  s_vec.segment(output,suffix,ls);

  if (hybrid_search(s_vec,bot,top,bitnum) )
    error("search error in hybrid_incremental.");

  if (bot < 0) {
    phase = TRACE;

    if (r_type == PREFIX_RULE) goto impossible;

    state = initial_state;
    jj    = suffix + 1;
    goto trace;
  }

  if (bot < top) goto finish;

  orig = origin.extract_long(bot);

  old_orig = orig;
  rule_by_origin(orig,rul);
  ll       = rul.left.length;

  if (ls < ll) {
    rul.left.subsyst = rul.right.subsyst = NULL;
    goto finish;
  }

  if (!rul.redundant &&
      (r_type == SUBWORD_RULE || ll == len) &&
      (exclude.subsyst != this || exclude.orig != old_orig) )
    goto finish;

  rul.left.subsyst = rul.right.subsyst = NULL;
  if (r_type == PREFIX_RULE) goto impossible;
  jj    = suffix + 1;
  state = initial_state;
  goto trace;
  
 impossible:
  aux.set_entry(len,0,LONG_MAX);
  return;

 finish:
  if (phase == TRACE)
    aux.set_entry(len,0,state);

  else {
    aux.set_entry(len,0,-(ls+1) );
    aux.set_entry(len,1,bot);
    aux.set_entry(len,2,top);
  }

  return;
}



/////////////////////////////////////////////////////////////////
//
//                subsystem::global_test
//
/////////////////////////////////////////////////////////////////

packed_rule subsystem::global_test(
  _boolean_t         subword_only,
  vector_long    &output,
  rule_reference  exclude
)
{
  packed_rule rul;

  if (r_type == PREFIX_RULE && subword_only) return rul;

  switch(i_type) {
    case AUTOMATON:  rul = automaton_global(output,exclude);
                     break;
    case RABIN_KARP: rul = rabin_karp_global(output,exclude);
                     break;
    case HYBRID:     rul = hybrid_global(output,exclude);
                     break;
    case NO_INDEX:   if (r_type == IDENTITY)
                       rul = identity_global(output,exclude);
  }

  return rul;
}



/////////////////////////////////////////////////////////////////
//
//                subsystem::identity_global
//
/////////////////////////////////////////////////////////////////

packed_rule subsystem::identity_global(
  vector_long    &output,
  rule_reference  exclude
)
{
  packed_rule  rul;

  // Identities not currently supported in global rewriting.

  rul.left.subsyst = rul.right.subsyst = NULL;

  return rul;
}



/////////////////////////////////////////////////////////////////
//
//                subsystem::automaton_global
//
/////////////////////////////////////////////////////////////////

packed_rule subsystem::automaton_global(
  vector_long    &output,
  rule_reference  exclude
)
{
  long         len,start,state,j,x,xbit,orig,old_orig;
  long         diff,k;
  long         last_diff = LONG_MAX;
  packed_rule  rul,p_rule;

  len   = output.get_length();
  start = 0;

 loop:
  while (len - start >= llmin) {
    state = initial_state;

    for (j = start; j < len; j++) {
      x = output[j];

      for (k = n_bits-1; k >= 0; k--) {
        xbit  = (x>>k)&1;
        state = table.extract_long2(state,xbit);

        if (state == LONG_MAX) {
          // This must be a prefix system and nothing
          // found.

          return rul;
        }

        if (state < 0) {
          old_orig = orig = -state - 1;
          rule_by_origin(orig,p_rule);

          if (this     != exclude.subsyst ||
              old_orig != exclude.orig      ) {
            diff = p_rule.right.length -
                   p_rule.left.length;

            if (!p_rule.redundant && diff < last_diff) {
              rul        = p_rule;
              rul.offset = j + 1;
              last_diff  = diff;
            }
          }

          else if (r_type == PREFIX_RULE) return rul;

          start = j + 2 - p_rule.left.length;
          goto loop;
        }
      }
    }
    
    break;
  }

  return rul;
}



/////////////////////////////////////////////////////////////////
//
//                subsystem::rabin_karp_global
//
/////////////////////////////////////////////////////////////////

packed_rule subsystem::rabin_karp_global(
  vector_long    &output,
  rule_reference  exclude
)
{
  long         len,sig,j,x,y,extra,xbit,k,m,ll,diff,orig;
  long         match,old_orig;
  long         last_diff = LONG_MAX;
  packed_rule  rul,p_rule;

  len   = output.get_length();
  sig   = 0;

  for (j = 0; j < len; j++) {
    if (r_type == PREFIX_RULE && j >= llmax) return rul;
    x = output[j];

    if (j >= window_size) {
      y = output[j-window_size];
      extra = zp_prod(neg_power,y,rkprime);
      sig += extra;
      if (sig >= rkprime) sig -= rkprime;
    }

    for (k = n_bits-1; k >= 0; k--) {
      xbit = (x>>k)&1;
      sig  = sig + sig + xbit;
      if (sig >= rkprime) sig -= rkprime;
    }

    if (j < llmin-1) continue;

    k = zp_prod(sig,128,rkprime) % hash_prime;

    for (;;) {
      m = hash_table.extract_long(k);
      if (m == LONG_MAX) break;

      if (sig == signature.extract_long(m) ) {
        orig = origin.extract_long(m);
        old_orig = orig;
        rule_by_origin(orig,p_rule);
        ll       = p_rule.left.length;

        if (!p_rule.redundant &&
            (r_type == PREFIX_RULE  && ll == j+1  ||
             r_type == SUBWORD_RULE && ll <= j+1    ) ) {
          p_rule.offset = j + 1;
          match = p_rule.is_subword(output);

          if (match &&
              (this     != exclude.subsyst ||
               old_orig != exclude.orig      ) ) {
            diff = p_rule.right.length - ll;

            if (diff < last_diff) {
              rul       = p_rule;
              last_diff = diff;
            }
          }
        }
      }

      if (++k >= hash_prime) k -= hash_prime;
    }
  }

  return rul;
}



/////////////////////////////////////////////////////////////////
//
//                subsystem::hybrid_global
//
/////////////////////////////////////////////////////////////////

packed_rule subsystem::hybrid_global(
  vector_long    &output,
  rule_reference  exclude
)
{
  long                 len,start,state,j,x,k,xbit;
  long                 bot,top,bitnum;
  long                 suffix,ls,diff,orig,old_orig,ll;
  long                 last_diff = LONG_MAX;
  packed_rule          rul,p_rule;
  vector_long_segment  s_vec;

  len   = output.get_length();
  start = 0;

 loop:
  while (len - start >= llmin) {
    state = initial_state;

    for (j = start; j < len; j++) {
      x = output[j];

      for (k = n_bits-1; k >= 0; k--) {
        xbit = (x>>k)&1;
        bot  = table.extract_long2(state,0);

        if (bot >= 0) {
          state = table.extract_long2(state,xbit);

          if (state == LONG_MAX) {
            // This must be a prefix system and no left
            // side has been found.

            return rul;
          }

          else {
            bot    = -bot - 1;
            top    = -table.extract_long2(state,0) - 1;
            bitnum = state_length(state);
            suffix = j - bitnum/n_bits;
            ls     = len - suffix;
            s_vec.segment(output,suffix,ls);

            if (hybrid_search(s_vec,bot,top,bitnum) &&
                top != bot                                    )
              error("Search error in hybrid_global.");

            if (bot >= 0 && bot == top) {
              orig = origin.extract_long(bot);
              old_orig = orig;
              rule_by_origin(orig,p_rule);
              p_rule.offset = j + 1;
              ll = p_rule.left.length;

              if ( (r_type == SUBWORD_RULE && ll <= ls ||
                    r_type == PREFIX_RULE  && ll == j+1  ) &&
                   (this != exclude.subsyst ||
                    old_orig != exclude.orig      )        ) {
                diff = p_rule.right.length - ll;

                if (!p_rule.redundant && diff < last_diff) {
                  rul = p_rule;
                  last_diff = diff;
                }
              }
            }
          }

          if (r_type == PREFIX_RULE) return rul;

          start = j + 2 - ll;
          goto loop;
        }
      }
    }
  }

  return rul;
}



/////////////////////////////////////////////////////////////////
//
//                     subsystem::reduce
//
/////////////////////////////////////////////////////////////////

_boolean_t subsystem::reduce(void)
{
  long            orig,old_orig,i,nredund,ll,lr,maxllnew,num;
  long            new_orig,incr,bsize;
  _boolean_t         changed,reduced,new_found;
  rule            rul;
  packed_rule     p_rule;
  rule_reference  exclude;

  update_time();

  new_found = FALSE;

  if (print_level >= 2)
  cout << "Reducing subsystem at " << (ulong)this 
       << ", nrules = " << nrules << endl;

  if (nrules <= 0) return new_found;

  bsize = parent->get_bsize();

  if (bsize < max(llmax,lrmax) )
    error("buffer size too small in subsystem::reduce");

  exclude.subsyst = this;

  rul.set_max_length(bsize);
  nredund = 0;
  orig = 0;

  incr = max(20,nrules/50);
  initialize_dots('#',incr,50);

  for (i = 0; i < nrules; i++) {
    print_dots();
    old_orig = orig;
    rule_by_origin(orig,p_rule);

    if (p_rule.redundant)
      error("redundant rule found in subsystem::reduce");

    unpack(p_rule,rul);
    exclude.orig = old_orig;
    parent->rewrite(rul,exclude,changed,reduced);
    
    if (!changed &&
        (i_type != NO_INDEX || !reduced ||
                  !parent->check_rule(rul) ) ) continue;

    packed_rules.flag_redundant(old_orig);
    nredund++;
    if (parent->orient_rule(rul) ) continue;

    if (reduced) parent->balance_rule(rul);

    if (reduced && parent->check_rule(rul) ) {
      parent->save_rule(rul);
      new_found = TRUE;
    }

    else parent->save_broken_rule(rul);
  }

  end_dots();

  if (nredund > 0) {
    num = nrules;
    reset_statistics();
    orig = new_orig = 0;

    for (i = 0; i < num; i++) {
      old_orig = orig;
      rule_by_origin(orig,p_rule);
      if (p_rule.redundant) continue;

      statistics(p_rule);

      if (new_orig < old_orig)
        packed_rules.umove(new_orig,old_orig,orig-old_orig);
      else new_orig = orig;
    }

    free = new_orig;

    while (free-1 <= packed_rules.get_max_index() - CHUNK_SIZE)
      packed_rules.retract();

    make_index();
  }

  return new_found;
}



/////////////////////////////////////////////////////////////////
//
//                     subsystem::heuristics
//
/////////////////////////////////////////////////////////////////

_boolean_t subsystem::heuristics(void)
{
  long            orig,old_orig,i,nredund,ll,lr,maxllnew,num;
  long            new_orig,incr,bsize;
  _boolean_t         changed,reduced;
  rule            rul;
  packed_rule     p_rule;

  update_time();

  if (i_type != RABIN_KARP || next_heur >= nrules) return FALSE;

  bsize = parent->get_bsize();

  if (bsize < max(llmax,lrmax) )
    error("buffer size too small in subsystem::heuristics");

  rul.set_max_length(bsize);


  incr = max(20,nrules/50);
  initialize_dots('%',incr,50);

  if (print_level >= 2) 
  cout << "Applying heuristics to subsystem at " << (ulong)this
       << ", nrules = " << nrules << ", next_heur = "
       << next_heur << endl;

  for ( ; next_heur < nrules; next_heur++) {
    print_dots();
    rule_by_number(next_heur,p_rule);

    unpack(p_rule,rul);
    parent->heuristics(rul);
  }

  end_dots();

  return TRUE;
}



/////////////////////////////////////////////////////////////////
//
//                 subsystem::make_origin
//
/////////////////////////////////////////////////////////////////

void subsystem::make_origin(void)
{
  long  i,orig;
  packed_rule   p_rule;  // This is just a dummy argument.

  origin.clear();
  while (origin.get_max_index() < nrules-1) origin.extend();
//  origin.set_max_length( 100 + (nrules*12)/10 );
//  origin.set_length(nrules);
  orig = 0;

  for (i = 0; i < nrules; i++) {
    origin.insert_long(i,orig);
    rule_by_origin(orig,p_rule);
  }

  sort = NO_SORT;
}



/////////////////////////////////////////////////////////////////
//
//                     subsystem::make_index
//
/////////////////////////////////////////////////////////////////

void subsystem::make_index(void)
{
  // Clear any previous index.

  origin.clear();
  signature.clear();
  hash_table.clear();
  table.clear();

  if (i_type == NO_INDEX) {
    make_origin();    // Is this really necessary?
    return;
  }

  if (i_type == RABIN_KARP) make_rabin_karp(0,0);
  
  else if (i_type == AUTOMATON)
    if (r_type == SUBWORD_RULE) make_subword_automaton();
    else make_prefix_automaton();
    
  else if (i_type == HYBRID)
    if (r_type == SUBWORD_RULE) make_subword_hybrid();
    else make_prefix_hybrid();
}



/////////////////////////////////////////////////////////////////
//
//                      subsystem::append
//
/////////////////////////////////////////////////////////////////

void subsystem::append(
  subsystem   *subsyst1
)
{
  long          nrules1,orig,orig1,space,len;
  long          space1,i,old_orig1,free1;
  packed_rule   p_rule;

  if (r_type != subsyst1->r_type)
    error("different types in append.");

  nrules1    = subsyst1->nrules;
  free1   = subsyst1->get_free();
  orig = free;

  packed_rules.copy_and_free(free,&subsyst1->packed_rules,
    0,free1);

  for (i = 0; i < nrules1; i++) {
    rule_by_origin(orig,p_rule);
    statistics(p_rule);
  }

  free = orig;
  sort = NO_SORT;
}



/////////////////////////////////////////////////////////////////
//
//                 subsystem::rehash_rabin_karp
//
/////////////////////////////////////////////////////////////////

void subsystem::rehash_rabin_karp(void)
{
  long  nsig,i;

  hash_prime = next_prime(100 + 2*nrules);
  while ( hash_prime-1 > hash_table.get_max_index() )
    hash_table.extend();
  
  for (i = 0; i < hash_prime; i++)
    hash_table.insert_long(i,LONG_MAX);

  for (i = 0; i < nrules; i++) rabin_karp_insert( i );
}



/////////////////////////////////////////////////////////////////
//
//                  subsystem::rabin_karp_insert
//
/////////////////////////////////////////////////////////////////

void subsystem::rabin_karp_insert(
  long  i
)
{
  long  j;

  j = zp_prod(signature.extract_long(i),128,rkprime)%hash_prime;

  while (hash_table.extract_long(j) != LONG_MAX)
    if (hash_prime == ++j) j = 0;

  hash_table.insert_long(j,i);
}



/////////////////////////////////////////////////////////////////
//
//                 subsystem::rabin_karp_search
//
/////////////////////////////////////////////////////////////////

long subsystem::rabin_karp_search(
  vector_long  &word,
  long          sig
)
{
  long         k,i,len,orig,ll;
  packed_rule  p_rule;
  
  len = word.get_length();
  k = zp_prod(sig,128,rkprime)%hash_prime;

  while (LONG_MAX != (i = hash_table.extract_long(k) ) ) {
    if (signature.extract_long(i) == sig) {
      orig = origin.extract_long(i);
      rule_by_origin(orig,p_rule);
      ll = p_rule.left.length;

      if (!p_rule.redundant &&
          (r_type == SUBWORD_RULE && ll <= len ||
           r_type == PREFIX_RULE  && ll == len   ) ) {
        p_rule.offset = len;
        if ( p_rule.is_subword(word) ) return orig;
      }
    }

    if (++k >= hash_prime) k = 0;
  }

  return -1;
}



/////////////////////////////////////////////////////////////////
//
//                 subsystem::cummulative_test
//
/////////////////////////////////////////////////////////////////

_boolean_t subsystem::cummulative_test(
  vector_long  &word,
  rule_type     rtp
)
{
  if (i_type == RABIN_KARP)
    return rabin_karp_cummulative(word,rtp);

  else if (i_type == AUTOMATON)
    return automaton_cummulative(word,rtp);
  
  else if (i_type == HYBRID)
    return hybrid_cummulative(word,rtp);  

  error("Other cummulative tests not implemented.");
  return FALSE;
}



/////////////////////////////////////////////////////////////////
//
//                 subsystem::rabin_karp_cummulative
//
/////////////////////////////////////////////////////////////////

_boolean_t subsystem::rabin_karp_cummulative(
  vector_long  &word,
  rule_type     rtp
)
{
  cout << "rabin_karp_cummulative not yet implemented";
  return FALSE;
}



/////////////////////////////////////////////////////////////////
//
//                    subsystem::hybrid_cummulative
//
/////////////////////////////////////////////////////////////////

_boolean_t subsystem::hybrid_cummulative(
  vector_long  &word,
  rule_type     rtp
)
{
  long                 len,state,x,k,bot,top,phase,jj,ls;
  long                 suffix,bitnum,orig,ll,start;
  vector_long_segment  s_vec;
  packed_rule          rul,p_rule;

  len   = word.get_length();
  if (len <= 0) error("Empty word in hybrid_cummulative");
  
  if (rtp == PREFIX_RULE) start = 0;
  else start = 1;

  state = initial_state;  
  jj    = start;
  
 trace:
  phase = TRACE;
  
  for ( ; jj < len; jj++) {
    x = word[jj];

    for (k = n_bits-1; k >= 0; k--) {
      state = table.extract_long2(state,(x>>k)&1);
      if (state == LONG_MAX) return TRUE; // Should really abort!
      bot = table.extract_long2(state,0);

      if (bot < 0) {
        bot    = -bot - 1;
        top    = -table.extract_long2(state,1) - 1;
        bitnum = state_length(state);
        suffix = jj - (bitnum-1)/n_bits;
        ls     = len - suffix;
        goto search;
      }

    }
  }

  goto finish;

 search:
  phase = SEARCH;
  
  s_vec.segment(word,suffix,ls);

  if (hybrid_search(s_vec,bot,top,bitnum) )
    error("search error in hybrid_incremental.");

  if (bot < 0) {
    phase = TRACE;

    if (r_type == PREFIX_RULE) return TRUE;

    state = initial_state;
    jj    = suffix + 1;
    goto trace;
  }

  if (bot < top) goto finish;
  orig = origin.extract_long(bot);
  rule_by_origin(orig,p_rule);
  ll     = p_rule.left.length;
  if (ls < ll) goto finish;

  if (!p_rule.redundant &&
      (r_type == SUBWORD_RULE || ll == len) )
    return TRUE;

  jj    = suffix + 1;
  state = initial_state;
  goto trace;
  
 finish:
  if (phase == TRACE)
    ov_aux.set_entry(len,0,state);

  else {
    ov_aux.set_entry(len,0,-(ls+1) );
    ov_aux.set_entry(len,1,bot);
    ov_aux.set_entry(len,2,top);
  }

  return FALSE;
}



/////////////////////////////////////////////////////////////////
//
//               subsystem::automaton_cummulative
//
/////////////////////////////////////////////////////////////////

_boolean_t subsystem::automaton_cummulative(
  vector_long  &word,
  rule_type     rtp
)
{
  long   state,len,i,x,k,start;

  len   = word.get_length();
  
  if (rtp == PREFIX_RULE) start = 0;
  else start = 1;

  state = initial_state;

  for (i = start; i < len; i++) {
    x = word[i];

    for (k = n_bits-1; k >= 0; k--) {
      state = table.extract_long2(state,(x>>k)&1);
      if (state < 0) return TRUE;
    }
  }

  ov_aux.set_entry(len,0,state);
  return FALSE;
}



/////////////////////////////////////////////////////////////////
//
//                 subsystem::overlap_test
//
/////////////////////////////////////////////////////////////////

long subsystem::overlap_test(
  vector_long &word,
  long         len,
  packed_rule &p_rule
)
{
//  word.print();
  
  if (i_type == RABIN_KARP)
    return rabin_karp_overlap(word,len,p_rule);

  else if (i_type == AUTOMATON)
    return automaton_overlap(word,len,p_rule);
  
  else if (i_type == HYBRID)
    return hybrid_overlap(word,len,p_rule);

  error("Other parts of overlap test not implemented.");
  return 0;
}



/////////////////////////////////////////////////////////////////
//
//              subsystem::hybrid_overlap
//
/////////////////////////////////////////////////////////////////

long subsystem::hybrid_overlap(
  vector_long    &word,
  long            len,
  packed_rule    &p_rule
)
{
  long                 lw,state,x,k,bot,top,phase,jj,ls;
  long                 suffix,bitnum,orig,ll;
  long                 offset;
  vector_long_segment  s_vec;

  lw    = word.get_length();

//  state = ov_aux.get_entry(lw-1,0);

// The following is to speed up accessing entries of the matrix
// ov_aux.

  offset = lw + lw + lw;
  state = ov_aux[offset-3];

  if (state == LONG_MAX)
    error("LONG_MAX found in hybrid_overlap");

  if ( state >= 0) {
    jj = lw - 1;
    goto trace;
  }

  else {
    ls = -state;
    suffix = lw - ls;
    bot    = ov_aux[offset-2];
//    bot    = ov_aux.get_entry(lw-1,1);
    top    = ov_aux[offset-1];
//    top    = ov_aux.get_entry(lw-1,2);
    bitnum = (ls-1)*n_bits;
    goto search;
  }

 trace:
  phase = TRACE;
  
  for ( ; jj < lw; jj++) {
    x = word[jj];

    for (k = n_bits-1; k >= 0; k--) {
      state = table.extract_long2(state,(x>>k)&1);
      bot   = table.extract_long2(state,0);

      if (bot < 0) {
        bot    = -bot - 1;
        top    = -table.extract_long2(state,1) - 1;
        bitnum = state_length(state);
        suffix = jj - (bitnum-1)/n_bits;
        ls     = lw - suffix;
        goto search;
      }
    }
  }
  
  goto finish;

 search:
  phase = SEARCH;
  
  s_vec.segment(word,suffix,ls);

  if (hybrid_search(s_vec,bot,top,bitnum) )
    error("search error in hybrid_incremental.");

  if (bot < 0) {
    phase = TRACE;
    state = initial_state;
    jj    = suffix + 1;
    goto trace;
  }

  if (bot < top) goto finish;
  orig = origin.extract_long(bot);
  rule_by_origin(orig,p_rule);
  ll     = p_rule.left.length;
  if (ls < ll) goto finish;

  if (suffix < len) return 1;
  else return 2;
  
 finish:
  if (phase == TRACE) {
    ov_aux.vector_long::set_entry(offset,state);
    ls = state_length(state)/n_bits;
    suffix = lw - ls;
    if (suffix >= len) return 3;
  }

  else {
    ov_aux.vector_long::set_entry(offset,-(ls+1) );
    ov_aux.vector_long::set_entry(offset+1,bot);
    ov_aux.vector_long::set_entry(offset+2,top);
    if (suffix >= len) return 3;
  }

  return 4;
}



/////////////////////////////////////////////////////////////////
//
//                 subsystem::rabin_overlap
//
/////////////////////////////////////////////////////////////////

long subsystem::rabin_karp_overlap(
  vector_long &word,
  long         len,
  packed_rule &p_rule
)
{
  error("rabin_karp_overlap not implemented");
  return 4;
}



/////////////////////////////////////////////////////////////////
//
//                 subsystem::automaton_overlap
//
/////////////////////////////////////////////////////////////////

long subsystem::automaton_overlap(
  vector_long &word,
  long         len,
  packed_rule &p_rule
)
{
  long  lw,state,x,k,orig;

  lw    = word.get_length();
  state = ov_aux.get_entry(lw-1,0);
  x     = word[lw-1];

  for (k = n_bits-1; k >= 0; k--)
    state = table.extract_long2(state,(x>>k)&1);

  if (state >= 0) {
    ov_aux.set_entry(lw,0,state);

    if (state_length(state) <= (lw-len)*n_bits) return 3;
    return 0;
  }

  orig   = -state - 1;
  rule_by_origin(orig,p_rule);

  if (p_rule.left.length <= lw-len) return 2;
  p_rule.offset = lw;       // Is this needed?!!!
  return 1;
}



/////////////////////////////////////////////////////////////////
//
//                   subsystem::make_rabin_karp
//
/////////////////////////////////////////////////////////////////

// Create a Rabin-Karp index for a subsystem.

void subsystem::make_rabin_karp(
  long          num,    // The number of rules to allow for.  If
                        // 0, then the current number plus 20% is
                        // used.
  long          win     // if positive, the number of generators
                        // in the window (otherwise use the old
                        // value).
)
{
  long          i,orig;
  vector_long   buffer;
  packed_rule   p_rule;

  if (print_level >= 2) 
    cout << "Making Rabin-Karp index for subsystem at "
         << (long) this << endl;

  if (win <= 0) win = window_size;
  window_size = min(max(1,win),llmin);
  
  // Create signature table.

  make_origin();
  while (nrules-1 > signature.get_max_index() )
    signature.extend();

  rkprime   =
   (long) previous_prime( (long) 1<<(sizeof(long)*CHAR_BIT-3) );
   
  neg_power = 1;

  for (i = 1; i <= (window_size-1)*n_bits; i++) {
    neg_power <<= 1;
    if (neg_power >= rkprime) neg_power -= rkprime;
  }

  neg_power = rkprime - neg_power;

  // Create hash table.

  hash_prime =
    (long) next_prime( 100 + 2*nrules );

  while (hash_prime-1 > hash_table.get_max_index() )
    hash_table.extend();

  for (i = 0; i < hash_prime; i++)
    hash_table.insert_long(i,LONG_MAX);

  // Enter the rules in the index.

  if (nrules == 0) return;

  buffer.set_max_length(llmax);
  buffer.set_length(llmax);
  
  for (i = 0; i < nrules; i++) {
    orig = origin.extract_long(i);
    rule_by_origin(orig,p_rule);
    unpack(p_rule.left,buffer,0);
    buffer.set_length(p_rule.left.length);

    signature.insert_long(i,compute_signature(buffer) );
    rabin_karp_insert(i);
  }

  return;
}



/////////////////////////////////////////////////////////////////
//
//                     subsystem::compute_signature
//
/////////////////////////////////////////////////////////////////

long subsystem::compute_signature(
  vector_long  &word
)
{
  long  sig,j,k,len;

  len   = word.get_length();  
  sig   = 0;

  for (j = len-window_size; j < len; j++) {
    for (k = 0; k < n_bits; k++) {
      sig <<= 1;
      if (sig >= rkprime) sig -= rkprime;
    }

    sig += word[j];
    if (sig >= rkprime) sig -= rkprime;
  }

  return sig; 
}



/////////////////////////////////////////////////////////////////
//
//                    subsystem::state_length
//
/////////////////////////////////////////////////////////////////

long subsystem::state_length(
  long state
)
{
  long  len;

  if (state <= initial_state) return 0;

  len = 0;
  while ( state >= len_start.safe_get_entry(len+1) ) len++;
  return len;
}



/////////////////////////////////////////////////////////////////
//
//               subsystem::make_prefix_automaton
//
/////////////////////////////////////////////////////////////////

// Create a prefix automaton index for a subsystem.  Such
// an index is used only for prefix rules.

void subsystem::make_prefix_automaton(void)
{
  long    i,orig;
  
  if (print_level >= 2) 
    cout << "Making prefix automaton index for subsystem at "
         << (long) this << endl;

  if (r_type != PREFIX_RULE)
    error("Prefix automaton requested for subword rules.");

  if (0 >= nrules) return;

  sort_rules(LEX);    
  prefix_data(1);   // Proper prefixes.

  //  an entry of LONG_MAX is undefined.  Any other nonnegative
  //  entry is a state and a negative entry has the form
  //  -(orig+1), where orig is the origin of a rule.

  set_len_start(LONG_MAX);

  while (nstates-1 > table.get_max_index2() ) table.extend();

  initial_state = 0;
  
  for (i = 0; i < 2; i++)
    table.insert_long2(initial_state,i,LONG_MAX);
    
  orig = 0;

  for (i = 0; i < nrules; i++) prefix_insert(orig);
  
  reset_len_start();
  
  return;
}



/////////////////////////////////////////////////////////////////
//
//                subsystem::make_prefix_hybrid
//
/////////////////////////////////////////////////////////////////

// Create a hybrid index for a prefix subsystem.

void subsystem::make_prefix_hybrid(void)
{
  long   max_states,x,y;
    
  if (print_level >= 2)
    cout << "Making prefix hybrid index for subsystem at "
         << (long)this << endl;

  if (nrules <= 0) return;
  
  if (r_type != PREFIX_RULE)
    error("prefix hybrid requested for subword rules.");
    
  sort_rules(LEX);
  prefix_data(0);                 // All prefixes.

  //  Permit additional states corresponding to prefixes of
  //  increasing lengths until the space for the transition table
  //  slightly exceeds hybrid_factor times the space for the
  //  rules.

  x = len_start[0] + len_start[1];
  y = (long) (100 + (hybrid_factor*free)/(2*sizeof(long) ) );
  max_states = max(x,y);
  
  set_len_start(max_states);
  while (nstates-1 > table.get_max_index2() ) table.extend();
  initial_state = 0;
  hybrid_first_pass();

  return;
}



/////////////////////////////////////////////////////////////////
//
//               subsystem::make_subword_automaton
//
/////////////////////////////////////////////////////////////////

// Make a subword automaton index for a subsystem.

void subsystem::make_subword_automaton(void)
{
  long           i,orig,state,s,ltail,llbitmax;
  long           e0,e1,ls,t,j,bit;
  packed_rule    p_rule;
  bit_extractor  b_ext(this);
  
  if (print_level >= 2)
    cout << "Making subword automaton index for subsystem at "
         << (long)this << endl;

  if (r_type != SUBWORD_RULE)
    error("subword automaton requested for prefix subsystem");

  prefix_data(1);    // Proper prefixes.

  /*
    an entry of LONG_MAX is undefined.  Any other nonnegative
    entry is a state and a negative entry has the form
    -(orig+1), where orig is the origin of a rule.  The initial
    state is n_bits-1.  There are n_bits-1 "dummy states" which
    are needed to synchronize the states, which are defined by
    bit strings with strings of generators.  For 0 <= i <=
    n_bits-2, state i represents any bit string of length i+1
    which is not a prefix of a bitstring defining a state.  The
    "normal" states continue n_bits, n_bits+1, ....
  */
  
  llbitmax = llmax*n_bits;

  len_start.set_entry(0,n_bits);
  set_len_start(LONG_MAX);

  initial_state = n_bits - 1;
  while (nstates-1 > table.get_max_index2() ) table.extend();
  
  for (i = 0; i < initial_state; i++)
    for (j = 0; j < 2; j++)
      table.insert_long2(i,j,i+1);

  for (j = 0; j < 2; j++) 
    table.insert_long2(initial_state,j,LONG_MAX);
    
  orig = 0;
  for (i = 0; i < nrules; i++) prefix_insert(orig);

  reset_len_start();

//  for (i = 0; i < llbitmax+1; i++)
//    cout << i << " " << len_start[i] << "  ";

//  cout << endl << endl;

  // Compute the remaining transitions.

  for (i = 0; i < 2; i++)
    if ( table.extract_long2(initial_state,i) == LONG_MAX)
      table.insert_long2(initial_state,i,0);


  for (state = initial_state+1; state < nstates; state++) {
    for (i = 0; i < 2; i++)
      if (table.extract_long2(state,i) == LONG_MAX) {
        // Find a rule whose left side passes through state.

        s = state;
        ltail = 0;

        while (s >= 0) {
          ltail++;

          if (ltail > llbitmax) {
            error("ltail too long in make_subword_automaton");
          }

          e0 = table.extract_long2(s,0);
          e1 = table.extract_long2(s,1);

          if (e0 == LONG_MAX && e1 == LONG_MAX) {
            error(
              "Invalid table entries in make_subword_automaton");
          }

          if (e0 < 0) s = e0;
          else if (e1 < 0) s = e1;
          else if (e0 != LONG_MAX) s = e0;
          else s = e1;
        }

        orig = -s-1;

        // orig is the origin of a rule for which the bit
        // prefix of length ls of the left side defines
        // state.

        rule_by_origin(orig,p_rule);
        ls     = p_rule.left.length*n_bits - ltail;

        if (ls < n_bits) table.insert_long2(state,i,ls);

        else {
          t = initial_state; 
           
          b_ext.reset(p_rule.left.origin,n_bits);

          for (j = n_bits; j < ls; j++) {
            bit = b_ext.next();
            t   = table.extract_long2(t,bit);
            if (t < 0)
              error(
              "Rules not reduced in make_subword_automaton.");
          }
            
          table.insert_long2(state,i,table.extract_long2(t,i) );
        }
      }
  }

  return;
}



/////////////////////////////////////////////////////////////////
//
//                  subsystem::make_subword_hybrid
//
/////////////////////////////////////////////////////////////////

// Create a hybrid index for a subword subsystem.

void subsystem::make_subword_hybrid(void)
{
  long     x,y,max_states,i,j,maxlen,s,t,k;
  _boolean_t  back;
    
  if (print_level >= 2)
    cout << "Making subword hybrid index for subsystem at "
         << (long)this << endl;

  if (nrules <= 0) return;
  
  if (r_type != SUBWORD_RULE)
    error("subword hybrid index requested for prefix subsystem");

  sort_rules(LEX);
  prefix_data(0);    // All prefixes.

  /*
     Allow initially for n_bits-1 "dummy" states and the initial
     state, which has number n_bits-1.  Permit additional states
     corresponding to prefixes of increasing lengths until the
     space for the transition table slightly exceeds
     hybrid_factor times the space for the rules.
  */

  len_start.set_entry(0,n_bits);
  
  x = len_start[0] + len_start[1];
  y = (long) (100 + (hybrid_factor*free)/(2*sizeof(long) ) );
  max_states = max(x,y);

  set_len_start(max_states);
  while (nstates-1 > table.get_max_index2() ) table.extend();
  initial_state = n_bits - 1;

  for (i = 0; i < initial_state; i++)
    for (j = 0; j < 2; j++) table.insert_long2(i,j,i+1);
    
  hybrid_first_pass();

  // Fill in the remaining entries of the transition table.

  maxlen = llmax*n_bits;
  matrix_long  buff(maxlen+1,3);

  buff.set_entry(0,0,LONG_MAX);
  buff.set_entry(0,1,initial_state);

  for (i = 1; i <= n_bits; i++) buff.set_entry(i,2,i-1);

  for (i = 0; i < 2; i++)
    if ( table.extract_long2(initial_state,i) == LONG_MAX)
      table.insert_long2(initial_state,i,0);

  for (i = 1; i <= maxlen; i++) {
    j = 1;
    buff.set_entry(1,0,0);

    for (;;) {
      x = buff.get_entry(j,0);
      s = buff.get_entry(j-1,1);
      t = table.extract_long2(s,x);

      if (t > s) {
        buff.set_entry(j,1,t);
        if (j > n_bits)
          buff.set_entry(j,2,
               table.extract_long2(buff.get_entry(j-1,2) ,x) );
               
        if (j == i) back = TRUE;
        else back = FALSE;

        if (back) 
          for (k = 0; k < 2; k++)
            if (table.extract_long2(t,k) == LONG_MAX)
              table.insert_long2(t,k,
                 table.extract_long2(buff.get_entry(j,2),k) );
      }

      else back = TRUE;

      if (back) {
        while (j > 0 && buff.get_entry(j,0) == 1) j--;
        if (j == 0) break;
        buff.set_entry(j,0,1);
      }

      else buff.set_entry(++j,0,0);
    }
  }

  return;
}



/////////////////////////////////////////////////////////////////
//
//                 subsystem::prefix_data
//
/////////////////////////////////////////////////////////////////

// Obtain data about the number of left sides of each length and
// the number of prefixes of left sides of each length.

void subsystem::prefix_data(
  long  flag           // flag of 1 means proper prefixes
                       // flag of 0 means all prefixes
)
{
  long         bitlenmax,lmin,i,j,new_buff,k;
  vector_char  buff[2];
  packed_rule  p_rule;
  
  if (sort != LEX) sort_rules(LEX);
 
  bitlenmax = max(0,n_bits*llmax - flag);
  
  len_start.set_max_length(bitlenmax+2);
  len_start.set_length(bitlenmax+2);

  buff[0].set_max_length(bitlenmax+1);
  buff[1].set_max_length(bitlenmax+1);  

  for (i = 0; i <= bitlenmax+1; i++) len_start.set_entry(i,0);

  if (0 >= nrules) return;

  rule_by_number(0,p_rule);
  left_bit_list(p_rule,buff[0]);
  new_buff = 1;

  for (j = 0; j <= buff[0].get_length()-flag; j++)
    len_start.set_entry(j,1);

  for (i = 1; i < nrules; i++) {
    rule_by_number(i,p_rule);
    left_bit_list(p_rule,buff[new_buff]);
    
    lmin = min( buff[0].get_length(), buff[1].get_length() );

    for (j = 0; j < lmin; j++)
       if (buff[0][j] != buff[1][j]) break;
    
    if (j >= lmin) {
      cout << buff[0] << endl;
      cout << buff[1] << endl;
      error("Rules not prefixed reduced in prefix_data");
    }

    for (k = j+1; k <= buff[new_buff].get_length()-flag; k++)
      len_start.set_entry(k,len_start[k]+1);
      
    new_buff = 1 - new_buff;
  }

  return;
}



/////////////////////////////////////////////////////////////////
//
//                 subsystem::prefix_insert
//
/////////////////////////////////////////////////////////////////

// Make entries in an automaton index for a rule which has
// already been stored in a subsystem.  Return the origin of the
// next rule.

void subsystem::prefix_insert(
  long          &orig
)
{
  long           state,bit,bitlen,i,j,s,old_orig;
  packed_rule    p_rule;
  bit_extractor  b_ext(this);

  old_orig = orig;  
  rule_by_origin(orig,p_rule);
  b_ext.reset(p_rule.left.origin,0);

  state = initial_state;
  bitlen = p_rule.left.length*n_bits;

  for (i = 0; i < bitlen; i++) {
    bit = b_ext.next();
    s   = table.extract_long2(state,bit);
    if (s < 0) goto not_reduced;

    if (i < bitlen-1) {
      if (s == LONG_MAX) {
        s = len_start[i+1];
        len_start.set_entry(i+1,s+1);
        table.insert_long2(state,bit,s);
        
        for (j = 0; j < 2; j++)
          table.insert_long2(s,j,LONG_MAX);
      }

      state = s;
    }
  }

  if (s != LONG_MAX) goto not_reduced;
  table.insert_long2(state,bit,-(old_orig+1) );

  return;

 not_reduced:
  error("Rules not prefixed reduced in prefix_insert");
}



/////////////////////////////////////////////////////////////////
//
//                    subsystem::binary_search
//
/////////////////////////////////////////////////////////////////

/*
   Given a range of rules in a subsystem which are sorted
   lexicographically and whose left sides agree in the first
   bitnum-1 bits, split the range according to the bitnum-th bit.
   The rules are assumed to be left-reduced.  In particular, no
   left side is a prefix of other left side.  If the bitnum-th
   bit of the first rule does not exist, then 1 is returned.
   Otherwise, 0 is returned.
*/

long subsystem::binary_search(
  long         bot,
  long         top,
  long         bitnum,
  long        &bot0,
  long        &top0,
  long        &bot1,
  long        &top1
)
{
  long           top_orig,bot_orig,mid_orig;
  long           ltop,lbot,lmid;
  long           topbit,botbit,midbit,low,high,mid;
  long           offset,shift;
  ulong          ent;

  if (bot < 0 || top >= nrules) {
    error("Argument error in binary_search");
  }
  
  offset = 1 + bitnum/_LONG_BIT;
  shift  = _LONG_BIT*offset - bitnum - 1;

  top_orig = origin.extract_long(top);
  bot_orig = origin.extract_long(bot);

  ent  = packed_rules.extract_ulong(bot_orig++);
  lbot = (ent<<1)>>1;
  ent  = packed_rules.extract_ulong(top_orig++);
  ltop = (ent<<1)>>1;

  if (bitnum >= lbot*n_bits) return 1;
  
  botbit =
    1&packed_rules.extract_ulong(bot_orig + offset)>>shift;

  topbit =
    1&packed_rules.extract_ulong(top_orig + offset)>>shift;

  if (botbit == topbit)
    if (botbit == 0) {
      bot0 = bot;
      top0 = top;
      bot1 = -1;
    }

    else {
      bot0 = -1;
      bot1 = bot;
      top1 = top;
    }

  // A binary search must be done.

  else {
    low  = bot;
    high = top;

    while (high-low > 1) {
      mid = (low+high)/2;

      mid_orig = origin.extract_long(mid);
      ent  = packed_rules.extract_ulong(mid_orig++);
      lmid = (ent<<1)>>1;

      if (lmid*n_bits <= bitnum) {
        printf("length error in binary_search\n");
        exit(1);
      }
      
      midbit =
        1&packed_rules.extract_ulong(mid_orig + offset)>>shift;

      if (midbit == 0) low = mid;
      else high = mid;;
    }

    bot0 = bot;
    top0 = low;
    bot1 = high;
    top1 = top;
  }

  return 0;
}



/////////////////////////////////////////////////////////////////
//
//               subsystem::hybrid_search
//
/////////////////////////////////////////////////////////////////

long subsystem::hybrid_search(
  vector_long  &word,
  long         &bot,
  long         &top,
  long         start_bit
)
{
  long   len,j,k,bitnum,x,xbit,bot0,top0,bot1,top1;

  len   = word.get_length();
  j     = start_bit/n_bits;
  k     = n_bits - 1 - start_bit%n_bits;
  bitnum = start_bit;

  while (j < len) {
    x = word[j];

    while (k >= 0) {
      xbit = (x>>k)&1;

      if (binary_search(bot,top,bitnum,bot0,top0,bot1,top1) ) {
        top = bot;
        return 1;
      }

      if (xbit == 0) {
        bot = bot0;
        top = top0;
        if (bot0 < 0) break;
      }

      else {
        bot = bot1;
        top = top1;
        if (bot1 < 0) break;
      }

      k--;
      bitnum++;
    }

    if (bot < 0) return 0;
    
    j++;
    k = n_bits - 1;
  }

  return 0;
}



/////////////////////////////////////////////////////////////////
//
//                    subsystem::left_bit_list
//
/////////////////////////////////////////////////////////////////

void subsystem::left_bit_list(
  packed_rule   p_rule,
  vector_char  &list
)
{
  long           lb,i;
  bit_extractor  b_ext(this);
  
  lb = n_bits*p_rule.left.length;
  list.set_max_length(lb+1);
  list.set_length(lb);
  b_ext.reset(p_rule.left.origin,0);
  
  for (i = 0; i < lb; i++)
    if (b_ext.next() == 0) list.set_entry(i,'0');
    else list.set_entry(i,'1');
  
  list.set_entry(lb,'\0');
  
  return;
}



/////////////////////////////////////////////////////////////////
//
//                    subsystem::set_len_start
//
/////////////////////////////////////////////////////////////////

// After prefix_data has been run and len_start[0] adjusted for
// the correct number of initial and dummy states,
// make len_start[i] the first state of length i.  Return the
// total number of states.

void subsystem::set_len_start(
  long  max     // A crude upper bound for the number of states.
)
{
  long    i,max_prefix,num;
  
  nstates = 0;
  max_prefix = len_start.get_length() - 2;
  
  for (i = 0; i <= max_prefix+1; i++) {
    num = len_start[i];
    len_start.set_entry(i,nstates);
    nstates += num;
    
    if (nstates > max) {
      len_start.set_length(i+2);
      len_start.set_entry(i+1,nstates);
      break;
    }
  }
}



/////////////////////////////////////////////////////////////////
//
//                    subsystem::reset_len_start
//
/////////////////////////////////////////////////////////////////

// After the left sides have been entered into the automaton,
// reset len_start so that len_start[i] is the first start of
// length  i.

void subsystem::reset_len_start(void)
{
  long   max_prefix,i;
  
  max_prefix = len_start.get_length() - 1;
  
  // len_start[0] has not changed.
  
  for (i = max_prefix; i > 1; i--)
    len_start.set_entry(i,len_start[i-1]);
    
  len_start.set_entry(1,initial_state+1);
}



/////////////////////////////////////////////////////////////////
//
//                 subsystem::hybrid_first_pass
//
/////////////////////////////////////////////////////////////////

void subsystem::hybrid_first_pass(void)
{
  long   next_state,state,len;
  long   bot,top,bot0,top0,bot1,top1;
  
  table.insert_long2(initial_state,0,-1);
  table.insert_long2(initial_state,1,-nrules);

  next_state  = initial_state + 1;
  len         = 0;

  for (state = initial_state; next_state < nstates; state++) {
    if (state >= len_start[len+1]) len++;
    bot = -table.extract_long2(state,0) - 1;
    top = -table.extract_long2(state,1) - 1;

    if ( binary_search(bot,top,len,bot0,top0,bot1,top1) )
      if (bot < top) error("search error in hybrid_first_pass");
      else continue;
    
    else {
      
      if (bot0 >= 0) {
        table.insert_long2(state,0,next_state);
        table.insert_long2(next_state,0,-(bot0+1) );
        table.insert_long2(next_state,1,-(top0+1) );
        next_state++;
      }
 
      else table.insert_long2(state,0,LONG_MAX);

      if (bot1 >= 0) {
        table.insert_long2(state,1,next_state);
        table.insert_long2(next_state,0,-(bot1+1) );
        table.insert_long2(next_state,1,-(top1+1) );
        next_state++;
      }

      else table.insert_long2(state,1,LONG_MAX);
    }
  }

  return;
}



/////////////////////////////////////////////////////////////////
//
//                     subsystem::sort_rules
//
/////////////////////////////////////////////////////////////////

// Sort the rules in a subsystem.  The pointers in
// subsyst->rules.origin are sorted.

void subsystem::sort_rules(
  sort_type      stp   // one of LEX, LEN, LENLENLEXLEX, or
                       // LENLEXLENLEX
)
{
  long     h,i,vorig,j,worig,uorig,old_vorig,old_worig;
  packed_rule  u_rule,v_rule,w_rule;

  if (stp != LEN    &&
      stp != LEX    &&
      stp != LENLENLEXLEX &&
      stp != LENLEXLENLEX    )
    error("Invalid sort_type in sort_rules");

  sort = stp;  

  make_origin();
  if (nrules <= 1) return;

  // perform an indirect shellsort

  for (h = 1; h < nrules; h = 3*h+1);

  do {
    h = h/3;

    for (i = h; i < nrules; i++) {
      old_vorig = vorig = origin.extract_long(i);
      rule_by_origin(vorig,v_rule);  // vorig is updated!
      j     = i;

      while (j >= h) {
        old_worig = worig = origin.extract_long(j-h);
        rule_by_origin(worig,w_rule);

        if ( 0 <= parent->
            compare_packed_rules(stp,v_rule,w_rule) ) break;

        origin.insert_long(j,old_worig);
        j -= h;
      }

      origin.insert_long(j,old_vorig);
    }
  }
  while (h > 1);

  return;
}



/////////////////////////////////////////////////////////////////
//
//                subsystem::check_aux
//
/////////////////////////////////////////////////////////////////

void subsystem::check_aux(
  action_type   act,
  _boolean_t    subword_only,
  long          len
)
{
  if (r_type == PREFIX_RULE && subword_only) return;

  switch (act) {
    case REWRITE:  rw_aux.set_max_rows(len+1);
                   rw_aux.set_num_rows(len+1);
    
                   if (i_type == AUTOMATON || i_type == HYBRID)
                     rw_aux.set_entry(0,0,initial_state);
                   else if (i_type == RABIN_KARP)
                     rw_aux.set_entry(0,0,0);
                     
                   break;

    case OVERLAP:  ov_aux.set_max_rows(len+1);
                   break;

    case BACKTRACK: ov_aux.set_max_rows(len+1);
                   ov_aux.set_num_rows(len+1);
    
                   if (i_type == AUTOMATON || i_type == HYBRID)
                     ov_aux.set_entry(0,0,initial_state);
                   else if (i_type == RABIN_KARP)
                     ov_aux.set_entry(0,0,0);
                     
                   break;
  }
}



/////////////////////////////////////////////////////////////////
//
//                 subsystem::write
//
/////////////////////////////////////////////////////////////////

void subsystem::write(
  fstream   &stream,
  char      *file_name,
  long       num
)
{
  long         orig,i;
  char         file[50];
  char         numc[2];
  char         response[50];
  packed_rule  p_rule;
  rule         rul;
  external_form  extfrm;
  fstream      stream1,stream2;
  
  if (strlen(file_name) > 0) {
    if (strlen(file_name) > 45)
      error("Name too long in subsystem::write");
    strcpy(file,file_name);
    strcat(file,".sb");
    numc[0] = '0' + num;
    numc[1] = '\0';
    strcat(file,numc);
    stream1.open(file, ios::out);
    stream << file << endl;;
  }
  
  else {
    subsystem_file.open(stream1);
    stream << subsystem_file << endl;
  }
  
  stream1 << "subsystem" << endl;
  stream1 << "rule_type:  ";
  
  if (r_type == PREFIX_RULE) stream1 << "prefix" << endl;
  else if (r_type == SUBWORD_RULE) stream1 << "subword" << endl;
  // else stream1 << "identity" << endl;
  
  stream1 << "index_type:  ";
  
  switch (i_type) {
    case AUTOMATON:   stream1 << "automaton" << endl;
                      break;
                     
    case RABIN_KARP:  stream1 << "rabin_karp" << endl;
                      break;
                      
    case HYBRID:      stream1 << "hybrid" << endl;
                      break;
                      
    case NO_INDEX:    stream1 << "none" << endl;
                      break;
  }
  
  stream1 << "external:  ";
  
  cout << "Writing subsystem " << num << endl;
  cout << "The current external format is ";

  switch (ext_form) {
    case NAMES:        cout << "names" << endl;
                       break;

    case LEN_NAMES:    cout << "len_names" << endl;
                       break;

    case LEN_NUMBERS:  cout << "len_numbers" << endl;
                       break;

    case LEN_6BIT:     cout << "len_6bit" << endl;
                       break;
  }

 repeat:
  cout << "Enter desired format:  ";
  cin  >> response;

  if (get_echo() ) cout << response << endl;

  if ( !cin.ignore(10000,'\n') ) {
    cout << "Error clearing command line" << endl;
    return;
  }


  if      ( 0 == strcmp(response,"names") )
    extfrm = NAMES;
  else if ( 0 == strcmp(response,"len_names") )
    extfrm = LEN_NAMES;
  else if ( 0 == strcmp(response,"len_numbers") )
    extfrm = LEN_NUMBERS;
  else if ( 0 == strcmp(response,"len_6bit") )
    extfrm = LEN_6BIT;

  else {
    cout << "Unknown format" << endl;
    goto repeat;
  }

  switch (extfrm) {
    case NAMES:  stream1 << "names" << endl;
                 break;
                 
    case LEN_NAMES:  stream1 << "len_names" << endl;
                     break;
                     
    case LEN_NUMBERS:  stream1 << "len_numbers" << endl;
                       break;
                       
    case LEN_6BIT:     stream1 << "len_6bit" << endl;
  }
  
  stream1 << "compressed:  ";
  
  if (compressed == TRUE) stream1 << "yes" << endl;
  else stream1 << "no" << endl;
  
  stream1 << "rule_file:  ";

  if (strlen(file_name) > 0) {
    if (strlen(file_name) > 45)
      error("Name too long in subsystem::write");
    strcpy(file,file_name);
    strcat(file,".rl");
    numc[0] = '0' + num;
    numc[1] = '\0';
    strcat(file,numc);
    stream2.open(file, ios::out);
    stream1 << file << endl;
  }
  
  else {
    subsystem_file.open(stream2);
    stream1 << subsystem_file << endl;
  }
  
  stream1.close();
  orig = 0;
  
  for (i = 0; i < nrules; i++) {
//  rule_by_origin(orig,p_rule);
    rule_by_number(i,p_rule);
    unpack(p_rule,rul);
    parent->print_word(stream2,extfrm,rul.left);
    stream2 << " ";
    parent->print_word(stream2,extfrm,rul.right);
    stream2 << endl;
  }
  
  stream2.close();
}



/////////////////////////////////////////////////////////////////
//
//                   subsystem::unpack
//
/////////////////////////////////////////////////////////////////

void subsystem::unpack(
  long          length,
  long          origin,
  long          bit_pos,
  vector_long  &wrd,
  long          offset
)
{
  long        i,zbits,wbits,bits_to_put,diff;
  ulong       z,w;
  void huge  *dat;

  if (length <= 0) return;

  if (offset+length > wrd.get_max_length() )
    wrd.set_max_length(offset+length);
    
  z = 0;
  zbits = 0;
  w = packed_rules.extract_ulong(origin++);
  wbits = _LONG_BIT - bit_pos;
  bits_to_put = length*n_bits;

  while (bits_to_put > 0) {
    while (zbits < n_bits) {
      if (wbits == 0) {
        w = packed_rules.extract_ulong(origin++);
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

    wrd.set_entry(offset++,z);
    zbits = z = 0;
    bits_to_put -= n_bits;
  }
}



/////////////////////////////////////////////////////////////////
//
//                     subsystem::details
//
/////////////////////////////////////////////////////////////////

void subsystem::details()
{
  long           i,j,ll,orig;
  long          *count;
  packed_rule    p_rule;

  if (nrules <= 0) {
    cout << "There are no rules to detail." << endl;
    return;
  }

  count = (long *) fxmllc( (llmax+1)*sizeof(long) );

  if (NULL == count) {
    cout << "Unable to allocate space for details." << endl;
    return;
  }

  for (i = 0; i <= llmax; i++) count[i] = 0;

  orig = 0;

  for (i = 0; i < nrules; i++) {
    rule_by_origin(orig,p_rule);
    ll = p_rule.left.length;
    count[ll]++;
  }

  j = 0;

  for (i = 1; i <= llmax; i++)
    if (count[i] > 0) {
      cout << setw(4) << i << setw(8) << count[i];

      if (4 == ++j) {
        cout << endl;
        j = 0;
      }

      else cout << " | ";
    }

  fxfree( (void*) count);
}
