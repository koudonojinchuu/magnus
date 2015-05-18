// system.C


#include "rkbp.h"

extern  double  hybrid_factor;
extern  ulong   mask[33];
extern  long    pack_size[12];
extern  long    print_level;


// The following macro is used in the input functions to check
// that a string str read in agrees with the string  s.

#define check(s) if ( 0 != strcmp(str,s) ) goto invalid_input


vector_long   balance_vector(0);
vector_long   heuristic_vector(0);
rule          heuristic_rule;


// The following hides a feature being developed for version
// 1.30

#define  IDENTITY   17




/////////////////////////////////////////////////////////////////
//
//             rewriting_system::rewriting_system
//
/////////////////////////////////////////////////////////////////

rewriting_system::rewriting_system(void)
: autn(50),  // '50' is arbitrary!
  system_file()
{
  a_type      = MONOID;
  o_type      = LNLX;
  use_heuristics = TRUE;
  n_gens      = 0;
  t_gens      = 1;
  n_bits      = 0;
  gens        = NULL;
  n_sub       = 0;
  rw_strategy = FROM_LEFT;
  rewrite_verbose = FALSE;
  pRewrite    = &rewriting_system::rewrite_from_left;
  kb_strategy = TWO_STEP;
  pKnuth_Bendix = &rewriting_system::kb_two_step;
  compare     = &rewriting_system::lnlx;
  savell      = LONG_MAX;
  savelr      = LONG_MAX;
  bsize       = 100;
  logll       = 0;
  n_rewrite   = 0;
  total_input = 0;
  n_applications = 0;
  total_left     = 0;
  noverlaps      = 0;
  nbalance       = 0;
  nsave          = 0;

  first_subsystem = NULL;
  last_subsystem  = NULL;
  first_save      = NULL;
  last_save       = NULL;
  broken_subword_rules = NULL;
  broken_prefix_rules  = NULL;
}



/////////////////////////////////////////////////////////////////
//
//           rewriting_system::~rewriting_system
//
/////////////////////////////////////////////////////////////////

rewriting_system::~rewriting_system(void)
{
  subsystem  *subsyst,*next;

  delete[] gens;

  subsyst = first_subsystem;

  while (subsyst != NULL) {
    next = subsyst->get_next_subsystem();
    delete subsyst;
    subsyst = next;
  }

  if (broken_prefix_rules != NULL)
    delete broken_prefix_rules;

  if (broken_subword_rules != NULL)
    delete broken_subword_rules;
}



/////////////////////////////////////////////////////////////////
//
//               rewriting_system::new_subsystem
//
/////////////////////////////////////////////////////////////////

subsystem *rewriting_system::new_subsystem(
  char  *str
)
{
  subsystem   *subsyst;
  
  subsyst = new subsystem;

  if (str != NULL) subsyst->set_subsystem_file(str);
  subsyst->set_parent(this);

  return subsyst;
}



/////////////////////////////////////////////////////////////////
//
//                  rewriting_system::input
//
/////////////////////////////////////////////////////////////////

// The rewriting system is assumed to be freshly initialized by
// the default constructor.

void rewriting_system::input(
  char   *file_name,
  _boolean_t  check
)
{
  long      exists,lname;
  char      file[100];
  _boolean_t   echo;
  ifstream  stream;

  echo = get_echo();

  lname = strlen(file_name);
  
  if (lname > 90) {
    cout << "Name too long in rewriting_system::input" << endl;
    return;
  }
    
  strcpy(file,file_name);
  strcat(file,".sys");
  exists = (lname > 0) &&
           (0 == strcmp(file_name,"none") || file_exists(file) );
  
  if (!exists && lname > 0) {
    cout << "file " << file << " does not exist." << endl;
    if (echo) exit(1);
  }
  
  while (!exists) {
    cout <<
    "\nEnter problem name (or \"none\" or \"stop\"):  ";
    scan_string(cin,file);

    if (0 == strcmp(file,"stop") ) return;
    if (0 == strcmp(file,"none") ) break;
    strcat(file,".sys");

    exists = file_exists(file);

    if (!exists) {
      cout << "file does not exist." << endl;
      if (echo) exit(1);
    }
  }

  if ( 0 == strcmp(file,"none") ) {
    system_file.set_data("");
    input(cin,TRUE,check);
  }

  else {
    system_file.set_data(file);
    stream.open(file, ios::in);
    input(stream,FALSE,check);
  }
}



void rewriting_system::input(
  istream  &stream,
  _boolean_t   prompt,
  _boolean_t   check
)
{
  long                  i,j,k,ninv,nsub,m;
  char                  first_gen;
  char                  str[500],str2[500],pfx[10];
  _boolean_t               echo;
  vector_char           vec;
  vector_long           wrd;
  subsystem            *subsyst;
  naming_convention     name_conv;
  numbering_convention  numb_conv;
  inverse_convention    inv_conv;
  convention            wt_conv;
  convention            lev_conv;

  echo = get_echo();

 get_word_type:
  if (prompt) {
    cout << "\nEnter word_type (monoid or group):  ";
    scan_string(stream,str);
  }

  else {
    stream >> str;
    check("system");
    stream >> str;
    check("word_type:");
    stream >> str;
  }

  if      ( 0 == strcmp(str,"monoid") ) a_type = MONOID;
  else if ( 0 == strcmp(str,"group") )  a_type = GROUP;

  else {
    cout << "Invalid word_type" << endl;
    if (prompt && !echo) goto get_word_type;
    exit(1);
  }

  if (prompt) {
    cout << "\nEnter ngens:  ";
    scan_long(stream,n_gens);
  }

  else {
    stream >> str;
    check("ngens:");
    stream >> n_gens;
  }

  if (a_type == MONOID) t_gens = n_gens;
  else t_gens = 2*n_gens;
  
  m = 2;
  n_bits = 1;
  
  while (m < t_gens) {
    n_bits++;
    m *= 2;
  }

  gens = new generator[t_gens];

  // Get naming convention.

 get_name_conv:
  if (prompt) {
    cout <<
    "\nEnter naming_convention, one of the following:\n" <<
    "prefix <string>, letters <letter>, or none:  ";
    stream >> str;
    if (echo) cout << str;
  }

  else {
    stream >> str;
    check("naming_convention:");
    stream >> str;
  }


  if (0 == strcmp(str,"prefix") ) {
    name_conv = PREFIX;

    if (prompt) {
      if (echo) cout << " ";
      scan_string(stream,pfx);
    }

    else stream >> pfx;
  }

  else if (0 == strcmp(str,"letters") ) {
    name_conv = LETTERS;

    if (prompt) {
      if (echo) cout << " ";
      scan_string(stream,str2);
    }

    else stream >> str2;

    if (strlen(str2) != 1) {
      cout << "letter convention requires a single letter"
           << endl;

      if (prompt && !echo) goto get_name_conv;
      else exit(1);
    }

    first_gen = *str2;

    if ( !islower(first_gen) || !islower(first_gen+n_gens-1) ) {
      cout << "invalid letter generator" << endl;

      if (prompt && !echo) goto get_name_conv;
      else exit(1);
    }
  }

  else {
    if (prompt && echo) cout << endl;
    if (0 == strcmp(str,"none") ) name_conv = NO_NAME;

    else {
      cout << "Invalid naming convention" << endl;
      if (prompt && !echo) goto get_name_conv;
      exit(1);
    }
  }

  if (a_type == GROUP) {
    // Get numbering convention.

   get_numb_conv:
    if (prompt) {
      cout <<
        "\nEnter numbering_convention (1_2, 1_-1, or none):  ";
      scan_string(stream,str);
    }

    else {
      stream >> str;
      check("numbering_convention:");
      stream >> str;
    }

    if      (0 == strcmp(str,"1_2") )  numb_conv = a_b;
    else if (0 == strcmp(str,"1_-1") ) numb_conv = a_A;

    else if (0 == strcmp(str,"none") ) {
      if (name_conv != NO_NAME) {
        cout << "Numbering_convention requires naming convention"
             << endl;
        if (prompt && !echo) return;
        exit(1);
      }

      numb_conv = NO_NUMB;
    }

    else {
      cout << "Invalid numbering convention" << endl;
      if (prompt && !echo) goto get_numb_conv;
      exit(1);
    }
      

    // Get inverse convention.

   get_inv_conv:
    if (prompt) {
      cout << endl <<
        "Enter inverse_convention (case, exponent, or none):  ";
      scan_string(stream,str);
    }

    else {
      stream >> str;
      check("inverse_convention:");
      stream >> str;
    }

    if      (0 == strcmp(str,"case") )     inv_conv = CASE;
    else if (0 == strcmp(str,"exponent") ) inv_conv = EXPONENT;

    else if (0 == strcmp(str,"none") ) {
      if (name_conv != NO_NAME) {
        cout << "Inverse_convention requires naming convention"
             << endl;

        if (prompt && !echo) return;
        exit(1);
      }

      inv_conv = NO_INV;
    }

    else {
      cout << "Invalid inverse convention" << endl;
      if (prompt && !echo) goto get_inv_conv;
      exit(1);
    }

    // If a naming convention was specified, we have enough
    // information to create the generator list.

    if (name_conv != NO_NAME) {
      for (i = 0; i < n_gens; i++) {
        if (numb_conv == a_b) {
          j = i;
          k = i + n_gens;
        }

        else {
          j = 2*i;
          k = j + 1;
        }

        if (name_conv == PREFIX) sprintf(str,"%s%ld",pfx,i+1);

        else {
          str[0] = first_gen + i;
          str[1] = '\0';
        }

        strcpy(str2,str);

        if (inv_conv == CASE) other_case(str2);
        else strcat(str2,"^-1");

        gens[j].set_name(str);
        gens[j].set_inv_numb(k);
        gens[k].set_name(str2);
        gens[k].set_inv_numb(j);
      }
    }  // if (name_conv != NO_NAME)

    else {   // We need more information.
      if (numb_conv == NO_NUMB) {
        if (prompt) {
          cout <<
          "\nEnter the generator names, including inverses,\n" <<
          "in increasing order:\n";
        }

        else {
          stream >> str;
          check("generators:");
        }

        for (i = 0; i < t_gens; i++) {
          stream >> str;
          gens[i].set_name(str);
          if (prompt && echo) cout << str << endl;
        }

        if (prompt) stream.ignore(10000,'\n');
      }

      if (inv_conv == NO_INV ) {
       get_inv_pairs:
        if (prompt)
          cout <<
          "\nEnter inverse_pairs of generators:\n";

        else {
          stream >> str;
          check("inverse_pairs:");
        }

        for (i = 0; i < n_gens; i++) {
          stream >> str >> str2;

          if (prompt && echo) cout << str << " " << str2 << endl;

          if (numb_conv == NO_NUMB) {
            j = gen_number(str);
            k = gen_number(str2);

            if (j < 0 || k < 0) {
              cout << "Invalid inverse pair" << endl;
              if (prompt && !echo) goto get_inv_pairs;
              exit(1);
            }

            gens[j].set_inv_numb(k);
            gens[k].set_inv_numb(j);
          }

          else if (numb_conv == a_b) {
            j = i + n_gens;
            gens[i].set_name(str);
            gens[i].set_inv_numb(j);
            gens[j].set_name(str2);
            gens[j].set_inv_numb(i);
          }

          else if (numb_conv == a_A) {
            j = 2*i;
            gens[j].set_name(str);
            gens[j].set_inv_numb(j+1);
            gens[j+1].set_name(str2);
            gens[j+1].set_inv_numb(j);
          }
        }

        if (prompt) stream.ignore(10000,'\n');
      }

      else if (numb_conv != NO_NUMB) {
        if (prompt) {
          cout <<
          "\nEnter the generator names, without inverses,\n" <<
          "in increasing order:\n";
        }

        else {
          stream >> str;
          check("generators:");
        }

        for (i = 0; i < n_gens; i++) {
          stream >> str;
          if (prompt && echo) cout << str << endl;
          strcpy(str2,str);

          if (inv_conv == CASE) other_case(str2);
          else (void) strcat(str2,"^-1");

          if (numb_conv == a_b) {
            j = i;
            k = i + n_gens;
          }

          else {
            j = 2*i;
            k = j + 1;
          }

          gens[j].set_name(str);
          gens[j].set_inv_numb(k);
          gens[k].set_name(str2);
          gens[k].set_inv_numb(j);
        }

        if (prompt) stream.ignore(10000,'\n');

      }  // if (numb_conv != NO_NUMB)

      else {  // numb_conv == NO_NUMB and inv_conv != NO_INV
        for (i = 0; i < t_gens; i++) {
          if (gens[i].get_inv_numb() >= 0) continue;

          gens[i].get_name(str);
          strcpy(str2,str);

          if (inv_conv == CASE) other_case(str2);
          else strcat(str2,"^-1");

          if (i >= (j = gen_number(str2) ) ) {
            cout << "Unable to find inverse with name "
                 << str2 << endl;

            if (prompt && !echo) return;
            exit(1);
          }

          gens[i].set_inv_numb(j);
          gens[j].set_inv_numb(i);
        }
      }  // numb_conv == NO_NUMB and inv_conv != NO_INV

    }  // No naming convention

    // Construct the inverse words.

    for (i = 0; i < t_gens; i++) {
      j = gens[i].get_inv_numb();
      gens[i].set_invertible(TRUE);
      gens[i].set_one(j);
    }

    make_generator_index();

  }  // if (a_type == GROUP)

  else {  // MONOID words
    if (name_conv != NO_NAME) {
      for (i = 0; i < n_gens; i++) {
        if (name_conv == PREFIX) sprintf(str,"%s%l",pfx,i+1);

        else {
          str[0] = first_gen + i;
          str[1] = '\0';
        }

        gens[i].set_name(str);
      }
    }  // if (name_conv != NO_NAME)

    else {  // name_conv == NO_NAME
      if (prompt)
        cout << "\nEnter generator names in increasing order:\n";

      else {
        stream >> str;
        check("generators:");
      }

      for (i = 0; i < n_gens; i++) {
        stream >> str;
        if (prompt && echo) cout << str << endl;
        gens[i].set_name(str);
      }
    }

    make_generator_index();

    // Get inverse data.

   get_ninv:
    if (prompt) {
      cout <<
      "\nEnter number of generators with inverses:  ";
      scan_long(stream,ninv);
    }

    else {
      stream >> str;
      check("ninv:");
      stream >> ninv;
    }

    if (ninv < 0) {
      cout << "ninv must be nonnegative" << endl;
      if (prompt && !echo) goto get_ninv;
      exit(1);
    }

    if (ninv > 0) {
      if (prompt) cout << "\nEnter inverse pairs:\n";

      else {
        stream >> str;
        check("inverse_pairs:");
      }

      for (i = 0; i < ninv; i++) {
        for (;;) {
          stream >> str >> vec;  // This assumes inverses
                                 // are short!
          if (prompt && echo) cout << str << " " << vec << endl;

          if (0 > (j = gen_number(str) ) ) 
            cout << "Can't find generator with name " << str
                 << endl;

          else if (parse(vec,wrd) )
            cout << "Can't parse " << vec << endl;
          else break;

          if (!prompt || echo) exit(1);
          cout << "Reenter inverse pair:  ";
        }

        gens[j].set_invertible(TRUE);
        gens[j].set_inverse(wrd);
      }
    }
  }  // else {  // MONOID word

  // Get weight data.

 get_weight_data:
  if (prompt) {
    cout << "\nEnter weight_convention (constant or none):  ";
    scan_string(stream,str);
  }

  else {
    stream >> str;
    check("weight_convention:");
    stream >> str;
  }

  if (0 == strcmp(str,"constant") ) {
    wt_conv = CONSTANT;
    for (i = 0; i < t_gens; i++) gens[i].set_weight(1);
  }

  else if (0 == strcmp(str,"none") ) {
    wt_conv = NONCONSTANT;
    
    if (prompt)
      cout << "\nEnter the weights of generators,\n" <<
      "in increasing order of the generators:\n";

    else {
      stream >>str;
      check("weights:");
    }

    for (i = 0; i < t_gens; i++) {
      stream >> j;

      if (prompt && echo) cout << j << endl;

      for (;;) {
        if (j > 0) break;
        cout << "Weights must be positive" << endl;
        if (!prompt || echo) exit(1);
        cout << "Reenter last weight:  ";
        stream >> j;
      }

      gens[i].set_weight(j);
    }
  }

  else {
    cout << "Invalid weight convention" << endl;
    if (prompt && !echo) goto get_weight_data;
    exit(1);
  }

  // Get level data.

 get_level_data:
  if (prompt) {
    cout << "\nEnter the level_convention\n" <<
      "(constant, basic_wreath, inverse_pair_wreath, or none):  ";

    scan_string(stream,str);
  }

  else {
    stream >> str;
    check("level_convention:");
    stream >> str;
  }

  if (0 == strcmp(str,"constant") ) {
    lev_conv = CONSTANT;
    for (i = 0; i < t_gens; i++) gens[i].set_level(1);
  }

  else if (0 == strcmp(str,"basic_wreath") ) {
    lev_conv = NONCONSTANT;

    if (a_type == MONOID)
      for (i = 0; i < t_gens; i++) gens[i].set_level(i+1);

    else {
      k = 1;

      for (i = 0; i < t_gens; i++) {
        j = gens[i].get_inv_numb();
        if (j <= i) continue;
        gens[j].set_level(k++);
        gens[i].set_level(k++);
      }
    }
  }

  else if (0 == strcmp(str,"inverse_pair_wreath") ) {
    lev_conv = NONCONSTANT;
    
    if( a_type != GROUP || numb_conv != a_A) {
      cout << "inverse_pair_wreath convention not allowed here."
           << endl;
      if (prompt && !echo) goto get_level_data;
      exit(1);
    }

    for (i = j = 0; i < n_gens; i++) {
      gens[j++].set_level(i+1);
      gens[j++].set_level(i+1);
    }
  }

  else if (0 == strcmp(str,"none") ) {
    lev_conv = NONCONSTANT;
    
    if (prompt)
      cout << "\nEnter the levels of generators,\n" <<
        "in increasing order of the generators:\n";

    else {
      stream >> str;
      check("levels:");
    }

    for (i = 0; i < t_gens; i++) {
      stream >> j;

      if (prompt && echo) cout << j << endl;

      for (;;) {
        if (j > 0) break;
        cout << "Levels must be positive." << endl;
        if (!prompt || echo) exit(1);
        cout << "Reenter last level:  ";
        stream >> j;
      }

      gens[i].set_level(j);
    }
  }

  else {
    cout << "Unknown level convention" << endl;
    if (prompt && !echo) goto get_level_data;
    exit(1);
  }
  
  use_heuristics = FALSE;

  if ( wt_conv == CONSTANT)
    if (lev_conv == CONSTANT) {
      set_o_type(LNLX);
      use_heuristics = TRUE;
    }

    else set_o_type(WRLNLX);

  else
    if (lev_conv == CONSTANT) set_o_type(WTLNLX);
    else set_o_type(WRWTLNLX);

  // Get the subsystems.

  if (prompt) {
    cout << "\nEnter the number of subsystems:  ";
    scan_long(stream,nsub);
  }

  else {
    stream >> str;
    check("nsub:");
    stream >> nsub;
  }

  if (!prompt) {
    stream >> str;
    check("subsystems:");
  }

  for (i = 0; i < nsub; i++) {
    subsyst = new subsystem;
    subsyst->set_parent(this);
    subsyst->input(stream,prompt,check);

    if (NO_INDEX == subsyst->get_i_type() ) {
      if (PREFIX_RULE == subsyst->get_r_type() )
        if (broken_prefix_rules == NULL)
          broken_prefix_rules = subsyst;
        else goto broken;

      else if (SUBWORD_RULE == subsyst->get_r_type() )
        if (broken_subword_rules == NULL)
          broken_subword_rules = subsyst;
        else goto broken;

      else if (IDENTITY == subsyst->get_r_type() )
        add_subsystem(subsyst);
    }

    else add_subsystem(subsyst);
  }

  return;

 invalid_input:
  cout << "Invalid input string \"" << str << "\"" << endl;
  exit(1);

 broken:
  cout << "Too many subsystems with no indexes." << endl;

  return;
}



