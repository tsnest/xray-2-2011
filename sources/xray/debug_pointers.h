////////////////////////////////////////////////////////////////////////////
//	Created 	: 16.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_DEBUG_POINTERS_H_INCLUDED
#define XRAY_DEBUG_POINTERS_H_INCLUDED

#ifdef DEBUG
#	define XRAY_USE_DEBUG_POINTERS	1
#else // #ifdef DEBUG
#	define XRAY_USE_DEBUG_POINTERS	0
#endif // #ifdef DEBUG

#include <xray/raw_ptr.h>
#include <xray/non_null_ptr.h>
#include <xray/loose_ptr.h>

#endif // #ifndef XRAY_DEBUG_POINTERS_H_INCLUDED