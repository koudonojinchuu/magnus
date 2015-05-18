// chunk.c

#include "rkbp.h"


static chunk  *head_chunk = NULL;
static long    nfree      = 0;
static long    nused      = 0;


/////////////////////////////////////////////////////////////////
//
//                          get_chunk
//
/////////////////////////////////////////////////////////////////

chunk *get_chunk(void)
{
  long    i;
  chunk  *ch;

  if (head_chunk != NULL) {
    ch         = head_chunk;
    head_chunk = head_chunk->data.next;
    nfree--;
    nused++;
  }

  else {
    ch = (chunk *) fxmllc( CHUNK_NUMBER*sizeof(chunk) );

    if (NULL == ch)
      cout << "Unable to allocate new chunk\n";

    else {
      head_chunk = ch + 1;

      for (i = 0; i < CHUNK_NUMBER-2;  i++)
        head_chunk[i].data.next = head_chunk + i + 1;

      head_chunk[CHUNK_NUMBER-2].data.next = NULL;
      nused++;
      nfree += CHUNK_NUMBER - 1;
    }
  }

  return ch;
}


/////////////////////////////////////////////////////////////////
//
//                          free_chunk
//
/////////////////////////////////////////////////////////////////

void free_chunk(
  chunk  *ch
)
{
  ch->data.next = head_chunk;
  head_chunk = ch;
  nused--;
  nfree++;
}



/////////////////////////////////////////////////////////////////
//
//                    chunk_status
//
/////////////////////////////////////////////////////////////////

void chunk_status(void)
{
  cout << endl;
  cout << "Chunks:  nused = " << nused << ", nfree = "
       << nfree << endl << endl;
}



/////////////////////////////////////////////////////////////////
//
//                chunky_array::chunky_array
//
/////////////////////////////////////////////////////////////////

chunky_array::chunky_array()
{
  nchunks = 0;
  max_indexp = max_index = max_index2 = -1;
  chunks = NULL;
}



/////////////////////////////////////////////////////////////////
//
//                 chunky_array::~chunky_array
//
/////////////////////////////////////////////////////////////////

chunky_array::~chunky_array()
{
  long   i;

  for (i = 0; i < nchunks; i++)
    if (chunks->data.pdata[i] != NULL) {
      free_chunk(chunks->data.pdata[i]);
      chunks->data.pdata[i] = NULL;
    }

  if (NULL != chunks) free_chunk(chunks);
  chunks = NULL;
}




/////////////////////////////////////////////////////////////////
//
//                    chunky_array::extend
//
/////////////////////////////////////////////////////////////////

void chunky_array::extend(void)
{
  long    i;
  chunk  *ch;

  if (NULL == chunks) {
    chunks = get_chunk();

    if (NULL == chunks) {
      cout << "Can't create a chunky array." << endl;
      exit(1);
    }

    for (i = 0; i < CHUNK_SIZEP; i++)
      chunks->data.pdata[i] = NULL;
  }

  if (nchunks >= CHUNK_SIZEP) goto failure;
  ch = get_chunk();
  if (NULL == ch) goto failure;

  chunks->data.pdata[nchunks++] = ch;
  max_indexp += CHUNK_SIZEP;
  max_index  += CHUNK_SIZE;
  max_index2 += CHUNK_SIZE2;

  return;

 failure:
  cout << "Unable to extend chunky vector\n";
  exit(1);
}



/////////////////////////////////////////////////////////////////
//
//                  chunky_array::clear
//
/////////////////////////////////////////////////////////////////

void chunky_array::clear()
{
  long    i;
  chunk  *ch;

  if (NULL == chunks) return;

  for (i = 0; i < nchunks; i++) {
    ch = chunks->data.pdata[i];

    if (NULL != ch) {
      free_chunk(ch);
      chunks->data.pdata[i] = NULL;
    }
  }

  nchunks = 0;
  max_indexp = max_index = max_index2 = -1;
  free_chunk(chunks);
  chunks = NULL;
}



/////////////////////////////////////////////////////////////////
//
//                   chunky_array::free
//
/////////////////////////////////////////////////////////////////

void chunky_array::free(
  long   i
)
{
  if (i >= nchunks)
    cout << "Attempt to free undefine chunk.\n";
  else if ( chunks->data.pdata[i] == NULL )
    cout << "Attempt to free null chunk.\n";

  else {
    free_chunk( chunks->data.pdata[i] );
    chunks->data.pdata[i] = NULL;
  }

  return;
}



/////////////////////////////////////////////////////////////////
//
//                    chunky_array::lmove
//
/////////////////////////////////////////////////////////////////

void chunky_array::lmove(
  long dest,
  long source,
  long length
)
{
  long  i;

  if (dest == source) return;

  if (dest < source)
    for (i = 0; i < length; i++)
      insert_long(dest++,extract_long(source++) );

  else
    cout << "Can't lmove to the right." << endl;


  return;
}



/////////////////////////////////////////////////////////////////
//
//                    chunky_array::umove
//
/////////////////////////////////////////////////////////////////

void chunky_array::umove(
  long &dest,
  long source,
  long length
)
{
  long  i;

  if (dest == source) return;

  if (dest < source)
    for (i = 0; i < length; i++)
      insert_ulong(dest++,extract_ulong(source++) );

  else 
    cout << "Can't umove to the right." << endl;

  return;
}



/////////////////////////////////////////////////////////////////
//
//                 chunky_array::copy_and_free
//
/////////////////////////////////////////////////////////////////

void chunky_array::copy_and_free(
  long           dest,
  chunky_array  *vec,
  long           source,
  long           length
)
{
  long   i,j,k,dest_chunk,dest_length,source_chunk,source_length;
  long   copy_length;

  while (length > 0) {
    if (dest > max_index) extend();
    dest_chunk    = dest>>CHUNK_BITS;
    dest_length   = (dest_chunk + 1)*CHUNK_SIZE - dest;
    source_chunk  = source>>CHUNK_BITS;
    source_length = (source_chunk + 1)*CHUNK_SIZE - source;

    copy_length = min(length,min(dest_length,source_length) );
    j = dest&CHUNK_MASK;
    k = source&CHUNK_MASK;

    for (i = 0; i < copy_length; i++)
      chunks->data.pdata[dest_chunk]->data.ldata[j++] =
        vec->chunks->data.pdata[source_chunk]->data.ldata[k++];

    length -= copy_length;
    dest   += copy_length;
    source += copy_length;
    if (CHUNK_SIZE == k) vec->free(source_chunk);
  }

  return;
}



/////////////////////////////////////////////////////////////////
//
//                   chunky_array::retract
//
/////////////////////////////////////////////////////////////////

void chunky_array::retract(void)
{
  if (nchunks <= 0) return;
  nchunks--;
  free_chunk(chunks->data.pdata[nchunks]);
  max_indexp -= CHUNK_SIZEP;
  max_index  -= CHUNK_SIZE;
  max_index2 -= CHUNK_SIZE2;
}



/////////////////////////////////////////////////////////////////
//
//                    chunky_array::flag_redundant
//
/////////////////////////////////////////////////////////////////

void chunky_array::flag_redundant(
  long orig
)
{
  ulong   z;

  z = extract_ulong(orig) | (1<<(_LONG_BIT-1) );
  insert_ulong(orig,z);
}
