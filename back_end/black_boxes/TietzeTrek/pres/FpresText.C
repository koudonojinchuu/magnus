// this file only applies for SUN version.  The analgous file on
// the Mac is CpresText

#ifndef Frank_GenuineMac
#include "crossTrainer.h"
#include "FpresText.h"
#include "text_functions.h"
#include "FTietzeDoc.h"
#include "rsrcInterface.h"
#include "GLfunctions.h"
#include "FtoolBox.h"

FpresText::FpresText(FTietzeDoc *theBoss){
	itsBoss=theBoss;
}

Boolean FpresText::ValidateString(StringPtr str){

//  only letters, numbers, tilde, blank, slash, and the minus sign allowed,
//  except in comments, which are ignored.  
//	Minus sign must follow alphabetic and precede numeric.  
//	Word must begin with alphabetic or tilde.  If tilde occurs, it must
//  occur exactly once at the beginning of the word.
//  can't have space between numerics, because this is visually misleading
//  A slash is the comment sign, anything after slash is ignored.

	long i,length;
	unsigned char item;
	Boolean expectingNumber=FALSE;
	short prevItem,thisItem,prevNonBlankItem;  
	length=str[0];
	if(length>TietzeLettersPerLine){
		gout <="invalid input detected";
		ExitToShell();	
	}
	
	prevItem=GLstartWord;
	prevNonBlankItem=GLstartWord;
	for(i=1;i<=length;++i){
		item=(unsigned char)str[i];
		thisItem=getCharType(item);
		if(thisItem==GLcomment)return TRUE;
		if(thisItem==GLtilde){
			if(prevNonBlankItem!=GLstartWord){
				gout <= "invalid input detected";
				ExitToShell();
				return FALSE;
			}
		}
		if(thisItem==GLother){
				gout <= "invalid input detected";
				ExitToShell();
			return FALSE;
		}
		if(expectingNumber && (thisItem!=GLnumber)){
				gout <= "invalid input detected";
				ExitToShell();
			return FALSE;
		}
		if(thisItem==GLminus){
			if(prevNonBlankItem!=GLletter){
				gout <= "invalid input detected";
				ExitToShell();
				return FALSE;
			}
		}
		else if(thisItem==GLnumber){
			if(prevItem==GLspace){
				if(prevNonBlankItem==GLnumber){
				gout <= "invalid input detected";
				ExitToShell();
					return FALSE;
				}
			}
			if(prevNonBlankItem==GLstartWord){
				gout <= "invalid input detected";
				ExitToShell();
				return FALSE;
			}
		}
		
		//update
		prevItem=thisItem;
		if(prevItem!=GLspace)prevNonBlankItem=prevItem;
		if(prevItem==GLminus)expectingNumber=TRUE;
		else expectingNumber=FALSE;
	}
	if(expectingNumber){
		// relation ended with a minus sign
				gout <= "invalid input detected";
				ExitToShell();
		return FALSE;
	}
	return(TRUE);
}
#endif
