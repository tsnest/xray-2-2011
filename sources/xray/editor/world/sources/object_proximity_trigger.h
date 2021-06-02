////////////////////////////////////////////////////////////////////////////
//	Created		: 15.08.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_PROXIMITY_TRIGGER_H_INCLUDED
#define OBJECT_PROXIMITY_TRIGGER_H_INCLUDED

#include "object_base.h"

using namespace System;

namespace xray
{
	namespace editor
	{
		ref class tool_misc;
		ref class object_collision_geometry;

		using xray::editor::wpf_controls::property_container;

		using namespace System::ComponentModel;

		ref class object_proximity_trigger: object_base
		{
		private:
			

		public:
						object_proximity_trigger		( tool_misc^ t );
			virtual		~object_proximity_trigger		( );

		public:
			virtual		void				load_contents			( ) override;
			virtual		void				unload_contents			( Boolean bdestroy ) override;
			virtual		void				load_props				( configs::lua_config_value const& t ) override;
			virtual		void				save					( configs::lua_config_value t ) override;
			virtual		property_container^	get_property_container	( ) override;

		public:
			[DisplayNameAttribute("collision geometry"), CategoryAttribute("general")]
			property	String^			collision_geometry
			{
				String^			get( );
				private: void	set( String^ );
			}
			//[DisplayNameAttribute("max tracked objects count"), CategoryAttribute("general")]
			property	Int32			max_tracked_objects_count;

		private:
			void					select_collision_geometry	( wpf_controls::property_editors::property^ , Object^ );
			void					collision_deleting			( );
			void					collision_geometry_loaded	( project_item_base^ track );

		private:
			typedef object_base		super;

		private:
			project_item_base^		m_collision_geometry;

		}; // class object_proximity_trigger

	} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_PROXIMITY_TRIGGER_H_INCLUDED