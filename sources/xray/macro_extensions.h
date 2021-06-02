////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.08.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MACRO_EXTENSIONS_H_INCLUDED
#define XRAY_MACRO_EXTENSIONS_H_INCLUDED

#if defined( _CPPUNWIND ) && !defined( _MANAGED )
#	error please disable exceptions
#endif // #if defined( _CPPUNWIND ) && !defined( _MANAGED )

#ifndef NDEBUG
#	define DEBUG
#endif // #ifdef NDEBUG

#include <xray/macro_compile_assert.h>
#include <xray/macro_strings.h>
#include <xray/macro_platform.h>
#include <xray/macro_compiler.h>
#include <xray/macro_building.h>
#include <xray/macro_unreferenced_parameter.h>
#include <xray/macro_todo.h>

#define XRAY_ENGINE_COMPANY_ID	"GSC Game World"
#define XRAY_ENGINE_VERSION		2
#define XRAY_ENGINE_SUBVERSION	0
#define XRAY_ENGINE_ID			"X-Ray Engine v" XRAY_MAKE_STRING(XRAY_ENGINE_VERSION) "." XRAY_MAKE_STRING(XRAY_ENGINE_SUBVERSION)

#define IMPLICIT

#endif // #ifndef XRAY_MACRO_EXTENSIONS_H_INCLUDED