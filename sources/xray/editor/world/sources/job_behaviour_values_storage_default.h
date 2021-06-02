////////////////////////////////////////////////////////////////////////////
//	Created		: 15.09.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef JOB_BEHAVIOUR_VALUES_STORAGE_DEFAULT_H_INCLUDED
#define JOB_BEHAVIOUR_VALUES_STORAGE_DEFAULT_H_INCLUDED
#include "job_behaviour_base.h"

namespace xray {
namespace editor {

ref class object_values_storage;

ref class job_behaviour_values_storage_default : public job_behaviour_base {

	typedef xray::editor::wpf_controls::property_container				wpf_property_container;
	typedef xray::editor::wpf_controls::property_container_collection	wpf_property_container_collection;


public:
	job_behaviour_values_storage_default( object_job^ parent_job );

public:
	virtual void	load			( configs::lua_config_value const& config ) override;
	virtual void	save			( configs::lua_config_value& config )		override;
	virtual void	get_properties	( wpf_property_container^ to_container )	override;

	virtual System::Collections::Generic::List<System::String^>^ get_available_events_list( ) override;

}; // class job_behaviour_values_storage_default

} // namespace editor
} // namespace xray

#endif // #ifndef JOB_BEHAVIOUR_VALUES_STORAGE_DEFAULT_H_INCLUDED