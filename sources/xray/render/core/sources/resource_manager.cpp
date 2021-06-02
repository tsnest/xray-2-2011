////////////////////////////////////////////////////////////////////////////
//	Created		: 01.05.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/resource_manager.h>
#include "shader_binary_source_cook.h"
#include "texture_storage.h"
#include <xray/render/core/shader_constant_table.h>
#include <xray/render/core/backend.h>
#include "texture_cook.h"
#include "dds.h"
#include <xray/render/core/utils.h>
#include <xray/render/core/state_descriptor.h>
#include <xray/render/core/res_state.h>
#include <xray/render/core/res_declaration.h>
#include <xray/render/core/res_signature.h>
#include <xray/render/core/render_target.h>
#include <xray/render/core/sampler_state_descriptor.h>
#include <xray/render/core/res_sampler_list.h>
#include <xray/render/core/res_xs.h>
#include <xray/render/core/res_geometry.h>
#include <xray/render/core/xs_data.h>
#include "include_handler.h"
#include <xray/render/core/res_xs_hw.h>
#include "com_utils.h"
#include <xray/render/core/effect_manager.h>
#include <xray/render/core/custom_config.h>
#include <xray/render/core/res_effect.h>
#include <xray/render/core/effect_compiler.h>
#include <xray/render/core/effect_descriptor.h>
#include "manager_common_inline.h"

#include <xray/render/core/gpu_timer.h>

#include <xray/resources_fs.h>

#include <xray/tasks_system.h>

#include "shader_binary_source_cook.h"

#pragma warning( push )
#pragma warning( disable : 4995 )
#	if !USE_DX10 
#		include <d3dx11tex.h>
#		include <d3dcompiler.h>
#	else // #if !USE_DX10 
#		include <d3dx10tex.h>
#	endif // #if !USE_DX10 
#pragma warning( pop )

namespace xray {
namespace render {

bool g_enable_resource_sharing = true;

////////////////////////////////////////////////////////////////////////

struct resource_manager_call_destructor_predicate {
	
	template <typename T>
	inline void	operator()	( T* resource ) const
	{
		resource_manager::call_resource_destructor( resource );
	}
}; // struct resource_manager_call_destructor_predicate

struct shader_hw_remove_predicate{

	shader_hw_remove_predicate( char const * name ): m_name (name) {}

