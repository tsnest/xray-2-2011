////////////////////////////////////////////////////////////////////////////
//	Created		: 28.03.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ENCODED_SOUND_WITH_QUALITIES_H_INCLUDED
#define ENCODED_SOUND_WITH_QUALITIES_H_INCLUDED

#include <xray/sound/encoded_sound_interface.h>
#include <xray/sound/world.h>
#include <xray/resources_cook_classes.h>

namespace xray {
namespace sound {

enum encoded_sound_type
{ 
	ogg_sound_type = 0,
};

class sound_instance_proxy_internal;

class encoded_sound_with_qualities : 
	public resources::unmanaged_resource,
	private boost::noncopyable
{
public:
	enum					{ max_quality_levels_count = 3 };
public:
							encoded_sound_with_qualities	( );
	virtual					~encoded_sound_with_qualities	( );

			encoded_sound_child_ptr const&	get_encoded_sound( world const& world, resources::positional_unmanaged_resource const* const resource_user );
			encoded_sound_child_ptr const&	get_encoded_sound( );
			encoded_sound_child_ptr const&	dbg_get_encoded_sound( u32 quality ) const;// { return m_qualities[quality]; }
	inline	u64					get_length_in_pcm		( ) const;
	inline	u64					get_length_in_msec		( ) const;
	inline	u32					get_samples_per_sec		( ) const;
	inline	channels_type		get_channels_type		( ) const;
	inline	u32					decompress				(	pbyte dest, 
															u32 pcm_pointer,
															u32& pcm_pointer_after_decompress,
															u32 bytes_needed 
														); 
private:	
	virtual bool	is_increasing_quality			( ) const;
	virtual void	increase_quality_to_target		( resources::query_result_for_cook * parent_query );
	virtual void	decrease_quality				( u32 new_best_quality );
	virtual float	satisfaction_with				( u32 quality_level, resources::positional_unmanaged_resource const* resource_user, u32 positional_users_count ) const;
			bool	add_quality						( encoded_sound_ptr child, u32 quality );
			void	on_quality_loaded				( resources::queries_result& resources );
private:
	fs::path_string						m_req_path;
	encoded_sound_child_ptr				m_qualities	[max_quality_levels_count];
	resources::query_result_for_cook*	m_parent_query;
	resources::class_id_enum			m_sound_interface_type;
	mutable u32							m_current_quality;
	bool								m_increasing_quality;
}; // class encoded_sound_with_qualities

typedef resources::resource_ptr < encoded_sound_with_qualities, resources::unmanaged_intrusive_base > encoded_sound_with_qualities_ptr;
typedef resources::child_resource_ptr < encoded_sound_with_qualities, resources::unmanaged_intrusive_base > encoded_sound_with_qualities_child_ptr;

inline
u64 encoded_sound_with_qualities::get_length_in_pcm ( ) const
{
	return m_qualities[m_current_quality]->get_length_in_pcm( );
}

inline
u64 encoded_sound_with_qualities::get_length_in_msec	( ) const
{ 
	return m_qualities[m_current_quality]->get_length_in_msec( );
}

inline
u32 encoded_sound_with_qualities::get_samples_per_sec	( ) const
{
	return m_qualities[m_current_quality]->get_samples_per_sec( ); 
}

inline
channels_type encoded_sound_with_qualities::get_channels_type	( ) const
{
	return m_qualities[m_current_quality]->get_channels_type( ); 
}

inline
u32 encoded_sound_with_qualities::decompress	(	pbyte dest,
														u32 pcm_pointer,
														u32& pcm_pointer_after_decompress,
														u32 bytes_needed )
{ 
	return m_qualities[m_current_quality]->decompress( dest, pcm_pointer, pcm_pointer_after_decompress, bytes_needed ); 
}

} // namespace sound
} // namespace xray

#endif // #ifndef ENCODED_SOUND_WITH_QUALITIES_H_INCLUDED
