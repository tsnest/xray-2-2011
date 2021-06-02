////////////////////////////////////////////////////////////////////////////
//	Created		: 10.02.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_LIGHT_H_INCLUDED
#define OBJECT_LIGHT_H_INCLUDED

#include "object.h"
#include "object_behaviour.h"
#include <xray/render/facade/game_renderer.h>
#include <xray/render/facade/light_props.h>

namespace stalker2 {

class object_light : public game_object_static, public object_controlled
{
	typedef game_object_static		super;
public:
					object_light			( game_world& w );
	virtual			~object_light			( );
	virtual void	load					( configs::binary_config_value const& t );
	virtual void	load_contents			( );
	virtual void	unload_contents			( );
			void	update_props			( );

			inline 	render::light_props& props		( ) { return m_props; }
	inline bool		current_state			( ) const { return m_current_state;}
			void	add_to_scene			( );
			void	remove_from_scene		( );
protected:
	render::light_props m_props;
	render::scene_ptr	m_scene;
	u32					m_light_id;
	bool				m_current_state;
}; // class object_light

typedef	intrusive_ptr<
			object_light,
			resources::unmanaged_intrusive_base,
			threading::single_threading_policy
		> object_light_ptr;

} // namespace stalker2

#endif // #ifndef OBJECT_LIGHT_H_INCLUDED