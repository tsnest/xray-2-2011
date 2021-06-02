////////////////////////////////////////////////////////////////////////////
//	Created		: 23.09.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef INCLUDE_HANDLER_H_INCLUDED
#define INCLUDE_HANDLER_H_INCLUDED

#include <xray/render/core/utils.h>
#include <xray/render/core/shader_include_getter.h>

namespace xray {
namespace render {

struct shader_file_info
{
	char	name[256];
	u32		time;
};

class include_handler : public ID3DInclude
{
public:
	include_handler( /*resource_manager::map_shader_sources* source_map, */resource_manager::shader_source* compiled_source, shader_include_getter* include_getter = 0): 
		/*m_sources( source_map), */m_compiled_source(compiled_source), m_include_getter(include_getter), 
#ifdef MASTER_GOLD
		includes_info(memory::g_mt_allocator)
#else // #ifdef MASTER_GOLD
		includes_info(::xray::debug::g_mt_allocator)
#endif // #ifdef MASTER_GOLD
	{
	}

	HRESULT __stdcall	Open	( D3D_INCLUDE_TYPE include_type, LPCSTR file_name, LPCVOID parent_data, LPCVOID *data_ptr, UINT *size_ptr)
	{
		XRAY_UNREFERENCED_PARAMETERS	( parent_data, include_type);
		
		if (m_include_getter)
			file_name = m_include_getter->get_shader_include(file_name);
		
		fs::path_string	path = resource_manager::ref().get_shader_path();
		path += "/";
		path += file_name;
		
		utils::fix_path( path.get_buffer());
		
		bool is_found_source = false;
		resource_manager::shader_source& shader_source = resource_manager::ref().get_shader_source_by_full_name(path.c_str(), is_found_source);
		if( !is_found_source)
		{
			return E_FAIL;
		}
		
		*data_ptr = &shader_source.data[0];
		*size_ptr = shader_source.data.size();
		
		//if (std::find(m_compiled_source->includes.begin(),m_compiled_source->includes.end(), path.c_str())==m_compiled_source->includes.end())
		//	m_compiled_source->includes.push_back( path.c_str());
		 
		shader_file_info		info;
		xray::strings::copy		(info.name, path.c_str());
		info.time				= shader_source.time;
		
		includes_info.push_back	(info);
		
		return	S_OK;
	}
	HRESULT __stdcall	Close	( LPCVOID	pData)
	{
		XRAY_UNREFERENCED_PARAMETER	( pData);
		return	S_OK;
	}
	
	vectora<shader_file_info>				includes_info;
	
private:
	resource_manager::shader_source*		m_compiled_source;
	shader_include_getter*					m_include_getter;
}; // class include_handler

} // namespace render
} // namespace xray

#endif // #ifndef INCLUDE_HANDLER_H_INCLUDED