/////////////////////////////////////////////////////////////////
//
//              rewriting_system::add_subsystem
//
/////////////////////////////////////////////////////////////////

void rewriting_system::add_subsystem(
  subsystem  *subsyst
)
{
  subsyst->set_next_subsystem(NULL);
  subsyst->set_prev_subsystem(last_subsystem);

  if ( last_subsystem != NULL) {
    last_subsystem->set_next_subsystem(subsyst);
    last_subsystem = subsyst;
  }

  else
    first_subsystem = last_subsystem = subsyst;

  n_sub++;

  return;
}



/////////////////////////////////////////////////////////////////
//
//            rewriting_system::make_generator_index
//
/////////////////////////////////////////////////////////////////

void rewriting_system::make_generator_index()
{
  long   i;
  char   str[500];

  for (i = 0; i < t_gens; i++) {
    gens[i].get_name(str);
    autn.enter_string(str,i);
  }

  return;
}



/////////////////////////////////////////////////////////////////
//
//                 rewriting_system::gen_number
//
/////////////////////////////////////////////////////////////////

long rewriting_system::gen_number(
  char  *str
)
{
  long  i;

  for (i = 0; i < t_gens; i++)
    if ( gens[i].compare_name(str) ) return i;

  return -1;
}



/////////////////////////////////////////////////////////////////
//
//               rewriting_system::parse
//
/////////////////////////////////////////////////////////////////

long rewriting_system::parse(
  vector_char  &vec,
  vector_long  &wrd
)
{
  long    lvec,j,g,state,current,p,bit,h,old;
  char    c;
  
  lvec = vec.get_length();
  wrd.set_length(0);

  if (lvec == 0 || lvec == 1 && vec[0] == '@') return 0;

  j       = 0;
  g       = LONG_MAX;
  state   = 0;
  current = 0;  

  while (current < lvec) {
    c = vec[current++];

    for (p = CHAR_BIT-2; p >= 0; p--) {
      bit   = (c>>p) & 1;
      state = autn.get_entry(state,bit);
      if (state == LONG_MAX) break;
    }

    if ( LONG_MAX != state &&
         LONG_MAX != (h = autn.get_entry(state,2) ) ) {
      //  The current prefix is a generator name.

      g   = h;
      old = current;
    }

    if (state == LONG_MAX || current >= lvec)
      if (g != LONG_MAX) {
        if (g >= t_gens) {
          cout << "Invalid generator in parse" << endl;
          return 1;
        }

        current = old;
        state   = 0;
        wrd.safe_set_entry(j++,g);
      }

      else {
        cout << "Unable to parse names string" << endl;
        return 1;
      }
  }

  return 0;
}



/////////////////////////////////////////////////////////////////
//
//                  rewriting_system::parse_6bit
//
/////////////////////////////////////////////////////////////////

long rewriting_system::parse_6bit(
  vector_char  &vec,
  long          len,
  vector_long  &word
)
{
  long   i,wbits,zbits,j,diff,bits_in_vec,bits_to_save;
  ulong  z,w;

  word.set_length(len);
  bits_in_vec = len*n_bits;
  bits_to_save = len*n_bits;
  i = j = 0;
  w = z = 0;
  wbits = zbits = 0;

  while (bits_to_save > 0) {
    while (zbits < n_bits) {
      diff = n_bits - zbits;

      if (wbits == 0 && bits_in_vec > 0) {
        w = vec[i++] - 48;
        w = w&mask[6];
        wbits = 6;
        bits_in_vec -= 6;
      }

      if (wbits > diff) {
        z = (z<<diff) | ((w>>(wbits-diff))&mask[diff]);
        zbits  = n_bits;
        wbits -= diff;
      }

      else if (wbits > 0) {
        z = (z<<wbits) | (w&mask[wbits]);
        zbits += wbits;
        wbits  = 0;
      }

      else {
        z = z<<(n_bits-zbits);
        zbits = n_bits;
      }
    }

    if (z >= t_gens) {
      cout << "Invalid generator in parse_6bit" << endl;
      return 1;
    }

    word.set_entry(j++,z);
    bits_to_save -= n_bits;
    zbits = z = 0;
  }

  return 0;
}



/////////////////////////////////////////////////////////////////
//
//                      set_hybrid_factor
//
/////////////////////////////////////////////////////////////////

void set_hybrid_factor(
  double  factor
)
{
  hybrid_factor = factor;
}



/////////////////////////////////////////////////////////////////
//
//                  rewriting_system::read_word
//
/////////////////////////////////////////////////////////////////

// Returns 0 if successful and 1 if end of file has been reached
// already.  If an error or end of file condition occurs in the
// middle of reading the word, the current input line is cleared
// and a value of 2 returned.

long rewriting_system::read_word(
  istream        &stream,
  external_form   exf,
  vector_long    &wrd
)
{
  long                lwx,i,g,mxl,code;
  static vector_char  buff,buff1;

  if (exf == LEN_NAMES   ||
      exf == LEN_NUMBERS ||
      exf == LEN_6BIT      ) {
    if ( !(stream >> lwx) ) return 1;
    mxl = wrd.get_max_length();
    wrd.set_max_length(max(lwx,mxl) );
  }

  else lwx = -1;

  if (exf == LEN_NUMBERS) {
    for (i = 0; i < lwx; i++) {
      if ( !(stream >> g) ) goto input_error;
      wrd.safe_set_entry(i,g);
    }

    wrd.set_length(lwx);
    return 0;
  }

  if (exf == LEN_6BIT) {
    if ( !(stream >> buff) ) goto input_error;

    if (parse_6bit(buff,lwx,wrd) ) goto input_error;
    return 0;
  }

  if (exf == NAMES || exf == LEN_NAMES) {
    code = continued_string(stream,buff,buff1);

    if (1 == code) return 1;
    if (2 == code) goto input_error;

    parse(buff,wrd);
    return 0;
  }

 input_error:
  cout << "Input error in read_word." << endl;

  if ( !stream.ignore(10000,'\n') ) {
    cout << "Unable to clear current line" << endl;
    exit(1);
  }

  return 2;
}



/////////////////////////////////////////////////////////////////
//
//                 rewriting_system::summary
//
/////////////////////////////////////////////////////////////////

void rewriting_system::summary(void)
{
  subsystem   *subsyst;

  cout << "\nalphabet_type:  ";

  if (a_type == MONOID) cout << "monoid";
  else cout << "group";

  cout << ",  ngens:  " << n_gens << ",   nbits:  " << n_bits
       << endl;

  cout << "rewriting strategy:  ";

  if (rw_strategy == FROM_LEFT) cout << "from_left" << endl;
  else cout << "global" << endl;
  
  cout << "Knuth-Bendix strategy:  ";
  
  if (kb_strategy == BASIC) cout << "basic" << endl;
  else cout << "two_step" << endl;
  
  if (n_sub == 1) cout << "There is 1 subsystem:";
  else
    cout << "There are " << n_sub << " subsystems:";

  cout << endl;

  subsyst = first_subsystem;

  while (subsyst != NULL) {
    subsyst->summary();
    subsyst = subsyst->get_next_subsystem();
  }

  if (broken_prefix_rules != NULL) {
    cout << "\nThere is a subsystem of broken prefix rules."
         << endl << endl;
    broken_prefix_rules->summary();
  }

  if (broken_subword_rules != NULL) {
    cout << "\nThere is a subsystem of broken subword rules."
         << endl << endl;
    broken_subword_rules->summary();
  }
}



/////////////////////////////////////////////////////////////////
//
//                 rewriting_system::word_weight
//
/////////////////////////////////////////////////////////////////

long rewriting_system::word_weight(
  vector_long   &wrd
)
{
  long   wt,len,i;

  wt  = 0;
  len = wrd.get_length();
  for (i = 0; i < len; i++) wt += get_weight(wrd[i]);

  return wt;
}



/////////////////////////////////////////////////////////////////
//
//                   rewriting_system::lnlx
//
/////////////////////////////////////////////////////////////////

long rewriting_system::lnlx(
  vector_long  &u,
  vector_long  &v
)
{
  long   lu,lv,i,ui,vi;

  if ( (lu = u.get_length() ) != (lv = v.get_length() ) )
    if (lu < lv) return -1;
    else return 1;

  for (i = 0; i < lu; i++)
    if ( (ui = u[i]) != (vi = v[i]) )
      if (ui < vi) return -1;
      else return 1;

  return 0;
}



/////////////////////////////////////////////////////////////////
//
//                   rewriting_system::wtlnlx
//
/////////////////////////////////////////////////////////////////

long rewriting_system::wtlnlx(
  vector_long  &u,
  vector_long  &v
)
{
  long   wtu,wtv;

  if ( (wtu = word_weight(u) ) != (wtv = word_weight(v) ) )
    if (wtu < wtv) return -1;
    else return 1;

  return lnlx(u,v);
}



/////////////////////////////////////////////////////////////////
//
//                   rewriting_system::wreath
//
/////////////////////////////////////////////////////////////////

