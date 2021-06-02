////////////////////////////////////////////////////////////////////////////
//	Created		: 03.02.2009
//	Author		: Igor Lobanchikov
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef NAMESPACE_WRAPPER_H_INCLUDED
#define NAMESPACE_WRAPPER_H_INCLUDED

//	Need this since we need to resolve namespace collisions
//	between different renderers for win 32/64 platform
//	for static library linkage configuration

#if 0
	#if XRAY_PLATFORM_WINDOWS
	#	ifdef	USE_DX10
	#		define	RENDER_NAMESPACE		nmspdx10
	#		define	RENDER_NAMESPACE_BEGIN	namespace nmspdx10{
	#		define	RENDER_NAMESPACE_END	}	//	namespace dx10
	#	else	//	#ifdef	USE_DX10
	#		define	RENDER_NAMESPACE		nmspdx9
	#		define	RENDER_NAMESPACE_BEGIN	namespace nmspdx9{
	#		define	RENDER_NAMESPACE_END	}	//	namespace dx9
	#	endif	//	#ifdef	USE_DX10
	#else	//	#ifdef	XRAY_PLATFORM_WINDOWS_32
	#	error define your platform here
	#endif	//	#ifdef	XRAY_PLATFORM_WINDOWS_32
#else // #if 0
#	define	RENDER_NAMESPACE		
#	define	RENDER_NAMESPACE_BEGIN	
#	define	RENDER_NAMESPACE_END	
#endif // #if 0

#endif // #ifndef NAMESPACE_WRAPPER_H_INCLUDED