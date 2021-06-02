////////////////////////////////////////////////////////////////////////////
//	Created		: 09.09.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VOLUMETRIC_SOUND_H_INCLUDED
#define VOLUMETRIC_SOUND_H_INCLUDED

#include "object_base.h"
#include <xray/sound/sound_emitter.h>

namespace xray {
namespace editor {

using namespace System;
//using System::Windows::Controls::Button;
using xray::editor::wpf_controls::control_containers::button;

ref class tool_sound;

typedef System::Action<button^> button_delegate;


ref class tool_sound;
ref class object_collision_geometry;
using xray::editor::wpf_controls::property_container;


public ref class volumetric_sound : public object_base
{
typedef object_base	super;
	public:
								volumetric_sound		( tool_sound^ tool, render::scene_ptr const& scene );
	virtual						~volumetric_sound		( );
	virtual void				load_contents			( ) override;
	virtual void				load_props				( configs::lua_config_value const& config_value ) override;
	virtual void				unload_contents			( bool ) override { }
	virtual	void				save					( configs::lua_config_value t ) override;
	virtual	property_container^	get_property_container	( )	override;
	virtual	void				on_selected				( bool selected ) override;
			void				play_looped_clicked		( button^ );
			void				on_sound_loaded	( resources::queries_result& data );
			void				on_config_loaded( resources::queries_result& data );
			void				load_sound		( );
public:
	typedef xray::editor::wpf_controls::property_grid::refresh_grid_on_change_attribute					refresh_grid_on_change_attribute;

	[DisplayNameAttribute("collision geometry"), CategoryAttribute("general")]
	property	String^			collision_geometry
	{
		String^			get( );
		private: void	set( String^ );
	}

	[ DisplayNameAttribute("wav"), DescriptionAttribute("select wav file"), CategoryAttribute("sound") ]
	[ refresh_grid_on_change_attribute ]
	property System::String^			wav_filename{
		System::String^					get		( )
		{ 
			return m_volumetric_config_file_name; 
		}
		void							set		( System::String^ t )		
		{
			m_volumetric_config_file_name = t;
			if (!System::String::IsNullOrEmpty(m_volumetric_config_file_name))
			{
				load_sound();
			}
			on_property_changed	( "wav" );
		}
	}

	[ DisplayNameAttribute("radius"), DescriptionAttribute("positional to stereo transition radius"), CategoryAttribute("sound") ]
	[ refresh_grid_on_change_attribute ]
	property float			radius	{
		float							get		( )
		{ 
			return m_radius; 
		}
		void							set		( float t )		
		{
			m_radius = t;
			update_radius				( );
			on_property_changed			( "radius" );
		}
	}
private:
			void				select_collision_geometry	( wpf_controls::property_editors::property^ , Object^ );
			void				collision_geometry_loaded	( project_item_base^ geom );
			void				update_radius				( );
			void				collision_deleting			( );
private:

	sound::sound_emitter_ptr*						m_sound_emitter_ptr;
	sound::sound_instance_proxy_ptr*				m_proxy;
	render::scene_ptr*								m_scene;
	tool_sound^										m_tool_sound;
	project_item_base^								m_collision_geometry;
	System::String^									m_volumetric_config_file_name;
	System::String^									m_sound_file_name;
	float											m_radius;
}; // class volumetric_sound

} // namespace editor
} // namespace xray

#endif // #ifndef VOLUMETRIC_SOUND_H_INCLUDED