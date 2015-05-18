// utility.C


// This file contains miscellaneous utility functions.

#include  "rkbp.h"



// The following are used in the recording of elapsed cpu time.

#define  TICKS_PER_SECOND   1000000

static   unsigned long   last_clock;
static   double          cpu_time;


// The following controls command echoing.

static   _boolean_t         echo = FALSE;


// The following are used in the generation of random numbers.

static  int    iff = 0;     // A flag signalling prior call
static  int    v[97];





/////////////////////////////////////////////////////////////////
//
//                          bool_t
//
/////////////////////////////////////////////////////////////////

// Convert an int to _boolean_t.

_boolean_t  bool_t(
  int x
)
{
  if (x == 0) return FALSE;

  return TRUE;
}



/////////////////////////////////////////////////////////////////
//
//                     compress_file
//
/////////////////////////////////////////////////////////////////

// Compress a file using the system compress command.

void compress_file(
  char  *file  // The name of the file to be compressed
)
{
  ulong   len;
  char   *buff;

  if ( !file_exists(file) ) {
    cout << "file does not exists in compress_file\n";
    return;
  }

  len = strlen(file);
  buff = (char *) fxmllc(len+50);
  if (buff == NULL) fxexit("compress_file",1);

  strcpy(buff,"compress ");
  strcat(buff,file);
  (void) system(buff);
  fxfree( (void*) buff );

  return;
}



/////////////////////////////////////////////////////////////////
//
//                    copy_file
//
/////////////////////////////////////////////////////////////////

// Copy file1 to file2 making as few assumptions about the
// actions of the operating system as possible.  If file2 exists,
// it is deleted.

void copy_file(
  char   *file1,
  char   *file2
)
{
  ulong   len;
  char   *buff;

  if ( file_exists(file2) ) remove(file2);

  len  = strlen(file1) + strlen(file2);
  buff = (char *) fxmllc(len+50);

  if (buff == NULL) error("no room in copy_file");

  #ifdef MSVC
     #define COPY   "copy "
  #else
     #define COPY   "cp "
  #endif

  strcpy(buff,COPY);
  strcat(buff,file1);
  strcat(buff,file2);
  (void) system(buff);
  fxfree( (void*) buff );

  return;
}



/////////////////////////////////////////////////////////////////
//
//                       error
//
/////////////////////////////////////////////////////////////////

// Print an error message (presumably reporting where the error
// occurred) and exit.

void error(char *s)
{
  cout << s << endl;
  exit(1);
}



/////////////////////////////////////////////////////////////////
//
//                        file_exists
//
/////////////////////////////////////////////////////////////////

// Return 1 if file exists and 0 otherwise. 

long file_exists(
  char *file      // name of file
)
{
  FILE   *stream;

  stream = fopen(file,"r");

  if (stream == NULL) return 0;

  fclose(stream);
  return 1;
}



/////////////////////////////////////////////////////////////////
//
//                          frand
//
/////////////////////////////////////////////////////////////////

// Return a random float uniformly distributed on the interval
// [0,1).  See irand.

float frand(int  idum)
{
  return ( (float) irand(idum) )/ (1 + (float)INT_MAX);
}



/////////////////////////////////////////////////////////////////
//
//                        get_echo
//
/////////////////////////////////////////////////////////////////

// echo is private to this file, so it is accessed by this
// function.

_boolean_t get_echo(void)
{
  return echo;
}



/////////////////////////////////////////////////////////////////
//
//                       get_time
//
/////////////////////////////////////////////////////////////////

// Return the elapsed cpu time in seconds.

double get_time(void)
{
  update_time();
  return cpu_time;
}



/////////////////////////////////////////////////////////////////
//
//                        irand
//
/////////////////////////////////////////////////////////////////

//  A random number generator based on  'Numerical Recipes'
//  by Press, Flannery, Teukolsky, and Vetterling.  It uses
//  the UNIX-supplied random number generator rand.  The value
//  returned by irand is a random nonnegative int.  The generator
//  is initialized on its first call and whenever its argument
//  is negative.  In these cases the absolute value of the
//  argument is used as the seed to the system function.

int irand(int idum)
{
  static int  y;
  int  j;

  if (idum < 0 || iff == 0) {
    //  Initialize. 

    iff = 1;
    srand(abs(idum));

    // The results of the first 97 calls to rand() are ignored.

    for (j = 0; j < 97; j++) (void) rand();
    for (j = 0; j < 97; j++) v[j] = rand();

    y = rand();
  }

  // Use the last random number to select which component of  v
  // will be the next random number.  Ignore the low order five
  // bits of  y.  This effectively requires int's to have at
  // least 16 bits.

  j = (y>>5) % 97;
  y = v[j];
  v[j] = rand();
  return y;
}



/////////////////////////////////////////////////////////////////
//
//                        other_case
//
/////////////////////////////////////////////////////////////////

// Convert all letters in a character string to the opposite
// case.  All other characters are unchanged.

void other_case(
  char *str
)
{
  int    c;
  char  *ptr;

  if (str == NULL) return;

  ptr = str;

  while ( (c = *ptr) != '\0') {
    if      ( islower(c) ) c = toupper(c);
    else if ( isupper(c) ) c = tolower(c);

    *ptr++ = c;
  }
}