long rewriting_system::wreath(
  vector_long  &u,
  vector_long  &v
)
{
  long    lbu,lbv,su,sv,levu,levv,sgn;
  static vector_long   ubuff(100), vbuff(100);
  vector_long_segment  useg,vseg;

  level_decomposition(u,v,ubuff,vbuff);
  lbu = ubuff.get_length();
  lbv = vbuff.get_length();

  while (lbu > 0 && lbv > 0) {
    su = lbu - 1;
    sv = lbv - 1;
    levu = get_level(ubuff[su]);
    levv = get_level(vbuff[sv]);

    if (levu != levv)
      if (levu > levv) return -1;
      else return 1;

    else {
      while (su > 0 && get_level(ubuff[su-1]) == levu) su--;
      while (sv > 0 && get_level(vbuff[sv-1]) == levv) sv--;

      useg.segment(ubuff,su,lbu-su);
      vseg.segment(vbuff,sv,lbv-sv);

      if (0 != (sgn = (this->*base_order)(useg,vseg) ) )
        return sgn;

      lbu = su;
      lbv = sv;
    }
  }

  if (lbu > 0) return 1;
  if (lbv > 0) return -1;
  return 0;
}



/////////////////////////////////////////////////////////////////
//
//             rewriting_system::level_decomposition
//
/////////////////////////////////////////////////////////////////

void rewriting_system::level_decomposition(
  vector_long  &u,
  vector_long  &v,
  vector_long  &ubuff,
  vector_long  &vbuff
)
{
  long   lu,lv,lm,lc;

  lu = u.get_length();
  lv = v.get_length();
  ubuff.set_max_length(lu);
  vbuff.set_max_length(lv);

  lm = min(lu,lv);
  lc = 0;
  while (lc < lm && u[lc] == v[lc]) lc++;

  while (lu > lc && lv > lc && u[lu-1] == v[lv-1]) {
    lu--;
    lv--;
  }

  level_sequence(u,lc,lu,ubuff);
  level_sequence(v,lc,lv,vbuff);
}



/////////////////////////////////////////////////////////////////
//
//              rewriting_system::level_sequence
//
/////////////////////////////////////////////////////////////////

void rewriting_system::level_sequence(
  vector_long   &u,
  long          lc,
  long          lu,
  vector_long  &ubuff
)
{
  long  lev,lbu,i,g,levg;

  lev = INT_MAX;
  lbu = 0;

  for (i = lc; i < lu; i++) {
    g    = u[i];
    levg = get_level(g);

    if (lev >= levg) {
      ubuff.set_entry(lbu++,g);
      lev = levg;
    }
  }

  ubuff.set_length(lbu);
}



/////////////////////////////////////////////////////////////////
//
//                rewriting_system::set_o_type
//
/////////////////////////////////////////////////////////////////

void rewriting_system::set_o_type(
  order_type  otp
)
{
  o_type = otp;

  if      (otp == LNLX)     compare = &rewriting_system::lnlx;
  else if (otp == WTLNLX)   compare = &rewriting_system::wtlnlx;
  else if (otp == WRLNLX)   compare = &rewriting_system::wrlnlx;
  else if (otp == WRWTLNLX) compare = &rewriting_system::wrwtlnlx;
}



/////////////////////////////////////////////////////////////////
//
//                 rewriting_system::rewrite
//
/////////////////////////////////////////////////////////////////

void rewriting_system::rewrite(
  rule            &rul,
  rule_reference   exclude,
  _boolean_t         &changed,
  _boolean_t         &reduced
)
{
  _boolean_t   lch,rch,lrd,rrd,subword_only;

  if (rul.r_type == IDENTITY)
    error("The rewriting of identity rules is not supported.");

  if (rul.r_type == SUBWORD_RULE) subword_only = TRUE;
  else subword_only = FALSE;

  (this->*pRewrite)(
        subword_only,rul.left,rul.left,exclude,lch,lrd);

  (this->*pRewrite)(
        subword_only,rul.right,rul.right,exclude,rch,rrd);

  changed = bool_t(lch||rch);
  reduced = bool_t(lrd&&rrd);
}



/////////////////////////////////////////////////////////////////
//
//             rewriting_system::rewrite_from_left
//
/////////////////////////////////////////////////////////////////

void rewriting_system::rewrite_from_left(
  _boolean_t         subword_only,
  vector_long    &input,
  vector_long    &output,
  rule_reference  exclude,
  _boolean_t        &changed,
  _boolean_t        &reduced
)
{
  long         lin,lout,lout_max,bbot,lb,ll,lr,i;
  packed_rule  rul;
  subsystem   *subsyst;

  // The max_length of output will not be changed.

  changed = FALSE;
  reduced = FALSE;

  lin = input.get_length();
  lout_max = output.get_max_length();

  if (lin > lout_max)
    error("output buffer too small in rewrite_from_left.");

  n_rewrite++;
  total_input += lin;

  if (rewrite_verbose) {
    cout << "Rewriting from left:  ";
    input.print();
  }

  if (n_sub <= 0) {
    output.set_copy(input);
    reduced = TRUE;
    return;
  }

  subsyst = first_subsystem;

  while (subsyst != NULL) {
    subsyst->check_aux(REWRITE,subword_only,lout_max);
    subsyst = subsyst->get_next_subsystem();
  }

  lout = lin;
  bbot = lout_max;
  lb   = lin;

  while (lb > 0) output.set_entry(--bbot, input[--lb]);
  output.set_length(0);

  while (bbot < lout_max) {
    output.set_entry(lb++, output[bbot++]);
    output.set_length(lb);
    subsyst = first_subsystem;

    while (subsyst != NULL) {
      subsyst->incremental_test(REWRITE,subword_only,
                        output,exclude,rul);

      if (rul.left.subsyst != NULL) break;

      subsyst = subsyst->get_next_subsystem();
    }

    if (subsyst == NULL) continue;  // The loop ended normally.

    ll    = rul.left.length;
    lr    = rul.right.length;
    lout += lr - ll;

    if (lout > lout_max) {
      // No room to use rule.  Abort rewriting.

      while (bbot < lout_max)
        output.set_entry(lb++, output[bbot++]);
        output.set_length(lb);
      return;
    }

    n_applications++;
    total_left += ll;
    changed = TRUE;
    lb    -= ll;
    bbot  -= lr;
    rul.right.unpack(output,bbot);

    if (rewrite_verbose) {
      cout << "Which becomes:   ";
      for (i = 0; i < lb; i++) cout << output[i] << " ";
      for (i = bbot; i < lout_max; i++)
         cout << output[i] << " ";
      cout << endl;
    }
  }
  
  output.set_length(lb);
  reduced = TRUE;
}



/////////////////////////////////////////////////////////////////
//
//             rewriting_system::rewrite_global
//
/////////////////////////////////////////////////////////////////

void rewriting_system::rewrite_global(
  _boolean_t         subword_only,
  vector_long    &input,
  vector_long    &output,
  rule_reference  exclude,
  _boolean_t        &changed,
  _boolean_t        &reduced
)
{
  long         lin,lout,lout_max,lnew,i,j,off,ll,lr,diff,diff1;
  packed_rule  rul,rul1;
  subsystem   *subsyst;

  // The max_length of output will not be changed.

  changed = FALSE;
  reduced = FALSE;

  lin = input.get_length();
  lout_max = output.get_max_length();

  if (lin > lout_max)
    error("output buffer too small in rewrite_global.");

  if (rewrite_verbose) {
    cout << "Global rewriting" << endl;
    input.print();
  }

  n_rewrite++;
  total_input += lin;

  if (n_sub <= 0) {
    if (rewrite_verbose) {
      cout << "No subsystems!" << endl;
    }

    output.set_copy(input);
    reduced = TRUE;
    return;
  }

  subsyst = first_subsystem;

  while (subsyst != NULL) {
    subsyst->check_aux(REWRITE,subword_only,lout_max);
    subsyst = subsyst->get_next_subsystem();
  }

  lout = lin;

  for (i = 0; i < lin; i++) output.set_entry(i,input[i]);
  output.set_length(lout);

  for (;;) {
    subsyst = first_subsystem;
    rul.left.subsyst = NULL;
    diff = LONG_MAX;

    while (subsyst != NULL) {
      rul1 = subsyst->global_test(subword_only,output,
                                       exclude);

      if (rul1.left.subsyst != NULL) {
        diff1 = rul1.right.length - rul1.left.length;

        if (rul.left.subsyst == NULL || diff1 < diff) {
          rul = rul1;
          diff = diff1;
        }
      }

      subsyst = subsyst->get_next_subsystem();
    }

    if (rul.left.subsyst == NULL) break;  // No left side was
                                          // found.

    ll   = rul.left.length;
    lr   = rul.right.length;
    off  = rul.offset;
    lnew = lout + lr - ll;

    if (lnew > lout_max) {
      // No room to use rule.  Abort rewriting.

      if (rewrite_verbose) {
        cout << "bsize exceeded" << endl;
        output.print();
      }

      return;
    }

    n_applications++;
    total_left += ll;
    changed = TRUE;

    if (ll > lr)
      for (i = off, j = i-ll+lr; i < lout; )
        output.set_entry(j++,output[i++]);

    else if (ll < lr)
      for (i = lout-1, j = i-ll+lr; i >= off; )
        output.set_entry(j--,output[i--]);

    rul.right.unpack(output,off-ll);

    lout = lnew;
    output.set_length(lout);

    if (rewrite_verbose) {
      cout << "Which becomes ";
      output.print();
    }
  }
  
  reduced = TRUE;
}



/////////////////////////////////////////////////////////////////
//
//            rewriting_system::reduce_all_subsystems
//
/////////////////////////////////////////////////////////////////

_boolean_t rewriting_system::reduce_all_subsystems(
  rule_type   rtype
)
{
  _boolean_t     new_call,new_loop,new_subsyst;
  subsystem  *subsyst;

  if (print_level >= 1)
    cout << "Reducing all subsystems of type " << rtype << endl;

  new_call = FALSE;

  do {
    new_loop = FALSE;
    subsyst = first_subsystem;

    while (subsyst != NULL) {
      if ( subsyst->get_r_type() == rtype) {
        new_subsyst = subsyst->reduce();
        new_loop    = new_loop||new_subsyst;
      }

      subsyst = subsyst->get_next_subsystem();
    }

    if (rtype == PREFIX_RULE && broken_prefix_rules != NULL) {
      new_loop = new_loop || broken_prefix_rules->reduce();

      if (0 == broken_prefix_rules->get_nrules() ) {
        delete broken_prefix_rules;
        broken_prefix_rules = NULL;
      }
    }

    else if (rtype == SUBWORD_RULE &&
             broken_subword_rules != NULL) {
      new_loop = new_loop || broken_subword_rules->reduce();

      if (0 == broken_subword_rules->get_nrules() ) {
        delete broken_subword_rules;
        broken_subword_rules = NULL;
      }
    }

    new_call = new_call||new_loop;
  } while (new_loop);

  return new_call;
}



/////////////////////////////////////////////////////////////////
//
//            rewriting_system::reduce_save_list
//
/////////////////////////////////////////////////////////////////

_boolean_t rewriting_system::reduce_save_list(void)
{
  _boolean_t     new_call,new_loop,new_subsyst;
  subsystem  *subsyst;

  if (print_level >= 1)
    cout << "Reducing the save list." << endl;

  new_call = FALSE;

  do {
    new_loop = FALSE;
    subsyst = first_save;

    while (subsyst != NULL) {
      new_subsyst = subsyst->reduce();
      new_loop = new_loop || new_subsyst;
      subsyst = subsyst->get_next_save();
    }

    new_call = new_call || new_loop;
  } while (new_loop);

  return new_call;
}



/////////////////////////////////////////////////////////////////
//
//                rewriting_system::overlap
//
/////////////////////////////////////////////////////////////////

void rewriting_system::overlap(
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
)
{
  long                 free1,orig1,lr1,ll1,llmax2,i,lov,ll2,lr2;
  long                 ll3,lr3;
  long                 g,status,offset,nrules1,incr;
  _boolean_t              changed,reduced;
  rule_type            rtp1,rtp2;
  vector_long_segment  useg;
  packed_rule          p_rule;
  rule                 rule1,rule2,rule3;
  rule_reference       exclude = {NULL, 0};

  update_time();

  if (print_level >= 1)
    cout << "Forming overlaps of subsystem at "
         << (ulong)subsyst1 << " with subsystem at "
         << (ulong)subsyst2 << endl;

  rtp1 = subsyst1->get_r_type();  
  rtp2 = subsyst2->get_r_type();

  successful = FALSE;
  nnew       = 0;
  neworig    = -1;

  if (IDENTITY == rtp1 || IDENTITY == rtp2) {
    cout << "Overlaps with identities not currently supported"
         << endl;

    return;
  }

  if (SUBWORD_RULE != rtp2) {
    cout <<
    "Second subsystem for overlaps must contain subword rules"
         << endl;

    return;
  }

  successful = TRUE;
  rule3.set_max_length(bsize);
  rule3.r_type = rtp1;
  llmax2 = subsyst2->get_llmax();

  // Check that subsystems listed in avoid have the auxiliary
  // space they need.

  for (i = 0; i < navoid; i++)
    avoid[i]->check_aux(OVERLAP,TRUE,maxlen);

  // Process the rules in subsyst1.

  nrules1 = subsyst1->get_nrules();
  incr    = max(10,nrules1/500);
  initialize_dots('.',incr,50);
  free1 = subsyst1->get_free();
  orig1 = start_orig;

  while (orig1 < free1) {
    print_dots();
    neworig = orig1;
    subsyst1->rule_by_origin(orig1,p_rule);
    if (p_rule.redundant) continue;

    ll1 = p_rule.left.length;
    lr1 = p_rule.right.length;

    if (ll1 >= maxlen || ll1+llmax2 <= minlen) continue;
    subsyst1->unpack(p_rule,rule1);

    for (i = 0; i < navoid; i++)
      if (avoid[i]->cummulative_test(rule1.left,rtp1))
        error("left side not reduced in overlap.");

    // Begin backtrack search for suffixes C which produce
    // minimal overlaps.

    lov = ll1;
    
    // make sure left side of rule1 is big enough for backtrack.

    rule1.left.set_max_length(maxlen);
    rule1.left.set_entry(lov++,0);
    rule1.left.set_length(lov);

    for (;;) {
      for (i = 0; i < navoid; i++) {
        status = avoid[i]->overlap_test(rule1.left,ll1,p_rule);

        // status == 1, rule1.left ends with rule returned in
        //              p_rule, whose left side has length
        //              greater than lov-ll1.
        // status == 2, rule1.left ends with rule return in
        //              p_rule, whose left side has length less
        //              than or equal to lov-ll1.
        // status == 3, no suffix of rule1.left which is a prefix
        //              of a rule in subsystem avoid[i] has
        //              length more than lov-ll1.
        // status == 4, none of 1, 2, or 3.

        if (status == 1)
          if (avoid[i] == subsyst2) goto overlap_found;
          else goto increment;
          
        if (status == 2) goto increment;

        if (status == 3 && avoid[i] == subsyst2) goto increment;
      }

      if (lov >= maxlen) goto increment;
      rule1.left.set_entry(lov++,0);
      rule1.left.set_length(lov);
      continue;

     overlap_found:
      noverlaps++;
      if (lov < minlen) goto increment;
      offset = lov - p_rule.left.length;
      if (offset >= ll1) goto increment;
      subsyst2->unpack(p_rule,rule2);
      ll2 = rule2.left.get_length();
      lr2 = rule2.right.get_length();

      if (lr1 + lov - ll1 > bsize ||
          lr2 + offset    > bsize   ) {
        successful = FALSE;
        goto increment;
      }

      rule3.left.set_copy(rule1.right);
      useg.segment(rule1.left,ll1,lov-ll1);
      rule3.left.append(useg);
      useg.segment(rule1.left,0,offset);
      rule3.right.set_copy(useg);
      rule3.right.append(rule2.right);

      rewrite(rule3,exclude,changed,reduced);
      if ( orient_rule(rule3) ) goto increment;

      if (!reduced) {
         successful = FALSE;
         goto increment;
      }

      balance_rule(rule3);
      ll3 = rule3.left.get_length();
      lr3 = rule3.right.get_length();
      
      if (ll3 <= savell && lr3 <= savelr) {
        save_rule(rule3);
        nnew++;
      }

      else successful = FALSE;

     increment:
      while(lov > 0 && (g = (rule1.left)[lov-1]) == t_gens-1)
        lov--;
      if (lov <= ll1) break;
      rule1.left.set_entry(lov-1,g+1);
      rule1.left.set_length(lov);
    }  // for (;;)
  }  // while (orig1 < free1)

  end_dots();
  neworig = -1;
  return;
}



