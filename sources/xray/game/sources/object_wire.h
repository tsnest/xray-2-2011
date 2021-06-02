////////////////////////////////////////////////////////////////////////////
//	Created		: 31.10.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_WIRE_H_INCLUDED
#define OBJECT_WIRE_H_INCLUDED

#include "object.h"
#include <xray/render/facade/model.h>

namespace stalker2{

class object_wire :public game_object_static
{
	typedef game_object_static				super;
public:
					object_wire				( game_world& w );
	virtual			~object_wire			( );
	virtual void	load					( configs::binary_config_value const& t );
	virtual void	load_contents			( );
	virtual void	unload_contents			( );

protected:
			void	resources_ready			( resources::queries_result& data );

	float3* 								m_points;
	u32										m_points_count;
	float									m_wire_width;
	xray::render::render_model_instance_ptr	m_visual;

}; // class object_sky

} // namespace stalker2

#endif // #ifndef OBJECT_WIRE_H_INCLUDED