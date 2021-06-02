////////////////////////////////////////////////////////////////////////////
//	Created		: 08.09.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef JOB_BEHAVIOUR_COMPOSITE_DEFAULT_H_INCLUDED
#define JOB_BEHAVIOUR_COMPOSITE_DEFAULT_H_INCLUDED

#include "job_behaviour_base.h"
#include "object_values_storage.h"

namespace xray {
namespace editor {

ref class value_storage_property_value;
ref class object_values_storage;

ref class job_behaviour_composite_default : public job_behaviour_base {

	typedef xray::editor::wpf_controls::property_container				wpf_property_container;
	typedef xray::editor::wpf_controls::property_container_collection	wpf_property_container_collection;
	typedef System::Collections::Generic::List<value_storage_property_value^> value_storage_properties_list;

public:
	job_behaviour_composite_default( object_job^ parent_job );

public:
	virtual void	load			( configs::lua_config_value const& config ) override;
	virtual void	save			( configs::lua_config_value& config )		override;
	virtual void	get_properties	( wpf_property_container^ to_container )	override;
	virtual void	initialize		( )	override;

	virtual System::Collections::Generic::List<System::String^>^ get_available_events_list( ) override;

private:
	value_storage_properties_list^	m_value_storage_properties_list;

}; // class job_behaviour_composite_default

ref class project_item_base;

public ref class value_storage_property_value : wpf_controls::i_property_value
{
public:
	value_storage_property_value( object_values_storage^ parent )
	{
		m_parent = parent;
	}
	virtual void			set_value		(System::Object^ value);
	virtual System::Object	^get_value		();
	void					on_parent_loaded( project_item_base^ parent );

	System::Collections::Generic::IEnumerable<System::String^>^ get_values_list( System::String^ type );

	virtual property System::Type^			value_type
	{
		System::Type^ get();
	}

	property object_values_storage^			parent
	{
		object_values_storage^				get( ){ return m_parent; }
	}

	property System::String^				parent_name
	{
		void								set( System::String^ loaded_name ){ m_loaded_parent_name = loaded_name; }
		System::String^						get( ){ return ( m_parent != nullptr ) ? parent->get_name() : m_loaded_parent_name ; }
	}

	


private:

	System::String^				m_value;
	object_values_storage^		m_parent;
	System::String^				m_loaded_parent_name;

}; // class value_storage_property_value




} // namespace editor
} // namespace xray

#endif // #ifndef JOB_BEHAVIOUR_COMPOSITE_DEFAULT_H_INCLUDED