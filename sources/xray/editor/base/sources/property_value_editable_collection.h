////////////////////////////////////////////////////////////////////////////
//	Created		: 31.08.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_PROPERTY_COLLECTION_HOLDER_H_INCLUDED
#define PARTICLE_PROPERTY_COLLECTION_HOLDER_H_INCLUDED

#include "property_value_base.h"

namespace xray {
namespace editor_base {	

ref class properties_holder;
	
public ref class property_value_editable_collection: property_value_base {

public: 
		property_value_editable_collection(	properties_holder^ holder,
											xray::editor::wpf_controls::property_container_base^	container,
											configs::lua_config_value const&	object_properties_config,
											configs::lua_config_value const&	properties_config,
											configs::lua_config_value const&	defaults_config,
											System::String^						property_path);
		!property_value_editable_collection()
		{
			CRT_DELETE(m_properties_config);
			CRT_DELETE(m_defaults_config);
			CRT_DELETE(m_node_properties_config);
		}
		~property_value_editable_collection()
		{
			this->!property_value_editable_collection();
		}

	private:
		editor::wpf_controls::property_container_base^				m_container;
		properties_holder^						m_properties_holder;
		configs::lua_config_value*				m_node_properties_config;
		configs::lua_config_value*				m_properties_config;
		configs::lua_config_value*				m_defaults_config;
		System::String^							m_property_path;

	public:
		bool add_property		( );
		bool remove_property	( System::Object^  property_id);
		bool move_property		( System::Int32 property_id, System::String^ order );

		virtual property on_property_value_changed^ on_property_value_changed;
	};//class property_collection_holder


} // namespace editor_base
} // namespace xray

#endif // #ifndef PARTICLE_PROPERTY_COLLECTION_HOLDER_H_INCLUDED