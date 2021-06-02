////////////////////////////////////////////////////////////////////////////
//	Created		: 30.12.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "memory_guard.h"

#if XRAY_USE_MEMORY_GUARD

static xray::command_line::key	s_no_memory_guard_key("no_memory_guard", "", "memory", "disables runtime memory guard protection");

namespace xray {
namespace memory {
namespace guard {

void initialize				( )
{
}

void finalize				( )
{
}

static inline u32 get_house_keeping_size_impl	( )
{
	return					3*sizeof(u32);
}

static bool enabled			( )
{
	static bool result		= !s_no_memory_guard_key.is_set();
	return					result;
}

static inline size_t wrapped_buffer_size	( size_t const buffer_size )
{
	return					buffer_size + get_house_keeping_size_impl();
}

void on_alloc				( pvoid& buffer, size_t& buffer_size, size_t const previous_size, pcstr const description )
{
	XRAY_UNREFERENCED_PARAMETER	( description );

	if ( !enabled() )
		return;

#pragma message(XRAY_TODO("add check \"object has been modified after it was freed\" - check 0xfeeefeee"))

	size_t const pure_buffer_size	= buffer_size - get_house_keeping_size_impl();
	size_t const count_with_magic	= math::align_up(buffer_size,sizeof(u32))/sizeof(u32);
	size_t const previous_count		= math::align_up(previous_size,sizeof(u32))/sizeof(u32);
	u32* numbers				= (u32*)buffer;

	numbers[0]					= (u32)pure_buffer_size;
	numbers[1]					= static_cast<u32>( debug::underrun_guard );
 	numbers[count_with_magic - 1] = static_cast<u32>( debug::overrun_guard );
 
	ASSERT						( count_with_magic >= 3 );
	size_t const count			= count_with_magic - 3;

	if ( count > previous_count ) {
		size_t const fill_count	= count - previous_count;
		memory::fill32			( numbers + 2 + previous_count, fill_count*sizeof(u32), static_cast<u32>(debug::uninitialized_memory), fill_count );
	}

	if ( count > previous_count ) {

		u32 &previous_last		= numbers[previous_count + 1];
		switch (previous_size % sizeof(u32)) {
			case 0 : break;
			case 1 : {
				previous_last	= (previous_last & ~(platform::little_endian() ? 0xffffff00 : 0x00ffffff)) | (platform::little_endian() ? 0xfdcdcd00 : 0x00cdcdfd);
				break;
					 }
			case 2 : {
				previous_last	= (previous_last & ~(platform::little_endian() ? 0xffff0000 : 0x0000ffff)) | (platform::little_endian() ? 0xfdcd0000 : 0x0000cdfd);
				break;
					 }
			case 3 : {
				previous_last	= (previous_last & ~(platform::little_endian() ? 0xff000000 : 0x000000ff)) | (platform::little_endian() ? 0xfd000000 : 0x000000fd);
				break;
			}
			default :			NODEFAULT();
		}
	}

	u32& last					= numbers[count_with_magic - 2];
	switch ( buffer_size % sizeof(u32) ) {
		case 0 : break;
		case 1 : {
			last				= platform::little_endian() ? 0xfdfdfdcd : 0xcdfdfdfd;
			break;
				 }
		case 2 : {
			last				= platform::little_endian() ? 0xfdfdcdcd : 0xcdcdfdfd;
			break;
				 }
		case 3 : {
			last				= platform::little_endian() ? 0xfdcdcdcd : 0xcdcdcdfd;
			break;
		}
		default :				NODEFAULT();
	}

	static u32 const house_keeping_size	= get_house_keeping_size_impl( );
	(pbyte&)buffer				+= 2*sizeof(u32);
	buffer_size					-= house_keeping_size;
}

pvoid get_real_pointer		( pvoid const pointer )
{
	if ( !enabled() )
		return				pointer;

	return					(pbyte)pointer - 2*sizeof(u32);
}

void on_free				( pvoid& pointer, bool const can_clear )
{
	if ( !enabled() )
		return;

	pointer						= get_real_pointer( pointer );

	u32* numbers				= (u32*)pointer;

	ASSERT						( numbers[1] == debug::underrun_guard, "memory corruption detected: buffer underrun" );

	size_t const count			= math::align_up( wrapped_buffer_size( numbers[0] ), sizeof(u32)) / sizeof(u32);
	ASSERT						( wrapped_buffer_size( numbers[0] ) <= sizeof(u32)*count , "internal error, please report" );
	ASSERT						( numbers[count - 1] == debug::overrun_guard, "memory corruption detected: buffer overrun" );

	u32 &last					= numbers[count - 2];
	switch ( numbers[0] % sizeof(u32) ) {
		case 0 : break;
		case 1 : {
 			ASSERT				( !platform::little_endian() || (last & 0xffffff00) == 0xfdfdfd00, "memory corruption detected: buffer overrun" );
			ASSERT				(  platform::little_endian() || (last & 0x00ffffff) == 0x00fdfdfd, "memory corruption detected: buffer overrun" );
			break;
		}
		case 2 : {
			ASSERT				( !platform::little_endian() || (last & 0xffff0000) == 0xfdfd0000, "memory corruption detected: buffer overrun" );
			ASSERT				(  platform::little_endian() || (last & 0x0000ffff) == 0x0000fdfd, "memory corruption detected: buffer overrun" );
			break;
		}
		case 3 : {
			ASSERT				( !platform::little_endian() || (last & 0xff000000) == 0xfd000000, "memory corruption detected: buffer overrun" );
			ASSERT				(  platform::little_endian() || (last & 0x000000ff) == 0x000000fd, "memory corruption detected: buffer overrun" );
			break;
		}
		default :				NODEFAULT();
	}

	if ( !can_clear )
		return;

	size_t const size_with_magic	=	wrapped_buffer_size( numbers[0] );
	memory::fill32				( numbers, size_with_magic, (u32)debug::freed_memory, size_with_magic / sizeof(u32) );
}

size_t get_house_keeping_size	( )
{
	if ( !enabled() )
		return					0;

	return						get_house_keeping_size_impl();
}

size_t	get_buffer_size			( pvoid buffer )
{
	(pbyte&)buffer				-= 2*sizeof(u32);
	return						*(u32*)buffer;
}

} // namespace guard
} // namespace memory
} // namespace xray

#endif // #if XRAY_USE_MEMORY_GUARD