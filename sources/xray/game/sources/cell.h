////////////////////////////////////////////////////////////////////////////
//	Created		: 04.06.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef CELL_H_INCLUDED
#define CELL_H_INCLUDED

#include "object.h"
#include <xray/render/facade/model.h>
#include <xray/collision/collision_object.h>
#include <xray/render/engine/base_classes.h>
#include "game_material_visibility_parameters.h"

namespace xray{
namespace physics{
	class bt_rigid_body;
}
}
namespace stalker2 {

class game;
class cell_manager;

#define invalid_key_value 0x7fffffff

struct cell_key
{
	typedef fixed_string<16> fixed_string16;

						cell_key	( int kx=invalid_key_value, int kz=invalid_key_value ):x(kx),z(kz)	{}
	static cell_key		pick		( float3 const& point );
	static cell_key		parse		( pcstr key_str );
	fixed_string16		to_string	( ) const;
	bool				operator <	( cell_key const& other ) const		{return (x==other.x)?(z<other.z):(x<other.x); }
	bool				operator ==	( cell_key const& other ) const		{return (x==other.x) && (z==other.z); }
	bool				valid		( ) const {return x!=invalid_key_value && z!=invalid_key_value; }
	void				invalidate	( ) {x=invalid_key_value; z=invalid_key_value; }
	int x;
	int z;
}; // struct key

class game_cell :	public resources::unmanaged_resource,
					public boost::noncopyable
{
public:
						game_cell						( );
	void				load							( );
	void				unload							( );
	void				load_contents					( cell_manager* m );
	bool				contents_loaded					( ) const;
	
private:
	void				load_impl						( );
	void				unload_impl						( );
	void				load_terrain					( );
	void				unload_terrain					( );
	void				on_contents_loaded				( resources::queries_result& data );
	void				on_terrain_visual_ready			( resources::queries_result& data );
	typedef vector<game_object_ptr_>					object_list;
	object_list											m_objects;

	xray::render::terrain_model_ptr						m_terrain;
	collision::collision_object*						m_terrain_collision;
	physics::bt_rigid_body*								m_terrain_rigid_body;
	game_material_visibility_parameters					m_visibility_parameters;
	cell_manager*										m_cell_manager;

public:
	configs::binary_config_ptr	m_config;
	game*						m_game;
	bool						m_quality;
}; // class game_cell

typedef	xray::resources::resource_ptr<
			game_cell,
			resources::unmanaged_intrusive_base
		> game_cell_ptr;

typedef associative_vector<cell_key, game_cell_ptr, vector>		cell_container;

#ifndef MASTER_GOLD
struct cell_cooker_user_data
{
	cell_key					key;
	configs::lua_config_ptr		config;
};
#endif // #ifndef MASTER_GOLD

} // namespace stalker2

#endif // #ifndef CELL_H_INCLUDED
