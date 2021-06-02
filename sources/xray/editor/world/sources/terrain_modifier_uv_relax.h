////////////////////////////////////////////////////////////////////////////
//	Created		: 25.12.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TERRAIN_MODIFIER_UV_RELAX_H_INCLUDED
#define TERRAIN_MODIFIER_SMOOTH_H_INCLUDED

#include "terrain_modifier_painter.h"

namespace xray {
namespace editor {

public value class pull_vector
{
public:
	float x;
	float z;
};

typedef System::Collections::Generic::Dictionary<u16, pull_vector>						pull_vector_dict;
typedef System::Collections::Generic::Dictionary<terrain_node_key, pull_vector_dict^>	key_pull_vector_dict;

public ref class terrain_modifier_uv_relax : public terrain_modifier_painter_base
{
	typedef terrain_modifier_painter_base	super;
public:
								terrain_modifier_uv_relax	( level_editor^ le, tool_terrain^ tool);
	virtual						~terrain_modifier_uv_relax( );
	virtual		void			start_input				( int mode, int button, collision::object const* coll, math::float3 const& pos ) override;
	virtual		void			execute_input			( ) override;
	virtual		void			end_input				( ) override;
	virtual		void			do_work					( ) override;

protected:
	virtual		void			on_working_set_changed	( ) override;
				void			do_work1				( );
	key_pull_vector_dict		m_pull_vectors;
}; // class terrain_modifier_uv_relax

} // namespace editor
} // namespace xray


#endif // #ifndef TERRAIN_MODIFIER_SMOOTH_H_INCLUDED