/////////////////////////////////////////////////////////////////
//
//                rewriting_system::random_overlap
//
/////////////////////////////////////////////////////////////////

void rewriting_system::random_overlap(
  subsystem  *subsyst1,
  subsystem  *subsyst2,
  long        maxlen,
  subsystem **avoid,
  long        navoid
)
{
  long                 free1,orig1,lr1,ll1,llmax2,i,lov,ll2,lr2;
  long                 ll3,lr3,j,ng;
  long                 g,status,offset,nrules1,incr;
  _boolean_t              changed,reduced;
  rule_type            rtp1,rtp2;
  vector_long          gen_list;
  vector_long_segment  useg;
  packed_rule          p_rule;
  rule                 rule1,rule2,rule3;
  rule_reference       exclude = {NULL, 0};

  update_time();

  gen_list.set_max_length(t_gens);
  rtp1 = subsyst1->get_r_type();  
  rtp2 = subsyst2->get_r_type();

  if (IDENTITY == rtp1 || IDENTITY == rtp2) {
    cout << "Overlaps with identities not currently supported"
         << endl;

    return;
  }

  if (SUBWORD_RULE != rtp2) {
    cout <<
    "Second subsystem for overlaps must contain subword rules"
         << endl;

    return;
  }

  rule3.set_max_length(bsize);
  rule3.r_type = rtp1;
  llmax2 = subsyst2->get_llmax();

  // Check that subsystems listed in avoid have the auxiliary
  // space they need.

  for (i = 0; i < navoid; i++)
    avoid[i]->check_aux(OVERLAP,TRUE,maxlen);

  // Process random rules in subsyst1.

  nrules1 = subsyst1->get_nrules();
  subsyst1->rule_by_number(irand(0)%nrules1,p_rule);
  if (p_rule.redundant) return;

  ll1 = p_rule.left.length;
  lr1 = p_rule.right.length;

  if (ll1 >= maxlen) return;
  subsyst1->unpack(p_rule,rule1);

  for (i = 0; i < navoid; i++)
    if (avoid[i]->cummulative_test(rule1.left,rtp1))
      error("left side not reduced in overlap.");

  // Begin backtrack search for suffixes C which produce
  // minimal overlaps.

  lov = ll1;
    
  rule1.left.set_max_length(maxlen);

  while (lov < maxlen) {
    lov++;
    ng = 0;
    rule1.left.set_length(lov);

    for (j = 0; j < t_gens; j++) {
      rule1.left.set_entry(lov-1,j);

      for (i = 0; i < navoid; i++) {
        status = avoid[i]->overlap_test(rule1.left,ll1,p_rule);

        // status == 1, rule1.left ends with rule returned in
        //              p_rule, whose left side has length
        //              greater than lov-ll1.
        // status == 2, rule1.left ends with rule return in
        //              p_rule, whose left side has length less
        //              than or equal to lov-ll1.
        // status == 3, no suffix of rule1.left which is a prefix
        //              of a rule in subsystem avoid[i] has
        //              length more than lov-ll1.
        // status == 4, none of 1, 2, or 3.

        if (status == 1)
          if (avoid[i] == subsyst2) {
            gen_list.set_entry(ng++,j);
            break;
          }
          else break;
          
        if (status == 2) break;

        if (status == 3 && avoid[i] == subsyst2) break;
      }

      if (i >= navoid) gen_list.set_entry(ng++,j);
    }

    if (0 == ng) return;

    rule1.left.set_entry(lov-1,gen_list[irand(0)%ng]);
    status = subsyst2->overlap_test(rule1.left,ll1,p_rule);
    if (1 == status) goto overlap_found;
  }

  if (status != 1) return;

 overlap_found:
  noverlaps++;
  offset = lov - p_rule.left.length;
  if (offset >= ll1) return;
  subsyst2->unpack(p_rule,rule2);
  ll2 = rule2.left.get_length();
  lr2 = rule2.right.get_length();

  if (lr1 + lov - ll1 > bsize ||
      lr2 + offset    > bsize   ) return;

  rule3.left.set_copy(rule1.right);
  useg.segment(rule1.left,ll1,lov-ll1);
  rule3.left.append(useg);
  useg.segment(rule1.left,0,offset);
  rule3.right.set_copy(useg);
  rule3.right.append(rule2.right);

  rewrite(rule3,exclude,changed,reduced);
  if ( orient_rule(rule3) ) return;

  if (!reduced) return;

  balance_rule(rule3);
  ll3 = rule3.left.get_length();
  lr3 = rule3.right.get_length();
  if (ll3 <= savell && lr3 <= savelr) save_rule(rule3);

  return;
}



/////////////////////////////////////////////////////////////////
//
//               rewriting_system::orient_rule
//
/////////////////////////////////////////////////////////////////

_boolean_t rewriting_system::orient_rule(
  rule  &rul
)
{
  long  cmp;

  cmp = (this->*compare)(rul.left, rul.right);

  if (cmp == 0) return TRUE;
  if (cmp == -1) rul.switch_sides();
  return FALSE;
}



/////////////////////////////////////////////////////////////////
//
//                rewriting_system::balance_rule
//
/////////////////////////////////////////////////////////////////

void rewriting_system::balance_rule(
  rule     &rul
)
{
  long                ll,lr,lrmax,i,g,linv,ls,lp,lb,j;
  long                sgn;
  _boolean_t             subword_only,changed,reduced;
  vector_long_segment lseg;
  rule_reference      exclude;   // Automatically null.

  // We assume the left and right sides are distinct, that the
  // rule is oriented correctly, and that both sides are reduced.

  if (IDENTITY == rul.r_type) {
    // Currently no balancing is done for identities.

    return;
  }

  nbalance++;

  if (rul.r_type == SUBWORD_RULE) subword_only = TRUE;
  else subword_only == FALSE;

  ll    = rul.left.get_length();
  lr    = rul.right.get_length();
  lrmax = rul.right.get_max_length();
  balance_vector.set_max_length(lrmax);

 loop:
  if (rul.r_type == SUBWORD_RULE) {
    // Cancel any common invertible generators from the front.

    i = 0;

    while (   i < ll && 
              i < lr &&
           (g = (rul.left)[i]) == (rul.right)[i] &&
            gens[g].get_invertible()                 ) i++;

    if (i > 0) {
      rul.left.truncate(i);
      rul.right.truncate(i);
      ll -= i;
      lr -= i;
    }
  }

  // Cancel common invertible generators from the end.

  while ( ll > 0 &&
          lr > 0 &&
         (g = rul.left[ll-1]) == rul.right[lr-1] &&
         gens[g].get_invertible()                   ) {
    ll--;
    lr--;
  }

  rul.left.set_length(ll);
  rul.right.set_length(lr);

  // Find the length of the longest obviously invertible suffix
  // of the left side (if any) and move its inverse to the end
  // of the right side.

  g = (rul.left)[ll-1];

  if (gens[g].get_invertible() ) {
    linv = gens[g].get_inv_length();
    ls = 1;

    while (ls < ll &&
           ls < linv &&
           (rul.left)[ll-ls-1] ==
                gens[g].get_inv_term(linv-ls) ) ls++;

    lb = lr + linv + 1 - ls;

    if (lb <= lrmax) {
      balance_vector.set_copy(rul.right);
      balance_vector.set_length(lb);

      for (j = 0; j < linv+1-ls; j++)
        balance_vector.set_entry(lr+j, gens[g].get_inv_term(j) );

      (this->*pRewrite)(
        subword_only,balance_vector,balance_vector,exclude,
        changed,reduced);

      if (!reduced) goto left_end;

      lseg.segment(rul.left,0,ll-ls);
      sgn = (this->*compare)(lseg,balance_vector);

      if (sgn > 0) {
        ll -= ls;
        rul.left.set_length(ll);
        rul.right.set_copy(balance_vector);
        lr = rul.right.get_length();
        goto loop;
      }
    }
  }

 left_end:
  if (rul.r_type == SUBWORD_RULE) {
    // Find the longest obviously invertible prefix of the left
    // side (if any) and move it to the front of the right side.

    g = (rul.left)[0];

    if (gens[g].get_invertible() ) {
      linv = gens[g].get_inv_length();
      lp = 1;

      while (lp < ll &&
             lp < linv &&
             (rul.left)[lp] ==
                  gens[g].get_inv_term(lp-1) ) lp++;

      lb = lr + linv + 1 - lp;

      if (lb <= lrmax) {
        for (j = 0; j < linv+1-lp; j++)
          balance_vector.set_entry(j,gens[g].get_inv_term(j) );
        balance_vector.set_length(linv+1-lp);
        balance_vector.append(rul.right);

        (this->*pRewrite)(subword_only,balance_vector,
                        balance_vector,exclude,changed,reduced);
        if (!reduced) goto no_more;

        lseg.segment(rul.left,1,ll-lp);
        sgn = (this->*compare)(lseg,balance_vector);

        if (sgn > 0) {
          rul.left.truncate(lp);
          ll -= lp;
          rul.right.set_copy(balance_vector);
          lr = rul.right.get_length();
          goto loop;
        }
      }
    }
  }

//  cout << " output " << endl;
//  rul.print();
//  cout << endl;

 no_more:
  return;
}



/////////////////////////////////////////////////////////////////
//
//             rewriting_system::heuristics (one rule)
//
/////////////////////////////////////////////////////////////////

void rewriting_system::heuristics(
  rule   &rul
)
{
  long                ll,lr,lrmax,i,g,linv,ls,lp,lb,j;
  long                sgn;
  _boolean_t             subword_only,changed,reduced;
  vector_long_segment lseg;
  rule_reference      exclude;   // Automatically null.

  if (IDENTITY == rul.r_type) {
    // Heuristics are not applied to identities.

    return;
  }

  // We assume the left and right sides are distinct and that
  // the rule is oriented correctly and balanced.

  heuristic_rule.set_max_length(bsize);

  if (rul.r_type == SUBWORD_RULE) subword_only = TRUE;
  else subword_only == FALSE;

  ll    = rul.left.get_length();
  lr    = rul.right.get_length();
  lrmax = rul.right.get_max_length();
  heuristic_vector.set_max_length(lrmax);

  // Find the length of the longest obviously invertible suffix
  // of the left side (if any) and move its inverse to the end
  // of the right side.

  g = (rul.left)[ll-1];

  if (gens[g].get_invertible() ) {
    linv = gens[g].get_inv_length();
    ls = 1;

    while (ls < ll &&
           ls < linv &&
           (rul.left)[ll-ls-1] ==
                gens[g].get_inv_term(linv-ls) ) ls++;

    lb = lr + linv + 1 - ls;

    if (lb <= lrmax) {
      heuristic_vector.set_copy(rul.right);
      heuristic_vector.set_length(lb);

      for (j = 0; j < linv+1-ls; j++)
        heuristic_vector.set_entry(lr+j,gens[g].get_inv_term(j));

      (this->*pRewrite)(subword_only,heuristic_vector,
                       heuristic_vector,exclude,changed,reduced);
      if (!reduced) goto left_end;

      lseg.segment(rul.left,0,ll-ls);
      sgn = (this->*compare)(lseg,heuristic_vector);

      if (sgn < 0) {
        heuristic_rule.right.set_copy(rul.left);
        heuristic_rule.right.set_length(ll-ls);
        heuristic_rule.left.set_copy(heuristic_vector);
        balance_rule(heuristic_rule);

        if (heuristic_rule.left.get_length() <= savell &&
            heuristic_rule.right.get_length() <= savelr   )
          save_rule(heuristic_rule);
      }
    }
  }

 left_end:
  if (rul.r_type == SUBWORD_RULE) {
    // Find the longest obviously invertible prefix of the left
    // side (if any) and move it to the front of the right side.

    g = (rul.left)[0];

    if (gens[g].get_invertible() ) {
      linv = gens[g].get_inv_length();
      lp = 1;

      while (lp < ll &&
             lp < linv &&
             (rul.left)[lp] ==
                  gens[g].get_inv_term(lp-1) ) lp++;

      lb = lr + linv + 1 - lp;

      if (lb <= lrmax) {
        for (j = 0; j < linv+1-lp; j++)
          heuristic_vector.set_entry(j,gens[g].get_inv_term(j) );
        heuristic_vector.set_length(linv+1-lp);
        heuristic_vector.append(rul.right);

        (this->*pRewrite)(subword_only,heuristic_vector,
                      heuristic_vector,exclude,changed,reduced);
        if (!reduced) goto no_more;

        lseg.segment(rul.left,lp,ll-lp);
        sgn = (this->*compare)(lseg,heuristic_vector);

        if (sgn < 0) {
          heuristic_rule.right.set_copy(rul.left);
          heuristic_rule.right.truncate(lp);
          heuristic_rule.left.set_copy(heuristic_vector);
          balance_rule(heuristic_rule);

          if (heuristic_rule.left.get_length() <= savell &&
              heuristic_rule.right.get_length() <= savelr   )
            save_rule(heuristic_rule);
        }
      }
    }
  }

 no_more:
  return;
}



/////////////////////////////////////////////////////////////////
//
//         rewriting_system::heuristics (save_list)
//
/////////////////////////////////////////////////////////////////

