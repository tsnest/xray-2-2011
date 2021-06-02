////////////////////////////////////////////////////////////////////////////
//	Created		: 24.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef LIGHTS_DB_H_INCLUDED
#define LIGHTS_DB_H_INCLUDED

#include "light.h"
#include <xray/render/base/light_props.h>

namespace xray {
namespace render {

struct editor_light
{
	editor_light( u32 id):
			id				(id)
			{}

	bool operator < ( editor_light const& other) const 
	{
		return id < other.id;
	}

	bool operator == ( u32 other) const 
	{
		return id == other;
	}

	u32			id;
	light_props	properties;
	ref_light	light;
};


class lights_db: public quasi_singleton<lights_db>
{
public:
	typedef render::vector<editor_light>				editor_lights;

public:
				lights_db				( );

	ref_light	get_sun					( )	{ return m_sun;}
	void		initialize_sun			( );
	light*		create					( );

	void		add_light				( u32 id, light_props const& props, bool beditor);
	void		update_light			( u32 id, light_props const& props, bool beditor);
	void		remove_light			( u32 id, bool beditor);

	editor_lights&		get_game_lights		() { return m_game_lights;}
	editor_lights&		get_editors_lights	() { return m_editor_lights;}

private:

	ref_light						m_sun;

	editor_lights					m_game_lights;
	editor_lights					m_editor_lights;

}; // class lights_db

} // namespace render 
} // namespace xray 


#endif // #ifndef LIGHTSDB_H_INCLUDED