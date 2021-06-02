////////////////////////////////////////////////////////////////////////////
//	Created		: 27.09.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_TIME_CHANNEL_H_INCLUDED
#define XRAY_ANIMATION_TIME_CHANNEL_H_INCLUDED

namespace xray {
namespace animation {

template < class DomainType >
class time_channel {
public:
	inline						time_channel				( );
	inline						~time_channel				( );

	static	size_t				count_internal_memory_size	( u32 knots_cnt ) ;
		
public:	
	inline	void				create_in_place				( pvoid memory_buff, u32 knots_count );

public:
	inline	float				min_param					( ) const { return  knots()[ 0 ]; }
	inline	float				max_param					( ) const { return  knots()[ m_knots_count - 1 ]; }

	inline	u32					knots_count					( ) const { return m_knots_count; }
	inline	u32					domains_count				( ) const { return (knots_count() > 0) ? knots_count() - 1 : 0; }

public:
	inline	DomainType const&	domain						( u32 id ) const;
	inline	DomainType&			domain						( u32 id );

	inline	float				knot						( u32 id ) const;
	inline	float&				knot						( u32 id );

public:
	inline	u32					domain						( float t, u32& current_domain ) const;
	inline	DomainType const*	domains						( ) const	{  return   (DomainType*) ( pbyte( this ) + domains_memory_position() ); }
	inline	float const*		knots						( ) const	{  return   (float*) ( pbyte( this ) + knots_memory_position() ); }

private:
	inline	size_t				domains_memory_size			( ) const	{	return	sizeof(DomainType) * ( m_knots_count - 1 ); }
	inline	size_t				domains_memory_position		( ) const	{	return  m_internal_memory_position; }
	inline	size_t				knots_memory_position		( ) const 	{	return  m_internal_memory_position + domains_memory_size(); }

	inline	DomainType*			domains						( ) 		{  return   (DomainType*) ( pbyte( this ) + domains_memory_position() ); }

	inline	float*				knots						( )			{  return   (float*) ( pbyte( this ) + knots_memory_position() ); }

private:
	u32							m_knots_count;
	size_t						m_internal_memory_position;
}; // class time_channel

} // namespace animation
} // namespace xray

#include <xray/animation/time_channel_inline.h>

#endif // #ifndef XRAY_ANIMATION_TIME_CHANNEL_H_INCLUDED