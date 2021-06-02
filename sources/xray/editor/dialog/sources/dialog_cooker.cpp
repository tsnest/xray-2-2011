////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_cooker.h"
#include "dialog.h"
#include "dialogs_manager.h"
#include "dialog_link.h"

#pragma unmanaged

using xray::dialog_editor::dialog_cooker;
using xray::dialog_editor::dialog;
using xray::dialog_editor::dialog_manager::dialogs_manager;
using xray::dialog_editor::dialog_link;
using namespace xray::resources;

dialog_cooker::dialog_cooker()
:translate_query_cook(dialog_class, reuse_true, use_any_thread_id)
{
}

void dialog_cooker::translate_query(xray::resources::query_result_for_cook& parent)
{
	LOGI_INFO("dialog_cooker", "dialog_cooker::translate_query called");
	fs_new::virtual_path_string dialog_path;
	dialog_path += "resources/dialogs/configs/";
	dialog_path += parent.get_requested_path();
	dialog_path += ".dlg";

	query_resource(
		dialog_path.c_str(),
		lua_config_class,
		boost::bind(&dialog_cooker::on_loaded, this, _1),
		parent.get_user_allocator(),
		NULL,
		&parent
		);
}

void dialog_cooker::on_loaded(queries_result& result)
{
	LOGI_INFO("dialog_cooker", "dialog_cooker::on_loaded called");

	query_result_for_cook* parent = result.get_parent_query();

	if(result[0].is_successful())
	{
		configs::lua_config_ptr cfg = static_cast_resource_ptr<configs::lua_config_ptr>(result[0].get_unmanaged_resource());

		dialog* dlg = NEW(dialog)(cfg->get_root());

		string512 file_path;
		pcstr val = result[0].get_requested_path();
		for(int i=0; i<3; ++i)
			val = strings::get_token(val, file_path, strings::length(val), '/');

		val = strings::get_token(val, file_path, strings::length(val), '.');
		dlg->set_name(file_path);

		parent->set_unmanaged_resource(dlg, resources::nocache_memory, sizeof(dialog));
	}
	else
	{
		xray::configs::lua_config_ptr cfg = configs::create_lua_config(parent->get_requested_path());
		dialog* dlg = NEW(dialog)(cfg->get_root());
		dlg->set_name(parent->get_requested_path());
		parent->set_unmanaged_resource(dlg, resources::nocache_memory, sizeof(dialog));
	}
	parent->finish_query(result_success);
}

void dialog_cooker::delete_resource(resource_base* res)
{
	LOGI_INFO	("dialog_cooker", "dialog_cooker::delete_unmanaged called");
	DELETE		(res);
}
