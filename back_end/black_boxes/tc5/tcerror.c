/* file to give error message */
yyerror(n)
int	n;
{ 	switch (n) 
	{	case 1: fprintf(fout," expect 1 for identity\n");
			return; 
		case 2: fprintf(fout," expect positive number\n");
			return; 
		case 3: fprintf(fout," insufficient space \n"); 
			return; 
		case 4: fprintf(fout," expect ';' for ending group generaters\n");
			return;
		case 5: fprintf(fout," there are not so many relators or subg gens\n"); 
			return;
		case 6: fprintf(fout," expect group gens or ';' \n");
			return;
		case 7: fprintf(fout," could not open input file.\n");
	}
	return; 
} 
/* end file */
