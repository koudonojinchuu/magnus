#ifndef __CONDITION_H__
#define __CONDITION_H__

#define WARNING 1
#define ERROR   2

void debugMessage( int msgType, bool condition, const char *condDescription, 
		   const char *fileName, int lineno );

//#define PRECONDITION(type,condition)	debugMessage( (type), (condition), "PRECONDITION "###condition, __FILE__, __LINE__)
#define PRECONDITION(type,condition)	debugMessage( (type), (condition), "PRECONDITION "#condition, __FILE__, __LINE__)
#define CONDITION(type,condition)	debugMessage( (type), (condition), "CONDITION "#condition, __FILE__, __LINE__)



/*
 * @dp old macros:
#define __DP_ASSERT(header,type,condition)    \
  do {					\
   if( !(condition) ) {			\
      cout << endl;			\
      if( (type) == WARNING )		\
        cout << "WARNING: ";		\
      else if( (type) == ERROR )	\
        cout << "ERROR: ";		\
    cout << endl << __FILE__ << ":" << __LINE__ << ": " \
	 << header << " \"" << #condition << "\" is false." << endl; \
   }					\
  } while( 0 )


#define PRECONDITION(type,condition)	__DP_ASSERT("PRECONDITION",(type),(condition))
#define CONDITION(type,condition)	__DP_ASSERT("CONDITION",(type),(condition))
*/
#endif
