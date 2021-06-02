////////////////////////////////////////////////////////////////////////////
//	Created		: 01.02.2011
//	Author		: Dmitriy Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_SOUND_H_INCLUDED
#define OBJECT_SOUND_H_INCLUDED

#include "object_base.h"
#include <xray/sound/sound.h>
#include <xray/sound/sound_emitter.h>
#include <xray/sound/world.h>
#include "level_editor.h"
#include "editor_world.h"


namespace xray{
namespace editor{

using namespace System;

using xray::editor::wpf_controls::control_containers::button;

ref class tool_sound;

typedef System::Action<button^> button_delegate;

struct callback_handler
{
	sound::command_result_enum on_finish_playing( )
	{
		LOG_DEBUG			( "on_finish_playing( )");
		return sound::command_result_executed;
	}
}; // struct callback_handler

public ref class object_sound : public object_base
{
	typedef object_base	super;
public:
							object_sound	( tool_sound^ tool, render::scene_ptr const& scene );
	virtual					~object_sound	( );
	virtual void			load_contents	( )										override;
	virtual	void			set_transform	( float4x4 const& transform )			override;
	virtual void			unload_contents	( bool )								override { }
	virtual wpf_controls::property_container^	get_property_container	( )			override;
	virtual void			render			( )										override;
	virtual	void			on_selected		( bool selected )						override;


private:
			void			initialize_collision( );
			void			on_sound_loaded	( resources::queries_result& data );
			void			load_sound		( );

			void			play_clicked		( button^ );
			void			play_looped_clicked	( button^ );
			void			play_once_clicked	( button^ );
			void			stop_clicked		( button^ );
			void			pause_clicked		( button^ );
			void			resume_clicked		( button^ );
			void			test_clicked		( button^ );
			void			fatality_clicked	( button^ );
			void			brutality_clicked	( button^ );
			void			stop_now_clicked	( button^ );

			double			min_seek_val		( ) { return 0.0f; }
			double			max_seek_val		( );

			void			emit				( );
			void			on_library_fs_iterator_ready	( vfs::vfs_locked_iterator const& fs_it );
			void			process_recursive_names			( vfs::vfs_iterator const& it, System::String^ path );

	typedef xray::editor::wpf_controls::property_editors::attributes::value_range_and_format_attribute	value_range_and_format_attribute;

	System::Collections::ArrayList^					m_file_names;

	callback_handler*								m_callback;

//	xray::sound::sound_ptr*							m_sound_instance_ptr;
	sound::sound_emitter_ptr*						m_sound_emitter_ptr;
	sound::sound_instance_proxy_ptr*				m_proxy;
	render::scene_ptr*								m_scene;
	System::String^ m_wav_file_name;
	tool_sound^		m_tool_sound;
	float3*			m_position;
	float3*			m_direction;
	float3*			m_up;

	float			m_seek_pos;
	int				m_sound_type;
	int				m_emitter_type;

	bool			m_loaded;
	bool			m_selected;
	bool			m_positional;
public:

	typedef xray::editor::wpf_controls::property_grid::refresh_grid_on_change_attribute					refresh_grid_on_change_attribute;

	[ DisplayNameAttribute("wav"), DescriptionAttribute("select wav file"), CategoryAttribute("sound") ]
	[ refresh_grid_on_change_attribute ]
	property System::String^			wav_filename{
		System::String^					get		( )
		{ 
			return m_wav_file_name; 
		}
		void							set		( System::String^ t )		
		{
			m_wav_file_name = t;
			if (!System::String::IsNullOrEmpty(m_wav_file_name))
			{
				load_sound();
			}
			on_property_changed	( "wav" );
		}
	}

	[ DisplayNameAttribute("type"), DescriptionAttribute("select sound type"), CategoryAttribute("sound") ]
	[ refresh_grid_on_change_attribute ]
	property int		calc	{
		int					get		( )
		{ 
			return m_sound_type; 
		}
		void				set		( int val )		
		{
			m_sound_type	= val;
			emit			( );
		}
	}

	[ DisplayNameAttribute("emitter_type"), DescriptionAttribute("select emitter type"), CategoryAttribute("sound") ]
	[ refresh_grid_on_change_attribute ]
	property int		emitter_type	{
		int					get		( )
		{ 
			return m_emitter_type; 
		}
		void				set		( int val )		
		{
			m_emitter_type	= val;
		}
	}


	//[DisplayNameAttribute("seek"), DescriptionAttribute("seek"), CategoryAttribute("sound")] 
	//property float			seek
	//{
	//	float				get		( )				{ return m_seek_pos; }
	//	void				set		( float v )		
	//	{ 
	//		m_seek_pos = v;
	//		if ( (*m_proxy).c_ptr() )
	//		{
	//			(*m_proxy)->seek( (u32) m_seek_pos * 1000);
	//			//(*m_proxy)->seek( 1000 );
	//			//(*m_proxy)->seek( 25000 );
	//			//(*m_proxy)->seek( 10000 );
	//			//(*m_proxy)->seek( 15000 );
	//			//(*m_proxy)->seek( 25000 );
	//			//(*m_proxy)->seek( 1000 );
	//		}
	//	}
	//}
}; // class object_sound

}// namespace editor
}// namespace xray

#endif // #ifndef OBJECT_SOUND_H_INCLUDED
