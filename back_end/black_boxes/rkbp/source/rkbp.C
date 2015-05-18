// rkbp.C

#include "rkbp.h"


static long        nargs;
static char       *args[20];
double             hybrid_factor = 0.5;
static char        file_name[50] = "";
static char        file[50];
static char        kb_strategy[50] = "";
static char        algorithm[50];
static int         timing = 0;

long               print_level = 0;


extern void      help(void);


int main()
{
  long              command_length,i,code;
  long              bsize=100, maxlen,prevlen,num_iter;
  long              savell,savelr,num;
  long              ntrials,depth,exponent;
  int               seed;
  ulong             maxmem;
  double            abort_factor,cpu;
  char              command[501],p_command[501];
  char              str[500],*pchar;
  _boolean_t           confluent;
  rule_type         rtype;
  rewriting_system *syst = NULL;
  void  huge       *dummy;


  start_time();

  cout << "\nRutgers Knuth-Bendix Package, Version 1.25\n\n";


  ////  Set defaults. //////////////////////////////


  set_echo(FALSE);

  prevlen = 0;
  abort_factor  = 2.0;
  cout << "\n";
  

  //// Run command read-execute loop. //////////////////////


  for (;;) {
    cpu = get_time();

    if (timing)
      cout << endl << "Total cpu time:  " << cpu 
           << " seconds." << endl << endl;

    cout << ">: ";

    // Read in the next command line.  Only the first 500
    // characters are significant.

    if ( !cin.get(command,500) ) {
      cout << "Input error reading command line." << endl;
      goto cont;
    }

    if ( !cin.ignore(10000,'\n') ) goto clear_error;

    command_length = strlen(command);
    if ( get_echo() ) cout << command << endl;

    if (command_length == 0) continue;

    strcpy(p_command,command);
    nargs = parse(p_command,args,20);
    if (nargs == 0) continue;

    if (args[0][0] == '!') {
      // This is a system command.  Pass it to operating system.

      i = 0;
      while (command[i] == ' ') i++;
      code = system(command+i+1);

      if (code != 0) cout << "Command returned code " << code
         << endl;

      continue;
    }

    if ('#' == args[0][0]) {
      // This is a comment.  Ignore it.

      continue;
    }

    // Execute the command.  Make sure the first word is in
    // lower case.


    to_lower(args[0]);

    //// abort_factor  ///////////////////////////////////////

    if      ( 0 == strcmp(args[0],"abort_factor") ) {
      if (nargs <= 1) {
        cout <<
          "Enter abort_factor (positive decimal number):  ";
        cin >> abort_factor;

        if (get_echo() ) cout << abort_factor << endl;

        if ( !cin.ignore(10000,'\n') ) goto clear_error;
      }

      else abort_factor = atof(args[1]);
    }

    ////  add_powers  /////////////////////////////////////////

    else if (0 == strcmp(args[0],"add_powers") ) {
      if (NULL == syst) goto no_sys;

      if (nargs <= 1) {
        cout << "Enter exponent:  ";
        cin >> exponent;

        if (get_echo() ) cout << exponent << endl;

        if ( !cin.ignore(10000,'\n') ) goto clear_error;
      }

      else exponent = atol(args[1]);

      if (nargs <= 2) {
        cout << "Enter maximum length of words:  ";
        cin >> maxlen;

        if (get_echo() ) cout << maxlen << endl;

        if ( !cin.ignore(10000,'\n') ) goto clear_error;
      }

      else maxlen = atol(args[2]);

      if (nargs <= 3) {
        cout <<
          "Enter max saved left side (positive integer):  ";
        cin  >> savell;

        if (get_echo() ) cout << savell << endl;
        
        if ( !cin.ignore(10000,'\n') ) goto clear_error;
      }
      
      else savell = atol(args[3]);

      if (nargs <= 4) {
        cout <<
          "Enter max saved right side (nonnegative integer):  ";
        cin  >> savelr;

        if (get_echo() ) cout << savelr << endl;
        
        if ( !cin.ignore(10000,'\n') ) goto clear_error;
      }
      
      else savelr = atol(args[4]);
      
      syst->set_save(savell,savelr);
      syst->add_powers(exponent,maxlen);
      prevlen = 0;
    }

    ////  add_random_powers  ////////////////////////////////////

    else if (0 == strcmp(args[0],"add_random_powers") ) {
      if (NULL == syst) goto no_sys;

      if (nargs <= 1) {
        cout << "Enter number of trials:  ";
        cin  >> ntrials;

        if (get_echo() ) cout << ntrials << endl;
        if ( !cin.ignore(10000,'\n') ) goto clear_error;
      }

      if (nargs <= 2) {
        cout << "Enter exponent:  ";
        cin >> exponent;

        if (get_echo() ) cout << exponent << endl;

        if ( !cin.ignore(10000,'\n') ) goto clear_error;
      }

      else exponent = atol(args[1]);

      if (nargs <= 3) {
        cout << "Enter maximum length of words:  ";
        cin >> maxlen;

        if (get_echo() ) cout << maxlen << endl;

        if ( !cin.ignore(10000,'\n') ) goto clear_error;
      }

      else maxlen = atol(args[2]);

      if (nargs <= 4) {
        cout <<
          "Enter max saved left side (positive integer):  ";
        cin  >> savell;

        if (get_echo() ) cout << savell << endl;
        
        if ( !cin.ignore(10000,'\n') ) goto clear_error;
      }
      
      else savell = atol(args[3]);

      if (nargs <= 5) {
        cout <<
          "Enter max saved right side (nonnegative integer):  ";
        cin  >> savelr;

        if (get_echo() ) cout << savelr << endl;
        
        if ( !cin.ignore(10000,'\n') ) goto clear_error;
      }
      
      else savelr = atol(args[4]);
      
      if (nargs <= 6) {
        cout << "Enter random seed:  ";
        cin  >> seed;
        if (get_echo() ) cout << seed << endl;
        if ( !cin.ignore(10000,'\n') ) goto clear_error;
      }

      else seed = atoi(args[6]);

      syst->set_save(savell,savelr);
      syst->add_random_powers(seed,ntrials,exponent,maxlen);
      prevlen = 0;
    }

    ////  bsize  //////////////////////////////////////////////

    else if ( 0 == strcmp(args[0],"bsize") ) {
      if (nargs <= 1) {
        cout << "Enter value for bsize:  ";
        cin >> bsize;

        if (get_echo() ) cout << bsize << endl;

        if ( !cin.ignore(10000,'\n') ) goto clear_error;
      }

      else bsize = atol(args[1]);

      if (syst != NULL) syst->set_bsize(bsize);
    }

    ////  counts  /////////////////////////////////////////////

    else if ( 0 == strcmp(args[0],"counts") ) {
      if (syst == NULL) goto no_sys;
      
      syst->print_counts();
    }

    ////  details  /////////////////////////////////////////////

    else if ( 0 == strcmp(args[0],"details") ) {
      if (syst == NULL) goto no_sys;

      if (nargs <= 1) {
        cout << "Enter number of subsystem (or -1 for all):  ";
        cin >> num;

        if (get_echo() ) cout << num << endl;

        if ( !cin.ignore(10000,'\n') ) goto clear_error;
      }

      else num = atol(args[1]);

      syst->details(num);
    }

    ////  dots  ////////////////////////////////////////////////

    else if ( 0 == strcmp(args[0],"dots") )    set_dots(TRUE);

    ////  echo  ////////////////////////////////////////////////

    else if ( 0 == strcmp(args[0],"echo") ) set_echo(TRUE);

    //// exit  /////////////////////////////////////////////////

    else if ( 0 == strcmp(args[0],"exit") )    break;

    ////  free  ////////////////////////////////////////////////

    else if ( 0 == strcmp(args[0],"free") ) {
      if (syst == NULL) goto no_sys;

      delete syst;
      syst = NULL;
      prevlen = 0;
    }

    ////  help  ////////////////////////////////////////////////

    else if ( 0 == strcmp(args[0],"help") ||
              args[0][0] == '?'             ) help();

    ////  heuristics  //////////////////////////////////////////

    else if ( 0 == strcmp(args[0],"heuristics") ) {
      if (syst == NULL) goto no_sys;
      syst->set_use_heuristics(TRUE);
    }

    ////  hybrid_factor  ///////////////////////////////////////

    else if ( 0 == strcmp(args[0],"hybrid_factor") ) {
      if (nargs <= 1) {
        cout <<
          "Enter hybrid_factor (positive decimal number):  ";
        cin  >> hybrid_factor;

        if (get_echo() ) cout << hybrid_factor << endl;

        if ( !cin.ignore(10000,'\n') ) goto clear_error;
      }

      else hybrid_factor = atof(args[1]);

      set_hybrid_factor(hybrid_factor);
    }

    ////  input  ///////////////////////////////////////////////

    else if ( 0 == strcmp(args[0],"input") ) {
      if (syst != NULL) {
        cout <<
         "A system already exists.  You must explicitly free\n"
        << "it.\n";

        continue;
      }

      syst = new rewriting_system;
      if (bsize > 0) syst->set_bsize(bsize);
      
      if (nargs > 1) syst->input(args[1],TRUE);
      else syst->input("",TRUE);
    }

    ////  kb  //////////////////////////////////////////////////

    else if ( 0 == strcmp(args[0],"kb") ) {
      if (syst == NULL) goto no_sys;

      if (nargs <= 1) {
        cout << "Enter maxlen (0 for prevlen+1):  ";
        cin  >> maxlen;

        if (get_echo() ) cout << maxlen << endl;
        
        if ( !cin.ignore(10000,'\n') ) goto clear_error;

        if (maxlen <= 0) maxlen = prevlen+1;
      }
      
      else maxlen = atol(args[1]);

      if (nargs <= 2) {
        cout << "Enter niter (-1 for infinity):  ";
        cin  >> num_iter;

        if (get_echo() ) cout << num_iter << endl;
        
        if ( !cin.ignore(10000,'\n') ) goto clear_error;
      }
      
      else num_iter = atol(args[2]);

      if (nargs <= 3) {
        cout <<
          "Enter max saved left side (positive integer):  ";
        cin  >> savell;

        if (get_echo() ) cout << savell << endl;
        
        if ( !cin.ignore(10000,'\n') ) goto clear_error;
      }
      
      else savell = atol(args[3]);

      if (nargs <= 4) {
        cout <<
          "Enter max saved right side (nonnegative integer):  ";
        cin  >> savelr;

        if (get_echo() ) cout << savelr << endl;
        
        if ( !cin.ignore(10000,'\n') ) goto clear_error;
      }
      
      else savelr = atol(args[4]);
      
      syst->set_save(savell,savelr);

      confluent = (syst->*(syst->pKnuth_Bendix))(maxlen,num_iter,
                  prevlen,abort_factor);

      if (confluent) prevlen = maxlen;
      else prevlen = 0;
    }

    ////  kb_random  //////////////////////////////////////////////////

    else if ( 0 == strcmp(args[0],"kb_random") ) {
      if (syst == NULL) goto no_sys;

      if (nargs <= 1) {
        cout << "Enter ntrials:  ";
        cin  >> ntrials;

        if (get_echo() ) cout << ntrials << endl;
        if ( !cin.ignore(10000,'\n') ) goto clear_error;
      }

      else ntrials = atol(args[1]);

      if (nargs <= 2) {
        cout << "Enter rule type (prefix or subword):  ";
        cin  >> str;
        if (get_echo() ) cout << str << endl;
        if ( !cin.ignore(10000,'\n') ) goto clear_error;
        pchar = str;
      }

      else pchar = args[2];

      to_lower(pchar);

      if (0 == strcmp(pchar,"prefix") )       rtype = PREFIX_RULE;
      else if (0 == strcmp(pchar,"subword") ) rtype = SUBWORD_RULE;

      else {
        cout << "Invalid rule type" << endl;
        continue;
      }

      if (nargs <= 3) {
        cout << "Enter maxlen:  ";
        cin  >> maxlen;

        if (get_echo() ) cout << maxlen << endl;
        
        if ( !cin.ignore(10000,'\n') ) goto clear_error;

        if (maxlen <= 0) maxlen = prevlen+1;
      }
      
      else maxlen = atol(args[3]);

      if (nargs <= 4) {
        cout <<
          "Enter max saved left side (positive integer):  ";
        cin  >> savell;

        if (get_echo() ) cout << savell << endl;
        
        if ( !cin.ignore(10000,'\n') ) goto clear_error;
      }
      
      else savell = atol(args[4]);

      if (nargs <= 5) {
        cout <<
          "Enter max saved right side (nonnegative integer):  ";
        cin  >> savelr;

        if (get_echo() ) cout << savelr << endl;
        
        if ( !cin.ignore(10000,'\n') ) goto clear_error;
      }
      
      else savelr = atol(args[5]);
      
      syst->set_save(savell,savelr);

      if (nargs <= 6) {
        cout << "Enter random seed:  ";
        cin  >> seed;
        if (get_echo() ) cout << seed << endl;
        if ( !cin.ignore(10000,'\n') ) goto clear_error;
      }

      else seed = atoi(args[6]);

      (void) irand(-abs(seed) );
      confluent = syst->kb_random_two_step(maxlen,rtype,ntrials);
      if (!confluent) prevlen = 0;
    }

    ////  kb_strategy  //////////////////////////////////////////

    else if ( 0 == strcmp(args[0],"kb_strategy") ) {
      if (syst == NULL) goto no_sys;
      
      if (nargs <= 1) {
        cout <<
           "Enter Knuth-Bendix strategy (basic or two_step):  ";
        cin >> kb_strategy;

        if (get_echo() ) cout << kb_strategy << endl;
        
        if ( !cin.ignore(10000,'\n') ) goto clear_error;
        
        args[1] = kb_strategy;
      }

      if ( 0 == strcmp(args[1],"basic") )
        syst->set_kb_strategy(BASIC);

      else if ( 0 == strcmp(args[1],"two_step") )
        syst->set_kb_strategy(TWO_STEP);
        
      else {
        cout << "Unknown Knuth-Bendix strategy.\n";
        goto cont;
      }
    }

    ////  log  //////////////////////////////////////////////////

    else if ( 0 == strcmp(args[0],"log") ) {
      if (NULL == syst) goto no_sys;

      if (nargs <= 1) {
        cout << "Enter maxlen:  ";
        cin  >> maxlen;
        if (get_echo() ) cout << maxlen << endl;
        if ( !cin.ignore(10000,'\n') ) goto clear_error;
      }

      else maxlen = atol(args[1]);

      if (nargs <= 2) {
        cout << "Enter log file name:  ";
        cin  >> str;
        if (get_echo() ) cout << str << endl;
        if ( !cin.ignore(10000,'\n') ) goto clear_error;
      }

      else strcpy(str,args[2]);

      syst->log_on(maxlen,str);
    }

    ////  memory  ///////////////////////////////////////////////

    else if ( 0 == strcmp(args[0],"memory") )  fxstat();

    ////  nodots  ///////////////////////////////////////////////

    else if ( 0 == strcmp(args[0],"nodots") ) set_dots(FALSE);

    ////  noecho  ///////////////////////////////////////////////

    else if ( 0 == strcmp(args[0],"noecho") ) set_echo(FALSE);

    //// noheuristics  //////////////////////////////////////////

    else if ( 0 == strcmp(args[0],"noheuristics") ) {
      if (NULL == syst) goto no_sys;
      syst->set_use_heuristics(FALSE);
    }

    ////  nolog  ////////////////////////////////////////////////

    else if ( 0 == strcmp(args[0],"nolog") ) {
      if (NULL == syst) goto no_sys;
      syst->log_off();
    }

    ////  output  ///////////////////////////////////////////////

    else if ( 0 == strcmp(args[0],"output") ) {
      if (syst == NULL) goto no_sys;
      
      if ( nargs <= 1) {
        for (;;) {
          cout <<
            "Enter problem name for output (or \"stop\"):  ";

          cin  >> file_name;

          if (get_echo() ) cout << file_name << endl;

          if ( !cin.ignore(10000,'\n') ) goto clear_error;

          if (0 == strcmp(file_name,"stop") ) break;
          strcpy(file,file_name);
          strcat(file,".sys");

          if (file_exists(file) ) {
            cout << "File " << file << " exits." << endl;
            if (get_echo() ) exit(1);
            continue;
          }

          syst->write(file_name);
          break;
        }
      }

      else syst->write(args[1]);
    }

    ////  print_level  //////////////////////////////////////////

    else if ( 0 == strcmp(args[0],"print_level") ) {
      if (nargs <= 1) {
        cout << "Enter print_level (a nonnegative integer):  ";
        cin  >> print_level;

        if (get_echo() ) cout << print_level << endl;

        if ( !cin.ignore(10000,'\n') ) goto clear_error;
      }

      else print_level = atol(args[1]);
    }

    ////  probe  ////////////////////////////////////////////////

    else if ( 0 == strcmp(args[0],"probe") ) {
      if (nargs <= 1) {
        cout << "Enter number of trials:  ";
        cin  >> ntrials;

        if (get_echo() ) cout << ntrials << endl;

        if ( !cin.ignore(10000,'\n') ) goto clear_error;
      }

      else ntrials = atol(args[1]);

      if (nargs <= 2) {
        cout << "Enter maximum length:  ";
        cin  >> depth;
        if (get_echo() ) cout << depth << endl;
        if ( !cin.ignore(10000,'\n') ) goto clear_error;
      }

      else depth = atol(args[2]);

      if (nargs <= 3) {
        cout << "Enter random seed:  ";
        cin  >> seed;
        if (get_echo() ) cout << seed << endl;
        if ( !cin.ignore(10000,'\n') ) goto clear_error;
      }

      else seed = atoi(args[3]);

      (void) syst->probe(seed,FALSE,ntrials,depth);
    }

    ////  probe_subword  ////////////////////////////////////////

    else if ( 0 == strcmp(args[0],"probe_subword") ) {
      if (nargs <= 1) {
        cout << "Enter number of trials:  ";
        cin  >> ntrials;
        if (get_echo() ) cout << ntrials << endl;
        if ( !cin.ignore(10000,'\n') ) goto clear_error;
      }

      else ntrials = atol(args[1]);

      if (nargs <= 2) {
        cout << "Enter maximum length:  ";
        cin  >> depth;
        if (get_echo() ) cout << depth << endl;
        if ( !cin.ignore(10000,'\n') ) goto clear_error;
      }

      else depth = atol(args[2]);

      if (nargs <= 3) {
        cout << "Enter random seed:  ";
        cin  >> seed;
        if (get_echo() ) cout << seed << endl;
        if ( !cin.ignore(10000,'\n') ) goto clear_error;
      }

      else seed = atoi(args[3]);

      syst->probe(seed,TRUE,ntrials,depth);
    }

    ////  quit  /////////////////////////////////////////////////

    else if ( 0 == strcmp(args[0],"quit") )    break;

    ////  report_memory  ////////////////////////////////////////

    else if ( 0 == strcmp(args[0],"report_memory") ) fxask();

    ////  report_timing  ////////////////////////////////////////

    else if ( 0 == strcmp(args[0],"report_timing") ) {
      cout << "Do you want cpu times reported (y or n):  ";

      if (YES) timing = 1;
      else timing = 0;
    }

    ////  report_rewriting  /////////////////////////////////////

    else if ( 0 == strcmp(args[0],"report_rewriting") ) {
      if (syst == NULL) goto no_sys;

      syst->ask_rewrite_verbose();
    }

    ////  restore  //////////////////////////////////////////////

    else if ( 0 == strcmp(args[0],"restore") ) {
      if (syst != NULL) {
        cout <<
         "A system already exists.  You must explicitly free\n"
        << "it.\n";

        continue;
      }

      syst = new rewriting_system;
      
      if (nargs > 1) syst->input(args[1],FALSE);
      else syst->input("",FALSE);
    }

    ////  rewrite_rules  ////////////////////////////////////////

    else if (0 == strcmp(args[0],"rewrite_rules") ) {
      if (syst == NULL) goto no_sys;

      syst->rewrite_rules();
      prevlen = 0;  // This line needs to be rethought.
    }

    ////  rewrite_words  ////////////////////////////////////////

    else if (0 == strcmp(args[0],"rewrite_words") ) {
      if (syst == NULL) goto no_sys;

      syst->rewrite_words();
    }

    ////  rewriting_strategy  ///////////////////////////////////

    else if ( 0 == strcmp(args[0],"rewriting_strategy") ) {
      if (syst == NULL) goto no_sys;
      
      if (nargs <= 1) {
        cout << "Enter rewriting algorithm (left or global):  ";
        cin >> algorithm;

        if (get_echo() ) cout << algorithm << endl;
        
        if ( !cin.ignore(10000,'\n') ) goto clear_error;
        
        args[1] = algorithm;
      }

      if      ( 0 == strcmp(args[1],"left") )
        syst->set_rw_strategy(FROM_LEFT);

      else if ( 0 == strcmp(args[1],"global") )
        syst->set_rw_strategy(GLOBAL);

      else {
        cout << "Unknown rewriting algorithm.\n";
      }
    }

    ////  settings  /////////////////////////////////////////////

    else if ( 0 == strcmp(args[0],"settings") ) {
      if (syst != NULL) bsize = syst->get_bsize();
      cout << setprecision(3) << "\n" <<
      "bsize         = " << bsize << "\n" <<
      "abort_factor  = " << setw(6) << abort_factor << "\n" <<

      "hybrid_factor = " << setw(8) << hybrid_factor << "\n" <<
         setprecision(6);

      if (syst != NULL)
        cout << "Confluent to length " << prevlen << "\n\n";
    }

    ////  summary  //////////////////////////////////////////////

    else if ( 0 == strcmp(args[0],"summary") ) {
      if (syst == NULL) goto no_sys;

      syst->summary();
    }


    else cout << "unknown command:  " << command << "\n";

    continue;

   clear_error:
    cout << "Error clearing command line." << endl;
    goto cont;

   no_sys:
    cout << "There is no current system." << endl;
    goto cont;

   cont:
    if ( get_echo() ) exit(1);
  }

  return 0;
}