void rewriting_system::heuristics(void)
{
  int          more;
  subsystem   *subsyst;

  if (print_level >= 1)
    cout << "Applying heuristics to entire save list." << endl;

  more = 1;

  while (more) {
    more = 0;
    subsyst = first_save;

    while (subsyst != NULL) {
      more = more || subsyst->heuristics();
      subsyst = subsyst->get_next_save();
    }
  }

  return;
}



/////////////////////////////////////////////////////////////////
//
//           rewriting_system::initialize_save_list
//
/////////////////////////////////////////////////////////////////

void rewriting_system::initialize_save_list(
  rule_type  rtp,
  long       enm,    // Estimated number of rules.
  long       ell,    // Estimated length of left sides.
  long       elr     // Estimated legnth of right sides.
)
{
  long        minll,tlen,dim;
  subsystem  *subsyst;

  minll = max( (3*ell)/4, ell-2);
  minll = max(minll,1);

  subsyst = new subsystem(this,rtp,RABIN_KARP);
  subsyst->make_rabin_karp(enm,minll);

  nsave = 0;
  first_save = last_save = subsyst;
  add_subsystem(subsyst);
}



/////////////////////////////////////////////////////////////////
//
//              rewriting_system::append_save_list
//
/////////////////////////////////////////////////////////////////

void rewriting_system::append_save_list(
  subsystem  *subsyst
)
{
  subsystem   *old_subs, *subs = first_save;

//  subs = first_save;

  while (subs != NULL) {
    subsyst->append(subs);
    old_subs = subs;
    subs     = subs->get_next_save();
    remove_subsystem(old_subs);
  }
}



/////////////////////////////////////////////////////////////////
//
//               rewriting_system::remove_subsystem
//
/////////////////////////////////////////////////////////////////

void rewriting_system::remove_subsystem(
  subsystem   *subsyst
)
{
  subsystem  *nsubs,*psubs,*nsave,*psave;

  nsubs = subsyst->get_next_subsystem();
  psubs = subsyst->get_prev_subsystem();
  nsave = subsyst->get_next_save();
  psave = subsyst->get_prev_save();

  if (first_subsystem == subsyst) first_subsystem = nsubs;
  if (last_subsystem  == subsyst) last_subsystem  = psubs;
  if (first_save      == subsyst) first_save      = nsave;
  if (last_save       == subsyst) last_save       = psave;

  if (nsubs != NULL) nsubs->set_prev_subsystem(psubs);
  if (psubs != NULL) psubs->set_next_subsystem(nsubs);
  if (nsave != NULL) nsave->set_prev_save(psave);
  if (psave != NULL) psave->set_next_save(nsave);

  n_sub--;

  delete subsyst;
}



/////////////////////////////////////////////////////////////////
//
//                rewriting_system::save_rule
//
/////////////////////////////////////////////////////////////////

void rewriting_system::save_rule(
  rule    &rul
)
{
  long        ll,win,pwin,pnum,dim;
  rule_type   rtype;
  subsystem  *subsyst,*prev,*subsyst1;
  
  nsave++;

  subsyst = first_save;
  if (subsyst == NULL)
    error("save list not initialized.");

  ll = rul.left.get_length();

  if (ll <= logll) {
    rtype = subsyst->get_r_type();

    if      (rtype == SUBWORD_RULE) log_stream << "S ";
    else if (rtype == PREFIX_RULE)  log_stream << "P ";
    else if (rtype == IDENTITY)     log_stream << "I ";

    print_word(log_stream,LEN_6BIT,rul.left);
    log_stream << " ";
    print_word(log_stream,LEN_6BIT,rul.right);
    log_stream << endl;
  }


  prev = NULL;

  while (subsyst != NULL) {
    win = subsyst->get_window();
    if (win <= ll) break;
    prev = subsyst;
    subsyst = subsyst->get_next_save();
  }

  if (prev == NULL) {
    subsyst->add_rule(rul,TRUE,FALSE);
    return;
  }

  pwin = prev->get_window();
  pnum = prev->get_nrules();

  while(pwin > ll) {
    pwin /= 2;
    pnum /= 2;
  }

  if (subsyst != NULL && pwin == win) {
    subsyst->add_rule(rul,TRUE,FALSE);
    return;
  }

  subsyst1 = new subsystem(this,rul.r_type,RABIN_KARP);
  subsyst1->make_rabin_karp(pnum,pwin);
  prev->set_next_save(subsyst1);
  subsyst1->set_prev_save(prev);
  subsyst1->set_next_save(subsyst);

  if (subsyst != NULL) subsyst->set_prev_save(subsyst1);
  else last_save = subsyst1;

  add_subsystem(subsyst1);
  subsyst1->add_rule(rul,TRUE,FALSE);
}



/////////////////////////////////////////////////////////////////
//
//                   rewriting_system::kb_basic
//
/////////////////////////////////////////////////////////////////

//  Keep just one primary subsystem of each type.  Return TRUE if
//  there is a change and FALSE otherwise.

_boolean_t rewriting_system::kb_basic(
  long      maxlen,      // the maximum length of overlaps 
  long     &num_iter,    // the maximum number of calls to
                         // overlap allowed.  a value of -1
                         // means infinitely many calls are
                         // permitted.
  long      prevlen,     // all overlaps of length up to this
                         // value have been processed.
  double    factor       // if the number of rules in a subsystem
                         // increases by more than this factor,
                         // the processing of overlaps is
                         // stopped, the rules are mutually
                         // reduced, and overlap processing begun
                         // again.
)
{
  rule_type    type0;
  _boolean_t      successful,confluent;
  long         nrules0,pass_number;
  long         nrules1,est_num,est_ll,est_lr;
  long         maxllnew,start_orig,minlen;
  long         navoid,nnew,neworig,change0,change1,maxsave;
  long         first_pass;
  subsystem  *subsyst0,*subsyst1,*avoid[1];
  
  if (print_level >= 1)
    cout << "\nkb_basic:  maxlen = " << maxlen 
         << ", num_iter = " << num_iter << endl << endl;

  // Set parameters used in reduce_all_subsystems.

  // There should be at most two subsystems and if there are two,
  // the first one should be a subword system and the second one
  // should a prefix system.

  confluent = TRUE;

  if (n_sub    == 0 ||
      num_iter == 0   ) return FALSE;

  if (n_sub > 2) {
    cout << "too many subsystems in kb_basic" << endl;
    exit(1);
  }

  subsyst0   = first_subsystem;
  type0      = subsyst0->get_r_type();
  nrules0    = subsyst0->get_nrules();

  if (type0 == PREFIX_RULE) {
    if (n_sub > 1) goto invalid_type;
    if (nrules0 == 0) return TRUE;

    // In the case of prefix rules only, forming overlaps is the
    // same as mutual reduction.

    est_num = max(100,nrules0/2);
    subsyst0->average_lengths(est_ll,est_lr);

    initialize_save_list(PREFIX_RULE,est_num,est_ll,est_lr);
    (void) reduce_all_subsystems(PREFIX_RULE);
    if (broken_prefix_rules != NULL) confluent = FALSE;
    append_save_list(subsyst0);
    if (subsyst0->origin_in_use() ) subsyst0->make_origin();
    subsyst0->make_index();

    return confluent;
  }

  // Form overlaps of subword rules.

  pass_number = 0;
  first_pass = TRUE;
  if (num_iter == 0) return FALSE;

  do {
    pass_number++;

    if (print_level >= 2)
      cout << "Pass number " << pass_number
           << " on subword rules" << endl;

    change0 = FALSE;
    nrules0 = subsyst0->get_nrules();
    maxsave = (long) (factor*nrules0 + 10);
    subsyst0->average_lengths(est_ll,est_lr);

    if (print_level >= 2)
      cout << "There are " << nrules0 << " rules" << endl;

    initialize_save_list(SUBWORD_RULE,maxsave,est_ll,est_lr);

    start_orig = 0;

    if (first_pass) {
      minlen     = prevlen+1;
      first_pass = FALSE;
    }

    else minlen = 0;

    navoid     = 1;
    avoid[0]   = subsyst0;

    overlap(subsyst0,subsyst0,start_orig,minlen,maxlen,
            maxsave,avoid,navoid,neworig,nnew,successful);

    confluent = confluent && successful;

    if (nnew > 0) {
      change0 = TRUE;
    }

    (void) reduce_all_subsystems(SUBWORD_RULE);
    if (broken_subword_rules != NULL) confluent = FALSE;
    append_save_list(subsyst0);

    if (subsyst0->origin_in_use() ) subsyst0->make_origin();
    subsyst0->make_index();
    if (num_iter > 0) num_iter--;
  }
  while (change0 && num_iter != 0);

  if (change0) confluent = FALSE;
  if (n_sub == 1) return confluent;

  subsyst1   = subsyst0->get_next_subsystem();
  nrules1    = subsyst1->get_nrules();

  if (subsyst1->get_r_type() != PREFIX_RULE) goto invalid_type;
  if (nrules1 == 0) return confluent;

  // Since new subword rules may have been found, the prefix
  // rules may not be reduced.

  // Form overlaps of prefix rules with subword rules.

  pass_number = 0;

  do {
    pass_number++;

    if (print_level >= 2)
      cout << "Pass number " << pass_number << " on prefix rules"
           << endl;

    change1 = FALSE;
    nrules1 = subsyst1->get_nrules();
    maxsave = (long) (factor*nrules1 + 10);
    subsyst1->average_lengths(est_ll,est_lr);

    if (print_level >= 2)
      cout << "There are " << nrules1 << " rules" << endl;

    initialize_save_list(PREFIX_RULE,maxsave,est_ll,est_lr);

    start_orig = 0;

    // Too many things may have happened to assume that
    // prevlen is still valid.

    minlen     = 0;
    navoid     = 1;
    avoid[0]   = subsyst0;

    overlap(subsyst1,subsyst0,start_orig,minlen,maxlen,
            maxsave,avoid,navoid,neworig,nnew,successful);

    confluent = confluent && successful;
    if (num_iter > 0) num_iter--;
    if (nnew > 0) change1 = TRUE;

    // Mutually reducing all prefix subsystems includes forming
    // all overlaps between the prefix rules.

    (void) reduce_all_subsystems(PREFIX_RULE);
    if (broken_prefix_rules != NULL) confluent = FALSE;
    append_save_list(subsyst1);
    subsyst1->make_index();
  }
  while (change1 && num_iter != 0);

  return confluent;

 invalid_type:
  error("invalid subsystem type in kb_basic");
  return confluent;
}


/////////////////////////////////////////////////////////////////
//
//                   system::kb_two_step
//
/////////////////////////////////////////////////////////////////

/*
    Keep two primary subsystems of each type.  Return TRUE
    if there is a change and FALSE otherwise.
*/

