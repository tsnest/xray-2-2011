////////////////////////////////////////////////////////////////////////////
//	Created		: 06.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RES_XS_HW_H_INCLUDED
#define RES_XS_HW_H_INCLUDED

#include <xray/render/core/resource_intrusive_base.h>
#include <xray/render/core/sampler_slot.h>
#include <xray/render/core/xs_data.h>

namespace xray {
namespace render {

template <typename shader_data>
class res_xs_hw : public resource_intrusive_base {
	friend class resource_manager;
	friend class resource_intrusive_base;

	res_xs_hw		();
	~res_xs_hw		();
	void destroy_impl() const;


public:

	HRESULT create_hw_shader( ID3DBlob* shader_code);

	typename shader_data::hw_interface* hardware_shader() { return m_shader_data.hardware_shader;}

	shader_data const & data() const { return m_shader_data; }

// 	shader_constant_table const &	constants	() const	{ return m_constants;}
// 	sampler_slots const &		samplers	() const	{ return m_samplers; }
// 	texture_slots const &		textures	() const	{ return m_textures; }
	inline bool	is_registered	( ) const		{ return m_is_registered; }
	inline void	mark_registered	( )				{ R_ASSERT( !m_is_registered); m_is_registered = true; }
	inline shared_string const& name( ) const	{ return m_name; }
	inline void	set_name		( shared_string const& name ) { R_ASSERT( !m_name.c_str() ); m_name = name; }

private:
	HRESULT create_hw_shader( ID3DBlob* shader_code, typename shader_data::hw_interface **hardware_shader);
	void	parse_resources	( ID3DShaderReflection* shader_reflection, 
								sampler_slots	&samplers, 
								texture_slots	&textures);
 
private:
	shader_data				m_shader_data;
	shared_string			m_name;
	bool					m_is_registered;
}; // class res_xs_hw

} // namespace render
} // namespace xray

#endif // #ifndef RES_XS_HW_H_INCLUDED