/////////////////////////////////////////////////////////////////
//
//                        help
//
/////////////////////////////////////////////////////////////////

void help(void)
{
  char  command[100];

  if ( nargs == 1 &&
       (0 == strcmp(args[0],"?") ||
        0 == strcmp(args[0],"help") ) ) {
    cout << "abort_factor  <decimal number>\n";
    cout << "add_powers <exponent> <maxlen>\n";
    cout << "add_random_powers <ntrials> <exponent> <maxlen> \
<savell> <savelr> <seed> \n";
    cout << "bsize  <positive integer>\n";
    cout << "counts\n";
    cout << "details <integer>\n";
    cout << "dots\n";
    cout << "echo\n";
    cout << "exit\n";
    cout << "free\n";
    cout << "help\n";
    cout << "heuristics\n";
    cout << "hybrid_factor  <decimal number>\n";
    cout << "input <problem_name>\n";
    cout << "kb <maxlen> <niter> <llsave> <lrsave>\n";
    cout << "kb_random <ntrials> <rule_type> <maxlen> <llsave>\n";
    cout << "          <lrsave> <seed>\n";
    cout << "kb_strategy <name>\n";

    if (!get_echo() ) {
      cout << "\nEnter carriage return to continue ";

      if ( !cin.ignore(10000,'\n') ) {
        cout << "I waited long enough!" << endl;
        exit(1);
      }

      cout << endl;
    }

    cout << "log <maxlen> <file>\n";
    cout << "memory\n";
    cout << "nodots\n";
    cout << "noecho\n";
    cout << "noheuristics\n";
    cout << "nolog\n";
    cout << "output <problem_name>\n";
    cout << "print_level <integer>\n";
    cout << "probe <ntrials> <max_len> <seed>\n";
    cout << "probe_subword <ntrials> <max_len> <seed>\n";
    cout << "quit\n";
    cout << "report_memory\n";
    cout << "report_timing\n";
    cout << "report_rewriting\n";
    cout << "restore <problem_name>\n";
    cout << "rewrite_rules\n";
    cout << "rewrite_words\n";
    cout << "rewriting_strategy <name>\n";
    cout << "settings\n";
    cout << "summary\n";
  }

  else {
    if (nargs == 2 && 0 == strcmp(args[0],"help") )
      strcpy(args[1],command);

    else if (nargs == 1 && args[0][0] == '?')
      strcpy(args[0]+1,command);

    else return;

    cout <<
      "Sorry.  No help available yet on individual commands.\n";
  }

  return;
}



// end of rkbp.C
