////////////////////////////////////////////////////////////////////////////
//	Created		: 28.01.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_SHADER_INCLUDE_GETTER_H_INCLUDED
#define XRAY_RENDER_CORE_SHADER_INCLUDE_GETTER_H_INCLUDED

namespace xray {
namespace render {

struct shader_include_getter
{
	virtual pcstr get_shader_include(pcstr include_file_name) const { return include_file_name;}
}; // struct shader_include_getter

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_CORE_SHADER_INCLUDE_GETTER_H_INCLUDED