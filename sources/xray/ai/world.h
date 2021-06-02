////////////////////////////////////////////////////////////////////////////
//	Created		: 22.11.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_WORLD_H_INCLUDED
#define XRAY_AI_WORLD_H_INCLUDED

#include <xray/ai/filter_types.h>

namespace xray {
namespace ai {

class brain_unit;
struct navigation_environment;
struct npc;
struct sensed_sound_object;
struct sensed_hit_object;
struct game_object;
struct npc_statistics;
struct animation_item;

typedef resources::unmanaged_resource_ptr	brain_unit_res_ptr;
typedef std::pair< pstr, u32 >				character_type;

struct XRAY_NOVTABLE world
{
	virtual	void		tick				( )							= 0;
	virtual	void		clear_resources		( )							= 0;
	
	virtual	void		add_brain_unit		( brain_unit_res_ptr brain )= 0;
	virtual	void		remove_brain_unit	( brain_unit_res_ptr brain )= 0;

	virtual	void		set_behaviour		(
							resources::unmanaged_resource_ptr new_behaviour,
							brain_unit_res_ptr brain
						)												= 0;

	virtual	void		set_ignore_filter	(
							brain_unit_res_ptr brain,
							ignorable_game_object const* begin,
							ignorable_game_object const* end
						)												= 0;
	// it's just a shortcut for set_ignore_filter( brain_unit, 0, 0 );
	virtual	void		clear_ignore_filter	( brain_unit_res_ptr brain )= 0;

	virtual void		on_destruction_event( game_object const& destroying_object ) const		= 0;

	virtual	void		on_sound_event		(
							npc& npc,
							sensed_sound_object const& perceived_sound
						) const											= 0;
	virtual	void		on_hit_event		(
							npc& npc,
							sensed_hit_object const& perceived_hit
						) const											= 0;
	virtual	void	fill_objects_dictionary	( configs::binary_config_value const& dictionary )	= 0;
	virtual	bool	is_npc_safe				( brain_unit_res_ptr brain ) const					= 0;
	
	virtual	u32		get_characters_count	( ) const					= 0;
	virtual	u32		get_groups_count		( ) const					= 0;
	virtual	u32		get_classes_count		( ) const					= 0;
	virtual	u32		get_outfits_count		( ) const					= 0;
	virtual	u32		get_weapons_count		( u32 const weapons_type ) const					= 0;
	virtual	pcstr	get_weapon_name_by_id	( u32 const type, u32 const id ) const				= 0;
	virtual	character_type get_character_attributes_by_index( u32 const index ) const			= 0;

	virtual	void	fill_npc_stats			( npc_statistics& stats, brain_unit_res_ptr brain ) const	= 0;
	virtual	u32		get_current_time_in_ms	( ) const					= 0;
	virtual	void	on_animation_finish		( animation_item const* const target, brain_unit_res_ptr brain ) = 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR	( world );
}; // struct world

struct behaviour_cook_params
{
	behaviour_cook_params		( ) :
		behaviour_config		( 0 )
	{
	}
	
	behaviour_cook_params		( configs::binary_config_value const* config ) :
		behaviour_config		( config )
	{
	}

	configs::binary_config_value const* 	behaviour_config;
};

} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_WORLD_H_INCLUDED
