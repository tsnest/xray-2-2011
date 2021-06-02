////////////////////////////////////////////////////////////////////////////
//	Created		: 11.08.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef GAME_MATERIAL_VISIBILITY_PARAMETERS_H_INCLUDED
#define GAME_MATERIAL_VISIBILITY_PARAMETERS_H_INCLUDED

namespace stalker2 {

class game_material_visibility_parameters
{
public:	
	inline			game_material_visibility_parameters	( float transparency ) :
						m_transparency					( transparency )
	{
	}
	
	inline	float	get_transparency_value				( ) const { return m_transparency; }

private:
	float			m_transparency;
};

} // namespace stalker2

#endif // #ifndef GAME_MATERIAL_VISIBILITY_PARAMETERS_H_INCLUDED