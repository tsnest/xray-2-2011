////////////////////////////////////////////////////////////////////////////
//	Created		: 22.11.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "ai_world.h"
#include <xray/ai/npc.h>
#include <xray/ai/collision_object.h>
#include "brain_unit_cook.h"
#include "behaviour_cook.h"
#include <xray/ai/npc_statistics.h>

namespace xray {
namespace ai {

namespace planning {
	void planner_test_2_actions	( );
	void planner_test			( );
	void planner_test_N			( );
	void blocks_world_test		( );
	void towers_of_hanoi_test	( );
} // namespace planning

ai_world::ai_world	( engine& engine ) :
	m_engine		( engine )
{
	m_npc_lives_timer.start();
	register_cooks	( );
//	planning::planner_test_2_actions( );
//	planning::planner_test			( );
//	planning::planner_test_N		( );	
//	planning::blocks_world_test		( );
//	planning::towers_of_hanoi_test	( );
}

ai_world::~ai_world	( )
{
	clear_dictionary( );
}

void ai_world::register_cooks		( )
{
	static brain_unit_cook			s_brain_unit_cook( this );
	static behaviour_cook			s_behaviour_cook( this );
	
	resources::register_cook		( &s_brain_unit_cook );
	resources::register_cook		( &s_behaviour_cook );
}

struct tick_brain_unit_predicate : private boost::noncopyable
{
	inline void operator()			( brain_unit* brain_unit ) const
	{
		brain_unit->tick			( );
	}
};

void ai_world::tick	( )
{
	m_brain_units.for_each			( tick_brain_unit_predicate() );
}

void ai_world::draw_ray	( float3 const& start_point, float3 const& end_point, bool sees_something ) const
{
	m_engine.draw_ray	( start_point, end_point, sees_something );
}

void ai_world::draw_frustum		(
		float fov_in_radians,
		float far_plane_distance,
		float aspect_ratio,
		float3 const& position,
		float3 const& direction,
		math::color color
	) const
{
	m_engine.draw_frustum		(
		fov_in_radians,
		far_plane_distance,
		aspect_ratio,
		position,
		direction,
		color
	);
}

void ai_world::add_brain_unit	( brain_unit_res_ptr brain )
{
	brain_unit_ptr brain_unit	= static_cast_resource_ptr< brain_unit_ptr >( brain );
	m_brain_units.push_back		( brain_unit.c_ptr() );
}

void ai_world::on_sound_event	( npc& npc, sensed_sound_object const& perceived_sound ) const
{
	m_sounds_subscriptions_manager.on_event( npc, perceived_sound );
}

void ai_world::on_hit_event		( npc& npc, sensed_hit_object const& perceived_hit ) const
{
	m_damage_subscriptions_manager.on_event( npc, perceived_hit );
}

void ai_world::on_destruction_event					( game_object const& object_to_be_destroyed ) const
{
	m_destruction_subscriptions_manager.on_event	( object_to_be_destroyed );
}

void ai_world::remove_brain_unit( brain_unit_res_ptr brain )
{
	brain_unit_ptr brain_unit	= static_cast_resource_ptr< brain_unit_ptr >( brain );
	brain_unit->clear_resources	( );
	m_brain_units.erase			( brain_unit.c_ptr() );
}

void ai_world::clear_resources			( )
{
}

void ai_world::get_colliding_objects	( math::aabb const& query_aabb, ai_objects_type& results )
{
	m_engine.get_colliding_objects		( query_aabb, results );
}

void ai_world::get_visible_objects		( math::cuboid const& cuboid, update_frustum_callback_type const& update_callback )
{
	m_engine.get_visible_objects		( cuboid, update_callback );
}

bool ai_world::ray_query				(
		collision_object const* const object_to_pick,
		collision_object const* const object_to_ignore,
		float3 const& origin,
		float3 const& direction,
		float const max_distance,
		float const transparency_threshold,
		float& visibility_value
	) const
{
	return m_engine.ray_query			(
				object_to_pick,
				object_to_ignore,
				origin,
				direction,
				max_distance,
				transparency_threshold,
				visibility_value
			);
}

void ai_world::set_ignore_filter		(
		brain_unit_res_ptr brain,
		ignorable_game_object const* begin,
		ignorable_game_object const* end
	)
{
	brain_unit_ptr brain_unit			= static_cast_resource_ptr< brain_unit_ptr >( brain );
	brain_unit->set_filter				( begin, end );
}

void ai_world::clear_ignore_filter		( brain_unit_res_ptr brain )
{
	brain_unit_ptr brain_unit			= static_cast_resource_ptr< brain_unit_ptr >( brain );
	brain_unit->set_filter				( 0, 0 );
}

static pstr clone_string_from_config	( configs::binary_config_value const& prototype )
{
	pcstr object_name					= (pcstr)prototype;
	u32 const length					= strings::length( object_name ) + 1;
	pstr clone							= XRAY_NEW_ARRAY_IMPL( g_allocator, char, length );
	strings::copy						( clone, length, object_name );
	return								clone;
}

static void fill_objects_names			( configs::binary_config_value const& dictionary, ai_world::available_objects_types& objects )
{
	configs::binary_config_value::const_iterator it		= dictionary.begin();
	configs::binary_config_value::const_iterator it_end = dictionary.end();

	for ( ; it != it_end; ++it )
		objects.push_back				( clone_string_from_config( *it ) );
}

static void fill_characters				( configs::binary_config_value const& dictionary, ai_world::available_characters_types& objects )
{
	configs::binary_config_value::const_iterator it		= dictionary.begin();
	configs::binary_config_value::const_iterator it_end = dictionary.end();

	for ( ; it != it_end; ++it )
	{
		configs::binary_config_value const& value		= *it;
		objects.push_back				( std::make_pair( clone_string_from_config( value["name"] ), value["id"] ) );
	}
}

static void clear_objects_names			( ai_world::available_objects_types& objects )
{
 	for ( int i = objects.size() - 1; i >= 0; --i )
 		XRAY_DELETE_ARRAY_IMPL			( g_allocator, objects[i] );
}

static void clear_characters			( ai_world::available_characters_types& objects )
{
 	for ( int i = objects.size() - 1; i >= 0; --i )
 		XRAY_DELETE_ARRAY_IMPL			( g_allocator, objects[i].first );
}

void ai_world::clear_dictionary			( )
{
	clear_characters					( m_npc_characters );
	clear_objects_names					( m_energy_weapons );
	clear_objects_names					( m_light_weapons );
	clear_objects_names					( m_heavy_weapons );
	clear_objects_names					( m_sniper_weapons );
	clear_objects_names					( m_melee_weapons );
	clear_objects_names					( m_npc_outfits );
	clear_objects_names					( m_npc_classes );
	clear_objects_names					( m_npc_groups );
} 

void ai_world::fill_objects_dictionary	( configs::binary_config_value const& dictionary )
{
	if ( dictionary.value_exists( "characters" ) )
		fill_characters					( dictionary["characters"], m_npc_characters );
	
	if ( dictionary.value_exists( "groups" ) )
		fill_objects_names				( dictionary["groups"], m_npc_groups );

	if ( dictionary.value_exists( "classes" ) )
		fill_objects_names				( dictionary["classes"], m_npc_classes );

	if ( dictionary.value_exists( "outfits" ) )
		fill_objects_names				( dictionary["outfits"], m_npc_outfits );

	if ( dictionary.value_exists( "melee_weapons" ) )
		fill_objects_names				( dictionary["melee_weapons"], m_melee_weapons );

	if ( dictionary.value_exists( "sniper_weapons" ) )
		fill_objects_names				( dictionary["sniper_weapons"], m_sniper_weapons );

	if ( dictionary.value_exists( "heavy_weapons" ) )
		fill_objects_names				( dictionary["heavy_weapons"], m_heavy_weapons );

	if ( dictionary.value_exists( "light_weapons" ) )
		fill_objects_names				( dictionary["light_weapons"], m_light_weapons );

	if ( dictionary.value_exists( "energy_weapons" ) )
		fill_objects_names				( dictionary["energy_weapons"], m_energy_weapons );
}

static u32 get_id_by_name	( ai_world::available_objects_types const& objects, pcstr name )
{
	for ( u32 i = 0; i < objects.size(); ++i )
		if ( strings::equal( objects[i], name ) )
			return			i;

	LOG_ERROR				( "object %s is absent in dictionary", name );
	UNREACHABLE_CODE		( return u32(-1) );
}

u32 ai_world::get_group_id_by_name	( pcstr group_name ) const
{
	return get_id_by_name			( m_npc_groups, group_name );
}

u32 ai_world::get_character_id_by_name	( pcstr character_name ) const
{
	for ( u32 i = 0; i < m_npc_characters.size(); ++i )
		if ( strings::equal( m_npc_characters[i].first, character_name ) )
			return			m_npc_characters[i].second;

	return							u32(-1);	
}

character_type ai_world::get_character_attributes_by_index( u32 const index ) const
{
	return							m_npc_characters[index];
}

u32 ai_world::get_class_id_by_name	( pcstr class_name ) const
{
	return get_id_by_name			( m_npc_classes, class_name );
}

u32 ai_world::get_outfit_id_by_name	( pcstr outfit_name ) const
{
	return get_id_by_name			( m_npc_outfits, outfit_name );
}

u32 ai_world::get_melee_weapon_id_by_name	( pcstr weapon_name ) const
{
	return get_id_by_name			( m_melee_weapons, weapon_name );
}

u32 ai_world::get_sniper_weapon_id_by_name	( pcstr weapon_name ) const
{
	return get_id_by_name			( m_sniper_weapons, weapon_name );
}

u32 ai_world::get_heavy_weapon_id_by_name	( pcstr weapon_name ) const
{
	return get_id_by_name			( m_heavy_weapons, weapon_name );
}

u32 ai_world::get_light_weapon_id_by_name	( pcstr weapon_name ) const
{
	return get_id_by_name			( m_light_weapons, weapon_name );
}

u32 ai_world::get_energy_weapon_id_by_name	( pcstr weapon_name ) const
{
	return get_id_by_name			( m_energy_weapons, weapon_name );
}

u32 ai_world::get_node_id_by_name	( pcstr node_name ) const
{
	return m_engine.get_node_by_name( node_name );
}

void ai_world::get_available_weapons( npc* owner, weapons_list& list_to_be_filled )
{
	return m_engine.get_available_weapons( owner, list_to_be_filled );
}

struct find_npc_by_id : private boost::noncopyable
{
	inline	find_npc_by_id	( u32 const id_to_find ) :
		npc_id				( id_to_find ),
		found_brain_unit	( 0 ),
		id_was_found		( false )
	{
	}
	
