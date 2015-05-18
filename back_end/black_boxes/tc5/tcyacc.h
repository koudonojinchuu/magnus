
#include "tcyacc1.h"
extern Enum_def *pt;
#ifndef ___alias___
#define ___alias___
/*------ structure enum_def ---------------------------*/
#define  Enum_name      pt->name
#define  Gen_st         pt->gen
#define  Gen_no         pt->gen_no
#define  Relators       pt->relators
#define  Rel_no         pt->rel_no
#define  Rel_len	pt->rel_len
#define  Subg_name      pt->s_name
#define  S_gens         pt->s_gens
#define  Sgen_no        pt->sgen_no
#define  Sgn_len	pt->sgn_len
#define  Compact	pt->compact
#define  Max_coset	pt->max_coset
#define  Max    	pt->max
#define  Time           pt->time
#define  F_type		pt->f_type
#define  Ct		pt->ct
#define  A_factor		pt->a_factor
#define  Rt		pt->rt
#define  Fill_factor    pt->fill_factor
#define  Fi        	pt->fi
#define  Rel_in_sgp     pt->rel_in_sgp
#define  RelInSg       	pt->rel_in_sg
#define  Diag		pt->diag
#define	 Strategy 	   pt->strategy
#define  Message_interval  pt->message_interval
#define  Workspace      pt->workspace 
#define  Space		pt->space
#define  Page_strategy	pt->page_strategy
#define  Unsort 	pt->unsort
/* definitions for options */
#define  Restart	pt->restart
#define  Sr_l		pt->sr_l
#define  Sa_l		pt->sa_l
#define  Res_l		pt->res_l
#define  In_file	pt->in_file
#define  Newrel		pt->newrel
#define  Newsg		pt->newsg
#define  Pr_l		pt->pr_l
#define  Pr		pt->pr
#define  Cc_l		pt->cc_l
#define  Cc		pt->cc
#define  Sc_l		pt->sc_l
#define  Scn		pt->scn
#define	 Del_l		pt->del_l
#define  Tw_l		pt->tw_l
#define  Twn		pt->twn
#define  Nc_l		pt->nc_l
#define  Ncn		pt->ncn
#define  Cy_l		pt->cy_l
#define  Oo_l		pt->oo_l
#define  Oon		pt->oon
#define  Rc_l		pt->rc_l
#define  Rcn		pt->rcn
#define  RcStop		pt->rcstop
#define  RcDesire	pt->rcdesire
#endif 
