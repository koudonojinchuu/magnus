#ifndef Frank_rsrcInterface
#define Frank_rsrcInterface



// TABLE OF CONTENTS--DON'T PANIC!!!

// MENU IDs

// WIND ID's

// DLOG ID's

// Application Command ID's

// button titles

// static strings

// dialog validation constants

// The presentation dialog commands, 
// dialog item numbers, and constants

// The Nielsen dialog commands, 
// dialog item numbers, and constants

// The trivial doc stuff

// The Memory menu commands, dialog command, and
// dialog item numbers

// the memory trigger dialog item numbers

// interface related constants (a grab bag)

// THIS CONCLUDES THE TABLE OF CONTENTS--PANIC NOW!!!


// MENU IDs

enum{presentMenuID=4,
	memoryMenuID,	
// not supported	
	bQsMenuID};		

// WIND ID's

enum{xDebugDlgID=500};	

// DLOG ID's

enum{xTietzeDlgID=1030,		
	xMemDlgID,
	xMemTriggerDlgID,
	xBQsDlgID,
	xTrivDlgID};	

// Application Command ID's

enum{ presentNewCmd=4000,
	presentOpenCmd,
// not supported
	bQsOpenCmd=5000};

// button titles


enum{GLfindBasisID=1,		// button title IDs
	GLstopID,
	GLdoneID,
	GLgoID,
	GLmakeCoverID,
	GLsearch}; 
	
// static strings


enum{GLsettingUp=1,
	GLfiniteGroup,
	GLcloning,
	GLgrowing,
	GLfolding,
	GLswappedOut,
	GLoriginalBasis,
	GLtrivialWords};
	
// dialog validation constants

//	STRdlgValidation=133, defined in TCL file Constants.h
//  is the STR# ID for the dialog validation strings
enum{valFieldIsRequired=1,			
	valFieldTooLong,
	valIntFailed,						
	valIntRangeFailed,
	GLbadChar,			// new string IDs for dialog validation
	GLbadExp,
	GLbadNeg,
	GLbadTilde,
	GLlineTooLong,
	GLunknownLetter};

enum{restartingID=1};		// indices into #STR dialog strs	



// The presentation menu commands, dialog commands, 
// dialog item numbers, and constants
								
enum{	cmdAddString=2000,	// Presentation dialog commands
		cmdChangeString,
		cmdDeleteString,
		GLbasisCmd,			
		GLbasisClearCmd
};
		
		
enum{ 	GLpresEdit=1,
		GLpresAdd,
		GLpresChange,	
		GLpresDelete,
		GLpresScroll,
		GLbasisRunPane,		// Presentation dialog item numbers
		GLbasisBtn,			// than ID's it encloses.  Live and learn.
		GLlettersNo=9,
		GLrelationsNo=11,	// CAREFUL! ID for enclosing pane must be smaller
		TTtrivial=12,
		TTfinite,
		TTcyclic,
		TTfree,
		TTfreeFactor};
		
#define TietzeLettersPerLine	60	// Max number of characters in a 
									// CTietzeDoc string array line


// The Nielsen dialog commands, 
// dialog item numbers, and constants

	
enum{GLbQsCmd=5100,
	GLbQsOrderCmd,
	GLbQsAbelianCmd,
	GLbQsRankCmd,
	GLbQsTrivialCmd};	// basic Q's dialog commands

enum{GLbQsEdit=1,
	GLbQsRunPane,		// Basic Q's dialog item numbers
	GLbQsVertices,
	GLbQsVerticesNo,
	GLbQsEdges,
	GLbQsEdgesNo,		
	GLbQsBtn,
	GLbQsScrollGraph,
	GLbQsScrollWords,
	GLbQsFinite,   
	GLbQsOrderBox,
	GLbQsOrderNo,
	GLbQsAbelianBox,
	GLbQsRankBox,
	GLbQsRankNo,
	GLbQsTrivial};
		
#define bQsLettersPerLine	60	// Max number of characters in a 
								// CbQsDoc string array line
#define TietzeScrollBasisWidth 200 

// trivial doc stuff

enum{ trivWordScPn=1,
	newWordScPn,
	trivFromText=9,
	trivToText,
	trivRunPane,
	trivGoBtn=13};
	
enum{ trivAddCmd=7000,
	trivRemoveCmd,
	trivShowPresCmd,
	trivGoCmd,
	trivShowOrignalCmd};	

#define trivLettersPerLine	60	// Max number of characters in a 
								// CtrivDoc string array line

#define trivDisplayWidth 198

// The Memory menu commands, dialog command, and
// dialog item numbers

enum{memoryShowCmd=6000,	// memory menu command IDs
	memoryHideCmd};
		
		
enum{	changeWarningCmd=2500};	// Memory dialog command

enum{ 	GLmemTotal=2,		// Memory dialog item numbers
		GLmemTrigger=5,
		GLmemTriggerBtn=7,
		GLmemContig=9};
	
		
// the memory trigger dialog item numbers
		
enum{	GLmemTriggerEditText=2};	// Memory Trigger dialog item numbers


// interface related constants (a grab bag)
#define oneK 1024

#endif

