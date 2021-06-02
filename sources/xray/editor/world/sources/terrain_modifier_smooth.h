////////////////////////////////////////////////////////////////////////////
//	Created		: 25.12.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TERRAIN_MODIFIER_SMOOTH_H_INCLUDED
#define TERRAIN_MODIFIER_SMOOTH_H_INCLUDED

#include "terrain_modifier_control.h"

namespace xray {
namespace editor {

public ref class terrain_modifier_smooth : public terrain_modifier_height
{
	typedef terrain_modifier_height	super;
public:
								terrain_modifier_smooth	( level_editor^ le, tool_terrain^ tool);
	virtual						~terrain_modifier_smooth( );
	virtual		void			start_input				( int mode, int button, collision::object const* coll, math::float3 const& pos ) override;
	virtual		void			execute_input			( ) override;
	virtual		void			do_work					( ) override;
}; // class terrain_modifier_smooth

} // namespace editor
} // namespace xray


#endif // #ifndef TERRAIN_MODIFIER_SMOOTH_H_INCLUDED