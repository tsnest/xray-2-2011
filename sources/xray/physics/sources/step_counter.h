////////////////////////////////////////////////////////////////////////////
//	Created 	: 04.02.2008
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#ifndef PHYSICS_STEP_COUNTER_H_INCLUDED
#define PHYSICS_STEP_COUNTER_H_INCLUDED



class step_counter_ref;

class step_counter
{
	u32	m_count;
	friend	class step_counter_ref;
public:
		step_counter( ):m_count( 0 )		{};

inline			void			step		( )									{ ++m_count; }
inline			bool			is_equal	( const step_counter &sc ) const	{ return sc.m_count == m_count; }
inline			bool			multiple_of	( u32 i ) const						{ return remainder_of( i ) == 0; }
inline			u32				remainder_of( u32 i ) const						{ return m_count%i ; }
private:

	inline								step_counter( const step_counter& c ): m_count( c.m_count )		{};
	inline		const	step_counter&	operator=	( const step_counter& c )	{ m_count = c.m_count; return *this; };
	inline						void	invalidate	( ){ m_count = u32(-1); }

}; // class step_counter

class step_counter_ref
{
	step_counter				m_step_counter;

public:
 inline		step_counter_ref( const step_counter& c ):	m_step_counter( c )	{}
 inline		step_counter_ref( ){ m_step_counter.invalidate(); }

public:
 inline		bool	is_equal		( const step_counter_ref &sr ) const			{ return sr.m_step_counter.is_equal( m_step_counter ); }
 inline		bool	multiple_of		( u32 i ) const									{ return m_step_counter.multiple_of( i ); }
 inline		u32		remainder_of	( u32 i ) const									{ return m_step_counter.remainder_of( i ); }
 inline		const	step_counter_ref&	operator=	( const step_counter_ref& c )	{ m_step_counter = c.m_step_counter; return *this; };

}; // class step_counter_ref

#endif