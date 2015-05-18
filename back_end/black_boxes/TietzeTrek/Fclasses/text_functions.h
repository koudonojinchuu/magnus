#ifndef Frank_textfunctions
#define Frank_textfunctions


class  FfreeWord;
#ifdef Frank_GenuineMac
CLASS  CStringArray;
#else
class FStringArray;
#endif

class FwordToStringArrayInfo:public FObject{
public:
	short displayWidth;     	//width in pixels of display rectangle
#ifdef Frank_GenuineMac
	CStringArray *strs;				//the string array to append to
#else
	FStringArray *strs;
#endif
	short lettersPerLine;			//the max number of characters allowed in a line
	short slack;					//a fudge factor for tuning the actual display
	long lettersNo;					//the number of letters admissable letters
									//in the free word
	unsigned char **numberToLetter; //the lut for the translating the word
};

// text utilities
	short getCharType(unsigned char item);
	void removeBlanks(
#ifdef Frank_GenuineMac
			CStringArray *strs,				
#else
			FStringArray *strs,
#endif
		long index);
	void wordToStringArray(FfreeWord *w,FwordToStringArrayInfo *info);
	void buildItem(FwordToStringArrayInfo *info,unsigned char *item,
			   	   Str255 str,int width);
	void stringParse(
#ifdef Frank_GenuineMac
		CStringArray *aStringArray,				
#else
		FStringArray *aStringArray,
#endif
		Handle theData);

	
enum{GLstartWord,GLletter,GLnumber,GLminus,GLspace,GLcomment,
	GLtilde,GLother};

//  ascii is forever!!!

#define GLMINUS 0x2d
#define GLSPACE 0x20
#define GLTILDE 0x7e
#define GLslash 0x2f
#define GLbeginCaps 0x40
#define GLendCaps 0x5b
#define GLbeginLow 0x60
#define GLendLow 0x7b
#define GLbeginNum 0x2f
#define GLendNum 0x3a
#define GLcarriageReturn '\r'
#define GLnewLine  '\n'

class  lineAttributesInfo{
public:	
				Str255 s;						//input to getLineAttributes
				Boolean skipLine;				// output
				Boolean continuationLine;		// output
				long firstLetter;				// output
				};

#endif

