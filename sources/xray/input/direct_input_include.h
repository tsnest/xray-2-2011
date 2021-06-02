////////////////////////////////////////////////////////////////////////////
//	Module 		: direct_input_include.h
//	Created 	: 21.11.2008
//  Modified 	: 21.11.2008
//	Author		: Dmitriy Iassenev
//	Description : direct input include file
////////////////////////////////////////////////////////////////////////////

#ifndef DIRECT_INPUT_INCLUDE_H_INCLUDED
#define DIRECT_INPUT_INCLUDE_H_INCLUDED

#include <xray/core/os_preinclude.h>
#undef NOUSER
#undef NOMSG
#include <xray/core/os_include.h>

#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>

#endif // #ifndef DIRECT_INPUT_INCLUDE_H_INCLUDED