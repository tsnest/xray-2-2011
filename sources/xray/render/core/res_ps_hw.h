////////////////////////////////////////////////////////////////////////////
//	Created		: 06.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RES_PS_HW_H_INCLUDED
#define RES_PS_HW_H_INCLUDED

namespace xray {
namespace render {
	
struct ps_data;

template <typename shader_data>
class res_xs_hw;

typedef res_xs_hw<ps_data>	res_ps_hw;

class resource_intrusive_base;
typedef intrusive_ptr<res_ps_hw, resource_intrusive_base, threading::single_threading_policy>	ref_ps_hw;

} // namespace render
} // namespace xray

#endif // #ifndef RES_PS_H_INCLUDED