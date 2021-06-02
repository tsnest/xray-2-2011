////////////////////////////////////////////////////////////////////////////
//	Created 	: 19.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MEMORY_READER_H_INCLUDED
#define XRAY_MEMORY_READER_H_INCLUDED

#include <xray/memory_reader_wrapper.h>

namespace xray {
namespace memory {

class XRAY_CORE_API reader : public reader_wrapper< reader > {
public:
	inline			reader		( non_null< u8 const, u8 const* >::ptr data, u32 size, u32 position = 0 );
	inline	void	r			( pvoid destination, u32 destination_size, u32 size );
			pcstr	r_string	( );
			void	skip_string	( );
	inline	void	advance		( u32 offset );
	inline	void	seek		( u32 new_position );
	inline	void	rewind		( );

public:
	inline	bool	eof			( ) const;
	inline	u32		elapsed		( ) const;
	inline	u32		tell		( ) const;
	inline	u32		length		( ) const;
	inline	pcbyte	pointer		( ) const;
	inline	pcbyte	data		( ) const;

private:
	pcbyte	m_data;
	pcbyte	m_pointer;
	u32		m_size;
}; // class reader

} // namespace memory
} // namespace xray


namespace xray {
namespace memory {

template class XRAY_CORE_API reader_wrapper< reader >;

} // namespace memory
} // namespace xray

#include <xray/memory_reader_inline.h>

#endif // #ifndef XRAY_MEMORY_READER_H_INCLUDED