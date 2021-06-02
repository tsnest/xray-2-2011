////////////////////////////////////////////////////////////////////////////
//	Created 	: 17.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef DEBUG_FLOAT_H_INCLUDED
#define DEBUG_FLOAT_H_INCLUDED

// these defines are in the <float.h> of MSVC STL, but we use STLPort instead

#define _FPE_INVALID            	0x81
#define _FPE_DENORMAL           	0x82
#define _FPE_ZERODIVIDE         	0x83
#define _FPE_OVERFLOW           	0x84
#define _FPE_UNDERFLOW          	0x85
#define _FPE_INEXACT            	0x86
#define _FPE_UNEMULATED         	0x87
#define _FPE_SQRTNEG            	0x88
#define _FPE_STACKOVERFLOW      	0x8a
#define _FPE_STACKUNDERFLOW     	0x8b
#define _FPE_EXPLICITGEN        	0x8c

#endif // #ifndef DEBUG_FLOAT_H_INCLUDED