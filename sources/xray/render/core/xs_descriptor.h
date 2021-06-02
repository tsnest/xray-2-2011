////////////////////////////////////////////////////////////////////////////
//	Created		: 13.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RES_XS_BINDER_H_INCLUDED
#define RES_XS_BINDER_H_INCLUDED

#include "res_xs_hw.h"
#include "xs_data.h"

namespace xray {
namespace render {

template < typename shader_data>
class xs_descriptor 
{
	typedef xray::intrusive_ptr<res_xs_hw<shader_data>, resource_intrusive_base, threading::single_threading_policy>	ref_xs_hw;

public:

	xs_descriptor( );

	void reset( res_xs_hw<shader_data> * xs_hw );

 	bool set_sampler	( char const * name, res_sampler_state  * state);
 	bool set_texture	( char const * name, res_texture * texture);

	// checks weather the shader uses the given texture.
	bool use_texture	( char const * name);

	// checks weather the shader uses the given sampler.
	bool use_sampler	( char const * name);

	shader_data &		data() { return m_shader_data;}
	shader_data const&	data() const		{ return m_shader_data;}
	ref_xs_hw	 const	hardware_shader() const	{ return m_hardware_shader;}

private:
	ref_xs_hw				m_hardware_shader;
	shader_data				m_shader_data;

}; // class res_xs_descriptor

typedef  xs_descriptor<vs_data>		vs_descriptor;
typedef  xs_descriptor<gs_data>		gs_descriptor;
typedef  xs_descriptor<ps_data>		ps_descriptor;


} // namespace render
} // namespace xray

#endif // #ifndef RES_XS_BINDER_H_INCLUDED