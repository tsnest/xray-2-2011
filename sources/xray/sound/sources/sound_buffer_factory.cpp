////////////////////////////////////////////////////////////////////////////
//	Created		: 10.03.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_buffer_factory.h"

namespace xray {
namespace sound {

sound_buffer_factory::sound_buffer_factory( u8* buffer, u32 size, u32 max_buffers ) :
	m_sound_buffers_allocator( buffer, size )
{
	for ( u32 i = 0; i < max_buffers - 1; ++i )
	{
		sound_buffer* new_buffer = static_cast_checked<sound_buffer*>(
			XRAY_MALLOC_IMPL(
				m_sound_buffers_allocator,
				real_sound_buffer_size_aligned,
				"sound buffers pool"
			)
		);
		new (new_buffer) sound_buffer( );
		m_free_sound_buffers.push_back	( new_buffer );
	}

	if ( max_buffers != 0 )
	{
		m_mute_buffer		= static_cast_checked<sound_buffer*>(
			XRAY_MALLOC_IMPL(
				m_sound_buffers_allocator,
				real_sound_buffer_size_aligned,
				"sound buffers pool"
			)
		);
		new (m_mute_buffer) sound_buffer( );
	}
}

sound_buffer_factory::~sound_buffer_factory	( )
{
	m_cached_sound_buffers.clear();

	sound_buffer* lru_sound_buffer = m_lru_sound_buffers.front();
	while ( lru_sound_buffer )
	{
		sound_buffer* object_to_be_deleted	= lru_sound_buffer;
		lru_sound_buffer			 = sound_buffer_list_type::get_next_of_object( lru_sound_buffer );
		object_to_be_deleted->~sound_buffer( );
		XRAY_FREE_IMPL	( m_sound_buffers_allocator, object_to_be_deleted );
	}


	sound_buffer* free_sound_buffer = m_free_sound_buffers.front();
	while ( free_sound_buffer )
	{
		sound_buffer* object_to_be_deleted	= free_sound_buffer;
		free_sound_buffer = sound_buffer_list_type::get_next_of_object( free_sound_buffer );
		object_to_be_deleted->~sound_buffer( );
		XRAY_FREE_IMPL	( m_sound_buffers_allocator, object_to_be_deleted );
	}
}

sound_buffer* sound_buffer_factory::new_sound_buffer ( encoded_sound_ptr const& encoded_sound, u32 pcm_offset, u32& next_pcm_offset )
{
	lightweight_sound_buffer tmp( encoded_sound, pcm_offset );
	sound_buffers_set_type::iterator it = m_cached_sound_buffers.find( tmp, m_cached_sound_buffers.key_comp() );
	//it									= m_cached_sound_buffers.end( );
	//LOG_INFO					("new_sound_buffer");

	//// for test
	//sound_buffers_set_type::iterator it_test		= m_cached_sound_buffers.begin();
	//sound_buffers_set_type::iterator it_end_test	= m_cached_sound_buffers.end();
	//for (;it_test != it_end_test; ++it_test)
	//{
	//	sound_buffer* tmp = &(*it_test);
	//	int i = 0;
	//}

	// for test's
	//{
	//	sound_buffer* ptr = m_lru_sound_buffers.front();
	//	while ( ptr )
	//	{
	//		LOG_DEBUG		("reference_count %d", ptr->reference_count( ));
	//		ptr = ptr->m_next;
	//	}
	//}
	// end for test's

	sound_buffer* result			= 0;

	if ( it != m_cached_sound_buffers.end( ) )
	{
		
		//LOG_DEBUG					("cached buffer finded");
		//LOG_DEBUG					("m_cached_sound_buffers.size( ) %d", m_cached_sound_buffers.size( ));
		result						= &( *it );
		next_pcm_offset				= result->next_sound_buffer_offset( );
		m_lru_sound_buffers.erase	( result );
	}
	else 
	{
		if (!m_free_sound_buffers.empty())
		{
			//LOG_DEBUG					("get unused sound buffer");
			result						= m_free_sound_buffers.pop_front( );
		}
		else
		{
			//LOG_DEBUG					("get last used sound buffer");
			result						= find_least_recently_used( );
			m_cached_sound_buffers.erase( *result );
			m_lru_sound_buffers.erase	( result );
		}

		result->fill_buffer				( encoded_sound, pcm_offset, next_pcm_offset );
		m_cached_sound_buffers.insert	( *result );
	}

	R_ASSERT						( result );
	m_lru_sound_buffers.push_back	( result );
	result->increment				( );
	return							result;
}

void sound_buffer_factory::delete_sound_buffer	( sound_buffer* buffer )
{
	buffer->decrement			( );
	//LOG_DEBUG("delete sound buffer: reference_count %d", buffer->reference_count( ));
}

sound_buffer* sound_buffer_factory::find_least_recently_used( )
{
	//// for test's
	//{
	//	sound_buffer* ptr = m_lru_sound_buffers.front();
	//	while ( ptr )
	//	{
	//		LOG_DEBUG		("reference_count %d", ptr->reference_count( ));
	//		ptr = ptr->m_next;
	//	}
	//}
	//// end for test's

	sound_buffer* ptr = m_lru_sound_buffers.front();
	while ( ptr )
	{
		//LOG_DEBUG		("reference_count %d", ptr->reference_count( ));
		if ( ptr->reference_count( ) == 0 )
			return ptr;
		ptr = sound_buffer_list_type::get_next_of_object( ptr );
	}

	UNREACHABLE_CODE		( return 0 );
}

sound_buffer* sound_buffer_factory::new_no_cahceable_sound_buffer(	encoded_sound_ptr const& encoded_sound,
																	u32 pcm_offset,
																	u32& next_pcm_offset
																)
{
	sound_buffer* result = 0;
	if (!m_free_sound_buffers.empty())
	{
		//LOG_DEBUG					("get unused sound buffer");
		result						= m_free_sound_buffers.pop_front( );
	}
	else
	{
		//LOG_DEBUG					("get last used sound buffer");
		result						= find_least_recently_used( );
		m_cached_sound_buffers.erase( *result );
		m_lru_sound_buffers.erase	( result );
	}

	result->fill_buffer				( encoded_sound, pcm_offset, next_pcm_offset );

	R_ASSERT						( result );
	m_lru_sound_buffers.push_back	( result );
	result->increment				( );

	return result;
}

sound_buffer* sound_buffer_factory::get_mute_sound_buffer	( encoded_sound_ptr const& encoded_sound )
{
	m_mute_buffer->fill_mute_buffer( encoded_sound );
	return m_mute_buffer;
}


} // namespace sound
} // namespace xray
