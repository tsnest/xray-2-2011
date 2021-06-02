////////////////////////////////////////////////////////////////////////////
//	Created 	: 28.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef OPCODE_INCLUDE_H_INCLUDED
#define OPCODE_INCLUDE_H_INCLUDED

#undef  BOOL

#ifdef XRAY_STATIC_LIBRARIES
#	define ICE_NO_DLL
#endif // #ifdef XRAY_STATIC_LIBRARIES

#pragma warning( push )
#pragma warning( disable : 4995 ) // warning C4995: 'strcmp': name was marked as #pragma deprecated
#pragma warning( disable : 4005 ) // warning C4005: 'ARRAYSIZE' : macro redefinition
#pragma warning( disable : 4312 ) // warning C4312: 'type cast' : conversion from 'const udword' to 'const Opcode::AABBTreeNode *' of greater size
#include <opcode/opcode.h>
#pragma warning( pop )

#endif // #ifndef OPCODE_INCLUDE_H_INCLUDED