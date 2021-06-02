////////////////////////////////////////////////////////////////////////////
//	Created		: 14.05.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef GUI_ACTIONS_H_INCLUDED
#define GUI_ACTIONS_H_INCLUDED

namespace xray {
namespace editor {

ref class window_ide;
ref class input_engine;
ref class level_editor;
class editor_world;

public ref class editor_world_action abstract: public editor_base::action_single
{
public:
					editor_world_action		( System::String^ name, editor_world* world );
protected:
	editor_world*	m_editor_world;
};

public ref class action_undo : public editor_world_action
{
	typedef editor_world_action	super;
public:
					action_undo				( System::String^ name, editor_world* world ):super(name, world){};
	
	virtual bool	do_it					() override;
	virtual bool	enabled					() override;
}; // class action_undo

public ref class action_redo : public editor_world_action
{
	typedef editor_world_action	super;
public:
					action_redo				( System::String^ name, editor_world* world ):super(name, world){};

	virtual bool	do_it					( ) override;
	virtual bool	enabled					( ) override;
}; // class action_redo

public ref class action_show_sound_editor : public editor_world_action
{
	typedef editor_world_action	super;
public:
					action_show_sound_editor( System::String^ name, editor_world* world ):super(name, world){};

	virtual bool	do_it					( ) override;
}; // class action_show_sound_editor

public ref class action_show_animation_editor : public editor_world_action
{
	typedef editor_world_action	super;
public:
					action_show_animation_editor( System::String^ name, editor_world* world):super(name, world){};

	virtual bool	do_it					( ) override;
}; // class action_show_animation_editor

public ref class action_show_dialog_editor : public editor_world_action
{
	typedef editor_world_action	super;
public:
					action_show_dialog_editor( System::String^ name, editor_world* world):super(name, world){};

	virtual bool	do_it					( ) override;
}; // class action_show_dialog_editor

public ref class action_show_particle_editor : public editor_world_action
{
	typedef editor_world_action	super;
public:
					action_show_particle_editor( System::String^ name, editor_world* world):super(name, world){};

	virtual bool	do_it					( ) override;
}; // class action_show_particle_editor

public ref class action_show_texture_editor : public editor_world_action
{
	typedef editor_world_action	super;
public:
					action_show_texture_editor( System::String^ name, editor_world* world):super(name, world){};

	virtual bool	do_it					( ) override;
}; // class action_show_texture_editor

public ref class action_show_material_editor : public editor_world_action
{
	typedef editor_world_action	super;

public:
					action_show_material_editor( System::String^ name, editor_world* world ):super(name, world){};

	virtual bool	do_it					( ) override;
}; // class action_show_material_editor

public ref class action_show_material_instance_editor : public editor_world_action
{
	typedef editor_world_action	super;

public:
					action_show_material_instance_editor( System::String^ name, editor_world* world ):super(name, world){};

	virtual bool	do_it					( ) override;
}; // class action_show_material_instance_editor

public ref class action_show_model_editor : public editor_world_action
{
	typedef editor_world_action	super;

public:
					action_show_model_editor( System::String^ name, editor_world* world):super(name, world){};

	virtual bool	do_it					( ) override;
}; // class action_show_model_editor

public ref class action_show_render_options_editor : public editor_world_action
{
	typedef editor_world_action	super;

public:
	action_show_render_options_editor( System::String^ name, editor_world* world ):super(name, world){};

	virtual bool	do_it					( ) override;
}; // class action_show_render_options_editor

public ref class action_pause : public editor_world_action
{
	typedef editor_world_action	super;
public:
					action_pause			( System::String^ name, editor_world* world):super(name, world){};

	virtual bool	do_it					( ) override;
	virtual editor_base::checked_state checked_state () override;
}; // class action_pause

public ref class action_reload_shaders : public editor_world_action
{
	typedef editor_world_action	super;
public:
	action_reload_shaders			(System::String^ name, editor_world* world):super(name, world){};

	virtual bool	do_it			( ) override;
}; // class action_reload_shaders

public ref class action_reload_modified_textures : public editor_world_action
{
	typedef editor_world_action	super;
public:
	action_reload_modified_textures		(System::String^ name, editor_world* world):super(name, world){};

	virtual bool	do_it				( ) override;
}; // class reload_modified_textures

public ref class action_terrain_debug_mode : public editor_world_action
{
	typedef editor_world_action	super;
public:
	action_terrain_debug_mode			(System::String^ name, editor_world* world):super(name, world), current_state(false){};

	virtual bool	do_it					( ) override;
	virtual editor_base::checked_state checked_state () override;
	bool current_state;
}; // class action_terrain_debug_mode

public ref class action_show_render_statistics : public editor_world_action
{
	typedef editor_world_action	super;
public:
	action_show_render_statistics			(System::String^ name, editor_world* world):super(name, world), current_state(false){};
	
	virtual bool	do_it					( ) override;
	virtual editor_base::checked_state checked_state () override;
	bool current_state;
}; // class action_show_render_statistics

public ref class action_editor_mode : public editor_world_action
{
	typedef editor_world_action	super;
public:
					action_editor_mode		(System::String^ name, editor_world* world):super(name, world){};

	virtual bool	do_it					() override;
	virtual editor_base::checked_state checked_state () override;
}; // class action_pause

public ref class action_focus : public editor_base::action_single
{
	typedef editor_base::action_single super;
public:
	action_focus( System::String^ name, level_editor^ le, bool grab_pivot_only);

	virtual bool	do_it					( ) override;
	virtual bool	enabled					( ) override;

private:
			void	do_focus_selection		( );
			void	do_focus_pick_collision	( );

	level_editor^	m_level_editor;
	float			m_halo_factor;
	bool			m_grab_pivot_only;
}; // class action_focus

public ref class action_select_editor_control :public  editor_base::action_single
{
	typedef editor_base::action_single	super;
public:
							action_select_editor_control ( System::String^ name, level_editor^ le, System::String^ control_id );

	virtual bool			do_it						( ) override;
	virtual editor_base::checked_state	checked_state	( ) override;

private:
	level_editor^			m_custom_editor;
	System::String^			m_control_id;

}; // class action_select_editor_control

public ref class action_drop_objects : public editor_base::action_single
{
	typedef editor_base::action_single	super;
public:
	action_drop_objects ( System::String^ name, level_editor^ le );

	virtual bool	do_it	( ) override;
	virtual bool	enabled	( ) override;

private:
	level_editor^			m_level_editor;
}; // class action_drop_objects

public ref class action_sound_debug_stream_writer : public editor_base::action_single
{
	typedef editor_base::action_single	super;
public:
										action_sound_debug_stream_writer ( System::String^ name, level_editor^ le );

	virtual bool						do_it			( ) override;
	virtual editor_base::checked_state	checked_state	( ) override;
private:
	level_editor^			m_level_editor;
	bool					m_current_state;
};

public ref class action_sound_debug_stream_writer_dump : public editor_base::action_single
{
	typedef editor_base::action_single	super;
public:
										action_sound_debug_stream_writer_dump	( System::String^ name, level_editor^ le );

	virtual bool						do_it									( ) override;
	virtual bool						enabled									( )	override;
private:
	level_editor^			m_level_editor;
};

} // namespace editor	
} // namespace xray

#endif // #ifndef GUI_ACTIONS_H_INCLUDED