	template <typename T>
	inline bool	operator()	( T const & element ) const
	{
		return strcmp( element->second, m_name) == 0;
	}

private:
	char const *	m_name;
};

static bool is_equal_formats(DXGI_FORMAT left, DXGI_FORMAT right)
{
	if (left == right)
		return true;
	
	if (left == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB  && right == DXGI_FORMAT_R8G8B8A8_UNORM ||
		left == DXGI_FORMAT_BC1_UNORM_SRGB		 && right == DXGI_FORMAT_BC1_UNORM ||
		left == DXGI_FORMAT_BC2_UNORM_SRGB		 && right == DXGI_FORMAT_BC2_UNORM ||
		left == DXGI_FORMAT_BC3_UNORM_SRGB		 && right == DXGI_FORMAT_BC3_UNORM ||
		left == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB  && right == DXGI_FORMAT_B8G8R8A8_UNORM ||
		left == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB  && right == DXGI_FORMAT_B8G8R8X8_UNORM ||
		left == DXGI_FORMAT_BC7_UNORM_SRGB		 && right == DXGI_FORMAT_BC7_UNORM ||
		right == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB && left == DXGI_FORMAT_R8G8B8A8_UNORM ||
		right == DXGI_FORMAT_BC1_UNORM_SRGB		 && left == DXGI_FORMAT_BC1_UNORM ||
		right == DXGI_FORMAT_BC2_UNORM_SRGB		 && left == DXGI_FORMAT_BC2_UNORM ||
		right == DXGI_FORMAT_BC3_UNORM_SRGB		 && left == DXGI_FORMAT_BC3_UNORM ||
		right == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB && left == DXGI_FORMAT_B8G8R8A8_UNORM ||
		right == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB && left == DXGI_FORMAT_B8G8R8X8_UNORM ||
		right == DXGI_FORMAT_BC7_UNORM_SRGB		 && left == DXGI_FORMAT_BC7_UNORM)
	return true;
	
	return false;
}

static bool read_srgb_flag(u8 const* dds_ptr, u32 dds_size)
{
	// Add struct additional_parameters {...};
	bool is_srgb_option			= false;
	xray::memory::copy			(
		&is_srgb_option, 
		sizeof(is_srgb_option), 
		dds_ptr + dds_size - sizeof(is_srgb_option), 
		sizeof(is_srgb_option)
	);
	return is_srgb_option;
}

static DXGI_FORMAT get_typeless_format(DXGI_FORMAT format)
{
	switch (format)
	{
		case DXGI_FORMAT_BC1_UNORM: return DXGI_FORMAT_BC1_TYPELESS;
		case DXGI_FORMAT_BC2_UNORM: return DXGI_FORMAT_BC2_TYPELESS;
		case DXGI_FORMAT_BC3_UNORM: return DXGI_FORMAT_BC3_TYPELESS;
		case DXGI_FORMAT_BC7_UNORM: return DXGI_FORMAT_BC7_TYPELESS;
		//case DXGI_FORMAT_R8G8B8A8_UNORM: return DXGI_FORMAT_R8G8B8A8_TYPELESS;
		default:					return format;
	};
}

// static DXGI_FORMAT get_srgb_format(DXGI_FORMAT format)
// {
// 	switch (format)
// 	{
// 		case DXGI_FORMAT_BC1_UNORM:
// 		case DXGI_FORMAT_BC1_TYPELESS: 
// 			return DXGI_FORMAT_BC1_UNORM_SRGB;
// 		case DXGI_FORMAT_BC2_UNORM: 
// 		case DXGI_FORMAT_BC2_TYPELESS: 
// 			return DXGI_FORMAT_BC2_UNORM_SRGB;
// 		case DXGI_FORMAT_BC3_UNORM: 
// 		case DXGI_FORMAT_BC3_TYPELESS:
// 			return DXGI_FORMAT_BC3_UNORM_SRGB;
// 		case DXGI_FORMAT_BC7_UNORM: 
// 		case DXGI_FORMAT_BC7_TYPELESS:
// 			return DXGI_FORMAT_BC7_UNORM_SRGB;
// 		default:
// 			return format;
// 	};
// }


static void begin_command_list(D3D_QUERY_DESC& query_desc, ID3DQuery*& out_empty_query_ptr)
{
	xray::render::device::ref().d3d_device()->CreateQuery(&query_desc, &out_empty_query_ptr);
}

static void end_command_list(ID3DQuery*& out_empty_query_ptr)
{
	xray::render::device::ref().d3d_context()->End(out_empty_query_ptr);
	while( S_OK != xray::render::device::ref().d3d_context()->GetData(out_empty_query_ptr, 0, 0, 0) );
	out_empty_query_ptr->Release();
}

static ID3DBaseTexture* make_copy_with_srgb_format(ID3DBaseTexture* in_texture)
{
	// Make copy.
	// http://www.gamedev.net/topic/605930-id3dx10font-and-srgb/
	// http://timothylottes.blogspot.com/2011/02/aliasing-srgb-and-non-srgb-on-all-apis.html
	
	if (!in_texture)
		return 0;
	
	D3D_RESOURCE_DIMENSION		type;
	in_texture->GetType			(&type);
	
	if (type == D3D_RESOURCE_DIMENSION_TEXTURE2D)
	{
		ID3DQuery*				out_empty_query_ptr;
		D3D_QUERY_DESC			query_desc;
		query_desc.MiscFlags	= 0;
		query_desc.Query		= D3D11_QUERY_EVENT;
		begin_command_list		(query_desc, out_empty_query_ptr);
		
		D3D_TEXTURE2D_DESC		texDesc;
		ID3DTexture2D*	T		= static_cast<ID3DTexture2D*>(in_texture);
		T->GetDesc				(&texDesc);
		
		texDesc.Format			= get_typeless_format(texDesc.Format);
		ID3DTexture2D *copy		= 0;
		HRESULT result			= device::ref().d3d_device()->CreateTexture2D(&texDesc, 0, &copy);
		CHECK_RESULT			(result);
		
		device::ref().d3d_context()->CopyResource(copy, in_texture);
		
		end_command_list		(out_empty_query_ptr);
		
		return					copy;
	}
	else
	{
		R_ASSERT				(0, "sRGB for 3d and 1d types not implemented yet.");
		return					0;
	}
}



// HRESULT	shader_compile( 
// 	LPCSTR						name,
// 	LPCSTR						source,
// 	UINT						source_len,
// 	const shader_defines_list&	defines,
// 	include_handler*			includes,
// 	resource_manager::map_shader_sources& sources,
// 	LPCSTR						entry_point,
// 	LPCSTR						target,
// 	DWORD						flags,
// 	ID3DBlob**					out_shader,
// 	ID3DBlob**					out_error_msgs)
// {
// 	XRAY_UNREFERENCED_PARAMETERS	(&name, &sources);
// 	
// 	fixed_vector<D3D_SHADER_MACRO, shader_macros_max_cout> d3d_shader_macroses;
// 	for (shader_defines_list::const_iterator it = defines.begin(); it != defines.end(); ++it)
// 	{
// 		D3D_SHADER_MACRO	macro;
// 		macro.Name			= it->name.c_str();
// 		macro.Definition	= it->definition.c_str();
// 		d3d_shader_macroses.push_back(macro);
// 	}
// 	
// 	HRESULT	result = D3DXCompileFromMemory(
// 			source, 
// 			source_len, 				
// 			"",
// 			d3d_shader_macroses.empty() ? NULL : &*d3d_shader_macroses.begin(), 
// 			(ID3DInclude*)includes, 
// 			entry_point,
// 			target, 
// 			flags, 
// 			0,
// 			0,
// 			out_shader,
// 			out_error_msgs,
// 			0
// 			);
// 
// 	return result;
// }

bool resource_manager::add_shader_code(char const* shader_name, char const* source_code, bool rewrite_exists)
{
	resource_manager::shader_source source;
	source.time = 0;
	
	u32 code_length = static_cast_checked<u32>(strlen(source_code));
	source.data.resize(code_length);
	xray::memory::copy(&source.data[0], code_length, source_code, code_length);
	
	if ( rewrite_exists || m_sources.find(shader_name)==m_sources.end() )
	{
		fs_new::virtual_path_string new_path;
		new_path.appendf("%s/%s", get_shader_path(), shader_name);
		m_sources[new_path.c_str()] = source;
		return true;
	}
	return false;
}

template<typename shader_data> 
void resource_manager::recompile_xs_shaders(render::vector<fs_new::virtual_path_string> const& changed_files)
{
	render::vector<res_xs_hw<shader_data>*>			xs_shaders_to_reload;
	render::vector<fs_new::virtual_path_string>		shader_names;
	
#if 0
	render::map<shader_name_config_pair, res_xs_hw<shader_data>* > & xs_container = get_xs_container<shader_data>();	
	
	for (map_shader_sources::const_iterator src_it=m_sources.begin(); src_it!=m_sources.end(); ++src_it)
	{
		fs_new::virtual_path_string file_name = src_it->first.c_str();
		
		pcstr shader_type_name = shader_type_traits<(enum_shader_type)shader_data::type>::short_name();
		
		fs_new::virtual_path_string ext;
		ext = file_name.substr(file_name.rfind('.') + 1);
		
		if (ext!=shader_type_name)
			continue;
		
		
		/*
		if (std::find(changed_files.begin(), changed_files.end(), src_it->first) == changed_files.end())
		{
			bool found_in_includes = false;
			for (render::vector<pcstr>::const_iterator inc_it=src_it->second.includes.begin(); inc_it!=src_it->second.includes.end(); ++inc_it)
			{
				if (std::find(changed_files.begin(), changed_files.end(), *inc_it) != changed_files.end())
				{
					found_in_includes = true;
					break;
				}
			}
			if (!found_in_includes)
				continue;
		}
		*/
		file_name = file_name.substr(file_name.rfind('/')+1);
		file_name.set_length(file_name.rfind('.'));
		
		shader_names.push_back(file_name);
/*
		shader_configuration null_configuration;
		null_configuration.configuration = 0x00;
		shader_configuration max_configuration;
		max_configuration.configuration = 0xFFFFFFFF;
		
		shader_name_config_pair range_start	( file_name.c_str(), null_configuration);
		shader_name_config_pair range_end	( file_name.c_str(), max_configuration);
		
		render::map<shader_name_config_pair, res_xs_hw<shader_data>* >::iterator remove_start	= xs_container.lower_bound( range_start);
		render::map<shader_name_config_pair, res_xs_hw<shader_data>* >::iterator remove_end		= xs_container.lower_bound( range_end);
		
		if( remove_start != xs_container.end() )
		{
			if( remove_end != xs_container.end() && remove_end->first == range_end)
				++remove_end;
			
			render::map<shader_name_config_pair, res_xs_hw<shader_data>* >::iterator it = remove_start;
			for (;it!=remove_end;++it)
			{
				xs_shaders_to_reload.push_back(it->second);
			}
			
			//xs_container.erase( remove_start, remove_end);
		}
		*/
	}
#endif // #if 0
	for (render::vector<fs_new::virtual_path_string>::const_iterator it = changed_files.begin(); it != changed_files.end(); ++it)
	{
		fs_new::virtual_path_string file_name = *it;
		file_name = file_name.substr(file_name.rfind('/')+1);
		file_name.set_length(file_name.rfind('.'));

		shader_names.push_back(file_name);
	}
	effect_manager::ref().recompile_shaders(shader_names);
	//effect_manager::ref().recompile_effects(xs_shaders_to_reload);
}

template <typename shader_data>
res_xs_hw<shader_data> *	resource_manager::create_xs_hw_impl ( const char * name, shader_configuration shader_config, shader_include_getter* include_getter, binary_shader_sources_type* shader_sources)
{
	XRAY_UNREFERENCED_PARAMETER(include_getter);
	
	if (!name)
		return NULL;
	
	typedef render::map< shader_name_config_pair, res_xs_hw<shader_data>* >  xs_registry_type;

	xs_registry_type & xs_hw_registry		= get_xs_container<shader_data>();
	
	fs_new::virtual_path_string	reg_name				(name);
	xs_registry_type::iterator it			= xs_hw_registry.find( shader_name_config_pair( reg_name.c_str(), shader_config));
	
	// TODO: Add check of global defines to shader_configuration
//	if (!m_is_shader_reloading && it != xs_hw_registry.end())
//	{
//		return it->second;
//	}
//	else
	{
		enum_shader_type shd_type			= (enum_shader_type)shader_data::type;
		binary_shader_sources_type::iterator found_binary_source_it = shader_sources->find(
			binary_shader_key_type(
				name, 
				shd_type, 
				shader_config
			)
		);
		
		if (found_binary_source_it != shader_sources->end())
		{
			res_xs_hw<shader_data>* xs_hw	= NEW(res_xs_hw<shader_data>);
			xs_hw->mark_registered			();
			xs_hw->set_name					(reg_name.c_str() );
			//xs_hw_registry.insert			(utils::mk_pair(shader_name_config_pair( xs_hw->name().c_str(), shader_config), xs_hw));
			
			xs_hw_registry[
				shader_name_config_pair(xs_hw->name().c_str(), shader_config)
			]								= xs_hw;
			
			if (FAILED(xs_hw->create_hw_shader(found_binary_source_it->second->shader_byte_code)))
			{
				LOG_ERROR					(
					"!%s: %s", 
					shader_type_traits<(enum_shader_type)shader_data::type>::short_name(),
					name
				);
				LOG_ERROR					("shader creation failed");
				FATAL						("!Shader is not created");
			}
			return xs_hw;
		}
		else
		{
// 			R_ASSERT( error_buf);
// 			pcstr const err_msg = ( LPCSTR)error_buf->GetBufferPointer();
// 			if( !m_compile_error_handler.empty())
// 				m_compile_error_handler	( enum_shader_type_vertex, name, err_msg);
			
			fs_new::virtual_path_string full_shader_name = get_full_shader_path(
				name, 
				(enum_shader_type)shader_data::type
			);
			
			map_shader_sources::iterator i	= m_previous_sources.find(full_shader_name.c_str());
			
			if (i != m_previous_sources.end())
				m_sources[full_shader_name.c_str()] = m_previous_sources[full_shader_name.c_str()];
			
			for (xray::render::vector<pcstr>::iterator	inc_it = i->second.includes.begin(); 
														inc_it != i->second.includes.end(); 
														++inc_it)
				if (m_previous_sources.find(*inc_it) != m_previous_sources.end())
					m_sources[*inc_it]		= m_previous_sources[*inc_it];
		}
	}

	return 0;

#if 0

	typedef render::map<shader_name_config_pair, res_xs_hw<shader_data>* >  xs_registry_type;

	xs_registry_type & xs_hw_registry = get_xs_container<shader_data>();

	// Here need to consider also defines 
	fs_new::virtual_path_string	reg_name( name);
	xs_registry_type::iterator it	= xs_hw_registry.find( shader_name_config_pair( reg_name.get_buffer(), shader_config));

	if( it != xs_hw_registry.end())
	{
		return it->second;
	}
	else
	{
#if 0
		for ( u32 compile_pass=0; compile_pass<2; compile_pass++)
#endif // #if 0
		{
			shader_defines_list	working_defines_list;
			
			shader_macros::ref().fill_shader_macro_list( working_defines_list, shader_config);
			
			u32 pos = reg_name.find( "( ");
			fs_new::virtual_path_string nm( pos==fs_new::virtual_path_string::npos ? name : reg_name.substr( 0, pos));
			
			fs_new::virtual_path_string				sh_name( get_shader_path());
			sh_name.appendf( "/%s.%s", nm.c_str(), shader_type_traits<(enum_shader_type)shader_data::type>::short_name());
			
			fixed_string<8>				target_profile_str; 
			target_profile_str.assignf( "%s_4_0", shader_type_traits<(enum_shader_type)shader_data::type>::short_name());
#if 0			
			LPCSTR						c_target = target_profile_str.get_buffer();//"vs_4_0";
			LPCSTR						c_entry  = "main";
#endif // #if 0			
			map_shader_sources::iterator src_it = m_sources.find( sh_name);
			ASSERT( src_it != m_sources.end(), "Shader not found. [%s]", sh_name.c_str()); //What to do if file not found?????
			ASSERT( !src_it->second.data.empty());
			
			shader_declarated_macroses_list declared_defines_list;
			if (found_shader_declarated_macroses(pcstr(&src_it->second.data[0]), declared_defines_list))
			{
				shader_macros::ref().merge_with_declared_macroses(declared_defines_list, working_defines_list);
			}
			
			include_handler				includer( /*&m_sources, */&src_it->second, include_getter);
			ID3D10Blob*					shader_code	= NULL;
			ID3D10Blob*					error_buf	= NULL;
			
			enum_shader_type shd_type	= (enum_shader_type)shader_data::type;
			binary_shader_sources_type::iterator found_binary_source_it = shader_sources->find(
				binary_shader_key_type(
					name, 
					shd_type, 
					shader_config
				)
			);
			
#if 0			
			HRESULT hr = shader_compile(
				name,
				( LPCSTR)&src_it->second.data[0],
				src_it->second.data.size(), 
				working_defines_list, 
				&includer,
				m_sources,
				c_entry,
				c_target,
				D3D_SHADER_DEBUG | D3D_SHADER_PACK_MATRIX_ROW_MAJOR //| D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY
				/*| D3DXSHADER_PREFER_FLOW_CONTROL*/,
				&shader_code, &error_buf
				);
#else // #if 0
			
			shader_code		= found_binary_source_it != shader_sources->end() ? found_binary_source_it->second->shader_byte_code : 0;
			HRESULT hr		= shader_code == 0 ? E_FAIL : S_OK;
#endif // #if 0
			
			if( SUCCEEDED(hr))
			{
				// TODO: Check if compiled code the same
				res_xs_hw<shader_data>* xs_hw	= NEW( res_xs_hw<shader_data>);
				xs_hw->mark_registered			( );
				xs_hw->set_name					( reg_name.get_buffer() );
				xs_hw_registry.insert			( utils::mk_pair( shader_name_config_pair( xs_hw->name().c_str(), shader_config), xs_hw));
				//		xs_hw->set_id				( gen_id( xs_hw_registry, m_vs_ids));
				
				hr = xs_hw->create_hw_shader( shader_code );
				
				if( FAILED( hr))
				{
					LOG_ERROR( "!%s: %s", shader_type_traits<(enum_shader_type)shader_data::type>::short_name(),name);
					LOG_ERROR( "shader creation failed");
					FATAL( "!Shader is not created");
				}
#if 0
				safe_release( shader_code);
				safe_release( error_buf);
#endif // #if 0
				return		xs_hw;
			}
			else
			{

				R_ASSERT( error_buf);
 				pcstr const err_msg = ( LPCSTR)error_buf->GetBufferPointer();
				
				if( !m_compile_error_handler.empty())
					m_compile_error_handler	( enum_shader_type_vertex, name, err_msg);
#if 0				
				LOG_ERROR( "shader creation failed (%s, %s)", name, err_msg);
				FATAL( "!%s \"%s\" is not created. \nerrors: \n%s", shader_type_traits<(enum_shader_type)shader_data::type>::name(), name, err_msg);

				safe_release( shader_code);
				safe_release( error_buf);
#endif // #if 0				
				// Try compile shader with previous source.
				map_shader_sources::iterator i = m_previous_sources.find(sh_name);

				if (i!=m_previous_sources.end())
				{
					m_sources[sh_name] = m_previous_sources[sh_name];
				}
				
				//for (xray::render::vector<pcstr>::iterator inc_it=i->second.includes.begin(); inc_it!=i->second.includes.end(); ++inc_it)
				//{
				//	if (m_previous_sources.find(*inc_it)!=m_previous_sources.end())
				//	{
				//		m_sources[*inc_it] = m_previous_sources[*inc_it];
				//	}
				//}
#if 0
				continue;
#endif // #if 0
			}
		}
	}

	return 0;

#endif // #if 0
}


res_vs_hw*	resource_manager::create_vs_hw( char const * name, shader_configuration shader_config, shader_include_getter* include_getter, binary_shader_sources_type* shader_sources)
{
	return create_xs_hw_impl<vs_data>( name, shader_config, include_getter, shader_sources);
}

res_gs_hw*	resource_manager::create_gs_hw( char const * name, shader_configuration shader_config, shader_include_getter* include_getter, binary_shader_sources_type* shader_sources)
{
	return create_xs_hw_impl<gs_data>( name, shader_config, include_getter, shader_sources);
}

res_ps_hw* resource_manager::create_ps_hw( LPCSTR name, shader_configuration shader_config, shader_include_getter* include_getter, binary_shader_sources_type* shader_sources)
{
	return create_xs_hw_impl<ps_data>( name, shader_config, include_getter, shader_sources);

// 	fs_new::virtual_path_string		reg_name( name);
// 	map_ps_hw::iterator it	= m_ps_hw_registry.find( shader_name_config_pair( reg_name.get_buffer(), shader_config));
// 
// 	if( it != m_ps_hw_registry.end())
// 	{
// 		return it->second;
// 	}
// 	else
// 	{
// 
// 		shader_macros::ref().fill_shader_macro_list( working_defines_list, shader_config);
// 
// 		res_ps_hw* ps	= NEW( res_ps_hw);
// 		ps->mark_registered();
// 		m_ps_hw_registry.insert( utils::mk_pair( shader_name_config_pair( ps->set_name( reg_name.get_buffer()), shader_config), ps));
// //		ps->set_id( gen_id( m_ps_hw_registry, m_ps_ids));
// 
// 		if( 0 == _stricmp( name, "null"))
// 		{
// 			//ps->m_ps = NULL;
// 			return ps;
// 		}
// 
// 		include_handler				includer( &m_sources);
// 		ID3D10Blob*					shader_code	= NULL;
// 		ID3D10Blob*					error_buf	= NULL;
// 		fs_new::virtual_path_string				sh_name( get_shader_path());
// 		u32 pos = reg_name.find( "( ");
// 		fs_new::virtual_path_string nm( pos==fs_new::virtual_path_string::npos ? name : reg_name.substr( 0, pos));
// 		sh_name.appendf( "/%s.ps", nm.c_str());
// 
// 		LPCSTR						c_target = "ps_4_0";
// 		LPCSTR						c_entry  = "main";
// 
// 		//shader_defines_list	defines;
// 		//make_defines( local_opts, defines);
// 
// 		map_shader_sources::iterator src_it = m_sources.find( sh_name);
// 		ASSERT( src_it != m_sources.end()); //What to do if file not found?????
// 		ASSERT( !src_it->second.empty());
// 
// 		HRESULT hr = shader_compile( name,
// 			( LPCSTR)&src_it->second.data[0],
// 			src_it->second.size(), working_defines_list, 
// 			&includer,
// 			c_entry,
// 			c_target,
// 			D3D_SHADER_DEBUG | D3D_SHADER_PACK_MATRIX_ROW_MAJOR
// 			/*| D3DXSHADER_PREFER_FLOW_CONTROL*/,
// 			&shader_code, &error_buf);
// 
// 		if( SUCCEEDED( hr))
// 		{
// 			hr = ps->create_hw_shader( shader_code);
// 			if( FAILED( hr))
// 			{
// 				LOG_ERROR( "!PS: %s", name);
// 				LOG_ERROR( "!shader creation failed");
// 				FATAL( "!Shader is not created");
// 			}
// 		}
// 		else
// 		{
// 			ASSERT( error_buf);
// 			pcstr const err_msg = ( LPCSTR)error_buf->GetBufferPointer();
// 
// 			if( !m_compile_error_handler.empty())
// 				m_compile_error_handler	( enum_shader_type_pixel, name, err_msg);
// 
// 			XRAY_UNREFERENCED_PARAMETER	( err_msg );
// 			FATAL( "!Pixel shader \"%s\" is not created. \nerrors: \n%s", name, err_msg);
// 		}
// 
// 		safe_release( shader_code);
// 		safe_release( error_buf);
// 
// 		return		ps;
// 	}
}

template <typename shader_data>
void resource_manager::release_impl( res_xs_hw<shader_data> const* xs_hw)
{
	if( !xs_hw->is_registered())
		return;
	
	render::map<shader_name_config_pair, res_xs_hw<shader_data>* > & xs_container = get_xs_container<shader_data>();
	render::map<shader_name_config_pair, res_xs_hw<shader_data>* >::iterator begin = xs_container.begin(), end = xs_container.end(), it = xs_container.begin();
	
	while( identity(true))
	{
		if ( it==end)
		{
			//if (!m_is_shader_reloading)
			//{
				DELETE( xs_hw, resource_manager_call_destructor_predicate());
				//LOG_ERROR( "! ERROR: Failed to find compiled %s '%s'",shader_type_traits<(enum_shader_type)shader_data::type>::name(), xs_hw->m_name.c_str());
			//}
			break;
		}
		else if ( it->second==xs_hw)
		{
			xs_container.erase(it);
			DELETE( xs_hw, resource_manager_call_destructor_predicate());
			break;
		}
		++it;
	}
	
	/*
	char* name = (char*)( xs_hw->m_name.c_str());

	shader_configuration null_configuration;
	null_configuration.configuration = 0x00;
	shader_configuration max_configuration;
	max_configuration.configuration = 0xFFFFFFFF;
	
	shader_name_config_pair range_start	( name, null_configuration);
	shader_name_config_pair range_end	( name, max_configuration);
	
	
	render::map<shader_name_config_pair, res_xs_hw<shader_data>* > & xs_container = get_xs_container<shader_data>();

	render::map<shader_name_config_pair, res_xs_hw<shader_data>* >::iterator remove_start	= xs_container.lower_bound( range_start);
	render::map<shader_name_config_pair, res_xs_hw<shader_data>* >::iterator remove_end		= xs_container.lower_bound( range_end);
	
	if( remove_start != xs_container.end() )
	{
		if( remove_end != xs_container.end() && remove_end->first == range_end)
			++remove_end;
	
		xs_container.erase( remove_start, remove_end);
	
		//m_vs_ids.push_back( it->second->get_id());
		DELETE( xs_hw, resource_manager_call_destructor_predicate());
	
		return;
	}
	
	if (!m_is_shader_reloading)
	{
		LOG_ERROR( "! ERROR: Failed to find compiled %s '%s'",shader_type_traits<(enum_shader_type)shader_data::type>::name(), xs_hw->m_name.c_str());
	}
	*/
}

void	resource_manager::release	( res_vs_hw const *			vs)
{
	release_impl(vs);
}

void	resource_manager::release	( res_gs_hw const *			gs)
{
	release_impl(gs);
}

void	resource_manager::release	( res_ps_hw const *			ps)
{
	release_impl(ps);
}

pcstr resource_manager::get_shader_path() const
{
	return "resources/shaders/sm_4_0";
}

pcstr resource_manager::get_converted_shader_path() const
{
	return "resources.converted/shaders/sm_4_0";
}

static pcstr shader_type_to_extension(enum_shader_type const type)
{
	switch(type)
	{
		case enum_shader_type_vertex:	return "vs";
		case enum_shader_type_pixel:	return "ps";
		case enum_shader_type_geometry: return "gs";
		default: NODEFAULT(return "");
	}
}

fs_new::virtual_path_string resource_manager::get_full_shader_path(pcstr short_shader_path, enum_shader_type shader_type) const
{
	fs_new::virtual_path_string				 full_shader_name;
	full_shader_name.assignf					(
		"%s/%s.%s",
		resource_manager::ref().get_shader_path(),
		short_shader_path,
		shader_type_to_extension(shader_type)
	);
	return full_shader_name;
}

resource_manager::shader_source& resource_manager::get_shader_source_by_short_name(pcstr shader_name, enum_shader_type shader_type, bool& out_is_found)
{
	return get_shader_source_by_full_name(get_full_shader_path(shader_name, shader_type).c_str(), out_is_found);
}

resource_manager::shader_source& resource_manager::get_shader_source_by_full_name(pcstr full_shader_name, bool& out_is_found)
{
	map_shader_sources::iterator found_source	= m_sources.find(full_shader_name);
	out_is_found								= found_source != m_sources.end();
	return										found_source->second;
}

static bool is_file_extension(pcstr file_name, pcstr ext)
{
	s32 len = strings::length(file_name) - 1;
	
	if (len > 0)
	{
		while (len >= 0)
		{
			if (file_name[len]=='.')
				return strings::equal(&file_name[len + 1], ext);
			
			--len;
		}
	}
	
	return strings::equal("", ext);
}

void resource_manager::process_files( vfs::vfs_iterator const &	begin)
{
	vector<vfs::vfs_iterator>	stack;
	stack.push_back( begin);
	vector<resources::request>	requests;
	while ( !stack.empty())
	{
		vfs::vfs_iterator curDir = stack.back();
		stack.pop_back();
		
		vfs::vfs_iterator it  = curDir.children_begin(),
							end = curDir.children_end();

		for ( ; it != end; ++it)
		{
//			fs_new::virtual_path_string name;
			if( it.is_folder())
			{
				stack.push_back( it);
			}
			else
			{
				fs_new::virtual_path_string	const & file_name	=	it.get_virtual_path();
				
				fs_new::native_path_string const &  source_absolute_path	=	it.get_physical_path();
				
				fs_new::physical_path_info const & source_info	=	resources::get_physical_path_info(source_absolute_path);
				
				//u32 pos = file_name.size()-3;
				//if( file_name.substr( pos) == ".vs" || 
				//	file_name.substr( pos) == ".ps" ||
				//	file_name.substr( pos) == ".h"  ||
				//	file_name.substr( pos) == ".s")
				
				if (!is_file_extension(file_name.c_str(), "ps") &&
					!is_file_extension(file_name.c_str(), "vs") &&
					!is_file_extension(file_name.c_str(), "gs") &&
					!is_file_extension(file_name.c_str(), "h"))
					continue;
				
				map_shader_sources::iterator found_source = m_sources.find(file_name);
				if (found_source==m_sources.end() || found_source->second.time < source_info.last_time_of_write() )
				{
					m_files_list.push_back( file_name);
					m_file_times_list.push_back( source_info.last_time_of_write() );
				}
			}
		}
	}

	if( m_files_list.empty())
	{
		m_loading_incomplete = false;
		return;
	}

	//to ensure that list does not reallocate
	for ( u32 i = 0; i < m_files_list.size(); ++i)
	{
		resources::request	req;
		req.path = m_files_list[i].c_str();
		req.id   = resources::raw_data_class_no_reuse;
		requests.push_back( req);
	}

	resources::query_resources( &requests[0], requests.size(),
		boost::bind( &resource_manager::on_raw_files_load, this, _1),
		::xray::render::g_allocator);
}



const char* get_textures_path()
{
	return "resources/textures";
}

resource_manager::resource_manager( ) :
	m_loading_incomplete		( false),
	m_is_shader_reloading		( false),
	m_need_recompile_shader_if_source_reloaded( true),
	m_texture_storage			( NULL),
	m_texture_storage_staging	( NULL),
	m_compile_error_handler		( NULL)
{
	static shader_binary_source_cook shader_binary_source_cooker;
	register_cook(&shader_binary_source_cooker);
	
	load_shader_sources	();
	
	m_texture_storage = NEW (texture_storage)();
	m_texture_storage_staging = NEW (texture_storage)();

#ifndef MASTER_GOLD
	fs_new::virtual_path_string textures_path	=	get_textures_path();
	textures_path.rtrim						(fs_new::virtual_path_string::separator);
	m_watcher_subscribe_id	=	xray::resources::subscribe_watcher(
		textures_path, 
		boost::bind(& resource_manager::on_texture_source_changed, this, _1)
	);
#endif // #ifndef MASTER_GOLD	
}

resource_manager::~resource_manager( )
{
	for( u32 i = 0; i< m_states.size(); ++i)
		DELETE( m_states[i], resource_manager_call_destructor_predicate());

	DELETE( m_texture_storage);
	DELETE( m_texture_storage_staging);

	unregister_cook(resources::shader_binary_source_class);

#ifndef MASTER_GOLD
	xray::resources::unsubscribe_watcher(m_watcher_subscribe_id);
#endif // #ifndef MASTER_GOLD
}

static void change_substring(fs_new::virtual_path_string * src_and_dest, pcstr what, pcstr to)
{
	xray::fs_new::virtual_path_string result;
	xray::fs_new::virtual_path_string::size_type pos = src_and_dest->find(what);

	if(pos!=src_and_dest->npos)
	{
		u32 what_len = xray::strings::length(what);
		result.assign(src_and_dest->begin(), src_and_dest->begin()+pos); // head
		result.append(to); // body
		result.append(src_and_dest->begin()+pos+what_len, src_and_dest->end()); // tail
		* src_and_dest = result;
	}
}

