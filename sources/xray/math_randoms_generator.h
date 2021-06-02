#ifndef XRAY_RANDOMS_GENERATOR_H_INCLUDED
#define XRAY_RANDOMS_GENERATOR_H_INCLUDED

namespace xray {
namespace math {

class random32
{
public:
	inline			random32	( u32 const start_seed = 0 ) : m_seed( start_seed ) { }
	inline	u32		seed		( )													{ return m_seed; }
	inline	void	seed		( u32 const seed )									{ m_seed = seed; }
	inline	u32		random		( u32 const range )	
	{
		m_seed		= 0x08088405*m_seed + 1;
		return		u32(u64(m_seed)*u64(range) >> 32);
	}

	inline	float	random_f	( float const range )	
	{
		return		range * ( random( 1024 * 1024 ) / ( 1024.f * 1024.f ) );
	}

	inline	size_t	operator ( )( u32 const range)	
	{
		return		random( range );
	}
	
	inline	void	serialize	( memory::writer& w ) const
	{
		w.write_u32			( m_seed );
	}

	inline	void	deserialize	( memory::reader& r )
	{
		m_seed		= r.r_u32( );
	}

private:
	u32				m_seed;
};

class random32_with_count : public random32 {
private:
	typedef random32	super;

public:
	inline			random32_with_count	( u32 const start_seed = 0 ) :
		super		( start_seed ),
		m_count		( 0 ),
		m_thread_id	( threading::current_thread_id() )
	{
	}

	inline	u32		random		( u32 const range)	
	{
		//R_ASSERT	( m_thread_id == threading::current_thread_id() );
		threading::interlocked_increment( m_count );
		return		super::random( range );
	}

	inline	size_t	operator ( )( u32 const range)	
	{
		return		random( range );
	}

	inline	u32		count		( ) const
	{
		return		m_count;
	}

	inline	void	set_current_thread_id	( )
	{
		m_thread_id	= threading::current_thread_id( );
	}

private:
	threading::atomic32_type	m_count;
	u32							m_thread_id;
}; // class random32_with_count
	 
} // namespace math
} // namespace xray

#endif // #ifndef XRAY_RANDOMS_GENERATOR_H_INCLUDED