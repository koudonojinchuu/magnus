/* file to give error message */
int
yyerror(c)
char * c;
{
  char l = c[0];
  switch (l) 
	{	case '1': fprintf(fout," expect 1 for identity\n");
			return 0; 
		case '2': fprintf(fout," expect positive number\n");
			return 0; 
		case '3': fprintf(fout," insufficient space \n"); 
			return 0;
		case '4': fprintf(fout," expect ';' for ending group generaters\n");
			return 0;
		case '5': fprintf(fout," there are not so many relators or subg gens\n"); 
			return 0;
		case '6': fprintf(fout," expect group gens or ';' \n");
			return 0;
		case '7': fprintf(fout," could not open input file.\n");
	}
	return 0; 
} 
/* end file */