 //static xray::fs_new::virtual_path_string get_short_texture_name(pcstr name)
 //{
 //	xray::fs_new::virtual_path_string result = name;
 //	change_substring(&result, get_textures_path(), "");
 //	
 //	xray::fs_new::virtual_path_string::size_type pos = result.rfind('.');
 //	if (pos!=xray::fs_new::virtual_path_string::npos)
 //		result.set_length(pos);
 //	
 //	return result;
 //}

#ifndef MASTER_GOLD
void resource_manager::on_texture_source_changed(xray::vfs::vfs_notification const & info)
{
	if (!info.physical_new_path && !info.physical_path)
		return;
	
	if (( info.type==xray::vfs::vfs_notification::type_modified ||
		  info.type==xray::vfs::vfs_notification::type_renamed )
		  && ((info.physical_new_path && strings::length(info.physical_new_path)) || (info.physical_path && strings::length(info.physical_path))))
	{
		pstr textures_position			= (info.physical_path && strings::length(info.physical_path)) 
										?
										strstr(info.physical_path, "textures\\") :
										strstr(info.physical_new_path, "textures\\");
		
		// Photoshop temporary file
		if (info.physical_new_path && strstr(info.physical_new_path, ".tmp") ||
			info.physical_path && strstr(info.physical_path, ".tmp"))
			return;
		
		pstr textures_position_it		=	textures_position;
		while (*textures_position_it)
		{
			if (*textures_position_it=='\\')
				*textures_position_it = '/';
			
			textures_position_it++;
		}
		
		xray::fs_new::virtual_path_string result	= textures_position;
		
		change_substring				(&result, "textures/", "");
		change_substring				(&result, ".tga", "");
		change_substring				(&result, ".options", "");
		
		map_texture::iterator found		= m_texture_registry.find(result.c_str());
		
		if (found!=m_texture_registry.end())
			if (std::find(m_textures_to_reload.begin(), m_textures_to_reload.end(), found->first)==m_textures_to_reload.end())
				m_textures_to_reload.push_back(found->first);
	}
}
#endif // #ifndef MASTER_GOLD

static ref_texture create_color_grading_base_lut(u32 const size)
{
	struct pixel
	{
#	pragma warning(push)
#	pragma warning(disable:4201)
		union
		{
			u32		clr;
			struct
			{
				u8	r;
				u8	g;
				u8	b;
				u8	a;
			};
		};
#	pragma warning(pop)
	};
	
	u32 const data_size			= size * size * size * sizeof(pixel);
	pixel* temp_data			= (pixel*)ALLOCA(data_size);
	
	for (u32 z = 0; z < size; z++)
	{
		for (u32 y = 0; y < size; y++)
		{
			for (u32 x = 0; x < size; x++)
			{
				pixel* p	= temp_data + z * size * size + y * size + x;
				p->r		= static_cast_checked<u8>(x * size);
				p->g		= static_cast_checked<u8>(y * size);
				p->b		= static_cast_checked<u8>(z * size);
				p->a		= 255;
			}
		}
	}
	
	D3D_SUBRESOURCE_DATA	data;
	data.pSysMem			= temp_data;
	data.SysMemSlicePitch	= data_size / size;
	data.SysMemPitch		= data_size / (size * size);
	
	return					resource_manager::ref().create_texture3d(
		"$user$test3d", 
		size, 
		size, 
		size, 
		&data, 
		DXGI_FORMAT_R8G8B8A8_UNORM, 
		D3D_USAGE_IMMUTABLE, 
		1
	);
}

ref_texture resource_manager::get_color_grading_base_lut()
{
	if (!m_color_grading_base_lut)
	{
		m_color_grading_base_lut = create_color_grading_base_lut(16);
	}
	
	return m_color_grading_base_lut;
}


#if 0
void	resource_manager::initialize_texture_storage	( xray::configs::lua_config_value const & cfg)
{
	size_t const memory_amount = device::ref().video_memory_size();
	
	map_texture::iterator		it = m_texture_registry.begin();
	map_texture::const_iterator	end = m_texture_registry.end();
	
	size_t mem_usage = 0;
	for( ; it != end; ++it)
		mem_usage += utils::calc_texture_size( it->second->width(), it->second->height(), it->second->format(), it->second->mips_count());
	
	size_t memory_to_use = memory_amount - mem_usage - 128*1024*1024; //mb
	float staging_percent = 2.f;
	
 	m_texture_storage->initialize			( cfg, size_t(memory_to_use * (100 - staging_percent)/100), D3D_USAGE_DEFAULT);
	m_texture_storage_staging->initialize	( cfg, size_t(memory_to_use * staging_percent / 100), D3D_USAGE_STAGING);
}
#endif

void resource_manager::recompile_shaders( render::vector<pcstr> const& changed_global_defines_list)
{
	render::vector<fs_new::virtual_path_string> shader_files_to_recompile;
	
	for (map_shader_sources::const_iterator shd_it=m_sources.begin(); shd_it!=m_sources.end(); ++shd_it)
	{
		shader_declarated_macroses_list in_shader_declared_defines_list;
		
		if (!found_shader_declarated_macroses(pcstr(&shd_it->second.data[0]), in_shader_declared_defines_list))
			continue;
		
		for (render::vector<pcstr>::const_iterator def_it=changed_global_defines_list.begin(); def_it!=changed_global_defines_list.end(); ++def_it)
		{
			if (std::find(in_shader_declared_defines_list.begin(), in_shader_declared_defines_list.end(), *def_it) != in_shader_declared_defines_list.end())
			{
				shader_files_to_recompile.push_back( shd_it->first );
			}
		}
	}
	
	recompile_xs_shaders<ps_data>(shader_files_to_recompile);
	//recompile_xs_shaders<vs_data>(shader_files_to_recompile);
	//recompile_xs_shaders<gs_data>(shader_files_to_recompile);
}


void resource_manager::on_raw_files_load( resources::queries_result& data)
{
	ASSERT( data.is_successful());
	ASSERT( data.size() == m_files_list.size());
	
	m_loading_incomplete = false;
	
	for ( u32 i = 0; i < data.size(); ++i)
	{
#pragma message (XRAY_TODO("Lain 2 IronNick: just save managed_resource_ptr here instead of using vector"))
		xray::resources::managed_resource_ptr	shader_source	=	data[i].get_managed_resource();
		R_ASSERT								(shader_source);
		xray::resources::pinned_ptr_const<u8>	shader_source_buffer	(shader_source);

		fs_new::virtual_path_string file_path			 = m_files_list[i];
		if (m_sources.find(file_path)!=m_sources.end())
			m_previous_sources[file_path] = m_sources[file_path];
		
		m_sources[file_path].data.assign( 
			shader_source_buffer.c_ptr(), 
			shader_source_buffer.c_ptr() + shader_source_buffer.size());

		m_sources[file_path].time = m_file_times_list[i];
	}
	
	if (m_is_shader_reloading && m_need_recompile_shader_if_source_reloaded)
	{
		recompile_xs_shaders<ps_data>(m_files_list);
		//recompile_xs_shaders<vs_data>(m_files_list);
		//recompile_xs_shaders<gs_data>(m_files_list);
		
		m_is_shader_reloading = false;
	}
	
	//release file names and times list
	{
		vector<fs_new::virtual_path_string>	empty;
		m_files_list.swap( empty);
		
		vector<u32> empty2;
		m_file_times_list.swap(empty2);
	}
}

void resource_manager::reload_modified_textures()
{
	textures_to_reload_vector textures_to_reload = m_textures_to_reload;
	
	for (textures_to_reload_vector::iterator it = textures_to_reload.begin(); it != textures_to_reload.end(); ++it)
	{
		fs_new::virtual_path_string path;
		fs_new::virtual_path_string path_add = *it;
		
		path.assignf("%s/%s.dds", get_textures_path(), path_add.c_str());
		
		resources::query_resource(
			path.c_str(),
			resources::texture_wrapper_class, 
			boost::bind(&resource_manager::on_texture_loaded, this, _1, 0, true),
			::xray::render::g_allocator
		);
	}
}

void resource_manager::reload_shader_sources(bool is_recompile_shaders)
{
	m_is_shader_reloading					   = true;
	m_need_recompile_shader_if_source_reloaded = is_recompile_shaders;
	
	load_shader_sources();
}

void resource_manager::load_shader_sources()
{
	m_files_list.clear();
	m_file_times_list.clear();
	
	m_loading_incomplete = true;
	
	resources::query_vfs_iterator( get_shader_path(), 
		boost::bind( &resource_manager::process_files, this, _1 ),
		::xray::render::g_allocator, resources::recursive_true);
	
	if (!m_is_shader_reloading)
	while ( m_loading_incomplete)
	{
		if( threading::g_debug_single_thread)
			resources::tick				();

		resources::dispatch_callbacks	();
	}
//	else
//		resources::query_fs_iterator_and_wait( get_shader_path(), 
//			boost::bind( &resource_manager::process_files, this, _1 ),
//			::xray::render::g_allocator, resources::recursive_true);
// 	}
// 	else {
// 		resources::request requests[] = {
// 			{ "resources/shaders/sm_4_0/test.vs",	resources::raw_data_class },
// 			{ "resources/shaders/sm_4_0/test.ps",	resources::raw_data_class },
// 			{ "resources/shaders/sm_4_0/color.ps",	resources::raw_data_class },
// 			{ "resources/shaders/sm_4_0/common.h",	resources::raw_data_class },
// 			{ "resources/shaders/sm_4_0/shared/common.h",	resources::raw_data_class },
// 			{ "resources/shaders/sm_4_0/terrain.vs",	resources::raw_data_class },
// 			{ "resources/shaders/sm_4_0/terrain.ps",	resources::raw_data_class },
// 
// 		};
// 		for ( u32 i=0, n=array_size( requests); i < n; ++i) {
// 			m_files_list.push_back	( requests[i].path);
// 		}
// 
// 		resources::query_resources( &requests[0], array_size( requests),
// 			resources::query_callback( this, &resource_manager::on_raw_files_load),
// 			::xray::render::g_allocator);
// 	}
}

shader_constant_table* resource_manager::create_const_table( shader_constant_table const& proto)
{
	shader_constant_table new_table = proto;
// 	if( new_table.empty())
// 		return NULL;

	new_table.apply_bindings( m_const_bindings);

	for ( u32 it=0; it < m_const_tables.size(); ++it)
		if( identity(m_const_tables[it]->equal( new_table) && g_enable_resource_sharing))
			return m_const_tables[it];

	m_const_tables.push_back( NEW( shader_constant_table)( new_table));
	m_const_tables.back()->mark_registered();
	
	return m_const_tables.back		();
}

void resource_manager::release( shader_constant_table const* const_table)
{
	if( !const_table->is_registered())
		return;

	if( reclaim( m_const_tables, const_table))
	{
		DELETE( const_table, resource_manager_call_destructor_predicate());
		return;
	}

	LOG_ERROR( "!ERROR: Failed to find compiled shader_constant-table");
}

shader_constant_host const * resource_manager::register_constant_binding( shader_constant_binding const & binding)
{
	m_const_bindings.add( binding);

	shader_constant_host * host =  backend::ref().register_constant_host( binding.name(), binding.type() );
	host->m_source = binding.source();

	return host;
}

res_texture* resource_manager::find_texture	( char const * name)
{
	//R_ASSERT( name && name[0]);

	map_texture::iterator it = m_texture_registry.find( name);

	if( it!=m_texture_registry.end())
		return	it->second;

	return NULL;
}

res_texture* resource_manager::create_texture	( LPCSTR physical_name, u32 mip_level_cut, bool use_pool, bool load_async, bool use_converter)
{
	// DBG_VerifyTextures	();
	if( physical_name && 0 == strcmp( physical_name,"null"))
		return 0;

	//R_ASSERT( physical_name && physical_name[0]);

#ifdef	DEBUG
	//simplify_texture( tex_name);
#endif	//	DEBUG

	

	res_texture* res = find_texture( physical_name);
	if( res != NULL)
		return	res;
	else
		return load_texture( physical_name, mip_level_cut, use_pool, load_async, use_converter);
}

static pcstr resources_textures_converted_string	=	"resources.converted/textures/";
static pcstr resources_textures						=	"resources/textures/";

static void fix_texture_name(fs_new::virtual_path_string& str)
{
	change_substring(&str, resources_textures, "");
	change_substring(&str, resources_textures_converted_string, "");
	fs_new::virtual_path_string::size_type pos = str.rfind('.');
	if (pos!=fs_new::virtual_path_string::npos)
		str.set_length(pos);
}

void on_shader_loaded2(resources::managed_resource_ptr* out_result, xray::resources::queries_result& data)
{
	if (!data[0].is_successful())		
		return;
	
	*out_result = data[0].get_managed_resource();
}

void resource_manager::on_texture_loaded( resources::queries_result& data, u32 mip_level_cut, bool use_converter)
{
	fs_new::virtual_path_string name( data[0].get_requested_path());
 
 	if( !data.is_successful())//  || !data[0].get_unmanaged_resource())
 	{
 		LOG_ERROR( "Texture %s was not found!", name.c_str());
 		return;
 	}
	
	fix_texture_name(name);
	
	textures_to_reload_vector::iterator to_delete	= std::find(m_textures_to_reload.begin(), m_textures_to_reload.end(), name);
	if (to_delete!=m_textures_to_reload.end())
		m_textures_to_reload.erase					(to_delete);
	
	res_texture* tex = create_texture( name.c_str());
	
	if (use_converter)
	{
		resources::managed_resource_ptr managed_ptr = data[0].get_managed_resource();
		resources::pinned_ptr_const< texture_data_resource > managed_typed_ptr( managed_ptr );
		
		render::texture_data_resource const* resource	= managed_typed_ptr.c_ptr();
		
		u8 const *	dds_ptr		= (u8 const *) resource->buffer().c_ptr();
		u32			dds_size	= resource->buffer().size();
		
		bool is_srgb_option = read_srgb_flag(dds_ptr, dds_size);
		
		dds_size				-= sizeof(bool);
		
		//u16	rescale_min_max[8];
		//xray::memory::copy(rescale_min_max, sizeof(rescale_min_max), dds_ptr + dds_size - sizeof(rescale_min_max), sizeof(rescale_min_max));
		
//		TODO: remove all platform converted textures
// 		tex->m_rescale_min = float4(rescale_min_max[0], rescale_min_max[1], rescale_min_max[2], rescale_min_max[3]);
// 		tex->m_rescale_min /= float((WORD)-1);
// 		tex->m_rescale_max = float4(rescale_min_max[4], rescale_min_max[5], rescale_min_max[6], rescale_min_max[7]);
// 		tex->m_rescale_max /= float((WORD)-1);
		
		tex->m_rescale_min = float4(0.0f, 0.0f, 0.0f, 0.0f);
		tex->m_rescale_max = float4(1.0f, 1.0f, 1.0f, 1.0f);
		
		D3DX_IMAGE_INFO	dds_info = {0};
		CHECK_RESULT( D3DXGetImageInfoFromMemory( dds_ptr, dds_size, 0, &dds_info, 0));
		
		D3DX_IMAGE_LOAD_INFO load_info;
		
// 		load_info.Width				= dds_info.Width;
// 		load_info.Height			= dds_info.Height;
// 		load_info.Depth				= dds_info.Depth;
// 		load_info.FirstMipLevel		= 0;
// 		load_info.MipLevels			= dds_info.MipLevels;
// 		load_info.Usage				= D3D_USAGE_DEFAULT;
// 		load_info.MiscFlags			= dds_info.MiscFlags;
// 		load_info.Format			= dds_info.Format;
// 		load_info.pSrcInfo			= &dds_info;
		
		bool staging = false;
		if( staging)
		{
			load_info.Usage			= D3D_USAGE_STAGING;
			load_info.BindFlags		= 0;
			load_info.CpuAccessFlags = D3D_CPU_ACCESS_WRITE;
		}
		else
		{
			//load_info.Usage			= D3D_USAGE_DEFAULT;
			load_info.BindFlags		= D3D_BIND_SHADER_RESOURCE;
			load_info.pSrcInfo		= &dds_info;
			load_info.Format		= dds_info.Format;
		}
		
		//if (is_srgb_option)
		//{
		//	load_info.Filter		|= D3DX11_FILTER_SRGB_OUT;
		//}
		
		ID3DBaseTexture* base_tex = NULL;
		HRESULT res = D3DXCreateTextureFromMemory(
			device::ref().d3d_device(),
			dds_ptr,
			dds_size,
			&load_info,
			0,
			&base_tex,
			0
		);
		//if ( FAILED(res) ) {
		//	pvoid test_buffer		= DEBUG_MALLOC( dds_size, "test for texture" );
		//	HRESULT const result	= D3DXCreateTextureFromMemory( device::ref().d3d_device(), 
		//		test_buffer, 
		//		dds_size,
		//		&load_info,
		//		0,
		//		&base_tex,
		//		0
		//	);
		//	CHECK_RESULT			( result );
		//	DEBUG_FREE				( test_buffer );
		//}
		
		ID3DBaseTexture* base_tex_copy = NULL;
		
		if (is_srgb_option)
			base_tex_copy			= make_copy_with_srgb_format(base_tex);
		
		CHECK_RESULT(res);
		
		tex->set_hw_texture(is_srgb_option ? base_tex_copy : base_tex, mip_level_cut, false, is_srgb_option);
		
		if (base_tex)
			base_tex->Release();
		
		if (base_tex_copy)
			base_tex_copy->Release();
	}
	else
	{
		resources::pinned_ptr_const<u8>	ptr_man(data[0].get_managed_resource());

		D3DX_IMAGE_INFO	dds_info = {0};
		CHECK_RESULT( D3DXGetImageInfoFromMemory( ptr_man.c_ptr(), ptr_man.size(), 0, &dds_info, 0));

		D3DX_IMAGE_LOAD_INFO load_info;

		bool staging = false;
		if( staging)
		{
			load_info.Usage			= D3D_USAGE_STAGING;
			load_info.BindFlags		= 0;
			load_info.CpuAccessFlags = D3D_CPU_ACCESS_WRITE;
		}
		else
		{
			load_info.Usage		= D3D_USAGE_DEFAULT;
			load_info.BindFlags	= D3D_BIND_SHADER_RESOURCE;
		}

		ID3DBaseTexture* base_tex = NULL;
		HRESULT res = D3DXCreateTextureFromMemory( device::ref().d3d_device(), 
			ptr_man.c_ptr(), 
			ptr_man.size(),
			&load_info,
			0,
			&base_tex,
			0);
		CHECK_RESULT( res);
		tex->set_hw_texture( base_tex, mip_level_cut);
		base_tex->Release();
	}
}

void resource_manager::on_texture_loaded_staging( resources::queries_result& data, u32 mip_level_cut, bool use_converter)
{
 	fs_new::virtual_path_string name( data[0].get_requested_path());
 
 	if( !data.is_successful())// || !data[0].get_unmanaged_resource())
 	{
 		LOG_ERROR( "Texture %s not found!", name.c_str());
 		return;
 	}

	fix_texture_name(name);
	
	res_texture* tex = create_texture( name.c_str());

	if (use_converter)
	{
		resources::managed_resource_ptr managed_ptr = data[0].get_managed_resource();
		resources::pinned_ptr_const< texture_data_resource > managed_typed_ptr( managed_ptr );

		render::texture_data_resource const* resource	= managed_typed_ptr.c_ptr();
		
		u8 const *	dds_ptr		= (u8 const *) resource->buffer().c_ptr();
		u32			dds_size	= resource->buffer().size();
		
		u16	rescale_min_max[8];
		xray::memory::copy(rescale_min_max, sizeof(rescale_min_max), dds_ptr + dds_size - sizeof(rescale_min_max), sizeof(rescale_min_max));
		
		//		TODO: remove all platform converted textures
// 		tex->m_rescale_min = float4(rescale_min_max[0], rescale_min_max[1], rescale_min_max[2], rescale_min_max[3]);
// 		tex->m_rescale_min /= float((WORD)-1);
// 		tex->m_rescale_max = float4(rescale_min_max[4], rescale_min_max[5], rescale_min_max[6], rescale_min_max[7]);
// 		tex->m_rescale_max /= float((WORD)-1);
		
		tex->m_rescale_min = float4(0.0f, 0.0f, 0.0f, 0.0f);
		tex->m_rescale_max = float4(1.0f, 1.0f, 1.0f, 1.0f);
		
		dds_info dds_info;
		if( !load_dds_info( dds_ptr, dds_size, dds_info))
		{
			on_texture_loaded( data);
			return;
		}

		if( dds_info.mips_count != utils::calc_mipmap_count( dds_info.width, dds_info.height))
		{
			on_texture_loaded( data);
			return;
		}

		if( mip_level_cut >= dds_info.mips_count-1 )
		{
			ASSERT( 0);
			return;
		}

		u32 resize_factor = 1 << mip_level_cut;

		u32 orig_size =  utils::calc_texture_size( dds_info.width, dds_info.height, dds_info.format, dds_info.mips_count);

		dds_info.width		/=	resize_factor;
		dds_info.height		/=	resize_factor;
		dds_info.mips_count -=	mip_level_cut;
		dds_info.data_size	=	utils::calc_texture_size( dds_info.width, dds_info.height, dds_info.format, dds_info.mips_count);
		dds_info.data_ptr	+=	( orig_size - dds_info.data_size);

		res_texture * pool_texture		= m_texture_storage->get( dds_info.width, dds_info.height, dds_info.format);
		res_texture * staging_texture	= m_texture_storage_staging->get( dds_info.width, dds_info.height, dds_info.format);

		if( !pool_texture || !staging_texture)
			return;

		tex->set_hw_texture( pool_texture->hw_texture());

		load_dds_to_texture( dds_info, staging_texture);

		device::ref().d3d_context()->CopyResource( tex->hw_texture(), staging_texture->hw_texture());

		m_texture_storage_staging->release( staging_texture);
	}
	else
	{
		resources::pinned_ptr_const<u8>	ptr_man(data[0].get_managed_resource());

		dds_info dds_info;
		if( !load_dds_info( ptr_man.c_ptr(), ptr_man.size(), dds_info))
		{
			on_texture_loaded( data);
			return;
		}


		if( dds_info.mips_count != utils::calc_mipmap_count( dds_info.width, dds_info.height))
		{
			on_texture_loaded( data);
			return;
		}

		if( mip_level_cut >= dds_info.mips_count-1 )
		{
			ASSERT( 0);
			return;
		}

		u32 resize_factor = 1 << mip_level_cut;

		u32 orig_size =  utils::calc_texture_size( dds_info.width, dds_info.height, dds_info.format, dds_info.mips_count);

		dds_info.width		/=	resize_factor;
		dds_info.height		/=	resize_factor;
		dds_info.mips_count -=	mip_level_cut;
		dds_info.data_size	=	utils::calc_texture_size( dds_info.width, dds_info.height, dds_info.format, dds_info.mips_count);
		dds_info.data_ptr	+=	( orig_size - dds_info.data_size);

		res_texture * pool_texture		= m_texture_storage->get( dds_info.width, dds_info.height, dds_info.format);
		res_texture * staging_texture	= m_texture_storage_staging->get( dds_info.width, dds_info.height, dds_info.format);

		if( !pool_texture || !staging_texture)
			return;

		tex->set_hw_texture( pool_texture->hw_texture());

		load_dds_to_texture( dds_info, staging_texture);

		device::ref().d3d_context()->CopyResource( tex->hw_texture(), staging_texture->hw_texture());

		m_texture_storage_staging->release( staging_texture);
	}

//	u8 const *	dds_ptr		= (u8 const *) resource->m_buffer.c_ptr();
 //	u32			dds_size	= resource->m_buffer.size();

	/*fixed_string<512> name( data[0].get_requested_path());

	name = name.substr( 19);
	name = name.substr( 0, name.length()-4);

	if( !data.is_successful() || !data[0].get_managed_resource())
	{
		LOG_ERROR( "Texture %s not found!", name.c_str());
		return;
	}

	resources::pinned_ptr_const<u8>	ptr	( data[0].get_managed_resource());

	u8 const *	dds_ptr		= ptr.c_ptr();
	u32			dds_size	= ptr.size();
*/
}

res_texture* resource_manager::load_texture( char const * texture_name, u32 mip_level_cut, bool use_pool, bool load_async, bool use_converter)
{
	resources::class_id_enum class_id = use_converter ? resources::texture_wrapper_class : resources::raw_data_class;
	//fixed_string<512> path		  = use_converter ? "resources.converted/textures/" : "resources/textures/";
	fs_new::virtual_path_string path			  = "resources/textures/";
	
	if( !use_pool)
	{
		res_texture* tex = NEW( res_texture);
		tex->set_name( texture_name );
		tex->mark_registered();
		m_texture_registry.insert( utils::mk_pair( tex->name(), tex));

		if( strstr( tex->name(), "$user$") == 0 && texture_name && texture_name[0])
		{
			path += tex->name();
			path += ".dds";

			if (!load_async)
			resources::query_resource_and_wait( path.c_str(), 
				class_id, 
				boost::bind( &resource_manager::on_texture_loaded, this, _1, mip_level_cut, use_converter ),
				::xray::render::g_allocator);
			else
			resources::query_resource( path.c_str(), 
				class_id, 
				boost::bind( &resource_manager::on_texture_loaded, this, _1, mip_level_cut, use_converter ),
				::xray::render::g_allocator,
				0, 
				0,
				assert_on_fail_false);
		}

		return tex;
	}
	else
	{
		res_texture* tex = NEW( res_texture)( true);
		tex->mark_registered();
		tex->set_name( texture_name );
		m_texture_registry.insert( utils::mk_pair( tex->name(), tex));
		
		
		ASSERT( strstr( tex->name(), "$user$") == 0);
		path += tex->name();
		path += ".dds";
		ASSERT( strstr( path.c_str(), "\\")==0);
		//fix_path( path.get_buffer());
		if (!load_async)
		resources::query_resource_and_wait( path.c_str(), 
			class_id, 
			boost::bind( &resource_manager::on_texture_loaded_staging, this, _1, mip_level_cut, use_converter ),
			::xray::render::g_allocator);
		else
			resources::query_resource( path.c_str(), 
			class_id, 
			boost::bind( &resource_manager::on_texture_loaded_staging, this, _1, mip_level_cut, use_converter ),
			::xray::render::g_allocator);
		
		return tex;
	}
}

res_texture* resource_manager::create_texture2d_impl ( u32 width, u32 height, D3D_SUBRESOURCE_DATA const* data, DXGI_FORMAT format, D3D_USAGE usage, u32 mip_levels, u32 array_size )
{
	D3D_TEXTURE2D_DESC texure_desc;
	ZeroMemory( &texure_desc, sizeof(texure_desc));
	texure_desc.Width				= width;
	texure_desc.Height				= height;
	texure_desc.Format				= format;
	texure_desc.SampleDesc.Count	= 1; 
	texure_desc.SampleDesc.Quality	= 0; 
	texure_desc.Usage				= usage;
	texure_desc.MipLevels			= mip_levels;
	texure_desc.ArraySize			= array_size;
	texure_desc.BindFlags			= (usage == D3D_USAGE_STAGING) ? 0 : D3D_BIND_SHADER_RESOURCE;
	texure_desc.CPUAccessFlags		= ((usage == D3D_USAGE_DYNAMIC) || (usage == D3D_USAGE_STAGING)) ? D3D_CPU_ACCESS_WRITE | D3D_CPU_ACCESS_READ : 0;

	ID3DTexture2D*	d3d_texture;
	HRESULT res = device::ref().d3d_device()->CreateTexture2D( &texure_desc, data, &d3d_texture);
	CHECK_RESULT( res);

	res_texture* tex = NEW		( res_texture);

	tex->set_hw_texture			( d3d_texture, 0, usage == D3D_USAGE_STAGING);

	d3d_texture->Release		();

	return tex;
}

void resource_manager::release_impl	( res_texture const* texture)
{
	DELETE( texture, resource_manager_call_destructor_predicate());
}

res_texture* resource_manager::create_texture2d( char const* user_name, u32 width, u32 height, D3D_SUBRESOURCE_DATA const * data, DXGI_FORMAT format, D3D_USAGE usage, u32 mip_levels, u32 array_size)
{
	res_texture* tex = create_texture2d_impl( width, height, data, format, usage, mip_levels, array_size);
	tex->set_name( user_name);
	std::pair<map_texture::iterator, bool> res = m_texture_registry.insert	( utils::mk_pair( tex->name(), tex));

	ASSERT( res.second, "A texture with the specified name already exists in texture registry.");

	if( !res.second)
		return NULL;

	tex->mark_registered	();

	return tex;
}

res_texture* resource_manager::create_texture3d( char const * user_name, u32 width, u32 height, u32 depth, D3D_SUBRESOURCE_DATA const * data, DXGI_FORMAT format, u32 usage, u32 mip_levels)
{
	D3D_TEXTURE3D_DESC			desc;
	desc.Width					=	width;
	desc.Height					=	height;
	desc.Depth					=	depth;
	desc.MipLevels				=	mip_levels;
	desc.Format					=	format;
	desc.Usage					=	(D3D_USAGE)usage;
	
	desc.BindFlags				=	(usage == D3D_USAGE_IMMUTABLE) ? 
									 D3D_BIND_SHADER_RESOURCE : ((usage == D3D_USAGE_STAGING) ? 0 : D3D_BIND_SHADER_RESOURCE);
	
	if (usage == D3D_USAGE_IMMUTABLE)
	{
		desc.CPUAccessFlags		=	0;
	}
	else
	{
		if (usage == D3D_USAGE_DYNAMIC)
		{
			desc.CPUAccessFlags	=	D3D_CPU_ACCESS_WRITE;
		}
		else if (usage == D3D_USAGE_STAGING)
		{
			desc.CPUAccessFlags	=	D3D_CPU_ACCESS_READ;
		}
	}
	//desc.CPUAccessFlags			=	(usage == D3D_USAGE_IMMUTABLE) ? 
	//								 0 : (((usage == D3D_USAGE_DYNAMIC) || (usage == D3D_USAGE_STAGING)) ? D3D_CPU_ACCESS_WRITE | D3D_CPU_ACCESS_READ : 0);
	
	desc.MiscFlags				=	0;

	ID3DTexture3D* d3d_texture	=	NULL;
	
	CHECK_RESULT				(device::ref().d3d_device()->CreateTexture3D( &desc, data, &d3d_texture));
	R_ASSERT					(d3d_texture);

	res_texture* tex = NEW		(res_texture);
	tex->set_name				(user_name);
	m_texture_registry.insert	(utils::mk_pair( tex->name(), tex));

	tex->mark_registered		();
	tex->set_hw_texture			(d3d_texture, 0, usage == D3D_USAGE_STAGING);
	d3d_texture->Release		();

	return tex;
}

res_texture* resource_manager::create_texture_cube( char const * user_name, u32 width, u32 height, D3D_SUBRESOURCE_DATA const * data, DXGI_FORMAT format, u32 usage, u32 mip_levels, bool use_as_depth_stencil)
{
	D3D_TEXTURE2D_DESC			desc;
	desc.Width					= width;
	desc.Height					= height;
	desc.MipLevels				= 1;
	desc.ArraySize				= 6;
	desc.SampleDesc.Count		= 1;
	desc.SampleDesc.Quality		= 0;
	desc.Usage					= (D3D_USAGE)usage;
	desc.CPUAccessFlags			= 0;
	desc.Format					= format;
	desc.MiscFlags				= D3D_RESOURCE_MISC_TEXTURECUBE;
	desc.MipLevels				= mip_levels;
	desc.BindFlags				= D3D_BIND_SHADER_RESOURCE | (use_as_depth_stencil ? D3D_BIND_DEPTH_STENCIL : D3D_BIND_RENDER_TARGET);
	
	ID3DTexture2D* d3d_texture  = NULL;
	CHECK_RESULT( device::ref().d3d_device()->CreateTexture2D( &desc, data, &d3d_texture));
	R_ASSERT					( d3d_texture );
	
	res_texture* tex = NEW		( res_texture );
	tex->set_name				( user_name );
	m_texture_registry.insert	( utils::mk_pair( tex->name(), tex));
	
	tex->mark_registered		();
	tex->set_hw_texture			( d3d_texture);
	d3d_texture->Release		();
	
	return tex;
}

void resource_manager::release( res_texture const* texture)
{
	// DBG_VerifyTextures	();

	if( !texture->is_registered())
		return;

	pcstr const name = texture->name();
	map_texture::iterator it = m_texture_registry.find( name);

	if( it!=m_texture_registry.end())
	{
		m_texture_registry.erase( it);
		release_impl( texture);
		return;
	}

	LOG_ERROR( "! ERROR: Failed to find texture surface '%s'", texture->name() );
}


struct load_texture_delegate
{
	load_texture_delegate( ref_texture dest_texture, math::rectangle<math::int2> dest_rect, u32 arr_ind, char const* user_name):
			dest_texture		( dest_texture),
			dest_rect			( dest_rect),
			user_name			( user_name),
			arr_ind				( arr_ind)
			{}


