////////////////////////////////////////////////////////////////////////////
//	Created		: 24.12.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TERRAIN_MODIFIER_COMMAND_H_INCLUDED
#define TERRAIN_MODIFIER_COMMAND_H_INCLUDED

#include "project_defines.h"
#include "terrain_quad.h"

namespace xray {
namespace editor {

class editor_world;
ref class terrain_modifier_control_base;
ref class terrain_core;
ref class terrain_selection_control;
ref class terrain_control_base;

public ref class terrain_modifier_command abstract:	public editor_base::command
{
public:
							terrain_modifier_command		( terrain_control_base^ control, terrain_core^ core );

	virtual bool			commit							( ) override;
	virtual void			rollback						( ) override;
	virtual bool			end_preview						( ) override;
	virtual void			start_preview					( ) override;
protected:
	virtual void			commit_terrain					( terrain_node_key , s8 ) {};

	terrain_core^					m_terrain_core;
	terrain_control_base^			m_control;
	TerrainNodesList				m_object_list;
	bool							m_b_in_preview;
	s8								m_k;
}; // class terrain_modifier_command

public ref class terrain_modifier_height_command : public terrain_modifier_command
{
typedef terrain_modifier_command	super;

public:
							terrain_modifier_height_command	( terrain_modifier_control_base^ control, terrain_core^ core);
	virtual					~terrain_modifier_height_command( );
	virtual bool			end_preview						( ) override;
	virtual void			rollback						( ) override;
			void			set_data						( terrain_node_key object_key, height_diff_dict^ diff_list);
protected:
	virtual void			commit_terrain					( terrain_node_key key, s8 k ) override;
	key_height_diff_dict	m_height_diff_list;
	System::Collections::Generic::List<xray::editor_base::command^>	m_transform_commands;
}; // terrain_modifier_height_command

public ref class terrain_modifier_color_command : public terrain_modifier_command
{
typedef terrain_modifier_command	super;

public:
							terrain_modifier_color_command	( terrain_modifier_control_base^ control, terrain_core^ core );
	virtual bool			end_preview						( ) override;
			void			set_data						( terrain_node_key object_key, vertex_dict^ list );
protected:
	virtual void			commit_terrain					( terrain_node_key key, s8 k ) override;
	key_vertex_dict			m_vertex_list;
}; // terrain_modifier_color_command


public ref class terrain_selector_command : public editor_base::command
{
typedef editor_base::command	super;

public:
							terrain_selector_command		( terrain_core^ core, key_vert_id_dict^ list, enum_selection_method method );
	virtual bool			commit							( ) override;
	virtual void			rollback						( ) override;
protected:
	key_vert_id_dict^		m_selected_quads_list;
	terrain_core^			m_terrain_core;
	enum_selection_method	m_method;
}; // terrain_modifier_color_command

} // namespace editor
} // namespace xray

#endif // #ifndef TERRAIN_MODIFIER_COMMAND_H_INCLUDED