///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *	OPCODE - Optimized Collision Detection
 *	Copyright (C) 2001 Pierre Terdiman
 *	Homepage: http://www.codercorner.com/Opcode.htm
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_STDAFX_H__EFB95044_1D31_11D5_8B0F_0050BAC83302__INCLUDED_)
#define AFX_STDAFX_H__EFB95044_1D31_11D5_8B0F_0050BAC83302__INCLUDED_

#define XRAY_ENGINE_BUILDING

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include "api.h"

#define NEW( type )				XRAY_NEW_IMPL(			m_allocator, type )
#define NEW_A( type, count )	XRAY_NEW_ARRAY_IMPL(	m_allocator, type, count )
#define DELETE( pointer )		XRAY_DELETE_IMPL(		m_allocator, pointer )
#define DELETE_A( pointer )		XRAY_DELETE_ARRAY_IMPL(	m_allocator, pointer )

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__EFB95044_1D31_11D5_8B0F_0050BAC83302__INCLUDED_)
