/* fsaexists.c 4/11/94.
 * Calculates finite state automaton that accepts w_1 if and only if
 * input automaton accepts (w_1,w_2) for some w_2.
 *
 * SYNOPSIS:  fsaexists [-ip d/s[dr]] [-op d/s] [-silent] [-v] [-l/-h]
 *              filename
 *
 * Input is from filename, which should contain a fsa.
 * Output is to filename.exists
 *
 * OPTIONS:
 * -ip d/s[dr]  input in dense or sparse format - dense is default
 * -op d/s      output in dense or sparse format - default is as in current
 *               value of table->printing_format, in the fsa.
 * -v           verbose
 * -silent      no diagnostics
 * -l/-h        large/huge hash-tables (for big examples)
 */

#include <stdio.h>
#include "defs.h"
#include "fsa.h"
#include "definitions.h"

FILE *rfile,
     *wfile,
     *fopen();

void  badusage();

/* Functions defined in other files used in this file */
void  fsa_read();
fsa  *fsa_exists();
void  fsa_print();
void  fsa_clear();

main(argc, argv)
        int             argc;
        char           *argv[];
{ int arg;
  fsa fsain, *fsaexists;
  char inf[100], outf[100], fsaname[100], tempfilename[100];
  storage_type ip_store = DENSE;
  int dr = 0;
  storage_type op_store = DENSE;

  setbuf(stdout,(char*)0);
  setbuf(stderr,(char*)0);

  inf[0] = '\0';
  arg = 1;
  while (argc > arg) {
    if (strcmp(argv[arg],"-ip")==0) {
      arg++;
      if (arg >= argc)
        badusage();
      if (strcmp(argv[arg],"d")==0)
        ip_store = DENSE;
      else if (argv[arg][0] == 's') {
        ip_store = SPARSE;
        if (strlen(argv[arg]) > 1)
          dr = atoi(argv[arg]+1);
      }
      else
        badusage();
    }
    else if (strcmp(argv[arg],"-op")==0) {
      arg++;
      if (arg >= argc)
        badusage();
      if (strcmp(argv[arg],"d")==0)
        op_store = DENSE;
      else if (strcmp(argv[arg],"s")==0)
        op_store = SPARSE;
      else
        badusage();
    }
    else if (strcmp(argv[arg],"-silent")==0)
      print_level = 0;
    else if (strcmp(argv[arg],"-v")==0)
      print_level = 2;
    else if (strcmp(argv[arg],"-vv")==0)
      print_level = 3;
    else if (strcmp(argv[arg],"-l")==0)
      large = TRUE;
    else if (strcmp(argv[arg],"-h")==0)
      huge = TRUE;
    else {
       if (argv[arg][0] == '-')
         badusage();
       if (strcmp(inf,""))
         badusage();
       strcpy(inf,argv[arg]);
    }
    arg++;
  }
  if (strlen(inf)==0)
    badusage();

  strcpy(outf,inf);
  strcat(outf,".exists");

  if ((rfile = fopen(inf,"r")) == 0) {
    fprintf(stderr,"Cannot open file %s.\n",inf);
    exit(1);
  }
  fsa_read(rfile,&fsain,ip_store,dr,0,TRUE,fsaname);

  if (fsain.alphabet->type != PRODUCT || fsain.alphabet->arity != 2) {
    fprintf(stderr,"Error: fsa must be 2-variable.\n");
    exit(1);
  }
  
  strcpy(tempfilename,inf);
  strcat(tempfilename,"temp_exists_XXX");
  fsaexists = fsa_exists(&fsain,op_store,TRUE,tempfilename);

  if (print_level>1)
    printf("  Number of states of fsaexists before minimisation = %d.\n",
        fsaexists->states->size);
  fsa_minimize(fsaexists);
  if (print_level>1)
    printf("  Number of states of fsaexists after minimisation = %d.\n",
        fsaexists->states->size);

  strcat(fsaname,"_exists");
  wfile = fopen(outf,"w");
  fsa_print(wfile,fsaexists,fsaname);
  fclose(wfile);

  if (print_level>0)
    printf("#\"Exists\" fsa with %d states computed.\n",
            fsaexists->states->size);

  fsa_clear(fsaexists);
  tfree(fsaexists);
  exit(0);
}
void
badusage()
{
    fprintf(stderr,
 "Usage: fsaexists [-ip d/s[dr]] [-op d/s] [-silent] [-v] [-l/-h] filename\n");
    exit(1);
}
