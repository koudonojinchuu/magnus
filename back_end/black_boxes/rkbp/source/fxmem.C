/* fxmem.C */

/* The functions in this file supplement the services of malloc
   by allowing the user to specify a maximum total amount of
   memory to be manageed, to inquire about the amount of this
   maximum still unallocated, and to obtain summary statistics
   about memory usage.

   Note that implicit calls to malloc from C++ constructors are
   not covered by these facilities!

   If the variable  verbose  is nonzero, the changes in memory
   are reported.

   The macro FXSIZE defined in fxmem.h computes the usable size
   in bytes of an allocated block.

   The functions are:

   fxask   Ask whether memory reporting should be turned on.

   fxexit  Abort due to allocation problem.

   fxfree  Free an allocated block.

   fxmllc  Allocate a new block.  Same usage as malloc.

   fxrllc  Resize an existing block.  Same usage as realloc.

   fxstat  Print memory usage statistics.  */



#include  <stdio.h>
#include  "rkbp.h"

#ifdef MSVC
  #include <malloc.h>
#endif



/* Each block allocated by malloc is prefixed with a bucket whose
   element size gives the number of bytes in the block not
   counting the prefix bucket, that is, the usable size of the
   block.  */


/* The following static variables record memory usage. */


static  ulong  cur_alloc   = 0;   /* the amount of memory
                                      currently allocated */

static  ulong  max_alloc   = 0;   /* the maximum total
                                      amount of memory
                                      allocated at any
                                      time */

static  ulong  total_mllc  = 0;   /* the total of the amounts
                                      requested from fxmllc */

static  ulong  total_incr  = 0;   /* the total of the amounts
                                      by which fxrllc has been
                                      asked to increase the
                                      size of blocks */

static  ulong  total_decr  = 0;   /* the total of the amounts
                                      by which fxrllc has been
                                      asked to decrease the
                                      size of blocks */

static  ulong  nblocks     = 0;   /* the number of blocks
                                      currently allocated */

static  ulong  max_blocks  = 0;   /* the maximum number of
                                      allocated blocks */

static  ulong  max_size    = 0;   /* the size of the largest
                                      block allocated */

static  ulong  nmalloc     = 0;   /* the number of times a
                                      new block has been
                                      successfully
                                      requested */

static  ulong  nincrease   = 0;   /* the number of times a
                                      request to increase the
                                      size of a block has
                                      been successful */

static  ulong  ndecrease   = 0;   /* the number of times a
                                      request to decrease the
                                      size of a block has
                                      been successful */

static  ulong  nfree       = 0;   /* the number of times
                                      a block has been
                                      freed */

static  int  verbose       = 0;   /* controls reporting */



/****************************************************************

                        fxask

****************************************************************/

/* Ask whether memory reporting should be turned on. */

void fxask(void)
{
  cout << "Do you want memory reporting turned on (y/n):  ";

  if (YES) verbose = 1;
  else verbose = 0;

  return;
}




/****************************************************************

                            fxexit

****************************************************************/

void fxexit(
  char  *routine,
  long   loc
)
{
  cout << "Unable to allocate memory in " << routine
       << ", location " << loc << ".\n";

  exit(1);
}



/****************************************************************

                             fxfree

****************************************************************/

/* Frees an allocated block.  Same syntax as free. */

void fxfree(
  void huge *ptr
)
{
  ulong        size,total_size;
  bucket huge *buck;
  
  buck       = (bucket huge *)ptr - 1;
  size       = buck->size;
  total_size = size + sizeof(bucket);

  if (size < 50000) free( (void *) buck);

  else {
#ifdef MSVC
    hfree( (void *) buck);
#else
    free( (void *) buck);
#endif
  }

  cur_alloc -= total_size;
  nblocks--;
  nfree++;

  if (verbose) {
    printf("Free block of size %lu at %lu.\n",total_size,
       (ulong)buck);
  }

  return;
}



/****************************************************************

                           fxmllc

****************************************************************/

/* Allocate a new block of memory. Same usage as malloc. */

