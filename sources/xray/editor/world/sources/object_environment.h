////////////////////////////////////////////////////////////////////////////
//	Created		: 21.04.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_ENVIRONMENT_H_INCLUDED
#define OBJECT_ENVIRONMENT_H_INCLUDED

#include "object_base.h"
#include <xray/render/engine/base_classes.h>

namespace xray {
namespace editor {

ref class object_environment: public object_base
{
	typedef object_base super;
public:
					object_environment		( tool_base^ t, render::scene_view_ptr const& scene_view );
					~object_environment		( );
	
	virtual void	save					( configs::lua_config_value t )			override;
	virtual void	load_props				( configs::lua_config_value const& t )	override;
	virtual void	load_contents			( )										override;
	virtual void	unload_contents			( bool )								override;
	virtual wpf_controls::property_container^	get_property_container	( )			override;
	virtual bool			get_persistent			( ) override				{ return true;}

	[DisplayNameAttribute("post effect"), DescriptionAttribute("post effect material"), CategoryAttribute("general")] 
	property System::String^ material_name{
		System::String^		get	( )							{ return m_material_name;}
		void				set	( System::String^ value );
	}

	[DisplayNameAttribute("camera position"), DescriptionAttribute("camera position"), CategoryAttribute("general")] 
	property Float3		camera_position{
		Float3			get	( );
		void			set	( Float3 value );
	}

	[DisplayNameAttribute("camera direction"), DescriptionAttribute("camera direction"), CategoryAttribute("general")] 
	property Float3		camera_direction{
		Float3			get	( );
		void			set	( Float3 value );
	}


	void					requery_material			( );
	void					material_ready				( resources::queries_result& data );
	System::String^			m_material_name;
	render::scene_view_ptr*	m_scene_view;
}; // class object_environment

} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_ENVIRONMENT_H_INCLUDED
