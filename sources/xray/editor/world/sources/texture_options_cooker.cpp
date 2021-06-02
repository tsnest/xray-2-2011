////////////////////////////////////////////////////////////////////////////
//	Created		: 09.03.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "texture_options_cooker.h"
#include "texture_options.h"
#pragma managed(push)
#pragma unmanaged
#include <xray/intrusive_ptr.h>
#include <xray/fs_path_string.h>
#include <xray/configs_lua_config.h>
#pragma managed(pop)

namespace xray {
namespace editor {

using namespace resources;

texture_options_cooker::texture_options_cooker	() 
: translate_query_cook(	texture_options_class, reuse_true, use_user_thread_id) {}

void				texture_options_cooker::translate_query	(query_result & parent)
{
	fs::path_string				path;
	path.append					("resources/textures_new/sources/");
	path.append					(parent.get_requested_path());
	path.append					(".options");

	query_resource					(path.c_str(), 
									 config_lua_class, 
									 boost::bind(&texture_options_cooker::on_sub_resources_loaded, this, _1), 
									 parent.get_user_allocator(), 
									 NULL,
									 0,
									 &parent); 
}

void						texture_options_cooker::on_sub_resources_loaded		(queries_result & result)
{
	query_result_for_cook * 	parent	=	result.get_parent_query();
	editor::texture_options*	options = NULL;

	fs::path_string resource_path;
	query_result_for_cook & sub_request	= static_cast_checked<query_result_for_cook&>(result[0]);
	CURE_ASSERT							(sub_request.select_disk_path_from_request_path(&resource_path), return);
	resource_path.set_length			(resource_path.find("resources")+9);

	if ( result[0].is_success())
	{
		configs::lua_config_ptr config	= static_cast_resource_ptr<configs::lua_config_ptr>(result[0].get_unmanaged_resource());
		options							= NEW(editor::texture_options)(resource_path, config);
	}
	else
	{
		options							= NEW(editor::texture_options)(resource_path, fs::path_string(parent->get_requested_path()));
		if(options->type == texture_options::type_2d || options->type == texture_options::terrain)
		{
			fs::path_string options_root;
			options_root.append("resources/textures_new/sources/");
			options_root.append(parent->get_requested_path());
			options_root.set_length(options_root.rfind('/'));

			resources::query_fs_iterator(
 				options_root.c_str(), 
 				boost::bind(&texture_options_cooker::on_fs_iterator_ready, this, _1, options, parent),
				result.get_user_allocator()
 			);
			return;
		}
	}

	options->m_resource_name			= fs::path_string(parent->get_requested_path());

	parent->set_unmanaged_resource		(options, resources::memory_type_non_cacheable_resource, sizeof(editor::texture_options));
	parent->finish_query		(result_success);
}

void						texture_options_cooker::on_fs_iterator_ready		(resources::fs_iterator fs_iter, texture_options* options, query_result_for_cook* parent)
{
	options->m_resource_name			= fs::path_string(parent->get_requested_path());

	fs::path_string bump_name;
	fs::path_string normal_or_color_name;

	bump_name.append(options->m_resource_name.begin() + options->m_resource_name.rfind('/')+1, options->m_resource_name.end());
	normal_or_color_name.append(bump_name.begin(), bump_name.end());

	bump_name.append("_bump.tga");
	normal_or_color_name.append((options->type == texture_options::terrain)?"_color.tga":"_nmap.tga");

	fs::path_string full_path;
	fs::path_string disk_path;
	fs::path_string name;

	//fs_iter.get_disk_path(disk_path);
	//fs_iter.get_full_path(full_path);
	name = fs_iter.get_name();

	if(fs_iter.find_child(bump_name.c_str()) != fs_iter.children_end())
	{
		bump_name.set_length(bump_name.length()-4);
		options->bump_name = bump_name;
	}
	if(fs_iter.find_child(normal_or_color_name.c_str()) != fs_iter.children_end())
	{
		normal_or_color_name.set_length(normal_or_color_name.length()-4);
		if(options->type == texture_options::terrain)
			options->color_name = normal_or_color_name;
		else
			options->normal_name = normal_or_color_name;
	}
	
	parent->set_unmanaged_resource		(options, resources::memory_type_non_cacheable_resource, sizeof(editor::texture_options));
	parent->finish_query				(result_success);
}
void						texture_options_cooker::delete_resource			(unmanaged_resource* res)
{
	DELETE						(res);
}

} // namespace editor
} // namespace xray