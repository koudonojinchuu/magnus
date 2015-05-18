#ifndef Frank_nodeMsg
#define Frank_nodeMsg

// Welcome to the compile-time bottleneck!

// Nodes generate messages from their supervising
// node and respond to messages from their subnodes.
// Nodes also respond to their own messages in order
// to determine the proper subnode or method to reenter
// after a hiatus.

enum{
	dummyMsg=100,
	DisplayCountersMsg,
	
// Signals
	calibrateTT1msg,
	calibrateTT1pmsg,
	calibrateTT2msg,
	calibrateTT2pmsg,
	calibrateLibmsg,
	pVertexInitMsg,
	displayPresMsg,
	

// messages for the Tietze algorithm
// these travel up the chain of command from
// wherever they are detected
	trivialGroupMsg,
	cyclicGroupMsg,
	freeFactorMsg,
	finiteGroupMsg,
	freeGroupMsg,

// FTTpresNode support
	presAuRevoirMsg,
	presGoodbyeKissMsg,

// FrenumberPresNode support
	renumberPresAuRevoirMsg,
	renumberPresGoodbyeKissMsg,
		
// FstrToWordNode support
	removeBlanksMsg,
	emptyWordStrMsg,
	appendWordMsg,
	appendLastWordMsg,	
	strToWordAuRevoirMsg,
	strToWordGoodbyeKissMsg,
	
// FTTdriverNode support
	forgetPresMsg,
	driverAuRevoirMsg,
	driverGoodbyeKissMsg,	

// messages for the driver algorithm

// travels up chain of command
	equivalentPresentationFoundMsg,
	
// CpLib support
	equivalentFreePresFoundMsg,
	LibAuRevoirMsg,
	LibGoodbyeKissMsg,
	
// CpresCompare support
	CompareAuRevoirMsg,
	CompareGoodbyeKissMsg,	
	rTreeAuRevoirMsg,
	rTreeGoodbyeKissMsg,
	gTreeAuRevoirMsg,
	gTreeGoodbyeKissMsg,
	
// CgTreeNode support
	validPermutationMessage,
		
// CTT1node support
	TT1AuRevoirMsg,
	TT1GoodbyeKissMsg,
	
// CTT1pnode support
	TT1pAuRevoirMsg,
	TT1pGoodbyeKissMsg,
	
// CTT2node support
	incrementNewGenMsg,
	TT2AuRevoirMsg,
	TT2GoodbyeKissMsg,
	
// CTT2pnode support
	TT2pAuRevoirMsg,
	TT2pGoodbyeKissMsg,
	
// messages for the TT1 algorithm

// FuNode support
	uInitializePacketMsg,
	uBeforeEdgeMsg,
	uAfterEdgeMsg,
	uTerminatePacketMsg,
	uAuRevoirMsg,
	uGoodbyeKissMsg,
	
// CCayley support
	identifiedWordMsg,
	
// CwordsToGLvarNode support 
	terminateWordMsg,	
	wordsToGLvarAuRevoirMsg,			
	wordsToGLvarGoodbyeKissMsg,
		
// messages for the TT1p algorithm

	TT1pSubGoodbyeKissMsg
};		
	
#define AUTO_SET -1

// The following must not be
// altered

enum{
	
// Signals
	GLvarIsCleanMsg,
	GLvarIsDirtyMsg,
	
// FcleanUpNode support
	cleanUpMsg,
	
// FGLwalkNode support
	walkMsg,
	vertexMsg,
	maximalEdgeMsg,
	basisEdgeMsg,
	
// FfoldNode support
	foldAuRevoirMsg,
	foldGoodbyeKissMsg,
	
// FgrowNode support
	foldEntryLocationMsg,
	growAuRevoirMsg,
	growGoodbyeKissMsg,

// messages for cloning
	cloneAuRevoirMsg,
	cloneGoodbyeKissMsg

};		

#endif						