/////////////////////////////////////////////////////////////////
//
//                          parse
//
/////////////////////////////////////////////////////////////////

// Parse a character string into a sequence of words.  A word
// is defined to be a continuous sequence of non-white-space
// characters.  An array of pointers to the first characters in
// the words is returned.  Null characters are inserted at the
// end of each word.  If there are too many words, the extra
// words are ignored.  The number of words parsed is returned.

long parse(
  char    *string,   /* The string to be parsed. */
  char   **words,    /* Array of pointers to the words. */
  long     num_max   /* The maximum number of words. */
)
{
  long   num,len,c;
  char   *ptr;

  num       = 0;
  len       = 0;
  ptr       = string;

  if (NULL == string) return 0;
  if (num_max <= 0) return 0;

  if (NULL == words) error("No pointers in parse");

  while ('\0' != (c = *ptr) ) {
    if ( !isspace(c) && len == 0) {
      words[num++] = ptr;
      len = 1;
    }

    else if ( isspace(c) && len > 0) {
      *ptr = '\0';
      len  = 0;
      if (num >= num_max) break;
    }

    ptr++;
  }

  return num;
}



/////////////////////////////////////////////////////////////////
//
//                     response
//
/////////////////////////////////////////////////////////////////

// Read in a character string from the standard input.  The
// string is a response to a yes-no question.  The response is
// considered positive if it starts with a 'y'.

_boolean_t response(void)
{
  char  reply[100];

  scan_string(cin,reply);

  if ( reply[0] == 'y' || reply[0] == 'Y' ) return TRUE;
  return FALSE;
}



/////////////////////////////////////////////////////////////////
//
//                         scan_long
//
/////////////////////////////////////////////////////////////////

// Read in a long integer from an input stream and echo it
// to the standard output if in echo mode.  The integer is
// passed back to the calling routine in a parameter.

void scan_long(
  istream  &stream,
  long  &n
)
{
  stream >> n;

  if (!stream) error("Input error in scan_long");
  if (echo) cout << n;

  return;
}



/////////////////////////////////////////////////////////////////
//
//                       scan_string
//
/////////////////////////////////////////////////////////////////

// Read in a string from an input stream and echo it to the
// standard output if in echo mode.  The string is passed back
// to the calling program in a parameter.  The current line of
// input stream is cleared.

void scan_string(
  istream  &stream,
  char   *s
)
{
  stream >> s;
  stream.ignore(10000,'\n');

  if (!stream) error("Input error in scan_string");
  if (echo) cout << s << "\n";

  return;
}



/////////////////////////////////////////////////////////////////
//
//                         scan_ulong
//
/////////////////////////////////////////////////////////////////

// Read in a long unsigned integer from an input stream and
// echo it to the standard output if in echo mode.  The integer
// is passed back to the called routine in a parameter.

void scan_ulong(
  istream &stream,
  ulong  &n
)
{
  stream >> n;
  if (!stream) error("Input error in scan_ulong");
  if (echo)  cout << n;

  return;
}



/////////////////////////////////////////////////////////////////
//
//                        set_echo
//
/////////////////////////////////////////////////////////////////

// Change the current echoing.

void set_echo(
  _boolean_t   ech
)
{
  echo = ech;
}



/////////////////////////////////////////////////////////////////
//
//                   start_time
//
/////////////////////////////////////////////////////////////////

// Initialize the recording of elapsed cpu time.

void start_time(void)
{
  last_clock = clock();
  cpu_time   = 0.0;

  return;
}



/////////////////////////////////////////////////////////////////
//
//                    update_time
//
/////////////////////////////////////////////////////////////////

// Update the current elapsed cpu time.  This function must be
// called frequently since the system cpu clock overflows and is
// set to 0 often, typically every 36 minutes.

void update_time(void)
{
  unsigned long   t;

  t = clock();

  if (t > last_clock)
    cpu_time += ( (double)(t - last_clock) / TICKS_PER_SECOND);

  else if ( t < last_clock) {
    last_clock = ULONG_MAX - last_clock;
    cpu_time += ( (double)(last_clock + t) / TICKS_PER_SECOND);
  }

  last_clock = t;
  return;
}



/////////////////////////////////////////////////////////////////
//
//                        to_lower
//
/////////////////////////////////////////////////////////////////

// Convert the letters in a character string to lower case.

void to_lower(
  char   *s
)
{
  int    c;
  char  *t;

  for (t = s; '\0' != (c = *t); t++)
    if ( isupper(c) ) *t = tolower(c);

  return;
}



/////////////////////////////////////////////////////////////////
//
//                        uncompress_file
//
/////////////////////////////////////////////////////////////////

// Uncompress a file using the system uncompress command.  The
// name of the file passed to the function should be the
// uncompressed name, without the ".Z".  TRUE or FALSE is
// returned depending on whether the operation is successful or
// not.

_boolean_t uncompress_file(
  char  *file
)
{
  ulong   len;
  char   *buff;

  len  = strlen(file);
  buff = (char *) fxmllc(len+50);

  strcpy(buff,"uncompress ");
  strcat(buff,file);
  strcat(buff,".Z");

  if ( !file_exists(buff+11) ) return FALSE;
  (void) system(buff);
  fxfree( (void*) buff );

  return TRUE;
}




/* end of utility.C */
