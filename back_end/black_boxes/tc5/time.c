#include "sys/time.h"
#include "sys/resource.h"
#include "stdio.h"
extern FILE *fout;
/* #include "tctime.h"*/

void tc_time(end_enum)
int	end_enum;
{
/* end_enum==1 means enumerations end.  */
/* get date and time, get information about resource utilization */
#ifdef sparc
#define HZ 100
#endif
#ifndef sparc
#define HZ 50
#endif
	struct rusage *tcusage, tccusage; /* resource usage */
	struct timeval *tp,tpp; /* time of day */
	struct timezone *tzp, tzpp;
	int 	secd,ssecd,msecd,mssecd,ars,namelen;
	long 	CLock;
	static long betime;
	char	*tcdate,name[60]; 
/* once g26 with CT:0, RT:2000, after "TC collapse" , get wrong et; it was
*  fixed by define CLock, secd,ssecd,msecd,mssecd,ars,namelen; as static
*/
	tp = &tpp;
	tzp = &tzpp;
	tcusage = &tccusage;
	while (gettimeofday(tp, tzp) < 0);/* get time of day */
	if (betime == 0 || end_enum) { 
		betime = tp->tv_sec;
		tcdate = ctime(&betime);
		namelen = 60;
		gethostname(name, namelen); /* get host name */
		if (end_enum)
			fprintf(fout,"End TC v5.01 at %.24s on %s\n", tcdate,name);
		else
			fprintf(fout,"Start TC v5.01 at %.24s on %s\n", tcdate,name);
		return;
	}
	CLock = tp->tv_sec;
	while(getrusage(RUSAGE_SELF,tcusage) < 0);
/* get information about resource utilization */ 
	secd = tcusage->ru_utime.tv_sec; /* user time in seconds */
	ssecd = tcusage->ru_stime.tv_sec; /* system time in seconds */
	msecd = tcusage->ru_utime.tv_usec; /* user time in microseconds */
	mssecd = tcusage->ru_stime.tv_usec;/*system time in microseconds */
	msecd = (msecd / 10000) % 100;
	mssecd = (mssecd / 10000) % 100; 
/*
	ars = (tcusage->ru_ixrss + tcusage->ru_isrss + tcusage->ru_idrss) / (HZ * (secd + ssecd) + (msecd + mssecd ) / 2);
*/
	fprintf(fout,"et=%d ut=%d.%d st=%d.%d mrs=%d rpf=%d\n",CLock-betime,secd,msecd,ssecd,mssecd,tcusage->ru_maxrss,tcusage->ru_majflt);
	return;
}
float utl_second()
{
        float   ftime;
        struct rusage *tcusage, tccusage; /* resource usage */
	long     msecd;

	tcusage = &tccusage;
        while(getrusage(RUSAGE_SELF,tcusage) < 0);
        msecd = tcusage->ru_utime.tv_usec; /* user time in microseconds */
	ftime = (float) (msecd / 1000);
	ftime = ftime / 1000.0;
	ftime += tcusage->ru_utime.tv_sec;
	return (ftime);
}
/* End File */