	void execute( resources::queries_result& data)
	{
		if ( !data.is_successful() || !data[0].get_managed_resource())
		{
			dest_texture = 0;
			DELETE ((load_texture_delegate*)this);
			return;
		}
		
		fs_new::virtual_path_string name(data[0].get_requested_path());
		(void)&name;
		
		resources::managed_resource_ptr managed_ptr = data[0].get_managed_resource();
		resources::pinned_ptr_const< texture_data_resource > managed_typed_ptr( managed_ptr );
		
		render::texture_data_resource const* resource	= managed_typed_ptr.c_ptr();
		
		u8 const *	dds_ptr		= (u8 const *) resource->buffer().c_ptr();
		u32			dds_size	= resource->buffer().size();
		
		bool is_srgb			= read_srgb_flag(dds_ptr, dds_size);
		
		dds_size				-= sizeof(is_srgb);
		
		D3DX_IMAGE_INFO	dds_info = {0};
		CHECK_RESULT( D3DXGetImageInfoFromMemory( dds_ptr, dds_size, 0, &dds_info, 0));
		
		D3D_TEXTURE2D_DESC	dest_desc	= dest_texture->description ();
		
		if(	dds_info.Width			> dest_desc.Width 
			|| dds_info.Height		> dest_desc.Height 
			|| dds_info.Depth		!= 1
			//|| dds_info.MipLevels	< dest_desc.MipLevels
			|| !is_equal_formats(dds_info.Format, dest_desc.Format))
		{
			LOG_ERROR( "The loaded image is not compatible with destination texture!" ) ;
			return;
		}
		
		D3DX_IMAGE_LOAD_INFO load_info;
		
		load_info.Usage				= D3D_USAGE_DEFAULT;
		load_info.BindFlags			= 0;
		load_info.CpuAccessFlags	= 0;
		
		ID3DBaseTexture*	src_tex	= NULL;
		
		HRESULT res = D3DXCreateTextureFromMemory(
			device::ref().d3d_device(), 
			dds_ptr,
			dds_size,
			&load_info, 
			0,
			&src_tex,
			0
		);
		
		if (is_srgb)
			src_tex					= make_copy_with_srgb_format(src_tex);
		
		CHECK_RESULT( res);
		
		u32 mip_width	= dds_info.Width;
		u32 mip_height	= dds_info.Height;
		
		u32 size_diff	= math::min(dest_desc.Width / dds_info.Width, dest_desc.Height / dds_info.Height);
		
		for (u32 i = 0; i < dest_desc.MipLevels; i++)
		{
			for (u32 y = 0; y < size_diff; y++)
			for (u32 x = 0; x < size_diff; x++)
			{
				D3D_BOX		b;
				b.left		= 0;
				b.top		= 0;
				b.right		= mip_width;
				b.bottom	= mip_height;
				b.front		= 0;
				b.back		= 1;
				
				device::ref().d3d_context()->CopySubresourceRegion( dest_texture->hw_texture(), D3D10CalcSubresource( i, arr_ind, dest_desc.MipLevels), x * mip_width, y * mip_height, 0, src_tex, i, &b);
				
				device::ref().d3d_context()->Flush();
			}
			
			mip_width /= 2;
			mip_height /= 2;
		}
		
		DELETE ((load_texture_delegate*)this);
	}

private:
	math::rectangle<math::int2>	dest_rect;
	ref_texture					dest_texture;
	char const*					user_name;
	u32							arr_ind;
};

bool resource_manager::copy_texture_from_file( res_texture * dest_texture, math::rectangle<math::int2> dest_rect, u32 arr_ind, char const* src_name)
{
	fixed_string<512> path( "resources/textures/");

	path += src_name;
	path += ".dds";

	load_texture_delegate* load_delegate = NEW ( load_texture_delegate) ( dest_texture, dest_rect, arr_ind, src_name);

	resources::query_resource( path.c_str(), 
		resources::texture_wrapper_class,
		boost::bind( &load_texture_delegate::execute, load_delegate, _1 ),
		::xray::render::g_allocator);
	
	dest_texture->set_name( src_name );
	m_texture_registry.insert( utils::mk_pair( src_name, dest_texture));
	
	return true;
}

untyped_buffer* resource_manager::create_buffer ( u32 size, const void * data, enum_buffer_type type, bool dynamic)
{
	untyped_buffer* new_buffer = NEW(untyped_buffer)( size, data, type, dynamic);

	m_buffers.push_back( new_buffer);

	return new_buffer;
}

void resource_manager::release	( untyped_buffer const* buffer)
{
	if( reclaim( m_buffers, buffer))
	{
		DELETE( buffer, resource_manager_call_destructor_predicate());
		return;
	}
	
// 	// DELETE_PRIVATE implementation
// 	template < typename T >
// 	struct same_type_tester {
// 		static boost::yes	test( T* );
// 		static boost::no	test( ... );
// 	}; // struct same_type_tester
// 
// 	typedef untyped_buffer type;
// 	type* instance = buffer;
// 	COMPILE_ASSERT		( sizeof( same_type_tester<type>(instance) ) == sizeof( boost::yes ), Please_specify_correct_type );
// 	
// 	COMPILE_ASSERT		( !boost::is_polymorphic<untyped_buffer>::value, We_should_use_dynamic_cast_with_void_star_to_get_real_pointer_for_memory_allocator );
// 	buffer->~untyped_buffer	( );
// 	FREE				( buffer );

	ASSERT("The buffer not found in resource manager resources!");
}

res_state* resource_manager::create_state( state_descriptor & descriptor)
{
	ID3DRasterizerState*	rasterizer_state	=	m_rs_cache.get_state ( descriptor.m_rasterizer_desc);
	ID3DDepthStencilState*	depth_stencil_state =	m_dss_cache.get_state( descriptor.m_depth_stencil_desc);
	ID3DBlendState*			blend_state			=	m_bs_cache.get_state( descriptor.m_effect_desc);


	res_state* new_state = NEW( res_state)( rasterizer_state, 
											depth_stencil_state, 
											blend_state, 
											descriptor.m_stencil_ref);
	
	new_state->mark_registered();
//	new_state->set_id	( m_states.size()+1);
	m_states.push_back	( new_state);

	return new_state;
}
void resource_manager::release( res_state const* state)
{
	if( !state->is_registered())
		return;

	if( reclaim_with_id( m_states, state))
	{
		DELETE( state, resource_manager_call_destructor_predicate());
		return;
	}

	LOG_ERROR( "!ERROR: Failed to find compiled stateblock");
}

// res_declaration* resource_manager::create_declaration( DWORD fvf)
// {
// 	D3DVERTEXELEMENT9	dcl[MAX_FVF_DECL_SIZE];
// 	
// 	CHECK_RESULT( D3DXDeclaratorFromFVF( fvf, dcl));
// 	return create_declaration( dcl);
// }

res_declaration* resource_manager::create_declaration( D3D_INPUT_ELEMENT_DESC const* dcl, u32 count)
{
	// Search equal code
	for ( u32 it=0; it<m_declarations.size(); it++)
	{
		res_declaration* decl = m_declarations[it];
		if( identity(decl->equal( dcl, count) && g_enable_resource_sharing))
			return decl;
	}

	// Create _new
	res_declaration* new_decl = NEW( res_declaration)( dcl, count);
	new_decl->mark_registered();
	m_declarations.push_back( new_decl);

	return new_decl;
}

void resource_manager::release( res_declaration const* dcl)
{
	if( !dcl->is_registered())
		return;

	if( reclaim( m_declarations, dcl))
	{
		DELETE( dcl, resource_manager_call_destructor_predicate());
		return;
	}

	LOG_ERROR( "! ERROR: Failed to find compiled vertex-declarator");
}

res_signature*	resource_manager::create_signature	( ID3DBlob * signature)
{
	for ( u32 it=0; it<m_signatures.size(); it++)
	{
		if( identity(m_signatures[it]->equal( signature) && g_enable_resource_sharing))
			return m_signatures[it];
	}

	// Create _new
	res_signature* new_signature = NEW( res_signature)( signature);
	new_signature->mark_registered();
	m_signatures.push_back( new_signature);

	return new_signature;
}

void  resource_manager::release		( res_signature const* signature)
{
	if( !signature->is_registered())
		return;

	if( reclaim( m_signatures, signature))
	{
		DELETE( signature, resource_manager_call_destructor_predicate());
		return;
	}

	LOG_ERROR( "! ERROR: Failed to find created signature.");
}

res_input_layout*	resource_manager::create_input_layout	( res_declaration const * decl, res_signature const * signature)
{
	// Search equal code
	for ( u32 it=0; it<m_input_layouts.size(); it++)
	{
		res_input_layout* layout = m_input_layouts[it];
		if( identity(layout->equal( decl, signature) && g_enable_resource_sharing))
			return layout;
	}

	// Create _new
	res_input_layout* new_layout = NEW( res_input_layout)( decl, signature);
	new_layout->mark_registered();
	m_input_layouts.push_back( new_layout);

	return new_layout;
}

void	resource_manager::release		( res_input_layout const* layout)
{
	if( !layout->is_registered())
		return;

	if( reclaim( m_input_layouts, layout))
	{
		DELETE( layout, resource_manager_call_destructor_predicate());
		return;
	}

	LOG_ERROR( "! ERROR: Failed to find created layout");
}


render_target* resource_manager::create_volume_render_target( LPCSTR name, u32 w, u32 h, u32 d, DXGI_FORMAT fmt, enum_rt_usage usage, D3D_USAGE memory_usage)
{
	//R_ASSERT( name && name[0] && w && h && d);
	
	map_rt::iterator it = m_rt_registry.find( name);
	
	if( name && it != m_rt_registry.end())
	{
		return it->second;
	}
	else
	{
		render_target*	rt = NEW( render_target);
		rt->set_name( name);
		rt->mark_registered();
		m_rt_registry.insert( utils::mk_pair( rt->name().c_str(), rt));
		
		rt->create_3d( name, w, h, d, fmt, usage, memory_usage );
		
		return rt;
	}
}

render_target*	resource_manager::create_render_target( LPCSTR name, u32 w, u32 h, DXGI_FORMAT fmt, enum_rt_usage usage, ref_texture texture, u32 first_array_slice_index, D3D_USAGE memory_usage, u32 sample_count)
{
	//R_ASSERT( name && name[0] && w && h);

	// ***** first pass - search already created RT
	map_rt::iterator it = m_rt_registry.find( name);

	if( name && it != m_rt_registry.end())
	{
//		ASSERT( it->second->width() == w 
//				&& it->second->height() == h 
//				&& it->second->format() == fmt 
//				&& it->second->usage());

		return it->second;
	}
	else
	{
		render_target*	rt = NEW( render_target);
		rt->set_name( name);
		rt->mark_registered();
		m_rt_registry.insert( utils::mk_pair( rt->name().c_str(), rt));

		if (texture)
		{
			rt->create( name, w, h, fmt, usage, texture, first_array_slice_index);
		}
		else
		{
			//if( Device.b_is_Ready)
			rt->create( name, w, h, fmt, usage, memory_usage, sample_count);
		}
		
		return rt;
	}
}

void resource_manager::release( render_target const*	rt)
{
	if( !rt->is_registered())
		return;

	pcstr const name = rt->name().c_str();
	map_rt::iterator it	= m_rt_registry.find( name);

	if( it != m_rt_registry.end())
	{
		m_rt_registry.erase( it);
		DELETE( rt, resource_manager_call_destructor_predicate());
		return;
	}

	LOG_ERROR( "! ERROR: Failed to find render-target '%s'",rt->m_name.c_str());
}

shader_constant_buffer*	resource_manager::create_constant_buffer	( name_string_type const & name, enum_shader_type dest, D3D_CBUFFER_TYPE type, u32 size)
{
	for ( u32 it=0; it<m_const_buffers.size(); it++)
	{
		shader_constant_buffer*	buf		= m_const_buffers[it];
		if( buf->type()		== type 
			&& buf->size()		== size
			&& buf->dest()		== dest
			&& buf->name()		== name
			&& g_enable_resource_sharing)
				return buf;
	}
	shader_constant_buffer* cbuffer = NEW( shader_constant_buffer)( name, dest, type, size);

	cbuffer->mark_registered();
	m_const_buffers.push_back	( cbuffer);
	return cbuffer;
}

void	resource_manager::release ( shader_constant_buffer const* cbuffer)
{
	if( !cbuffer->is_registered())
		return;

	if( reclaim( m_const_buffers, cbuffer))
	{
		DELETE( cbuffer, resource_manager_call_destructor_predicate());
		return;
	}

	LOG_ERROR( "!ERROR: Failed to find shader_constant buffer");
}

res_sampler_state*	resource_manager::create_sampler_state	( sampler_state_descriptor const & sampler_props)
{
	return m_sampler_cache.get_state( sampler_props.m_desc);
}

res_texture_list* resource_manager::create_texture_list( texture_slots const & tex_list)
{
	for( u32 it = 0; it < m_texture_lists.size(); it++)
	{
		res_texture_list* base = m_texture_lists[it];
		if( identity(base->equal( tex_list) && g_enable_resource_sharing))
			return base;
	}

	res_texture_list* lst = NEW( res_texture_list)( tex_list);
	lst->mark_registered();
	m_texture_lists.push_back( lst);
//	lst->set_id( m_texture_lists.size());
	return lst;
}

void resource_manager::release( res_texture_list const* tex_list)
{
	if( !tex_list->is_registered())
		return;

	if( reclaim_with_id( m_texture_lists, tex_list))
	{
		DELETE( tex_list, resource_manager_call_destructor_predicate());
		return;
	}

	LOG_ERROR( "!ERROR: Failed to find compiled list of textures");
}


res_sampler_list* resource_manager::create_sampler_list	( sampler_slots const & smp_list)
{
	for( u32 it = 0; it < m_sampler_lists.size(); it++)
	{
		res_sampler_list* base = m_sampler_lists[it];
		if( identity(base->equal( smp_list) && g_enable_resource_sharing))
			return base;
	}

	res_sampler_list* lst = NEW( res_sampler_list)( smp_list);
	lst->mark_registered();
	m_sampler_lists.push_back( lst);
//	lst->set_id( m_sampler_lists.size());
	return lst;

}

void resource_manager::release		( res_sampler_list const* smp_list)
{
	if( !smp_list->is_registered())
		return;

	if( reclaim_with_id( m_sampler_lists, smp_list))
	{
		DELETE( smp_list, resource_manager_call_destructor_predicate());
		return;
	}

	LOG_ERROR( "!ERROR: Failed to find compiled list of samplers");
}

template <class shader, class binder>
bool operator == ( shader const * sh, binder const & bn)
{
	return  sh->equal( bn);
}

res_vs*	resource_manager::create_vs( vs_descriptor const & binder)
{
	vs_cache::iterator it = std::find( m_v_shaders.begin(), m_v_shaders.end(), binder);

	if( it != m_v_shaders.end())
	{
		return *it;
	}
	else
	{
		res_vs * vs = NEW(res_vs)( binder);
		vs->mark_registered();
		m_v_shaders.push_back( vs);
		vs->set_id( m_v_shaders.size());
		return vs;
	}
}

void resource_manager::release	( res_vs const* vs)
{
	if( !vs->is_registered())
		return;

	if( reclaim_with_id( m_v_shaders, vs))
	{
		DELETE( vs, resource_manager_call_destructor_predicate());
		return;
	}

	LOG_ERROR( "!ERROR: Failed to find VS.");
}

res_gs*	resource_manager::create_gs( gs_descriptor const & binder)
{
	gs_cache::iterator it = std::find( m_g_shaders.begin(), m_g_shaders.end(), binder);

	if( it != m_g_shaders.end())
	{
		return *it;
	}
	else
	{
		res_gs * gs = NEW(res_gs)( binder);
		gs->mark_registered();
		m_g_shaders.push_back( gs);
		gs->set_id( m_g_shaders.size());
		return gs;
	}
}

void resource_manager::release	( res_gs const* gs)
{
	if( !gs->is_registered())
		return;

	if( reclaim_with_id( m_g_shaders, gs))
	{
		DELETE( gs, resource_manager_call_destructor_predicate());
		return;
	}

	LOG_ERROR( "!ERROR: Failed to find GS.");
}

res_ps* resource_manager::create_ps( ps_descriptor const & binder)
{
	ps_cache::iterator it = std::find( m_p_shaders.begin(), m_p_shaders.end(), binder);

	if( it != m_p_shaders.end())
	{
		return *it;
	}
	else
	{
		res_ps * ps = NEW(res_ps)( binder);
		ps->mark_registered();
		m_p_shaders.push_back( ps);
		ps->set_id( m_p_shaders.size());

		return ps;
	}
}

void resource_manager::release	( res_ps const* ps)
{
	if( !ps->is_registered())
		return;

	if( reclaim_with_id( m_p_shaders, ps))
	{
		DELETE( ps, resource_manager_call_destructor_predicate());
		return;
	}

	LOG_ERROR( "!ERROR: Failed to find PS.");
}

res_geometry* resource_manager::create_geometry( D3D_INPUT_ELEMENT_DESC const* decl, u32 decl_size, u32 vertex_stride, untyped_buffer& vb, untyped_buffer& ib)
{
	R_ASSERT( decl);

	ref_declaration dcl	= resource_manager::ref().create_declaration( decl, decl_size);

	return create_geometry( &*dcl, vertex_stride, vb, ib);
}

res_geometry* resource_manager::create_geometry( res_declaration * dcl, u32 vertex_stride, untyped_buffer& vb, untyped_buffer& ib)
{
	u32 vb_stride = vertex_stride;
	res_geometry g( vb, ib, dcl, vb_stride);

	vec_geoms::const_iterator it = m_geometries.begin();
	vec_geoms::const_iterator end = m_geometries.end();
	for( ; it != end ; ++it)
	{
		if( identity((*it)->equal( g) && g_enable_resource_sharing))
			return *it;
	}

	res_geometry* geom = NEW( res_geometry)( vb, ib, dcl, vb_stride);
	geom->mark_registered();
	m_geometries.push_back( geom);

	return	geom;
}

void resource_manager::release( res_geometry const* geom)
{
	if( !geom->is_registered())
		return;

	if( reclaim( m_geometries, geom))
	{
		DELETE( geom, resource_manager_call_destructor_predicate());
		return;
	}
	LOG_ERROR( "!ERROR: Failed to find the geometry.");
}

res_render_output*	resource_manager::create_render_output		( HWND window, bool windowed)
{
	for ( u32 it=0; it<m_render_outputs.size(); it++)
	{
		if( m_render_outputs[it]->equal( window))
			return m_render_outputs[it];
	}

	res_render_output * r_output = NEW(res_render_output)( window, windowed);
	r_output->mark_registered();
	m_render_outputs.push_back( r_output);

	return r_output;
}

void resource_manager::release( res_render_output const* render_output)
{
	if( !render_output->is_registered())
		return;

	if( reclaim( m_render_outputs, render_output))
	{
		DELETE( render_output, resource_manager_call_destructor_predicate());
		return;
	}
	LOG_ERROR( "!ERROR: Failed to render output in registry.");
}

void resource_manager::copy		( untyped_buffer* dest, untyped_buffer* source)
{
	device::ref().d3d_context()->CopyResource( dest->hardware_buffer(), source->hardware_buffer());
}

void resource_manager::copy		( untyped_buffer* dest, u32 dest_pos, untyped_buffer* source, u32 src_pos, u32 size)
{
	D3D_BOX box;
	box.left	= src_pos;
	box.top		= 0;
	box.front	= 0;
	box.right	= src_pos + size;
	box.bottom	= 1;
	box.back	= 1;

	device::ref().d3d_context()->CopySubresourceRegion( dest->hardware_buffer(), 0, dest_pos, 0, 0, source->hardware_buffer(), 0, &box);
}


void resource_manager::copy2D	( res_texture* dest, u32 dest_x, u32 dest_y, res_texture* source, u32 src_x, u32 src_y, u32 size_x, u32 size_y, u32 dest_mip, u32 src_mip)
{
	ASSERT( src_x + size_x <= dest->width() && src_y + size_y <= dest->height() );

	D3D_BOX box;
	box.left	= src_x;
	box.top		= src_y;
	box.front	= 0;
	box.right	= src_x + size_x;
	box.bottom	= src_y + size_y;
	box.back	= 1;

	device::ref().d3d_context()->CopySubresourceRegion( dest->hw_texture(), 
														D3D10CalcSubresource( dest_mip, 0, dest->description().MipLevels), 
														dest_x, 
														dest_y, 
														0, 
														source->hw_texture(), 
														src_mip, 
														&box);
}

void resource_manager::copy3D	(res_texture* dest, 
								 u32 dest_x, 
								 u32 dest_y, 
								 u32 dest_z, 
								 res_texture* source, 
								 u32 src_x, 
								 u32 src_y, 
								 u32 src_z, 
								 u32 size_x, 
								 u32 size_y, 
								 u32 size_z,
								 u32 dest_mip, 
								 u32 src_mip)
{
	
#pragma message (XRAY_TODO("Add depth() function to ref_texture or add new type ref_texture_3d?"))
	
	D3D_BOX box;
	box.left	= src_x;
	box.top		= src_y;
	box.front	= src_z;
	box.right	= src_x + size_x;
	box.bottom	= src_y + size_y;
	box.back	= src_z + size_z;
	
	device::ref().d3d_context()->CopySubresourceRegion( dest->hw_texture(), 
														D3D10CalcSubresource( dest_mip, 0, dest->description().MipLevels), 
														dest_x, 
														dest_y, 
														dest_z, 
														source->hw_texture(), 
														src_mip, 
														&box);
}



void resource_manager::set_compile_error_handler	( shader_compile_error_handler const & handler)
{
	m_compile_error_handler = handler;
}

void resource_manager::register_sampler		( char const * name, res_sampler_state * sampler)
{
//	ASSERT( find_registered_sampler(name) == NULL, "A sampler with the specified name allready was registered!");
//	ASSERT( sampler);
	m_samplers_registry.push_back( samplers_registry_record( name_string_type(name), sampler) );
}

res_sampler_state * resource_manager::find_registered_sampler	( char const* name)
{
	samplers_registry::const_iterator it	= m_samplers_registry.begin();
	samplers_registry::const_iterator end	= m_samplers_registry.end();

	for( ; it != end; ++it)
	{
		if( it->first == name)
			return it->second;
	}

	return NULL;
}




} // namespace render 
} // namespace xray 

