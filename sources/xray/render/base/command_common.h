////////////////////////////////////////////////////////////////////////////
//	Created		: 16.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_BASE_COMMON_COMMON_H_INCLUDED
#define XRAY_RENDER_BASE_COMMON_COMMON_H_INCLUDED

namespace xray {
namespace render {

typedef fastdelegate::FastDelegate< void ( u32 )>	command_finished_callback;

} // namespace render 
} // namespace xray 

#endif // #ifndef XRAY_RENDER_BASE_COMMON_COMMON_H_INCLUDED