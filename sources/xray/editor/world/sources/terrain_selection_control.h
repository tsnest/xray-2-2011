////////////////////////////////////////////////////////////////////////////
//	Created		: 25.08.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TERRAIN_SELECTION_CONTROL_H_INCLUDED
#define TERRAIN_SELECTION_CONTROL_H_INCLUDED

#include "terrain_modifier_control.h"
#include "terrain_quad.h"

namespace xray {
namespace editor {

ref class level_editor;
ref class tool_terrain;

public ref class terrain_selection_control : public terrain_control_base
{
	typedef terrain_control_base super;
public:
								terrain_selection_control	( level_editor^ le, tool_terrain^ tool );
	virtual						~terrain_selection_control	( );
	virtual		void			start_input				( int mode, int button, collision::object const* coll, math::float3 const& pos )	override;
	virtual		void			execute_input			( ) 						override;
	virtual		void			end_input				( ) 						override;
	virtual		void			do_work					( ) 						override;
	virtual		void			load_settings			( RegistryKey^ key )		override;
	virtual		void			save_settings			( RegistryKey^ key )		override;
	virtual		void			activate				( bool b_activate )			override;

protected:
	virtual		void			create_command				( )	override;
	virtual		void			create_cursor				( render::scene_ptr const& scene )	override;
	virtual		void			refresh_active_working_set	( modifier_shape type ) override;

	System::Drawing::Point		m_last_screen_xy;
	key_vert_id_dict			m_processed_quad_id_list;
};

} // namespace editor
} // namespace xray

#endif // #ifndef TERRAIN_SELECTION_CONTROL_H_INCLUDED