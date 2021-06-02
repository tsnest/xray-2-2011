////////////////////////////////////////////////////////////////////////////
//	Created		: 17.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef EDITOR_CONTROL_H_INCLUDED
#define EDITOR_CONTROL_H_INCLUDED


namespace xray {
namespace editor {

class collision_object;
interface class editor_ide;
ref class editor_control_base;

public delegate void activate_control_event (editor_control_base^ c);
public delegate void deactivate_control_event (editor_control_base^ c);
public delegate void property_changed(editor_control_base^ c);


public ref class editor_control_base abstract
{
public:
								editor_control_base		( editor_ide^ ide ):m_ide(ide){}
			System::String^		id						( ) {return m_control_id;}
	virtual		void			initialize				( ) = 0;	
	virtual		void			destroy					( ) = 0;	
	virtual		void			show					( bool show ) = 0;
	virtual		void			activate				( bool b_activate );
	virtual		void			update					( ) = 0;
	virtual		void			start_input				( );
	virtual		void			execute_input			( ) = 0;	
	virtual		void			end_input				( );
	virtual		void			draw					( math::float4x4  const& view_matrix ) = 0;
	virtual		void			set_draw_geomtry		( bool draw ) = 0;
	virtual		void			resize					( float size ) = 0;
	virtual		void			set_mode_modfiers		( bool plane_mode, bool free_mode ) = 0;
	virtual		void			collide					( collision_object const* object, float3  const& position ) = 0;
	virtual		u32				acceptable_collision_type( ) = 0;
	virtual		void			load_settings			(RegistryKey^ key);
	virtual		void			save_settings			(RegistryKey^ key);
				bool			get_active				( ) {return m_active;}
				void			subscribe_on_activated	( activate_control_event^ d);
				void			subscribe_on_deactivated( deactivate_control_event^ d);
				void			subscribe_on_property_changed(property_changed^ d);

	virtual		void			change_property			(System::String^ prop_name, float const value);
	virtual		void			pin						(bool){};
public:
	property System::String^ name{
		System::String^	get() {return id();}
	}
	property float		sensitivity;

protected:
	event activate_control_event^		on_activated;
	event deactivate_control_event^		on_deactivated;
	event property_changed^				on_property_changed;

	editor_ide^					m_ide;
	System::String^				m_control_id;
	bool						m_active;

}; // class editor_control_base

} //namespace editor 
} //namespace xray

#endif // #ifndef EDITOR_CONTROL_H_INCLUDED