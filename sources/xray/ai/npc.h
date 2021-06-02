////////////////////////////////////////////////////////////////////////////
//	Created		: 22.11.2010
//	Author		: Dmitriy Iassenev && Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_NPC_H_INCLUDED
#define XRAY_AI_NPC_H_INCLUDED

namespace xray {
namespace ai {

struct game_object;
struct weapon;
struct animation_item;
struct sound_item;
struct movement_target;

struct XRAY_NOVTABLE npc
{
	virtual math::aabb			get_aabb			( ) const	= 0;
	virtual float4x4			get_eyes_matrix		( ) const	= 0;
	virtual	float3				get_position		( float3 const& requester ) const	= 0;
	virtual math::color			get_color			( ) const	= 0;
	virtual float3				get_eyes_direction	( ) const	= 0;
	virtual	float3				get_eyes_position	( ) const	= 0;
	virtual game_object const*	cast_game_object	( ) const	= 0;
	virtual	void				clear_resources		( )			= 0;
	virtual void				set_eyes_direction	( float3 const& direction )			= 0;
	virtual	bool				debug_draw_allowed	( ) const	= 0;

	virtual	u32					get_group_id		( ) const	= 0;
	virtual	u32					get_class_id		( ) const	= 0;
	virtual	u32					get_outfit_id		( ) const	= 0;

	virtual	bool				is_patrolling		( ) const	= 0;
	virtual	bool				is_at_cover			( ) const	= 0;
	virtual	bool				is_safe				( ) const	= 0;
	virtual	bool				is_invisible		( ) const	= 0;
	virtual	bool			is_target_in_melee_range( npc const* const target ) const		= 0;
	virtual	bool				is_at_node			( game_object const* const node ) const	= 0;
	virtual	bool				is_playing_animation( ) const	= 0;
	virtual	bool				is_moving			( ) const	= 0;

	virtual	void				prepare_to_attack	( npc const* const target, weapon const* const gun ) = 0;	
	virtual	void				attack				( npc const* const target, weapon const* const gun ) = 0;
	virtual	void				attack_melee		( npc const* const target, weapon const* const gun ) = 0;
	virtual	void				attack_from_cover	( npc const* const target, weapon const* const gun ) = 0;
	virtual	void				stop_attack			( npc const* const target, weapon const* const gun ) = 0;
	virtual	void				survey_area			( )			= 0;
	virtual	void				stop_patrolling		( )			= 0;
	virtual	void			stop_animation_playing	( )			= 0;
	virtual	void				reload				( weapon const* const gun )				= 0;
	
	virtual	void				play_animation		( animation_item const* const target )	= 0;
	virtual	void				move_to_position	( movement_target const* const target )	= 0;
	
protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR			( npc );
}; // struct npc

struct brain_unit_cook_params
{
	brain_unit_cook_params		( ) :
		world_user_type			( resources::unknown_data_class ),
		npc						( 0 )
	{
	}

	resources::class_id_enum	world_user_type;
	npc*						npc;
};

} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_NPC_H_INCLUDED
