////////////////////////////////////////////////////////////////////////////
//	Created		: 11.05.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MEMORY_WRITER_H_INCLUDED
#define XRAY_MEMORY_WRITER_H_INCLUDED

namespace xray {
namespace memory {

#pragma warning(push)
#pragma warning(disable:4251)

class XRAY_CORE_API writer_base
{
public:
					writer_base			( memory::base_allocator* allocator );
	virtual			~writer_base		( );
	// kernel
	virtual	void	seek				( u32 position )			= 0;
	virtual	u32		tell				( )							= 0;

	virtual	void	write				( pcvoid ptr, u32 count )	= 0;

	// generalized writing functions
	inline	void	write_u64			( u64 d );
	inline	void	write_u32			( u32 d );
	inline	void	write_u16			( u16 d );
	inline	void	write_u8			( u8 d );
	inline	void	write_s64			( s64 d );
	inline	void	write_s32			( s32 d );
	inline	void	write_s16			( s16 d );
	inline	void	write_s8			( s8 d );
	inline	void	write_float			( float d );

	inline	void	write_string		( pcstr p );
	inline	void	write_string_CRLF	( pcstr p );

			void	write_float3		( math::float3 const& vec );
			void	write_float2		( math::float2 const& vec );

	// generalized chunking
			u32		align				( );
			void	open_chunk			( u32 type );
			void	close_chunk			( );
			u32		chunk_size			( );// returns size of currently opened chunk, 0 otherwise
			void	write_chunk			( u32 type, pvoid data, u32 size );
	virtual	bool	valid				( ) const			{ return true; }
	virtual	void	flush				( ) = 0;
protected:
	memory::base_allocator*		m_allocator;
private:
	vectora<u32>				m_chunk_pos;
};

#pragma warning(pop)

class XRAY_CORE_API writer : public writer_base
{
public:
					writer		( memory::base_allocator* allocator );
	virtual			~writer		( );

	// kernel
	virtual void	write		( pcvoid ptr, u32 count );

	virtual void	seek		( u32 pos )	{ m_position = pos;			}
	virtual u32		tell		( )			{ return m_position;		}

	// specific
	inline u8*		pointer		( )			{ return m_data;			}
	inline u32		size		( ) const 	{ return m_file_size;		}
	inline void		clear		( )			{ m_file_size = 0; m_position = 0; }
	void			free_		( );
	bool			save_to		( pcstr fn, bool deassociate_resource = false );
	virtual	void	flush		( )			{ };

	bool			external_data;
private:
	u8*				m_data;
	u32				m_position;
	u32				m_mem_size;
	u32				m_file_size;

};

} // namespace memory
} // namespace xray

#include <xray/memory_writer_inline.h>
#endif // #ifndef XRAY_MEMORY_WRITER_H_INCLUDED