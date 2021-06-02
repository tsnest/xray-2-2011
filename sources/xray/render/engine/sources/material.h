////////////////////////////////////////////////////////////////////////////
//	Created		: 16.09.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_MATERIAL_H_INCLUDED
#define XRAY_RENDER_MATERIAL_H_INCLUDED

#include <boost/noncopyable.hpp>

namespace xray {
namespace render {

struct material_effects;
struct effect_options_descriptor;
class custom_config_value;
enum enum_vertex_input_type;

class material:	public resources::unmanaged_resource, public boost::noncopyable
{
public:
								material						( configs::binary_config_ptr in_config ): m_config(in_config) {}
	
	static void					initialize_nomaterial_material	( );
	static bool					is_nomaterial_material_ready	( );
	static material_effects&	nomaterial_material				( enum_vertex_input_type vi );
	static void					finalize_nomaterial_material	( );
	pcstr						get_material_name				( ) const { return m_material_name.c_str(); }
	configs::binary_config_value const& get_config				( ) { return m_config->get_root(); }
private:
	friend struct material_cook;
	friend class material_manager;
	friend class material_effects_instance_cook;
	
	fixed_string<128>		   m_material_name;
	configs::binary_config_ptr m_config;
}; // class material

typedef	resources::resource_ptr<
	material,
	resources::unmanaged_intrusive_base
> material_ptr;

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_MATERIAL_H_INCLUDED
