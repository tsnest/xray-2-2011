////////////////////////////////////////////////////////////////////////////
//	Created		: 06.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SAMPLER_SLOT_H_INCLUDED
#define SAMPLER_SLOT_H_INCLUDED

#include <xray/render/core/res_sampler_state.h>
#include <xray/render/core/res_common.h>

namespace xray {
namespace render {

typedef res_sampler_state *			ref_sampler_state;

class sampler_slot
{
public:
	sampler_slot	( ): slot_id( enum_slot_ind_null), state( NULL)	{ }
	sampler_slot	( name_string_type const& name, u32 slot_id, res_sampler_state* state): name( name), slot_id( slot_id), state( state)	{ }

// 	name_string_type const&	name() const		{ return name;}
// 	u32						slot_id() const		{ return slot_id;}

public:
	name_string_type		name;
	u32						slot_id;
	ref_sampler_state		state;
}; // class sampler_slot

//typedef		sampler_slot		(sampler_slots)	[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT];
typedef		fixed_vector<sampler_slot,D3D_COMMONSHADER_SAMPLER_SLOT_COUNT>	sampler_slots;

} // namespace render
} // namespace xray

#endif // #ifndef SAMPLER_SLOT_H_INCLUDED