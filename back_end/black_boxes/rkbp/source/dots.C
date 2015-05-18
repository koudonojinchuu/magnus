// dots.C


/*
   When a program is working hard without producing any output,
   it is useful to have a sequence of dots or other characters
   printed at frequent intervals to let the user know that the
   program is still alive.  The functions in this file facilitate
   this operation.

   A call to initialize_dots starts the procedure.  This call
   specifies the character to be printed, the frequency of the
   printing, and the number of characters per line.

   Calls to print_dots result in a character being printed every
   dt_num calls.


   A call to end_dots stops with process and prints a newline.
*/


#include <stdio.h>
#include <iostream.h>
#include "portable.h"
#include "utility.h"
#include "dots.h"


// Variables defining the printing of dots.

static  long    dt_chr;    // The character used as the dot

static  long    dt_cnt;    // The number of calls to print_dots
                           // since last dot printed

static  long    dt_inc;    // The number of calls to print_dots
                           // between dots

static  long    dt_num;    // The number of dots on current
                           // line

static  long    dt_on = FALSE;
                           // Flag which if FALSE suspends
                           // printing of dots.  Set by
                           // set_dots

static  long    dt_ttl;    // Maximum number of dots per line



/////////////////////////////////////////////////////////////////
//
//                        ask_dots
//
/////////////////////////////////////////////////////////////////

void ask_dots(void)
{
  cout << "Do you want dot printing turned on (y/n):  ";

  if (YES) set_dots(TRUE);
  else set_dots(FALSE);

  return;
}



/////////////////////////////////////////////////////////////////
//
//                        end_dots
//
/////////////////////////////////////////////////////////////////

// Print a final newline if necessary.

void end_dots(void)
{
  if (!dt_on) return;

  if (dt_num > 0) {
    cout << "\n";
    dt_num = 0;
  }

  return;
}



/////////////////////////////////////////////////////////////////
//
//                     initialize_dots
//
/////////////////////////////////////////////////////////////////

// Start the process of printing characters.

void initialize_dots(
  long   chr,
  long   inc,
  long   ttl
)
{
  dt_chr = chr;
  dt_inc = inc;
  dt_ttl = ttl;

  dt_cnt = 0;
  dt_num = 0;

  return;
}



/////////////////////////////////////////////////////////////////
//
//                        print_dots
//
/////////////////////////////////////////////////////////////////

// Print a character every dt_num calls.

void print_dots(void)
{
  if (!dt_on) return;

  if (dt_inc <= ++dt_cnt) {
    cout << (char) dt_chr;
    dt_cnt = 0;

    if(dt_ttl <= ++dt_num) {
      cout << "\n";
      dt_num = 0;
    }

    cout.flush();
  }

  return;
}



/////////////////////////////////////////////////////////////////
//
//                           set_dots
//
/////////////////////////////////////////////////////////////////

// Assign a value to the flag dt_on.

void set_dots(
  long   flag
)
{
  dt_on = flag;

  return;
}


// end of dots.C
