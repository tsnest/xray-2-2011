////////////////////////////////////////////////////////////////////////////
//	Created		: 17.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef EDITOR_CONTROL_H_INCLUDED
#define EDITOR_CONTROL_H_INCLUDED

#pragma managed( push, off )
#	include <xray/render/engine/base_classes.h>
#pragma managed( pop )

namespace xray {
	namespace render {
		class render_output_window;
	}
}

#pragma make_public	(xray::render::render_output_window)

namespace xray {

namespace collision{ class object; class geometry; }

namespace editor_base {

class collision_object_geometry;
ref class editor_control_base;

public delegate void activate_control_event		( editor_control_base^ c );
public delegate void deactivate_control_event	( editor_control_base^ c );
public delegate void property_changed			( editor_control_base^ c );

public delegate editor_control_base^ active_control_getter ( );

public ref class editor_control_base abstract
{
public:

								editor_control_base		( ){ initialize(); }
			System::String^		id						( ) {return m_control_id;}

	virtual		void			activate				( bool b_activate );
	virtual		void			update					( ) = 0;

	virtual		void			start_input				( int mode, int button, collision::object const* coll, math::float3 const& pos );
	virtual		void			execute_input			( ) = 0;	
	virtual		void			end_input				( );

	virtual		void			draw					( render::scene_view_pointer scene_view, render::render_output_window_pointer output_window ) = 0;

	virtual		u32				acceptable_collision_type( ) = 0;
	
	virtual		void			load_settings					( RegistryKey^ key );
	virtual		void			save_settings					( RegistryKey^ key );
				
				void			subscribe_on_activated			( activate_control_event^ d );
				void			subscribe_on_deactivated		( deactivate_control_event^ d );
				void			subscribe_on_property_changed	( property_changed^ d );

	virtual		void			change_property					( System::String^ prop_name, float const dx, float const dy );
	virtual		void			pin								( bool ){};
	virtual		bool			has_properties					( ){return false;}
public:
	[System::ComponentModel::DisplayNameAttribute("name"), System::ComponentModel::DescriptionAttribute("tool name"), System::ComponentModel::CategoryAttribute("general")]
	[System::ComponentModel::ReadOnlyAttribute(true)]
	property System::String^ name{
		System::String^	get() {return id();}
	}
	[System::ComponentModel::DisplayNameAttribute("sensitivity"), System::ComponentModel::DescriptionAttribute("tool sensitivity"), System::ComponentModel::CategoryAttribute("general")]
	property float		sensitivity;

protected:
	virtual		void			initialize				( ) = 0;	
	virtual		void			destroy					( ) = 0;	
	virtual		void			set_mouse_sensivity		( float const ) {}
	event activate_control_event^		on_activated;
	event deactivate_control_event^		on_deactivated;
	event property_changed^				on_property_changed;

	System::String^				m_control_id;
}; // class editor_control_base

} //namespace editor 
} //namespace xray

#endif // #ifndef EDITOR_CONTROL_H_INCLUDED