_boolean_t rewriting_system::kb_two_step(
  long      maxlen,      // the maximum length of overlaps
  long     &num_iter,    // the maximum number of basic steps
                         // allowed.  a value of -1
                         // means infinitely many calls are
                         // permitted.
  long      prevlen,     // all overlaps of length up to this
                         // value have been processed.
  double    factor       // if the number of rules in a subsystem
                         // increases by more than this factor,
                         // the processing of overlaps is
                         // stopped, the rules are mutually
                         // reduced, and overlap processing begun
                         // again.
)
{
  long         savell,savelr,est_ll,est_lr,est_num,dim;
  long         next_minlen,minlen,navoid,nrules0,nrules1,nrules2;
  long         nnew,nnew01,nnew10,nnew11,pass_number;
  long         maxsave,start_orig,neworig,maxllnew;
  _boolean_t      change0,successful,confluent;
  rule_type    type,type0,type1;
  index_type   itype0,itype1;
  subsystem   *avoid[2],*subsyst;
  subsystem   *prefix0,*prefix1,*subword0,*subword1;
  subsystem   *identity0;
  
  if (print_level >= 1)
    cout << "\nkb_two_step:  maxlen = " << maxlen 
         << ", num_iter = " << num_iter << endl;

  if (n_sub == 0 || num_iter   == 0 ) return FALSE;

  prefix0 = subword0 = identity0 = NULL;

  subsyst  = first_subsystem;

  while (subsyst != NULL) {
    type = subsyst->get_r_type();

    if (type == PREFIX_RULE && prefix0 == NULL)
      prefix0 = subsyst;
    else if (type == SUBWORD_RULE && subword0 == NULL)
      subword0 = subsyst;
    else if (type == IDENTITY && identity0 == NULL)
      identity0 = subsyst;

    subsyst = subsyst->get_next_subsystem();
  }

  confluent = TRUE;
  next_minlen = prevlen + 1;

  // Form overlaps of subword rules, if any.

  if (subword0 == NULL) goto prefix;

 subword:
  nrules0 = subword0->get_nrules();
  itype0  = subword0->get_i_type();

  if (print_level >= 2)
    cout << "First pass on subword rules" << endl
         << "There are " << nrules0 << " rules." << endl;

  if (num_iter == 0) return FALSE;

  change0 = FALSE;
  maxsave = (long) (factor*nrules0 + 10);
  subword0->average_lengths(est_ll,est_lr);

  initialize_save_list(SUBWORD_RULE,maxsave,est_ll,est_lr);

  start_orig = 0;
  minlen = next_minlen;
  next_minlen = 0;
  navoid     = 1;
  avoid[0]   = subword0;

  overlap(subword0,subword0,start_orig,minlen,maxlen,
          maxsave,avoid,navoid,neworig,nnew,successful);

  confluent = confluent && successful;

  if (broken_subword_rules != NULL) {
    (void) broken_subword_rules->reduce();

    if (0 == broken_subword_rules->get_nrules() ) {
      delete broken_subword_rules;
      broken_subword_rules = NULL;
    }
  }

  if (num_iter > 0) num_iter--;

  if (nsave > 0) {
    change0 = TRUE;

    if (neworig != -1) {
      // processing of overlaps aborted

      (void) reduce_all_subsystems(SUBWORD_RULE);
      append_save_list(subword0);
      if (subword0->origin_in_use() ) subword0->make_origin();
      subword0->make_index();
      goto subword;
    }

    if (use_heuristics) heuristics();
    (void) reduce_save_list();
    if (broken_subword_rules != NULL) confluent = FALSE;
    subword1 = new subsystem(this,SUBWORD_RULE,itype0);
                
    add_subsystem(subword1);
    append_save_list(subword1);

    if (subword1->origin_in_use() ) subword1->make_origin();
    subword1->make_index();
  }

  else {
    append_save_list(subword0);
  }

  pass_number = 1;

  while (change0) {
    nrules0 = subword0->get_nrules();
    nrules1 = subword1->get_nrules();

    if (num_iter == 0) {
      maxsave = nrules0+ nrules1 + 100;  // Is 100 needed?
      subword0->average_lengths(est_ll,est_lr);
      initialize_save_list(SUBWORD_RULE,maxsave,est_ll,est_lr);
      (void) reduce_all_subsystems(SUBWORD_RULE);
      subword0->append(subword1);
      remove_subsystem(subword1);
      append_save_list(subword0);

      if (subword0->origin_in_use() ) subword0->make_origin();
      subword0->make_index();
      return FALSE;
    }

    change0 = FALSE;
    pass_number++;

    if (print_level >= 2)
      cout << "Pass number " << pass_number
           << " on subword rules"<< endl;

    maxsave = (long) (factor*(nrules0 + nrules1) + 10);
    subword1->average_lengths(est_ll,est_lr);
    initialize_save_list(SUBWORD_RULE,maxsave,est_ll,est_lr);
    (void) subword0->reduce();
    nrules0 = subword0->get_nrules();

    if (print_level >= 2)
      cout << "There are " << nrules0 << " old rules and "
           << nrules1 << " new ones." << endl;

    if (nrules0 == 0 && nrules1 == 0) {
      printf("Where have all the rules gone in twostep?\n");
      exit(1);
    }

    start_orig = 0;
    minlen     = 0;
    avoid[0]   = subword0;
    avoid[1]   = subword1;
    navoid     = 2;

    nnew01 = nnew10 = nnew11 = 0;

    overlap(subword0,subword1,start_orig,minlen,maxlen,
            maxsave,avoid,navoid,neworig,nnew01,successful);

    confluent = confluent && successful;

    if (neworig == -1) {
      overlap(subword1,subword0,start_orig,minlen,maxlen,
              maxsave,avoid,navoid,neworig,nnew10,successful);

      confluent = confluent && successful;
    }

    if (neworig == -1) {
      overlap(subword1,subword1,start_orig,minlen,maxlen,
              maxsave,avoid,navoid,neworig,nnew11,successful);

      confluent = confluent && successful;
    }

    if (num_iter > 0) num_iter--;

    if (broken_subword_rules != NULL) {
      (void) broken_subword_rules->reduce();

      if (0 == broken_subword_rules->get_nrules() ) {
        delete broken_subword_rules;
        broken_subword_rules = NULL;
      }
    }

    if (nsave > 0) {
      change0 = TRUE;

      if (neworig == -1) {
        if (use_heuristics) heuristics();
        (void) reduce_save_list();
        if (broken_subword_rules != NULL) confluent = FALSE;

        subword0->append(subword1);
        remove_subsystem(subword1);
        subword1 = new subsystem(this,SUBWORD_RULE,itype0);

        add_subsystem(subword1);  
        append_save_list(subword1);

        if (subword0->origin_in_use() ) {
          subword0->make_origin();
          subword1->make_origin();
        }

        subword0->make_index();
        subword1->make_index();
      }

      else {
        (void) reduce_all_subsystems(SUBWORD_RULE);
        if (broken_subword_rules != NULL) confluent = FALSE;
        subword0->append(subword1);
        remove_subsystem(subword1);
        append_save_list(subword0);

        if (subword0->origin_in_use() )
          subword0->make_origin();

        subword0->make_index();
        goto subword;
      }
    }

    else {
      append_save_list(subword1);
      subword0->append(subword1);
      remove_subsystem(subword1);
      if (subword0->origin_in_use() ) subword0->make_origin();
      subword0->make_index();
    }
  }

  // Now consider prefix rules.

 prefix:
  if (prefix0 == NULL) return confluent;

  nrules0 = prefix0->get_nrules();
  itype0  = prefix0->get_i_type();

  if (print_level >= 2)
    cout << endl << "Reducing old prefix rules" << endl;

  est_num = max(100,nrules0/2);
  prefix0->average_lengths(est_ll,est_lr);
  initialize_save_list(PREFIX_RULE,est_num,est_ll,est_lr);
  (void) reduce_all_subsystems(PREFIX_RULE);
  if (broken_prefix_rules != NULL) confluent = FALSE;
  append_save_list(prefix0);
  prefix0->make_index();

  // Form overlaps of prefix rules with subword rules.

 prefix_loop:
  if (num_iter == 0) return FALSE;
  if (subword0 == NULL) return TRUE;

  if (print_level >= 2)
    cout << "First pass on prefix rules" << endl;

  pass_number = 1;
  nrules0 = prefix0->get_nrules();

  if (nrules0 == 0) return confluent;

  if (print_level >= 2)
    cout << "There are " << nrules0 << " prefix rules" << endl;

  change0 = FALSE;
  maxsave = (long) (factor*nrules0 + 10);
  prefix0->average_lengths(est_ll,est_lr);

  initialize_save_list(PREFIX_RULE,maxsave,est_ll,est_lr);

  start_orig = 0;
  minlen     = 0;
  navoid     = 1;
  avoid[0]   = subword0;

  overlap(prefix0,subword0,start_orig,minlen,maxlen,
          maxsave,avoid,navoid,neworig,nnew,successful);

  confluent = confluent && successful;

  if (num_iter > 0) num_iter--;

  if (nnew > 0) {
    change0 = TRUE;
    (void) reduce_all_subsystems(PREFIX_RULE);
     if (broken_prefix_rules != NULL) confluent = FALSE;

    if (neworig != -1) {
      // processing of overlaps aborted

      append_save_list(prefix0);
      prefix0->make_index();
      goto prefix_loop;
    }

    prefix1 = new subsystem(this,PREFIX_RULE,itype0);

    add_subsystem(prefix1);
    append_save_list(prefix1);

    prefix1->make_index();
  }

  else {
    append_save_list(prefix0);  // This just clears save list.
    return confluent;
  }

  // Make additional passes.

  while (change0) {
    if (num_iter == 0) {
      prefix0->append(prefix1);
      remove_subsystem(prefix1);
      prefix0->make_index();
      return FALSE;
    }

    pass_number++;

    if (print_level >= 2)
      cout << "Pass number " << pass_number
           << " on prefix rules" << endl;

    change0 = FALSE;
    nrules0 = prefix0->get_nrules();
    nrules1 = prefix1->get_nrules();

    if (nrules1 == 0) break;

    if (print_level >= 2)
      cout << "There are " << nrules0 << " old prefix rules and "
           << nrules1 << " new ones" << endl;

    maxsave = (long) (factor*(nrules0 + nrules1) + 10);
    prefix0->average_lengths(est_ll,est_lr);

    initialize_save_list(PREFIX_RULE,maxsave,est_ll,est_lr);

    start_orig = 0;
    minlen     = 0;
    avoid[0]   = subword0;
    navoid = 1;

    overlap(prefix1,subword0,start_orig,minlen,maxlen,
            maxsave,avoid,navoid,neworig,nnew,successful);

    confluent = confluent && successful;

    if (nnew > 0) {
      change0 = TRUE;
      (void) reduce_all_subsystems(PREFIX_RULE);
      if (broken_prefix_rules != NULL) confluent = FALSE;
      prefix0->append(prefix1);
      remove_subsystem(prefix1);
      prefix1 = new subsystem(this,PREFIX_RULE,itype0);
                  
      add_subsystem(prefix1);
      append_save_list(prefix1);

      if (neworig == -1) {
        prefix0->make_index();
        prefix1->make_index();
      }

      else {
        prefix0->append(prefix1);
        remove_subsystem(prefix1);
        append_save_list(prefix0);
        prefix0->make_index();
        goto prefix_loop;
      }
    }

    else {
      prefix0->append(prefix1);
      remove_subsystem(prefix1);
      append_save_list(prefix0);  // This just clears save list.
      prefix0->make_index();
    }
  }

 return confluent;

 invalid_type:
  printf("invalid subsystem type in twostep\n");
  exit(1);
}



/////////////////////////////////////////////////////////////////
//
//           rewriting_system::kb_random_two_step
//
/////////////////////////////////////////////////////////////////

_boolean_t rewriting_system::kb_random_two_step(
  long      maxlen,      // the maximum length of overlaps
  rule_type rtype,       // the type of AB in overlaps ABC
  long      ntrials      // the number of trials
)
{
  long         savell,savelr,est_ll,est_lr,est_num,dim;
  long         next_minlen,minlen,navoid,nrules0,nrules1,nrules2;
  long         nnew,nnew01,nnew10,nnew11,pass_number;
  long         maxsave,start_orig,neworig,maxllnew,ii;
  _boolean_t      change0,successful,confluent;
  rule_type    type,type0,type1;
  index_type   itype0,itype1;
  subsystem   *avoid[2],*subsyst;
  subsystem   *prefix0,*prefix1,*subword0,*subword1;
  subsystem   *identity0;
  
  if (print_level >= 1) {
    cout << "\nkb_random_two_step:  maxlen = " << maxlen 
         << ", ntrials = " << ntrials << endl;

    if (SUBWORD_RULE == rtype) cout << "Subword rules" << endl;
    else cout << "Prefix rules" << endl;
  }

  if (n_sub == 0) return FALSE;

  prefix0 = subword0 = identity0 = NULL;

  subsyst  = first_subsystem;

  while (subsyst != NULL) {
    type = subsyst->get_r_type();

    if (type == PREFIX_RULE && prefix0 == NULL)
      prefix0 = subsyst;
    else if (type == SUBWORD_RULE && subword0 == NULL)
      subword0 = subsyst;
    else if (type == IDENTITY && identity0 == NULL)
      identity0 = subsyst;

    subsyst = subsyst->get_next_subsystem();
  }

  if (SUBWORD_RULE == rtype) {
    // Form overlaps of subword rules, if any.

    if (subword0 == NULL) {
      cout << "There are no subword rules." << endl;
      return FALSE;
    }

    nrules0 = subword0->get_nrules();
    if (0 >= nrules0) return FALSE;

    subword0->average_lengths(est_ll,est_lr);
    initialize_save_list(SUBWORD_RULE,1000,est_ll,est_lr);

    navoid     = 1;
    avoid[0]   = subword0;

    for (ii = 0; ii < ntrials; ii++)
      random_overlap(subword0,subword0,maxlen,avoid,navoid);

    cout << "New rules found:  " << nsave << endl;

    if (nsave > 0) {
      if (use_heuristics) heuristics();
      (void) reduce_all_subsystems(SUBWORD_RULE);
      append_save_list(subword0);
      if (subword0->origin_in_use() ) subword0->make_origin();
      subword0->make_index();

      if (NULL != prefix0) {
        nrules0 = prefix0->get_nrules();
        if (0 >= nrules0) return FALSE;
        if (NULL == subword0) return FALSE;

        est_num = max(100,nrules0/2);
        prefix0->average_lengths(est_ll,est_lr);
        initialize_save_list(PREFIX_RULE,est_num,est_ll,est_lr);
        (void) reduce_all_subsystems(PREFIX_RULE);
        append_save_list(prefix0);
        prefix0->make_index();
      }

      return TRUE;
    }

    else {
      append_save_list(subword0);
      return FALSE;
    }
  }

  // We must consider prefix rules.

  else if (PREFIX_RULE == rtype) {
    if (NULL == prefix0) {
      cout << "There are no prefix rules." << endl;
      return FALSE;
    }

    nrules0 = prefix0->get_nrules();
    if (0 >= nrules0) return FALSE;
    if (NULL == subword0) return FALSE;

    est_num = max(100,nrules0/2);
    prefix0->average_lengths(est_ll,est_lr);
    initialize_save_list(PREFIX_RULE,est_num,est_ll,est_lr);

    navoid     = 1;
    avoid[0]   = subword0;

    for (ii = 0; ii < ntrials; ii++)
      random_overlap(prefix0,subword0,maxlen,avoid,navoid);

    if (nsave > 0) {
      (void) reduce_all_subsystems(PREFIX_RULE);
      append_save_list(prefix0);
      prefix0->make_index();
      return TRUE;
    }

    else {
      append_save_list(prefix0);  // This just clears save list.
      return FALSE;
    }
  }

  return FALSE;  // This line should not be executed.
}



/////////////////////////////////////////////////////////////////
//
//              rewriting_system::compare_packed_rules
//
/////////////////////////////////////////////////////////////////

long rewriting_system::compare_packed_rules(
  sort_type    stp,
  packed_rule  v_rule,
  packed_rule  w_rule
)
{
  long    lex;

  switch(stp) {
    case LEN:
      if (v_rule.left.length == w_rule.left.length)
        return 0;

      else if (v_rule.left.length < w_rule.left.length)
        return -1;

      else return 1;

    case LEX:
      return compare_lex(v_rule.left, w_rule.left);

    case LENLENLEXLEX:
      if (v_rule.left.length != w_rule.left.length)
        if (v_rule.left.length < w_rule.left.length)
          return -1;

        else return -1;

      if (v_rule.right.length != w_rule.right.length)
        if (v_rule.right.length < w_rule.right.length)
          return -1;

        else return 1;

      if (0 != (lex = compare_lex(v_rule.left,w_rule.left) ) )
        return lex;

      return compare_lex(v_rule.right,w_rule.right);


    case LENLEXLENLEX:
      if (v_rule.left.length != w_rule.left.length)
        if (v_rule.left.length < w_rule.left.length)
          return -1;

        else return -1;

      if (0 != (lex = compare_lex(v_rule.left,w_rule.left) ) )
        return lex;

      if (v_rule.right.length != w_rule.right.length)
        if (v_rule.right.length < w_rule.right.length)
          return -1;

        else return 1;

      return compare_lex(v_rule.right,w_rule.right);
  }

  return 0;  // This is unnecessary, but it makes the compiler
             // happy.
}



/////////////////////////////////////////////////////////////////
//
//                  rewriting_system::compare_lex
//
/////////////////////////////////////////////////////////////////

long rewriting_system::compare_lex(
  word_reference  u_ref,
  word_reference  v_ref
)
{
  long           i,llu,llv,llm,gu,gv;
  gen_extractor  u_ext(u_ref.subsyst);
  gen_extractor  v_ext(v_ref.subsyst);

  llu = u_ref.length;
  llv = v_ref.length;
  llm = min(llu,llv);

  u_ext.reset(u_ref.origin,u_ref.bit_pos);
  v_ext.reset(v_ref.origin,v_ref.bit_pos);

  for (i = 0; i < llm; i++) {
    gu = u_ext.next();
    gv = v_ext.next();

    if (gu != gv)
      if (gu < gv) return -1;
      else return 1;
  }

  if (llu != llv)
    if (llu < llv) return -1;
    else return 1;

  return 0;
}



/////////////////////////////////////////////////////////////////
//
//                    rewriting_system::write
//
/////////////////////////////////////////////////////////////////

