
/* file that forms a menu */ 
#include "curses.h"
#include "stdio.h" 
#include "string.h"

#define 	NCOLS 	80 
#define		NLINES	24
char *cc; 
instr(y,x,s) 
int  y,x; 
char **s; /* allocate storage for s and input its content */ 
{	char *malloc();
	char *ch; 
	int  i,c,n;
	
	n = 0;
	ch = (char *) malloc(1000);  
	move(y,x);
	for (i = x; ;){
		move(y,i); 
		refresh();
		c = getch();
		if (c == '\n')  {  break; }
		if (c == '\177' && n > 0) { i--; n--;ch[n]=' ';
			 mvaddch(y,i,ch[n]); 
			refresh();
		} else { addch(c); refresh(); ch[n++] = c;i++; }
	}
	if (n) { 
        	*s = (char *) malloc(n+1);
		if (*s == 0) { 
			fprintf(stderr,"out of memory"); 
			exit(); 
		} else strncpy(*s,ch,n); 
	}
	free(ch);  
} 
inchar(y,x,c)  /* get a char from standard input */ 
int  y,x; 
int *c; 
{	int r; 
	
	move(y,x); 
	refresh(); 
	if ((r = getch()) != '\n'){ 
		*c = r; addch(r); refresh();
	} 
} 	
void main(){
	char	*ch,*enuname,*g_gen,*g_rel,*subg,*s_gen;
	int 	para,costab;
	char	*max_cosets,*f_type,*time1,*fill_factor,*realors_in_subgroup; 
	char	*a_factor,*compact,*message_interval,*workspace; 
	char	*diagnostics;
	char	*tcintc;
	FILE	*tcout;

/* put default values in variables */ 
	tcintc = "tcintc";
	enuname = "First";
	subg = "H";
	s_gen = ";";  
	f_type = "0;"; 
	a_factor = "0;";
	max_cosets = "0;"; 
	time1 = "0.0;"; /* infinite time */ 
	compact = "0;";
	fill_factor = "0;"; 
	realors_in_subgroup = "0;"; 
	message_interval = "0;"; 
	diagnostics = "0;";
	workspace = "200000;"; 
/* display menu */ 
	if (initscr() == ERR )  exit();
	cbreak();
	noecho(); 
	
	ch="ENUMeration Name";
	mvaddstr(3,5,ch);
	mvaddstr(4,5,"Group generators:"); 
	mvaddstr(5,5,"Relators:"); 
	mvaddstr(8,5,"Subgroup name:"); 
	mvaddstr(9,5,"Subgroup generators: ;"); 
	mvaddstr(14,5,"Parameters: ");
	mvaddstr(16,10,"Compaction %: 0;"); 
	mvaddstr(17,10,"Max cosets: the maximum number of cosets(min,max,step) 0;"); 
	mvaddstr(18,10,"CT Factor(min,max,step): 0;");
	mvaddstr(19,10,"RT factor(min,max,step): 0;"); 
	mvaddstr(20,10,"Time(limited execution time):0.0;"); 
	mvaddstr(21,10,"Fill Factor(min,max,step):0;"); 
	mvaddstr(22,10,"NO. of Relators in Subgroup(min,max,step):0;"); 
	mvaddstr(23,10,"Messages:0;"); 
	mvaddstr(24,10,"Diagnostics:0;");
	mvaddstr(25,10,"Workspace:200000;"); 
	mvaddstr(26,10,"END"); 
	mvaddstr(38,2,"Messages 0 for brief output;");
	mvaddstr(36,1,"NOTE:");
	refresh(); 	
/* input various data */
	instr(3,22,&enuname);
	instr(4,22,&g_gen); 
	instr(5,15,&g_rel); 
	instr(8,20,&subg);
	instr(9,26,&s_gen); 
	mvaddstr(14, 17,"default? (Y/N) "); 
	inchar(14,32,&para); 
	if (para != 'y' && para != 'Y') { /* change default values */   
		inchar(16,24,&compact); 
		instr(17,65,&max_cosets); 	  
		instr(18,35,&f_type); 
		instr(19,35,&a_factor); 
		instr(20,39,&time1); 
		instr(21,36,&fill_factor); 
		instr(22,52,&realors_in_subgroup); 
		instr(23,19,&message_interval); 
		instr(24,22,&diagnostics);
		instr(25,20,&workspace); 
	} 
/* write data to data file */ 
	tcout = fopen(tcintc, "w"); 
	fprintf(tcout,"ENUMeration:%s\n",enuname); 
	fprintf(tcout,"GRoup generators:%s\n",g_gen); 
	fprintf(tcout,"RELators:%s\n",g_rel); 
	fprintf(tcout,"SUBGroup:%s\n",subg); 
	fprintf(tcout,"GENerators:%s\n",s_gen);  
	fprintf(tcout,"PARameters:\n");  
	fprintf(tcout,"\t COMpact:%s;\n",compact);  
	fprintf(tcout,"\t MAX Cosets (min,max,step):%s\n",max_cosets);  
	fprintf(tcout,"\t CT factor(min,max;step):%s\n",f_type);  
	fprintf(tcout,"\t RT factor(min,max;step):%s\n",a_factor);  
	fprintf(tcout,"\t TIme:%s\n",time1);  
	fprintf(tcout,"\t FIll factor (min,max;step):%s\n",fill_factor);  
	fprintf(tcout,"\t NO. of relators in subgroup (min,max;step):%s\n",realors_in_subgroup);  
	fprintf(tcout,"\t MEssages:%s\n",message_interval);  
	fprintf(tcout,"\t WOrkspace:%s\n",workspace);
	fprintf(tcout,"\t DIagnostics:%s\n",diagnostics);
	fprintf(tcout,"End\n");  
	refresh();
	fclose(tcout); 
	clear(); 
	refresh();
	endwin(); 
	echo();
}

	
