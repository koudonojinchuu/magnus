#include "tcyacc1.h"
#define   AI    127
#define   AO    128
#define   BYE   129
#define   CC    130
#define   COM   131
#define   CONT  132
#define   CT    133
#define   CY    134
#define   DI    135
#define   DR    136
#define   DS    137
#define   ENUM  138
#define   END   140
#define   FI    142
#define   GEN   143
#define   GR    144
#define   MAX   145
#define   ME    146
#define   NC    147
#define   NO    148
#define   OO    149
#define   PAR   150
#define   PR    151
#define   RC    152
#define   REL   153
#define   RES   154
#define   RL    155
#define   RT    156
#define   SA    157
#define   SC    158
#define   SG    159
#define   SORT	160
#define   SR    161
#define   SUBG  162
#define   TI   163
#define   TW   164
#define   WO   165
#define	KEY_LEN 	41	
typedef struct Keyword { /* Keywords  */
	char *name;
	int kval;
} Keyword;
static Keyword keywords[KEY_LEN] = {
	"AI", 	AI,
	"AO",	AO,
	"Bye",	BYE,
	"bye",	BYE,
	"CC",	CC,	
	"COM",	COM,
	"CONT",	CONT,
	"CT",	CT,
	"CY",	CY,
	"DI",	DI,
	"DR",	DR,
	"DS",	DS,
	"ENUM",	ENUM,
	"End",  END,
        "END",  END,
        "end",  END,
	"FI",	FI,
	"GEN",	GEN,
	"GR",	GR,
	"MAX",	MAX,
	"ME",	ME,
	"NC",	NC,
	"NO",	NO,
	"OO",	OO,
	"PAR",	PAR,
	"PR",	PR,
	"RC",	RC,
	"REL",	REL,
	"RES",	RES,
	"RL",	RL,
	"RT",	RT,
	"SA",	SA,
	"SC",	SC,
	"SG",	SG,
	"SORT",	SORT,
	"SR",	SR,
	"SUBG", SUBG,
        "TI",   TI,
        "TW",  TW,
        "WO",  WO,
	"",	0,
};

typedef struct Word_pt {  /* word pointer and the number of words  */
	Rel_stk_type *rel_pt;
	int 	numb;
} Word_pt;	
#define NumOfWord  w_pt->numb
#define Pt2Word   w_pt->rel_pt