void rewriting_system::write(
  char  *file_name
)
{
  long       i,j,wt0,wt1,lv0,lv1,ninv,num,nsub;
  char       file[50];
  subsystem *subsyst;
  fstream    stream;
  
  if (strlen(file_name) > 0) {
    if (strlen(file_name) > 45)
      error("Name too long in rewriting_system::write");
    strcpy(file,file_name);
    strcat(file,".sys");
    stream.open(file, ios::out);
  }
  
  else system_file.open(stream);
  
  stream << "system" << endl;
  stream << "word_type:  ";
  
  if (a_type == MONOID) stream << "monoid" << endl;
  else stream << "group" << endl;

  if (a_type == GROUP) {  
    stream << "ngens:  " << n_gens << endl;
    stream << "naming_convention:  none" << endl;
    stream << "numbering_convention:  none" << endl;
    stream << "inverse_convention:  none" << endl;
    stream << "generators:" << endl;
    
    for (i = 0; i < t_gens; i++) {
      gens[i].print_name(stream);
      stream << endl;
    }
    
    stream << "inverse_pairs:" << endl;
    
    for (i = 0; i < t_gens; i++) {
      j = gens[i].get_inv_numb();
      
      if (i < j) {
        gens[i].print_name(stream);
        stream << "  ";
        gens[j].print_name(stream);
        stream << endl;
      }
    }
  } // a_type == GROUP
  
  else { // a_type == MONOID
    stream << "ngens:  " << n_gens << endl;
    stream << "naming_convention:  none" << endl;
    stream << "generators:" << endl;
    
    for (i = 0; i < n_gens; i++) {
      gens[i].print_name(stream);
      stream << endl;
    }
    
    stream << "ninv:  ";
    
    ninv = 0;
    
    for (i = 0; i < n_gens; i++) 
      if (gens[i].get_invertible() )  ninv++;
      
    stream << ninv << endl;

    if (ninv > 0) {
      stream << "inverse_pairs:" << endl;

      for (i = 0; i < n_gens; i++)
        if (gens[i].get_invertible() ) {
          gens[i].print_name(stream);
          stream << "  ";
          print_inverse(stream,gens[i]);
          stream << endl;
        }
    }
  }  // a_type == MONOID
  
  stream << "weight_convention:  ";
  
  if (n_gens == 0) stream << "none" << endl;
  
  else {
    stream << "weights:" << endl;
    wt1 = wt0 = gens[0].get_weight();
    
    for (i = 1; i < t_gens; i++)
      if (wt0 != (wt1 = gens[i].get_weight() ) ) break;
      
    if (wt1 == wt0) stream << "constant" << endl;
    
    else {
      stream << "none" << endl;
      
      for (i = 0; i < t_gens; i++)
        stream << gens[i].get_weight() << endl;
    }
  }
  
  stream << "level_convention:  ";
  
  if (n_gens == 0) stream << "none" << endl << "levels:" << endl;
  
  else {
    lv1 = lv0 = gens[0].get_level();
    
    for (i = 1; i < t_gens; i++)
      if (lv0 != (lv1 = gens[i].get_level() ) )break;
      
    if (lv1 == lv0) stream << "constant" << endl;
    
    else {
      stream << "none" << endl << "levels:" << endl;
      
      for (i = 0; i < t_gens; i++)
        stream << gens[i].get_level() << endl;
    }
  }

  nsub = n_sub;
  if (broken_prefix_rules != NULL) nsub++;
  if (broken_subword_rules != NULL) nsub++;

  stream << "nsub:  " << nsub << endl;
  stream << "subsystems:  " << endl;
  
  subsyst = first_subsystem;
  num     = 1;
  
  while (subsyst != NULL) {
    subsyst->write(stream,file_name,num++);
    subsyst = subsyst->get_next_subsystem();
  }
  
  if (broken_prefix_rules != NULL)
    broken_prefix_rules->write(stream,file_name,num++);

  if (broken_subword_rules != NULL)
    broken_subword_rules->write(stream,file_name,num++);

  stream.close();
}



/////////////////////////////////////////////////////////////////
//
//                 rewriting_system::print_word
//
/////////////////////////////////////////////////////////////////

void rewriting_system::print_word(
  ostream       &stream,
  external_form  ext_form,
  vector_long   &word
)
{
  long   i,j,len,wbits,zbits,nchar,nb;
  ulong  z,w;
  
  len = word.get_length();
  
  if (ext_form == LEN_NAMES   ||
      ext_form == LEN_NUMBERS ||
      ext_form == LEN_6BIT      )
    stream << len << " ";
    
  if (ext_form == NAMES    ||
      ext_form == LEN_NAMES  )
    if (len == 0) stream << "@";
    
    else
      for (i = 0; i < len; i++)
        gens[word[i]].print_name(stream);
  
  else if (ext_form == LEN_NUMBERS)
    for (i = 0; i < len; i++) {
      stream << word[i];
      if (i < len-1) stream << " ";
    }
    
  else if (ext_form == LEN_6BIT)
    if (len == 0) stream << "@";
    
    else {
      nchar = (n_bits*len+5)/6;
      j = 0;
      z = w = 0;
      zbits = wbits = 0;
      
      for (i = 0; i < nchar; i++) {
        while (zbits < 6) {
          if (wbits == 0)
            if (j >= len) {
              z = z<<(6-zbits);
              zbits = 6;
            }
            
            else {
              w     = word[j++];
              wbits = n_bits;
            }
            
          if (wbits+zbits <= 6) {
            z      = (z<<wbits)|(w&mask[wbits]);
            zbits += wbits;
            wbits  = 0;
          }
          
          else {
            nb     = 6 - zbits;
            z      = (z<<nb)|((w>>(wbits-nb))&mask[nb]);
            zbits  = 6;
            wbits -= nb;
          }
        }
        
        stream << (char) (z+48);
        zbits = z = 0;
      }
    }
}



/////////////////////////////////////////////////////////////////
//
//                 rewriting_system::print_counts
//
/////////////////////////////////////////////////////////////////

void rewriting_system::print_counts()
{
  cout << endl;
  
  cout << "n_rewrite      = " << setw(12) << n_rewrite <<
       "   total_input    = " << setw(12) << total_input << endl;
  
  cout << "n_applications = " << setw(12) << n_applications <<
       "   total_left     = " << setw(12) << total_left << endl;
       
  cout << "noverlaps      = " << setw(12) << noverlaps <<
       "   nbalance       = " << setw(12) << nbalance << endl;
}



/////////////////////////////////////////////////////////////////
//
//                       ask_rewrite_verbose
//
/////////////////////////////////////////////////////////////////

void rewriting_system::ask_rewrite_verbose(void)
{
  cout << "Do you want rewrite reporting turned on (y/n):  ";

  if (YES) set_rewrite_verbose(TRUE);
  else set_rewrite_verbose(FALSE);

  return;
}



/////////////////////////////////////////////////////////////////
//
//             rewriting_system::save_broken_rule
//
/////////////////////////////////////////////////////////////////

void rewriting_system::save_broken_rule(
  rule  &rul
)
{
  if (rul.r_type == SUBWORD_RULE) {
    if (broken_subword_rules == NULL)
      broken_subword_rules =
            new subsystem(this,SUBWORD_RULE,NO_INDEX);

    broken_subword_rules->add_rule(rul,FALSE,FALSE);
  }

  else if (rul.r_type == PREFIX_RULE) {
    if (broken_prefix_rules == NULL)
      broken_prefix_rules =
            new subsystem(this,PREFIX_RULE,NO_INDEX);

    broken_prefix_rules->add_rule(rul,FALSE,FALSE);
  }

  else if (IDENTITY == rul.r_type) {
    // Identities should not occur as broken rules.
  }

  return;
}



/////////////////////////////////////////////////////////////////
//
//             rewriting_system::save_inverses
//
/////////////////////////////////////////////////////////////////

void rewriting_system::save_inverses(void)
{
  long      i,j,linv;
  rule      rul;

  rul.left.set_max_length(20);
  rul.right.set_length(0);

  for (i = 0; i < t_gens; i++)
    if(gens[i].get_invertible() ) {
      linv = gens[i].get_inv_length();
      rul.left.set_max_length(linv+1);
      rul.left.set_length(linv+1);
      rul.left.set_entry(0,i);

      for (j = 0; j < linv; j++)
         rul.left.set_entry(j+1,gens[i].get_inv_term(j) );

      save_rule(rul);
    }

  return;
}



/////////////////////////////////////////////////////////////////
//
//                 rewriting_system::details
//
/////////////////////////////////////////////////////////////////

void rewriting_system::details(
  long   i
)
{
  long        j,first,last;
  subsystem  *subsyst;

  if (i <= 0) {
    first = 1;
    last  = n_sub;
  }

  else first = last = i;

  subsyst = first_subsystem;

  for (j = 1; j < first; j++)
    if (subsyst != NULL) subsyst = subsyst->get_next_subsystem();

  for (j = first; j <= last; j++) {
    if (subsyst == NULL) {
      cout << "Null subsystem." << endl;
      return;
    }

    subsyst->details();
    cout << endl;
    subsyst = subsyst->get_next_subsystem();
  }

  return;
}



/////////////////////////////////////////////////////////////////
//
//                 rewriting_system::probe
//
/////////////////////////////////////////////////////////////////

// If ntrials is negative, printing of results is suppressed.

long rewriting_system::probe(
  int             seed,
  _boolean_t         subword_only,
  long            ntrials,
  long            depth
)
{
  long         i,j,k,ng;
  double      *count,num,sum;
  _boolean_t      report;
  vector_long  wrd,gens;
  packed_rule  rul;
  rule_reference  exclude = {NULL,0};
  subsystem   *subsyst;

  if (ntrials < 0) {
    report = FALSE;
    ntrials = -ntrials;
  }

  else report = TRUE;

  (void) irand(-abs(seed) );

  wrd.set_max_length(depth);
  gens.set_max_length(t_gens);

  count = (double *) fxmllc( (depth+1)*sizeof(double) );

  if (count == NULL) {
    cout << "Unable to allocate space for counts" << endl;
    return 0;
  }

  count[0] = ntrials;
  for (j = 1; j <= depth; j++) count[j] = 0.0;

  subsyst = first_subsystem;

  while (subsyst != NULL) {
    subsyst->check_aux(REWRITE,subword_only,depth);
    subsyst = subsyst->get_next_subsystem();
  }

  for (i = 0; i < ntrials; i++) {
    wrd.set_length(0);
    num = 1.0;

    for (j = 0; j < depth; j++) {
      ng = 0;

      for (k = 0; k < t_gens; k++) {
        wrd.set_entry(j,k);
        wrd.set_length(j+1);

        subsyst = first_subsystem;

        while (subsyst != NULL) {
          subsyst->incremental_test(REWRITE,subword_only,
                            wrd,exclude,rul);

          if (rul.left.subsyst != NULL) break;

          subsyst = subsyst->get_next_subsystem();
        }

        if (subsyst != NULL) continue;  // wrd is reducible.

        gens.set_entry(ng++,k);
      }

      if (0 == ng) break;
      wrd.set_entry(j,gens[irand(0)%ng]);

      subsyst = first_subsystem;

      while (subsyst != NULL) {
        subsyst->incremental_test(REWRITE,subword_only,wrd,
                                  exclude,rul);
        subsyst = subsyst->get_next_subsystem();
      }

      num *= ng;
      count[j+1] += num;
    }
  }

  sum = 0.0;

  for (j = 0; j <= depth; j++) {
    num = count[j]/ntrials;
    sum += num;

    if (report) {
      cout << setw(4) << j << "  " << setw(12) << num;

      if (2 == j%3) cout << endl;
      else cout << "   ";
    }
  }

  if (report)
    cout << endl << endl << "The total is " << sum << endl;

  fxfree( (void *)count );

  if (sum >100000000.0) return 100000000;

  return (long) sum;
}



/////////////////////////////////////////////////////////////////
//
//                rewriting_system::rewrite_rules
//
/////////////////////////////////////////////////////////////////

void rewriting_system::rewrite_rules(void)
{
  long              code,numequal,numbig,numsaved,numnotred;
  long              numout;
  char              str[500];
  _boolean_t           changed,reduced,echo;
  external_form     ext;
  rule_type         r_type,save_type;
  rule_reference    exclude = {NULL,0};
  rule              rul;
  subsystem        *subsyst;
  fstream           fstrm_in,fstrm_out;

  echo = get_echo();

  numequal = numnotred = numbig = numsaved = numout = 0;
  rul.set_max_length(bsize);

 get_in_file:
  cout <<
    "\nEnter name of input file (or \"stop\"):  ";

  cin >> str;
  if (echo) cout << str << endl;
  if ( !cin.ignore(10000,'\n') ) goto input_error;

  if( 0 == strcmp(str,"stop") ) return;

  if (!file_exists(str) ) {
    cout << "File does not exist" << endl;
    if (echo) exit(1);
    goto get_in_file;
  }

  fstrm_in.open(str, ios::in);

 get_type:
  cout <<
    "Enter rule type (subword or prefix):  ";

  cin >> str;
  if (echo) cout << str << endl;
  if ( !cin.ignore(10000,'\n') ) goto input_error;

  if      (0 == strcmp(str,"subword") ) r_type = SUBWORD_RULE;
  else if (0 == strcmp(str,"prefix") )  r_type = PREFIX_RULE;

  else {
    cout << "Invalid type" << endl;
    if (echo) exit(1);
    goto get_type;
  }

 get_format:
  cout << "Enter external format" << endl <<
          "(names, len_names, len_numbers, or len_6bit):  ";

  cin >> str;
  if (echo) cout << str << endl;
  if ( !cin.ignore(10000,'\n') ) goto input_error;

  if (0 == strcmp(str,"names") )
    ext = NAMES;

  else if (0 == strcmp(str,"len_names") )
    ext = LEN_NAMES;

  else if (0 == strcmp(str,"len_numbers") )
    ext = LEN_NUMBERS;

  else if (0 == strcmp(str,"len_6bit") )
    ext = LEN_6BIT;

  else {
    cout << "Invalid format" << endl;
    if (echo) exit(1);
    goto get_format;
  }

 get_out_file:
  cout <<
    "\nEnter name of output file (or \"none\" or \"stop\"):  ";

  cin >> str;
  if (echo) cout << str << endl;
  if ( !cin.ignore(10000,'\n') ) goto input_error;

  if (0 == strcmp(str,"stop") ) return;

  subsyst = NULL;

  if (0 == strcmp(str,"none") ) {
    save_type = r_type;

    cout << "\nEnter max saved left side (positive integer):  ";
    cin  >> savell;
    if (echo) cout << savell << endl;
    if ( !cin.ignore(10000,'\n') ) goto input_error;

    cout << "\nEnter max saved right side (nonnegative integer):  ";
    cin  >> savelr;
    if (echo) cout << savelr << endl;
    if (!cin.ignore(10000,'\n') ) goto input_error;

/*
   get_save_type:
    cout <<
    "\nEnter type for saving (prefix or subword):  ";

    cin >> str;
    if (get_echo() ) cout << str << endl;
    if ( !cin.ignore(10000,'\n') ) goto input_error;

    if (0 == strcmp(str,"prefix") ) save_type = PREFIX_RULE;
    else if (0 == strcmp(str,"subword") ) save_type = SUBWORD_RULE;
    else if (0 == strcmp(str,"identity") ) save_type = IDENTITY;

    else {
      cout << "Invalid type" << endl;
      if (echo) exit(1);
      goto get_save_type;
    }
*/
    subsyst = first_subsystem;

    while (subsyst != NULL && save_type != subsyst->get_r_type() )
      subsyst = subsyst->get_next_subsystem();

    if (NULL == subsyst) {
      subsyst = new subsystem(this,save_type,HYBRID);
      add_subsystem(subsyst);
    }
  }

  else if (file_exists(str) ) {
    cout << "File exists already" << endl;
    if (echo) exit(1);
    goto get_out_file;
  }

  if (NULL != subsyst) {
    initialize_save_list(save_type,1000,20,20);
    fstrm_out.open(str, ios::out);
  }

  for (;;) {
    code = read_word(fstrm_in,ext,rul.left);
    if (1 == code) break;
    if (2 == code) goto input_error;

    code = read_word(fstrm_in,ext,rul.right);

    if (1 == code || 2 == code) goto input_error;

    rewrite(rul,exclude,changed,reduced);

    if (orient_rule(rul) ) {
      numequal++;
      continue;
    }

    if (!reduced) numnotred++;
    else balance_rule(rul);

    if (NULL == subsyst) {
      numout++;
      print_word(fstrm_out,ext,rul.left);
      fstrm_out << " ";
      print_word(fstrm_out,ext,rul.right);
      fstrm_out << endl;
    }

    else if (reduced)
      if (check_rule(rul) ) {
        numsaved++;
        save_rule(rul);
      }

      else numbig++;
  }

  if (NULL == subsyst) {
    fstrm_in.close();
    fstrm_out.close();
    cout << "Number of equalities:  " << numequal << endl;
    cout << "Number not reduced:  " << numnotred << endl;
    cout << "Number too big:  " << numbig << endl;
    cout << "Number saved:  " << numsaved << endl;
  }

  else {
    cout << "Number of equalities:  " << numequal << endl;
    cout << "Number not reduced:  " << numnotred << endl;
    cout << "Number written out:  " << numout << endl;

    reduce_all_subsystems(save_type);
    append_save_list(subsyst);
  }

  return;

 input_error:
  cout << "Input error" << endl;
  return;
}



