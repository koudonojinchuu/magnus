/* gpgenmult2.c 28/11/94.
 * Calculates the general multiplier automaton of a short-lex automatic group,
 * for words in the generators of length 2.
 * This program assumes that kbprog with -wd option, gpwa, gpgenmult
 * and preferably gpcheckmult  have already been run of G.
 *
 * SYNOPSIS:
 * gpgenmult2 [-ip d/s[dr]] [-op d/s] [-silent] [-v] [-l/-h] [-f] groupname
 *
 * Input is from groupname.genmult
 * Output is to groupname.gm2, but under the -f option,
 * the unformatted transition table goes to groupname.gm2_ut.
 *
 * OPTIONS:
 * -ip d/s[dr]	input in dense or sparse format - dense is default
 * -op d/s	output in dense or sparse format - sparse is default
 * -v		verbose
 * -silent	no diagnostics
 * -l/-h	large/huge hash-tables (for big examples)
 * -f           read the transition table repeatedly from file while mimimizing.
 *              this avoids storing the large table, but is a little slower.
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
fsa  *fsa_genmult2();
void  fsa_labeled_minimize();
void  fsa_ip_labeled_minimize();
void  fsa_print();
void  fsa_clear();
void  fsa_table_dptr_init();

main(argc, argv)
        int             argc;
        char           *argv[];
{ int arg, i, g1, g2;
  fsa genmult, *genmult2ptr;
  char inf[100], outf[100], fsaname[100], tablefilename[100];
  storage_type ip_store = DENSE;
  int dr = 0;
  storage_type op_store = SPARSE;
  boolean readback = TRUE;

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
    else if (strcmp(argv[arg],"-f")==0)
      readback = FALSE;
    else {
       if (argv[arg][0] == '-')
         badusage();
       if (strcmp(inf,"")!=0)
         badusage();
       else
         strcpy(inf,argv[arg]);
    }
    arg++;
  }
  if (strlen(inf)==0)
    badusage();
  
  strcpy(tablefilename,inf);
  strcat(tablefilename,".gm2_ut");

  strcpy(outf,inf);
  strcat(outf,".gm2");

  strcat(inf,".gm");

  if ((rfile = fopen(inf,"r")) == 0) {
    fprintf(stderr,"Cannot open file %s.\n",inf);
      exit(1);
  }
  fsa_read(rfile,&genmult,ip_store,dr,0,TRUE,fsaname);
  fclose(rfile);

  genmult2ptr = fsa_genmult2(&genmult,op_store,TRUE,tablefilename,readback);

  if (print_level>1)
    printf("  #Number of states of genmult2 = %d.\n",genmult2ptr->states->size);

  if (readback)
    fsa_labeled_minimize(genmult2ptr);
  else
    fsa_ip_labeled_minimize(genmult2ptr);
  if (print_level>1)
    printf("  #Number of states of genmult2 after minimization = %d.\n",
             genmult2ptr->states->size);
  base_prefix(fsaname);
  strcat(fsaname,".gm2");
  wfile = fopen(outf,"w");
  fsa_print(wfile,genmult2ptr,fsaname);
  fclose(wfile);

  if (print_level>0)
    printf("#Generalised length-2 multiplier with %d states computed.\n",
            genmult2ptr->states->size);

  fsa_clear(genmult2ptr);
  tfree(genmult2ptr);
  exit(0);
}
 
void
badusage()
{
    fprintf(stderr,"Usage: \n");
    fprintf(stderr,
"gpgenmult2 [-ip d/s[dr]] [-op d/s] [-silent] [-v] [-l/-h] [-f] groupname.\n");
    exit(1);
}
