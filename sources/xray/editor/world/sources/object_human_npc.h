////////////////////////////////////////////////////////////////////////////
//	Created		: 30.03.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_HUMAN_NPC_H_INCLUDED
#define OBJECT_HUMAN_NPC_H_INCLUDED

#include "object_base.h"
#include "animated_model_instance.h"
#include "level_editor.h"
#include <xray/render/engine/base_classes.h>
#include <xray/animation/skeleton_animation.h>

using namespace System;

namespace xray {

namespace editor {

ref class tool_misc;

/*
public ref class model_name_ui_type_editor : public xray::editor::wpf_controls::property_editors::i_external_property_editor
{
public:
	virtual	void			run_editor	( xray::editor::wpf_controls::property_editors::property^ prop );
}; // ref class model_name_ui_type_editor
*/

public ref class animation_name_ui_type_editor : public xray::editor::wpf_controls::property_editors::i_external_property_editor
{
public:
	virtual	void			run_editor	( xray::editor::wpf_controls::property_editors::property^ prop );
}; // ref class animation_name_ui_type_editor

public ref class object_human_npc : public object_base
{
	typedef object_base	super;

public:
					object_human_npc	( tool_misc^ tool, render::scene_ptr const& scene );
	virtual			~object_human_npc	( );
	
	virtual void	destroy_collision	( )										override;
	virtual void	load_contents		( )										override;
	virtual void	unload_contents		( bool )								override;
	virtual	void	set_visible			( bool is_visible )						override;
	virtual	void	set_transform		( float4x4 const& transform )			override;
	virtual	void	on_selected			( bool selected )						override;
	virtual void	save				( configs::lua_config_value t )			override;
	virtual void	load_props			( configs::lua_config_value const& t )	override;
	virtual	void	render				( )										override;
	
	virtual wpf_controls::property_container^	get_property_container	( )		override;

private:
			void	on_model_loaded		( resources::queries_result& data );
			void	on_animation_loaded	( resources::queries_result& data );
			void	load_model			( System::String^ model_name );
			void	load_animation		( System::String^ animation_name );
			void	initialize_collision( );
			void	play_animation		( );

private:
	animated_model_instance_ptr*		m_model_instance;
	animation::skeleton_animation_ptr*	m_animation;
	//String^								m_model_name;
	String^								m_animated_model_name;
	String^								m_animation_name;
	execute_delegate^					m_onframe_delegate;
	render::scene_ptr*					m_scene;
	tool_misc^							m_parent_tool;
		
public:
	/*
	[DisplayNameAttribute( "npc model" ), DescriptionAttribute( "human npc animated model" ), CategoryAttribute( "visual" )]
	[DefaultValueAttribute( "" ), ReadOnlyAttribute( false )]
	[external_editor_attribute( model_name_ui_type_editor::typeid )]
	property System::String^		model
	{
		System::String^ 			get		( )
		{
			return					m_model_name;
		}
		
		void						set		( System::String^ model_name )
		{
			m_model_name			= model_name;
			load_model				( model_name );
			on_property_changed		("npc model");
		}
	}
	*/

	[DisplayNameAttribute( "idle state animation" ), DescriptionAttribute( "default animation for idle state" ), CategoryAttribute( "visual" )]
	[DefaultValueAttribute( "" ), ReadOnlyAttribute( false )]
	[external_editor_attribute( animation_name_ui_type_editor::typeid )]
	property System::String^		animation_idle
	{
		System::String^ 			get		( )
		{
			return					System::IO::Path::GetFileName( m_animation_name );
		}
		
		void						set		( System::String^ animation_name )
		{
			m_animation_name		= animation_name;
			load_animation			( animation_name );
			on_property_changed		( "idle state animation" );
		}
	}

	typedef xray::editor::wpf_controls::property_grid::refresh_grid_on_change_attribute refresh_grid_on_change_attribute;

	[DisplayNameAttribute( "animated model instance" ), DescriptionAttribute( "animated model instance for npc" ), CategoryAttribute( "visual" )]
	[refresh_grid_on_change_attribute]
	property System::String^			animated_model_filename
	{
		System::String^					get		( )
		{ 
			return						m_animated_model_name; 
		}
		
		void							set		( System::String^ model_filename )		
		{
			m_animated_model_name		= model_filename;
			
			if ( !System::String::IsNullOrEmpty( m_animated_model_name ) )
				load_model				( m_animated_model_name );
			
			on_property_changed			( "animated model instance" );
		}
	}
}; // class object_human_npc

} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_HUMAN_NPC_H_INCLUDED
