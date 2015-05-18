// utility.h 

// This header file declares various basic utility functions and
// related macros.


#define  NO                (response() == FALSE)
#define  YES               (response() == TRUE)



extern  _boolean_t       bool_t( int );
extern  void          compress_file(char *);
extern  void          copy_file(char *, char *);
extern  void          error(char *);
extern  long          file_exists(char *);
extern  float         frand(int);
extern  _boolean_t       get_echo(void);
extern  double        get_time(void);
extern  int           irand(int);
extern  void          other_case(char *str);
extern  long          parse(char *,char **,long);
extern  _boolean_t       response(void);
extern  void          scan_long(istream &stream,long &);
extern  void          scan_string(istream &stream,char *);
extern  void          scan_ulong(istream &stream,ulong &);
extern  void          set_echo(_boolean_t ech);
extern  void          start_time(void);
extern  void          to_lower(char *);
extern  _boolean_t       uncompress_file(char *);
extern  void          update_time(void);

// end of utility.h
