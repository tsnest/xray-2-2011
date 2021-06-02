////////////////////////////////////////////////////////////////////////////
//	Created		: 10.05.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_ENVIRONMENT_H_INCLUDED
#define OBJECT_ENVIRONMENT_H_INCLUDED

#include "object.h"
#include <xray/render/facade/game_renderer.h>

namespace stalker2{

class object_environment :public game_object_static
{
	typedef game_object_static				super;
public:
					object_environment		( game_world& w );
	virtual			~object_environment		( );
	virtual void	load					( configs::binary_config_value const& t );
	virtual void	load_contents			( );
	virtual void	unload_contents			( );
			void	requery_material		( ) {}
			void	material_ready			( resources::queries_result& data );
	render::scene_view_ptr					m_scene_view;
}; // class object_environment

} // namespace stalker2

#endif // #ifndef OBJECT_ENVIRONMENT_H_INCLUDED