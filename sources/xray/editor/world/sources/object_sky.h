////////////////////////////////////////////////////////////////////////////
//	Created		: 22.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_SKY_H_INCLUDED
#define OBJECT_SKY_H_INCLUDED

#include "object_base.h"
#include <xray/render/engine/base_classes.h>

using namespace System;

namespace xray 
{
	namespace render { struct material_effects_instance_cook_data; }
	namespace editor {

		using		xray::editor::wpf_controls::property_container;
		typedef		xray::render::material_effects_instance_cook_data		effects_cook_data;

		ref class object_sky: public object_base
		{
			typedef object_base super;
		public:
										object_sky				( tool_base^ t, render::scene_ptr const& scene );
										~object_sky				( );

		public:
			[DisplayNameAttribute("sky material"), DescriptionAttribute("sky material"), CategoryAttribute("general")] 
			property	String^			sky_material
			{
				String^			get	( );
				void			set	( String^ value );
			}

		public:
			virtual void				save					( configs::lua_config_value t )			override;
			virtual void				load_props				( configs::lua_config_value const& t )	override;
			virtual void				load_contents			( )										override;
			virtual void				unload_contents			( bool )								override;
			virtual property_container^	get_property_container	( ) override;
			virtual bool				get_persistent			( ) override{ return true; }

			void						requery_material		( );
			void						material_ready			( resources::queries_result& data );
			
		private:
			effects_cook_data**		m_cook_data;
			String^					m_material_name;
			render::scene_ptr*		m_scene;

		}; // class object_sky

	} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_SKY_H_INCLUDED
