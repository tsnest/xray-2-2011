////////////////////////////////////////////////////////////////////////////
//	Created		: 16.03.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_editor_resources_cooker.h"
#include "dialog.h"
#include "dialog_editor_resource.h"
#include "dialog_graph_node_layout.h"

#pragma unmanaged
using xray::dialog_editor::dialog_editor_resources_cooker;
using namespace xray::resources;
using xray::dialog_editor::dialog_resources;

dialog_editor_resources_cooker::dialog_editor_resources_cooker()
:translate_query_cook(dialog_resources_class, reuse_true, use_any_thread_id)
{
}

void dialog_editor_resources_cooker::translate_query(xray::resources::query_result_for_cook& parent)
{
	LOGI_INFO("dialog_editor_cook", "dialog_editor_cook::translate_query called");

	fs_new::virtual_path_string request_without_postfix = parent.get_requested_path();
	char const request_postfix[] = ".resource";
	u32 const length_without_postfix = request_without_postfix.length() - array_size(request_postfix) + 1;
	if(request_without_postfix.find(request_postfix)==length_without_postfix)
		request_without_postfix.set_length(length_without_postfix);

	fs_new::virtual_path_string layout_path;
	layout_path += "resources/dialogs/configs/";
	layout_path += request_without_postfix;
	layout_path += ".lt";

	request	arr[]	= {
		{ request_without_postfix.c_str(), dialog_class },
		{ layout_path.c_str(), lua_config_class },
	};

	query_resources(arr,
		2,
		boost::bind(&dialog_editor_resources_cooker::on_loaded, this, _1),
		parent.get_user_allocator(),
		NULL,
		&parent);

}

void dialog_editor_resources_cooker::on_loaded(queries_result & result)
{
	LOGI_INFO("dialog_editor_resources_cooker", "dialog_editor_resources_cooker::on_loaded called");
	query_result_for_cook* parent = result.get_parent_query();

	if(result[0].is_successful())
	{
		dialog* dlg = static_cast_checked<dialog*>(result[0].get_unmanaged_resource().c_ptr());
		dialog_resources* dr = NEW(dialog_resources)();
		dr->m_dialog = dlg;
		if(result[1].is_successful())
		{
			configs::lua_config_value root = static_cast_resource_ptr<configs::lua_config_ptr>(result[1].get_unmanaged_resource())->get_root();
			dr->m_layout = NEW(dialog_graph_node_layout)(root);
		}

		parent->set_unmanaged_resource(dr, resources::nocache_memory, sizeof(dialog_resources));
		parent->finish_query(result_success);
	}
	else
		parent->finish_query(result_error);
}

void dialog_editor_resources_cooker::delete_resource (resource_base * resource)
{
	LOGI_INFO	("dialog_editor_resources_cooker", "dialog_editor_resources_cooker::delete_unmanaged called");
	DELETE		(resource);
}
