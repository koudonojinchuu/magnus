// chunk.h

// This file defines the chunk, a large block of memory used for
// random access to dense data.


#define  CHUNK_SIZEP 16384
#define  CHUNK_SIZE  16384
#define  CHUNK_SIZE2 8192
#define  CHUNK_BITSP 14
#define  CHUNK_BITS  14
#define  CHUNK_BITS2 13
#define  CHUNK_MASKP 037777L
#define  CHUNK_MASK  037777L
#define  CHUNK_MASK2 017777L
#define  CHUNKY_LENGTH  16384
#define  CHUNK_NUMBER   7



/////////////////////////////////////////////////////////////////
//
//                          chunk
//
/////////////////////////////////////////////////////////////////

typedef struct chunk 
{
  union {
    struct chunk  *next;
    struct chunk  *pdata[CHUNK_SIZEP];
    long           ldata[CHUNK_SIZE];
    ulong          udata[CHUNK_SIZE];
    long           ldata2[CHUNK_SIZE2][2];
  }   data;
} chunk;


extern chunk  *get_chunk(void);
extern void    free_chunk(chunk *ch);
extern void    chunk_status(void);





/////////////////////////////////////////////////////////////////
//
//                     chunky_array
//
/////////////////////////////////////////////////////////////////

class chunky_array
{
 protected:
  long     nchunks;
  long     max_indexp;
  long     max_index;
  long     max_index2;
  chunk   *chunks;

 public:
  chunky_array();

  ~chunky_array();
  long get_nchunks() { return nchunks; }
  long get_max_indexp() { return max_indexp; }
  long get_max_index() { return max_index; }
  long get_max_index2() { return max_index2; }

  long extract_long( long i)
    { return chunks->data.pdata[i>>CHUNK_BITS]->
                     data.ldata[i&CHUNK_MASK]; }

  void insert_long( long i, long x)
    { chunks->data.pdata[i>>CHUNK_BITS]->
              data.ldata[i&CHUNK_MASK] = x; }

  ulong extract_ulong( long i)
    { return chunks->data.pdata[i>>CHUNK_BITS]->
                     data.udata[i&CHUNK_MASK]; }

  void insert_ulong( long i, ulong x)
    { chunks->data.pdata[i>>CHUNK_BITS]->
              data.udata[i&CHUNK_MASK] = x; }

  long extract_long2( long i, long j)
    { return chunks->data.pdata[i>>CHUNK_BITS2]->
                     data.ldata2[i&CHUNK_MASK2][j]; }

  void insert_long2( long i, long j, long x)
    { chunks->data.pdata[i>>CHUNK_BITS2]->
              data.ldata2[i&CHUNK_MASK2][j] = x; }

  void extend();

  void clear();

  void free( long i );

  void lmove(long dest, long source, long length);

  void umove(long &dest, long source, long length);

  void copy_and_free(
    long           dest,
    chunky_array  *vec,
    long           source,
    long           length
  );

  void retract(void);
  void flag_redundant( long orig );
};
