////////////////////////////////////////////////////////////////////////////
//	Created		: 15.09.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "project_items.h"
#include "object_job.h"
#include "object_values_storage.h"
#include "job_behaviour_values_storage_default.h"

namespace xray {
namespace editor {


job_behaviour_values_storage_default::job_behaviour_values_storage_default( object_job^ parent_job )
	:job_behaviour_base(parent_job)
{
}

void job_behaviour_values_storage_default::load			( configs::lua_config_value const& config )
{
	job_behaviour_base::load(config);
}
void job_behaviour_values_storage_default::save			( configs::lua_config_value& config )
{
	job_behaviour_base::save(config);
}

void job_behaviour_values_storage_default::get_properties	( wpf_property_container^ to_container )
{
	job_behaviour_base::get_properties( to_container );
}

System::Collections::Generic::List<System::String^>^  job_behaviour_values_storage_default::get_available_events_list( )
{
	System::Collections::Generic::List<System::String^>^ ret_lst = job_behaviour_base::get_available_events_list( );

	if (  parent_job->selected_job_resource == nullptr )
		return ret_lst;

	object_values_storage^ selected_resource = safe_cast< object_values_storage^ >( parent_job->selected_job_resource->get_object() );

	ret_lst->AddRange( selected_resource->get_values_by_type("strings") )	;

	return ret_lst;
}



} // namespace editor
} // namespace xray