void huge *fxmllc(
  ulong   size      /* the size in bytes of the block being
                          requested */
)
{
  ulong        total_size;
  bucket huge *buck;

  total_size = size + sizeof(bucket);

  if (size < 50000)
    buck = (bucket huge *) malloc( (size_t) total_size);

  else {

#ifdef MSVC
    total_size = (total_size + 1023) >> 10;
    buck =
      (bucket huge *) halloc( (long) total_size, (size_t) 1024);
#else
    buck = (bucket huge *) malloc( (size_t) total_size);
#endif

  }

  if (verbose) {
    if (buck != NULL)
      printf("Allocate block of size %lu at %lu.\n",total_size,
                (ulong)buck);
    else
      printf("Unable to allocate block of size &lu.\n",
             total_size);
  }

  if (buck == NULL) return NULL;

  buck->size  = size;
  total_mllc += size;
  cur_alloc  += total_size;

  if (cur_alloc > max_alloc) max_alloc = cur_alloc;
  if (max_blocks < ++nblocks) max_blocks++;
  nmalloc++;
  max_size = max(max_size,total_size);

  return (void huge *) (buck+1);
}



/****************************************************************

                            fxrllc

****************************************************************/

/* Changes the size of an allocated block.  Same syntax as
   realloc. */

void huge *fxrllc(
  void   huge  *ptr,
  ulong         size
)
{
  long       min_size,i;
  ulong      cur_total_size,new_total_size,diff,old_size;
  char       huge *s, huge *t;
  void       huge *new_ptr;
  bucket     huge *buck, huge *new_buck;
  
  buck     = (bucket huge *)ptr - 1;
  old_size = buck->size;

  cur_total_size = old_size + sizeof(bucket);
  new_total_size = size + sizeof(bucket);

#ifdef MSVC
  /* There is no hrealloc function. */

  new_ptr = fxmllc(size);
  if (new_ptr == NULL) goto failure;

  new_buck = (bucket huge *)new_ptr - 1; 
  min_size = min(size,old_size);

  s = (char huge *) ptr;
  t = (char huge *) new_ptr;
  for (i = 0; i < min_size; i++) *t++ = *s++;

  fxfree(ptr);
#else
  new_buck = (bucket huge *)
              realloc( (void huge *) buck,
                       (size_t) new_total_size);
  if (new_buck == NULL) goto failure;
  diff       = new_total_size - cur_total_size;
  cur_alloc += diff;

  if (cur_alloc > max_alloc) max_alloc = cur_alloc;
  max_size = max(max_size,new_total_size);
#endif

  new_buck->size = size;

  if (cur_total_size > new_total_size) {
    diff        = cur_total_size - new_total_size;
    total_decr += diff;
    ndecrease++;
  }

  else {
    diff        = new_total_size - cur_total_size;
    total_incr += diff;
    nincrease++;
  }

  if (verbose) {
    printf("Changed block at %lu of size %lu\n",
             (ulong)buck,cur_total_size);
    printf("   to block at %lu of size %lu.\n",
             (ulong)new_buck,new_total_size);
  }

  return (void huge *) (new_buck+1);

 failure:
  if (verbose)
    printf("Unable to change block at %lu from %lu to %lu.\n",
      (ulong)buck,cur_total_size,new_total_size);

  return NULL;
}



/****************************************************************

                       fxstat

****************************************************************/

/* Print statistics about the memory management function. */

void fxstat(void)
{
  cout << "\n";

  cout << "max_alloc  = " << setw(12) << max_alloc <<
       "   cur_alloc  = " << setw(12) << cur_alloc << endl;

  cout << "nmalloc    = " << setw(12) << nmalloc <<
       "   total_mllc = " << setw(12) << total_mllc << "\n";

  cout << "total_incr = " << setw(12) << total_incr <<
       "   total_decr = " << setw(12) << total_decr << "\n";

  cout << "nblocks    = " << setw(12) << nblocks <<
       "   max_blocks = " << setw(12) << max_blocks << "\n";

  cout << "nfree      = " << setw(12) << nfree <<
       "   max_size   = " << setw(12) << max_size   << "\n";

  cout << "nincrease  = " << setw(12) << nincrease <<
       "   ndecrease  = " << setw(12) << ndecrease  << "\n";

  cout << "\n";

  chunk_status();
  return;
}



/* end of fxmem.C */
