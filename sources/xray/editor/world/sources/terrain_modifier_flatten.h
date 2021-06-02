////////////////////////////////////////////////////////////////////////////
//	Created		: 26.12.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TERRAIN_MODIFIER_FLATTEN_H_INCLUDED
#define TERRAIN_MODIFIER_FLATTEN_H_INCLUDED

#include "terrain_modifier_control.h"

namespace xray {
namespace editor {

// result_height = strength
public ref class terrain_modifier_flatten : public terrain_modifier_height
{
	typedef terrain_modifier_height	super;
public:
								terrain_modifier_flatten	( level_editor^ le, tool_terrain^ tool );
	virtual						~terrain_modifier_flatten	( );
	virtual		void			start_input				( int mode, int button, collision::object const* coll, math::float3 const& pos )	override;
	virtual		void			execute_input				( ) override;
	virtual		void			do_work						( ) override;
	virtual		void			change_property				( System::String^ prop_name, float const dx, float const dy ) override;

protected:
	timing::timer*		m_timer;
}; // class terrain_modifier_flatten

} // namespace editor
} // namespace xray


#endif // #ifndef TERRAIN_MODIFIER_FLATTEN_H_INCLUDED