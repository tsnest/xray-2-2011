////////////////////////////////////////////////////////////////////////////
//	Created		: 19.10.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "behaviour_editor.h"
#include "behaviour_editor_form.h"

namespace xray{
namespace editor{

behaviour_editor::behaviour_editor()
{
	m_form = gcnew behaviour_editor_form;
	
	load_behaviours_library( );
}

void behaviour_editor::load_behaviours_library			( )
{
	query_result_delegate* rq = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &behaviour_editor::on_behaviours_library_loaded), g_allocator);
	xray::resources::query_resource(
		"resources/library/behaviours/behaviours_library.library",
		xray::resources::lua_config_class,
		boost::bind(&query_result_delegate::callback, rq, _1),
		g_allocator
		);
}

void behaviour_editor::on_behaviours_library_loaded		( xray::resources::queries_result& data )
{
	R_ASSERT(data.is_successful());
	configs::lua_config_ptr config	= static_cast_checked<configs::lua_config*>(data[0].get_unmanaged_resource().c_ptr());
	
	configs::lua_config_iterator it		= config->get_root()["behaviours"].begin( );
	configs::lua_config_iterator it_e	= config->get_root()["behaviours"].end	( );

	for ( ; it != it_e; ++it )
	{
// 		configs::lua_config_value current	= *it;
// 		pcstr behaviour_guid				= it.key( ); 

	}
}

}
}