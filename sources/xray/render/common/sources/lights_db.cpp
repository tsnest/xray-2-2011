////////////////////////////////////////////////////////////////////////////
//	Created		: 24.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "lights_db.h"

namespace xray {
namespace render {


enum
{
	chunk_light_dynamic = 6
};

struct f_light 
{
public:
    u32           type;             /* Type of light source */
    math::color          diffuse;          /* Diffuse color of light */
    math::color          specular;         /* Specular color of light */
    math::color          ambient;          /* Ambient color of light */
    float3         position;         /* Position in world space */
    float3         direction;        /* Direction in world space */
    float		    range;            /* Cutoff range */
    float	        falloff;          /* Falloff */
    float	        attenuation0;     /* Constant attenuation */
    float	        attenuation1;     /* Linear attenuation */
    float	        attenuation2;     /* Quadratic attenuation */
    float	        theta;            /* Inner angle of spotlight cone */
    float	        phi;              /* Outer angle of spotlight cone */

	void set(u32 ltType, float x, float y, float z)
	{
		ZeroMemory( this, sizeof(f_light) );
		type=ltType;
		diffuse.set(1.0f, 1.0f, 1.0f, 1.0f);
		specular = (diffuse);
		position.set(x,y,z);
		direction.set(x,y,z);
		direction.normalize_safe(direction);
		range = sqrt(math::float_max);
	}
    
	//void mul(float brightness)
	//{
	//    diffuse.mul_rgb		(brightness);
 //   	ambient.mul_rgb		(brightness);
	//    specular.mul_rgb	(brightness);
 //   }
};

lights_db::lights_db()
{
	initialize_sun();
}

light* lights_db::create()
{
	light*	L			= NEW(light);
	L->flags.bStatic	= false;
	L->flags.bActive	= false;
	L->flags.bShadow	= true;

	return				L;
}

void lights_db::initialize_sun	()
{
	ASSERT( !m_sun );

	m_sun = create		();

	m_sun->flags.bStatic			= true;
	m_sun->set_type			(light_type_direct);
	m_sun->set_shadow		(true);
	m_sun->set_color		(math::color(0.89578169f, 0.88f, 0.87f, 1));
	//m_sun->set_rotation	(float3(0.47652841f, -0.80543172f, -0.35242066f).normalize(), float3());
	m_sun->set_orientation	(float3(-0.1f, -0.99f, -0.5f).normalize(), float3());
}

void lights_db::add_light( u32 id, light_props const& props, bool beditor )
{
	editor_light				light_to_add(id);
	editor_lights& list			= beditor ? m_editor_lights : m_game_lights;

	editor_lights::iterator it	= std::lower_bound( list.begin(), list.end(), light_to_add);
	ASSERT						( (it == list.end()) || (it->id != id) );

	light_to_add.light = create();
	light_to_add.light->set_type		( props.type);
	light_to_add.light->set_shadow		( props.shadow_cast);
	light_to_add.light->set_position	( props.transform.c.xyz());
	light_to_add.light->set_orientation	( props.transform.i.xyz(), props.transform.k.xyz());
	light_to_add.light->set_range		( props.range);
	light_to_add.light->set_color		( props.color);

	list.insert					( it, light_to_add );

}

void lights_db::update_light( u32 id, light_props const& props, bool beditor)
{
	editor_light				tmp_light( id);
	editor_lights& list			= beditor ? m_editor_lights : m_game_lights;

	vector<editor_light>::iterator	it	= std::lower_bound( list.begin(), list.end(), tmp_light);

	ASSERT( it != list.end() && it->id == id );

	it->light->set_type			( props.type);
	it->light->set_shadow		( props.shadow_cast);
	it->light->set_position		( props.transform.c.xyz());
	it->light->set_orientation	( props.transform.i.xyz(), props.transform.k.xyz());
	it->light->set_range		( props.range);
	it->light->set_color		( props.color);
}

void lights_db::remove_light	( u32 id, bool beditor)
{
	editor_lights& list			= beditor ? m_editor_lights : m_game_lights;
	editor_lights::iterator new_end = std::remove	( list.begin(), list.end(), id );	
	list.erase (new_end, list.end( ) );

	LOG_INFO	( "model_manager::remove_light(id = %d), list.size() == %d ", id, list.size() );
}

} // namespace render 
} // namespace xray 
