////////////////////////////////////////////////////////////////////////////
//	Created		: 25.03.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_POSITION_CALCULATOR_H_INCLUDED
#define SOUND_POSITION_CALCULATOR_H_INCLUDED

#include <xray/render/facade/model.h>

namespace stalker2 {

class sound_position_calculator
{
public:
	sound_position_calculator	( render::skeleton_model_ptr const& model );

	float3	get_position		( pcstr bone_name, float3 offset ) const;

private:
	render::skeleton_model_ptr	m_skeleton_model;
}; // class sound_position_calculator

} // namespace stalker2

#endif // #ifndef SOUND_POSITION_CALCULATOR_H_INCLUDED