/////////////////////////////////////////////////////////////////
//
//                rewriting_system::add_powers
//
/////////////////////////////////////////////////////////////////

void rewriting_system::add_powers(
  long        exponent,
  long        maxlen
)
{
  long                 exp2,lw,nsub;
  long                 g,i,j,k,npow,nfail;
  _boolean_t              changed,reduced;
  vector_long          word;
  rule                 rul;
  packed_rule          p_rule;
  rule_reference       exclude = {NULL,0};
  subsystem           *subsyst,*subsyst1;

  nfail = 0;

  if (print_level >= 2)
    cout << "Adding powers, exponent = " << exponent
         << ", maxlen = " << maxlen << endl;

  update_time();

  rul.set_max_length(max(bsize,exponent*maxlen) );

  // Locate the first subword subsystem.

  subsyst1 = first_subsystem;

  while (subsyst1 != NULL &&
         SUBWORD_RULE != subsyst1->get_r_type() )
    subsyst1 = subsyst1->get_next_subsystem();

  if (subsyst1 == NULL) {
    cout << "No subword system in add_powers." << endl;
    return;
  }

  exp2 = (exponent*maxlen)/2;
  initialize_save_list(SUBWORD_RULE,1000,exp2,exp2);
  nsub = n_sub;

  // Check that subsystems have the auxiliary space they need.

  subsyst = first_subsystem;

  while (subsyst != NULL) {
    subsyst->check_aux(BACKTRACK,TRUE,maxlen);
    subsyst = subsyst->get_next_subsystem();
  }

  // Begin backtrack search for reduced words of length at most
  // maxlen.

  npow = probe(1137,TRUE,-200,maxlen);

  if (print_level >= 2)
    cout << "Estimated number of powers:  " << npow << endl;

  initialize_dots('^',max(10,npow/50),50);

  word.set_max_length(maxlen);
  lw = 1;
  word.set_length(lw);
  word.set_entry(0,0);

  while (0 < word.get_length() ) {
    subsyst = first_subsystem;

    while (subsyst != NULL) {
      subsyst->incremental_test(BACKTRACK,TRUE,word,exclude,
                                p_rule);
      if (p_rule.left.subsyst != NULL) goto increment;
      subsyst = subsyst->get_next_subsystem();
    }

    print_dots();
    rul.right.set_length(0);
    rul.left.set_length(lw*exponent);
    k = 0;

    for (i = 0; i < exponent; i++)
      for (j = 0; j < lw; j++)
        rul.left.set_entry(k++,word[j]);

    rewrite(rul,exclude,changed,reduced);

    if (!reduced) nfail++;

    else if (!orient_rule(rul) ) {
      balance_rule(rul);

      if (check_rule(rul) ) save_rule(rul);
      else nfail++;

      if (nsub < n_sub) {
        nsub++;
        subsyst = last_subsystem;
        subsyst->check_aux(BACKTRACK,TRUE,maxlen);

        for (i = 1; i <= lw; i++) {
          word.set_length(i);
          subsyst->incremental_test(BACKTRACK,TRUE,word,exclude,
                                    p_rule);
          if (p_rule.left.subsyst != NULL) break;
        }

        if (i <= lw) {
          lw = i;
          goto increment;
        }
      }
    }

    if (lw >= maxlen) goto increment;

    word.set_entry(lw++,0);
    word.set_length(lw);
    continue;

   increment:
    while(lw > 0 && (g = word[lw-1]) == t_gens-1) lw--;
    word.set_length(lw);
    if (lw <= 0) continue;
    word.set_entry(lw-1,g+1);
  }  // while (0 < word.get_length() )

  end_dots();

  // If rules were saved, reduce all subword rules and append
  // save list to subsyst1.

  cout << "The number of new rules is " << nsave << endl;
  cout << "The number of failures is " << nfail << endl;

  if (nsave > 0) {
    (void) reduce_all_subsystems(SUBWORD_RULE);
    append_save_list(subsyst1);
    subsyst1->make_index();
  }

  else append_save_list(subsyst1); // This just clears save list

  return;
}


/////////////////////////////////////////////////////////////////
//
//              rewriting_system::add_random_powers
//
/////////////////////////////////////////////////////////////////

void rewriting_system::add_random_powers(
  int         seed,
  long        ntrials,
  long        exponent,
  long        maxlen
)
{
  long                 exp2,lw,nsub;
  long                 g,i,j,k,npow,ii,jj,ng,nfail;
  _boolean_t              changed,reduced;
  vector_long          word,gens;
  rule                 rul;
  packed_rule          p_rule;
  rule_reference       exclude = {NULL,0};
  subsystem           *subsyst,*subsyst1;

  nfail = 0;

  if (print_level >= 2)
    cout << "Adding random powers, exponent = " << exponent
         << ", maxlen = " << maxlen
         << ", ntrials = " << ntrials << endl;

  update_time();

  (void) irand(-abs(seed) );
  gens.set_max_length(t_gens);
  rul.set_max_length(max(bsize,exponent*maxlen) );

  // Locate the first subword subsystem.

  subsyst1 = first_subsystem;

  while (subsyst1 != NULL &&
         SUBWORD_RULE != subsyst1->get_r_type() )
    subsyst1 = subsyst1->get_next_subsystem();

  if (subsyst1 == NULL) {
    cout << "No subword system in add_random_powers." << endl;
    return;
  }

  exp2 = (exponent*maxlen)/2;
  initialize_save_list(SUBWORD_RULE,1000,exp2,exp2);
  nsub = n_sub;

  // Check that subsystems have the auxiliary space they need.

  subsyst = first_subsystem;

  while (subsyst != NULL) {
    subsyst->check_aux(BACKTRACK,TRUE,maxlen);
    subsyst = subsyst->get_next_subsystem();
  }

  // Begin backtrack search for reduced words of length at most
  // maxlen.

  word.set_max_length(maxlen);

  for (ii = 0; ii < ntrials; ii++) {
    word.set_length(0);

    for (j = 0; j < maxlen; j++) {
      ng = 0;

      for (k = 0; k < t_gens; k++) {
        word.set_entry(j,k);
        word.set_length(j+1);

        subsyst = first_subsystem;

        while (subsyst != NULL) {
          subsyst->incremental_test(BACKTRACK,TRUE,
                            word,exclude,p_rule);

          if (p_rule.left.subsyst != NULL) break;

          subsyst = subsyst->get_next_subsystem();
        }

        if (subsyst != NULL) continue;  // word is reducible.

        gens.set_entry(ng++,k);
      }

      if (0 == ng) break;
      word.set_entry(j,gens[irand(0)%ng]);

      subsyst = first_subsystem;

      while (subsyst != NULL) {
        subsyst->incremental_test(BACKTRACK,TRUE,word,
                                  exclude,p_rule);
        subsyst = subsyst->get_next_subsystem();
      }

      rul.right.set_length(0);
      rul.left.set_length( (j+1)*exponent);
      k = 0;

      for (i = 0; i < exponent; i++)
        for (jj = 0; jj <= j; jj++)
          rul.left.set_entry(k++,word[jj]);

      rewrite(rul,exclude,changed,reduced);

      if (!reduced) nfail++;

      else if (!orient_rule(rul) ) {
        balance_rule(rul);

        if (check_rule(rul) ) save_rule(rul);
        else nfail++;

        if (nsub < n_sub) {
          nsub++;
          subsyst = last_subsystem;
          subsyst->check_aux(BACKTRACK,TRUE,maxlen);

          for (i = 1; i <= j+1; i++) {
            word.set_length(i);
            subsyst->incremental_test(BACKTRACK,TRUE,word,exclude,
                                      p_rule);
            if (p_rule.left.subsyst != NULL) break;
          }

          if (i <= j+1) break;
        }
      }   // if (!orient_rule
    }     // for (j = 0;
  }       // for (ii = 0;

  // If rules were saved, reduce all subword rules and append
  // save list to subsyst1.

  cout << "The number of new rules is " << nsave << endl;
  cout << "The number of failures is " << nfail << endl;

  if (nsave > 0) {
    (void) reduce_all_subsystems(SUBWORD_RULE);
    append_save_list(subsyst1);
    subsyst1->make_index();
  }

  else append_save_list(subsyst1); // This just clears save list

  return;
}



/////////////////////////////////////////////////////////////////
//
//             rewriting_system::rewrite_words -- void
//
/////////////////////////////////////////////////////////////////

void rewriting_system::rewrite_words(void)
{
  char      file[100],str[100];
  external_form  ext;
  _boolean_t   exists,subword_only;
  ifstream   instream;
  ofstream   outstream;

  exists = FALSE;

  while (!exists) {
    cout <<
    "\nEnter input file (or \"none\" or \"stop\"):  ";
    scan_string(cin,file);

    if (0 == strcmp(file,"stop") ) return;
    if (0 == strcmp(file,"none") ) break;

    exists = file_exists(file);
    if (!exists) cout << "file does not exist." << endl;
  }

  for (;;) {
    cout << "Enter external format" << endl <<
          "(names, len_names, len_numbers, or len_6bit):  ";

    cin >> str;
    if (get_echo() ) cout << str << endl;
    if ( !cin.ignore(10000,'\n') ) error("Input error");

    if (0 == strcmp(str,"names") )
      ext = NAMES;

    else if (0 == strcmp(str,"len_names") )
      ext = LEN_NAMES;

    else if (0 == strcmp(str,"len_numbers") )
      ext = LEN_NUMBERS;

    else if (0 == strcmp(str,"len_6bit") )
      ext = LEN_6BIT;

    else {
      cout << "Invalid format" << endl;
      continue;
    }

    break;
  }

  for (;;) {
    cout << "Enter type of rewriting (subword or prefix):  ";
    cin  >> str;
    if (get_echo() ) cout << str << endl;
    if ( !cin.ignore(10000,'\n') ) error("Input error");

    if (0 == strcmp(str,"subword") )
      subword_only = TRUE;

    else if (0 == strcmp(str,"prefix") )
      subword_only = FALSE;

    else {
      cout << "Invalid type" << endl;
      continue;
    }

    break;
  }

  if (0 != strcmp(file,"none") )
    for (;;) {
      cout <<
        "Enter name of output file (or \"stop\"):  ";

      cin >> file;
      if (get_echo() ) cout << file << endl;

      if ( !cin.ignore(10000,'\n') ) {
        instream.close();
        return;
      }

      if (0 == strcmp(str,"stop") ) {
        instream.close();
        return;
      }

      if (file_exists(file) ) {
        cout << "File exists" << endl;
        continue;
      }

      outstream.open(file, ios::out);
    }

  if (0 == strcmp(file,"none") ) {
    cout << "Signal end with empty word" << endl;
    rewrite_words(cin,ext,subword_only,cout);
  }

  else {
    instream.open(file, ios::in);
    rewrite_words(instream,ext,subword_only,outstream);
    instream.close();
    outstream.close();
  }

  return;
}



/////////////////////////////////////////////////////////////////
//
//             rewriting_system::rewrite_words -- stream
//
/////////////////////////////////////////////////////////////////

void rewriting_system::rewrite_words(
  istream       &instream,
  external_form  ext,
  _boolean_t        subword_only,
  ostream       &outstream
)
{
  long           code;
  vector_long    word;
  _boolean_t     changed,reduced;
  rule_reference exclude = {NULL,0};

  for (;;) {
    code = read_word(instream,ext,word);
    if (code == 1 || code == 2) break;

    if (0 == word.get_length() ) break;

    (this->*pRewrite)(
          subword_only,word,word,exclude,changed,reduced);

    print_word(outstream,ext,word);
    outstream << endl;
  }

  return;
}



/////////////////////////////////////////////////////////////////
//
//                rewriting_system::log_on
//
/////////////////////////////////////////////////////////////////

void rewriting_system::log_on(
  long   maxlen,
  char  *file
)
{
  if (maxlen <= 0) {
    cout << "maxlen must be positive for logging." << endl;
    return;
  }

  if (logll > 0) {
    // logging is already on.

    cout <<
      "Logging is currently on.  Only maxlen has been changed."
      << endl;

    logll = maxlen;
    return;
  }

  logll = maxlen;

  if (file_exists(file) ) log_stream.open(file, ios::app);
  else log_stream.open(file, ios::out);
}



/////////////////////////////////////////////////////////////////
//
//               rewriting_system::log_off
//
/////////////////////////////////////////////////////////////////

void rewriting_system::log_off(void)
{
  if (logll <= 0) {
    cout << "Logging is not currently on." << endl;
    return;
  }

  logll = 0;
  log_stream.close();
}



// end of system.C
