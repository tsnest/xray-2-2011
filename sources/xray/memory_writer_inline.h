////////////////////////////////////////////////////////////////////////////
//	Created		: 11.05.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MEMORY_WRITER_INLINE_H_INCLUDED
#define XRAY_MEMORY_WRITER_INLINE_H_INCLUDED

namespace xray {
namespace memory {

static pcstr crlf = "\n";

inline void writer_base::write_u64	( u64 d )
{
	write		( &d, sizeof(u64) );
}

inline void writer_base::write_u32	( u32 d )
{
	write		( &d, sizeof(u32) );
}

inline void writer_base::write_u16	( u16 d )
{
	write		( &d, sizeof(u16) );
}

inline void writer_base::write_u8	( u8 d )
{
	write		( &d, sizeof(u8) );
}

inline void writer_base::write_s64	( s64 d )
{
	write		( &d, sizeof(s64) );
}

inline void writer_base::write_s32	( s32 d )
{
	write		( &d, sizeof(s32) );
}

inline void writer_base::write_s16	( s16 d )
{
	write		( &d, sizeof(s16) );
}

inline void writer_base::write_s8	( s8 d )
{
	write		( &d, sizeof(s8) );
}

inline void writer_base::write_float( float d )
{
	write		( &d, sizeof(float) );
}

inline void	writer_base::write_string( pcstr p )
{
	write		( p, (u32)strlen(p) + 1 );
}

inline void writer_base::write_string_CRLF	( pcstr p )
{
	write		( p, (u32)strlen( p ));	
	write		( crlf, 1 );
}

inline void	writer_base::write_float3		( math::float3 const& vec )
{
	write		( &vec, sizeof(math::float3) );	
}

inline void	writer_base::write_float2		( math::float2 const& vec )
{
	write		( &vec, sizeof(math::float2) );
}

} // namespace memory
} // namespace xray

#endif // #ifndef XRAY_MEMORY_WRITER_INLINE_H_INCLUDED