////////////////////////////////////////////////////////////////////////////
//	Created		: 22.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef STALKER2_OBJECT_SKY_H_INCLUDED
#define STALKER2_OBJECT_SKY_H_INCLUDED

#include "object.h"
#include <xray/render/facade/game_renderer.h>

namespace xray {
	namespace render {
		struct material_effects_instance_cook_data;
	} // namespace render
} // namespace xray

namespace stalker2{

class object_sky :public game_object_static
{
	typedef game_object_static				super;
public:
					object_sky				( game_world& w );
	virtual			~object_sky				( );
	virtual void	load					( configs::binary_config_value const& t );
	virtual void	load_contents			( );
	virtual void	unload_contents			( );
			void	material_ready			( resources::queries_result& data, xray::render::material_effects_instance_cook_data* cook_data );
}; // class object_sky

} // namespace stalker2

#endif // #ifndef STALKER2_OBJECT_SKY_H_INCLUDED