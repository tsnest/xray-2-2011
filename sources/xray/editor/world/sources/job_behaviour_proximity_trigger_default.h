////////////////////////////////////////////////////////////////////////////
//	Created		: 16.08.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef JOB_BEHAVIOUR_PROXIMITY_TRIGGER_DEFAULT_H_INCLUDED
#define JOB_BEHAVIOUR_PROXIMITY_TRIGGER_DEFAULT_H_INCLUDED

#include "job_behaviour_base.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections::Generic;
using xray::editor::wpf_controls::property_editors::attributes::combo_box_items_attribute;
using xray::editor::wpf_controls::hypergraph::node_property_attribute;

namespace xray 
{
	namespace editor 
	{
		ref class object_job;
		ref class project_item_base;
		ref class object_collision_geometry;

		ref class job_behaviour_proximity_trigger_default: job_behaviour_base
		{
			typedef xray::editor::wpf_controls::property_container	wpf_property_container;

		public:
			job_behaviour_proximity_trigger_default					( object_job^ parent_job );

		public:
			[CategoryAttribute( "FFF" ), DisplayNameAttribute( "Testees Source" )]
			[combo_box_items_attribute( "HUD"/*, "Debug"*/ ), node_property_attribute( false, false )]
			property	String^			testees_source;

		public:
			virtual		void			load						( configs::lua_config_value const& config ) override;
			virtual		void			save						( configs::lua_config_value& config ) override;

			virtual		List<String^>^	get_available_events_list	( ) override;
			virtual		void			get_properties				( wpf_property_container^ to_container ) override;

		private:
			project_item_base^		m_collision_geometry;

		}; // class job_behaviour_proximity_trigger_default

	} // namespace editor
} // namespace xray

#endif // #ifndef JOB_BEHAVIOUR_PROXIMITY_TRIGGER_DEFAULT_H_INCLUDED