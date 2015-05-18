YFLAGS = -d   
CFLAGS = -lm   -g
HEADERS = tc.h defs.h ct.h miscellaneous.h
BINDIR = ./
CC = gcc
TC = $(BINDIR)/tc
#TC = tc
OBJS   = tcparse.o \
	 tclex.o \
        apply.o \
	assemble_edp.o \
	assemble_gens.o \
	check_index1.o \
        coinc.o \
        coinc_cols12.o \
        compact.o \
        proc_ded.o \
        print_ct.o \
        ctrenumber.o\
        enum.o \
	lexer.o \
	main.o \
	opt.o \
	parser.o \
        norelator.o \
	save_ct.o \
        set_relators.o \
	subg_proc.o \
        text.o \
        todd_coxeter.o \
	translate.o \
	time.o

.KEEP_STATE:

all di:	$(TC)

di := CFLAGS= -lm -ll -g -D__DI

$(TC):	$(OBJS)   
	$(CC)  $(OBJS) $(CFLAGS) -o $@ 
#	rm -f tcyacc.c tclex.c
tcyacc.o: tcyacc1.h tcyacc.h $(HEADERS) tcerror.c 
tclex.o:     tcyacc1.h y.tab.h 
lexer.o:	tcyacc1.h tcyacc.h parser.h
parser.o:	tcyacc1.h tcyacc.h parser.h
main.o:		tcyacc1.h tcyacc.h tc.h
apply.o: 	apply.h ded.h coset_def.h $(HEADERS)	
assemble_edp.o:	tc.h
assemble_gens.o:	tc.h tcyacc.h
check_index1.o:	$(HEADERS)
coinc.o:	coinc.h ded.h $(HEADERS)	
coinc_cols12.o:	coinc_cols12.h ded.h  $(HEADERS)
compact.o:	compact.h $(HEADERS)	
ctrenumber.o:	$(HEADERS)
enum.o:		enum.h ded.h coset_def.h $(HEADERS)
norelator.o:	norelator.h tc.h ct.h coset_def.h
print_ct.o:	 $(HEADERS)
proc_ded.o:	proc_ded.h ded.h $(HEADERS)
save_ct.o:	tc.h tcyacc.h
set_relators.o: $(HEADERS)
subg_proc.o:	$(HEADERS)
text.o:		 $(HEADERS)
todd_coxeter.o:	todd_coxeter.h tcyacc.h $(HEADERS) 
time.o:		
translate.o:	$(HEADERS)
tc.h:	defs.h

tcin:   tcscn.c
	$(CC)  tcscn.c -lcurses -ltermlib -o tcin

clean: 
	rm -f $(OBJS) tc 

mostlyclean: 
	rm -f $(OBJS) 
