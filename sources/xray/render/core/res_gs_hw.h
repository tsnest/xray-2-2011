////////////////////////////////////////////////////////////////////////////
//	Created		: 06.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RES_GS_HW_H_INCLUDED
#define RES_GS_HW_H_INCLUDED

namespace xray {
namespace render {

struct gs_data;

template <typename shader_data>
class res_xs_hw;
typedef res_xs_hw<gs_data>	res_gs_hw;

class resource_intrusive_base;
typedef intrusive_ptr< res_gs_hw, resource_intrusive_base, threading::single_threading_policy>	ref_gs_hw;

} // namespace render
} // namespace xray

#endif // #ifndef RES_GS_H_INCLUDED