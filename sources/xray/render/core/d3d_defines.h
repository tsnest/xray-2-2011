////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2009
//	Author		: Igor Lobanchikov
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef D3D_DEINES_H_INCLUDED
#define D3D_DEINES_H_INCLUDED

namespace xray {
namespace render {

u32 const enum_slot_ind_null	= 0xFFFFFFFF;

} // namespace render
} // namespace xray

#if USE_DX10
#	include <xray/render/core/DX10_defines.h>
#	define DX10_ONLY( expression )	expression
#else
#	include <xray/render/core/DX11_defines.h>
#endif	//	USE_DX10

#endif // #ifndef D3D_DEINES_H_INCLUDED