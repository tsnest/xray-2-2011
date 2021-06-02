////////////////////////////////////////////////////////////////////////////
//	Created		: 07.05.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/debug/library_linkage.h>
#include <xray/logging/library_linkage.h>
#include <xray/core/library_linkage.h>
#include <xray/fs/library_linkage.h>
#include <xray/vfs/library_linkage.h>
#include <xray/maya_animation/library_linkage.h>
#include <xray/physics/library_linkage.h>
#include <xray/animation/library_linkage.h>
#include <xray/render/facade/library_linkage.h>
#include <xray/render/engine/library_linkage.h>
#include <xray/render/core/library_linkage.h>
#include <xray/particle/library_linkage.h>
#include <xray/collision/library_linkage.h>

#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "OpenMayaUI.lib" )
#pragma comment( lib, "Foundation.lib" )
#pragma comment( lib, "OpenMaya.lib" )
#pragma comment( lib, "OpenMayaAnim.lib" )
#pragma comment( lib, "OpenMayaRender.lib" )

#if defined WIN64
	#pragma comment( lib, "TootleSoftwareOnlyStatic_2k8_MTDLL64.lib" )
#else
	#pragma comment( lib, "TootleSoftwareOnlyStatic_2k8_MTDLL.lib" )
#endif
