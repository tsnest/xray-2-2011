////////////////////////////////////////////////////////////////////////////
//	Created		: 17.02.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SOUND_PLAYER_H_INCLUDED
#define XRAY_AI_SOUND_PLAYER_H_INCLUDED

#include <xray/ai/sound_collection_types.h>

namespace xray {
namespace ai {

typedef boost::function< void ( ) >	finish_playing_callback;

struct sound_player : public resources::positional_unmanaged_resource
{
	virtual	void	play			(
						sound_collection_types sound_type,
						bool sound_is_positioned = false,
						float3 const& position = float3( 0.f, 0.f, 0.f )
						)				= 0;
	virtual	void	play			(
						resources::unmanaged_resource_ptr sound_to_be_played,
						finish_playing_callback const& finish_callback,
						float3 const& position
					)					= 0;
	virtual	void	play_once		(
						sound_collection_types sound_type,
						bool sound_is_positioned = false,
						float3 const& position = float3( 0.f, 0.f, 0.f )
					)					= 0;
	virtual	void	tick			( )	= 0;
	virtual void	clear_resources	( ) = 0;

	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( sound_player );
}; // struct sound_player

typedef	resources::resource_ptr < sound_player, resources::unmanaged_intrusive_base > sound_player_ptr;

} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SOUND_PLAYER_H_INCLUDED