	inline void operator()	( brain_unit* const brain )
	{
		game_object const* const target = brain->get_npc().cast_game_object();
		if ( target->get_id() == npc_id )
		{
			id_was_found	= true;
			found_brain_unit = brain;
		}
	}

	u32 const				npc_id;
	bool					id_was_found;
	brain_unit const* 		found_brain_unit;
};

bool ai_world::is_target_dead		( npc const* target ) const
{
	game_object const* target_obj	= target->cast_game_object();
	R_ASSERT						( target_obj );
	find_npc_by_id					searching_predicate( target_obj->get_id() );
	m_brain_units.for_each			( searching_predicate );
	return							!searching_predicate.id_was_found;
}

bool ai_world::is_weapon_loaded		( weapon const* target_weapon ) const
{
	return							target_weapon->is_loaded();
}

bool ai_world::is_npc_safe			( brain_unit_res_ptr brain ) const
{
	brain_unit_ptr brain_unit		= static_cast_resource_ptr< brain_unit_ptr >( brain );
	return							brain_unit->is_feeling_safe();
}

static pcstr get_name_by_id	( ai_world::available_objects_types const& objects, u32 const index )
{
	R_ASSERT				( index < objects.size(), "index is out of bounds" );
	return					objects[index];	
}

pcstr ai_world::get_weapon_name_by_id	( u32 const weapon_type, u32 const id ) const
{
	weapon_types_enum const type		= (weapon_types_enum)weapon_type;
	switch ( type )
	{
		case weapon_type_melee:
			return						get_name_by_id( m_melee_weapons, id );
		
		case weapon_type_sniper:
			return						get_name_by_id( m_sniper_weapons, id );
		
		case weapon_type_heavy:
			return						get_name_by_id( m_heavy_weapons, id );

		case weapon_type_energy:
			return						get_name_by_id( m_energy_weapons, id );

		case weapon_type_light:
			return						get_name_by_id( m_light_weapons, id );
		
		default:
			NODEFAULT					( return "" );
	}
}

u32 ai_world::get_weapons_count		( u32 const weapons_type ) const
{
	weapon_types_enum const type	= (weapon_types_enum)weapons_type;
	switch ( type )
	{
		case weapon_type_melee:
			return			m_melee_weapons.size();;
		
		case weapon_type_sniper:
			return			m_sniper_weapons.size();
		
		case weapon_type_heavy:
			return			m_heavy_weapons.size();

		case weapon_type_energy:
			return			m_energy_weapons.size();

		case weapon_type_light:
			return			m_light_weapons.size();
		
		default:
			NODEFAULT		( return 0 );
	}
}

void ai_world::fill_npc_stats		( npc_statistics& stats, brain_unit_res_ptr brain ) const
{
	brain_unit_ptr brain_unit		= static_cast_resource_ptr< brain_unit_ptr >( brain );
	npc const& npc_object			= brain_unit->get_npc();

	typedef ai::npc_statistics::general_info_type::content_type content_type;
	content_type					new_item_content( "group: " );
	u32 aux_id						= npc_object.get_group_id();
	new_item_content.append			( aux_id != u32(-1) ? get_name_by_id( m_npc_groups, aux_id ) : "none" );
	stats.general_state.content.push_back( new_item_content );
	
	new_item_content.clear			( );
	new_item_content.append			( "class: " );
	aux_id							= npc_object.get_class_id();
	new_item_content.append			( aux_id != u32(-1) ? get_name_by_id( m_npc_classes, aux_id ) : "none" );
	stats.general_state.content.push_back( new_item_content );

	new_item_content.clear			( );
	new_item_content.append			( "outfit: " );
	aux_id							= npc_object.get_outfit_id();
	new_item_content.append			( aux_id != u32(-1) ? get_name_by_id( m_npc_outfits, aux_id ) : "none" );
	stats.general_state.content.push_back( new_item_content );

	brain_unit->retrieve_statistics	( stats );
}

void ai_world::set_behaviour		( resources::unmanaged_resource_ptr new_behaviour, brain_unit_res_ptr brain )
{
	brain_unit_ptr brain_unit		= static_cast_resource_ptr< brain_unit_ptr >( brain );
	behaviour_ptr behaviour_to_set	= static_cast_resource_ptr< behaviour_ptr >( new_behaviour );
	brain_unit->set_behaviour		( behaviour_to_set );
}

void ai_world::on_animation_finish	( animation_item const* const target, brain_unit_res_ptr brain )
{
	brain_unit_ptr brain_unit		= static_cast_resource_ptr< brain_unit_ptr >( brain );
	brain_unit->on_finish_animation_playing( target );
}

} // namespace ai
} // namespace xray