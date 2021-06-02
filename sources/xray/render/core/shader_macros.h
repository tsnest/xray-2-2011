////////////////////////////////////////////////////////////////////////////
//	Created		: 22.09.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_SHADER_MACROS_H_INCLUDED
#define XRAY_RENDER_CORE_SHADER_MACROS_H_INCLUDED

#include <xray/render/core/quasi_singleton.h>
#include <xray/render/core/shader_configuration.h>
#include <xray/render/core/shader_declarated_macros.h>

namespace xray {
namespace render {

// with %s_DEF macros  * 2
enum {
	shader_macros_max_cout = 128,
};

struct shader_macro
{
	fs_new::virtual_path_string		name;
	fixed_string<256>				definition;
}; // shader_macros

typedef fixed_vector<shader_macro, shader_macros_max_cout> shader_defines_list;
typedef fixed_vector<const char*,shader_macros_max_cout>	   shader_defines_names_list;

class shader_macros : public quasi_singleton<shader_macros>
{
public:
	shader_macros	();
	
	void		fill_shader_macro_list		( shader_defines_list&  macros, shader_configuration shader_config);
	void		merge_with_declared_macroses( shader_declarated_macroses_list const& declared_macroses, shader_defines_list&  macros);
	
private:
	void		register_available_macros	( );
	void		fill_global_macros			( shader_defines_list&  macros);
	void		fill_shader_configuration_macros ( shader_defines_list&  macros, shader_configuration shader_config);
	
	shader_defines_names_list							m_available_macros;
	shader_defines_list									m_working_macro_list;
	
}; // class shader_macros

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_CORE_SHADER_MACROS_H_INCLUDED