////////////////////////////////////////////////////////////////////////////
//	Created		: 06.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RES_VS_HW_H_INCLUDED
#define RES_VS_HW_H_INCLUDED

namespace xray {
namespace render {

struct vs_data;

template <typename shader_data>
class res_xs_hw;

typedef res_xs_hw<vs_data>	res_vs_hw;

class resource_intrusive_base;
typedef intrusive_ptr< res_vs_hw, resource_intrusive_base, threading::single_threading_policy >				ref_vs_hw;
typedef intrusive_ptr< res_vs_hw const, resource_intrusive_base const, threading::single_threading_policy >	ref_vs_hw_const;

} // namespace render
} // namespace xray

#endif // #ifndef RES_VS_HW_H_INCLUDED