////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.10.2008
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_STRINGS_DETAIL_TUPLES_INLINE_H_INLUDED
#define XRAY_STRINGS_DETAIL_TUPLES_INLINE_H_INLUDED

template < typename T0 >
inline xray::strings::detail::tuples::tuples		( T0 p0 ) :
		m_count				(1)
{
	helper<0>::add_string	(*this, p0);
}

template < typename T0, typename T1 >
inline xray::strings::detail::tuples::tuples		( T0 p0, T1 p1 ) :
	m_count					(2)
{
	helper<0>::add_string	(*this, p0);
	helper<1>::add_string	(*this, p1);
}

template < typename T0, typename T1, typename T2 >
inline xray::strings::detail::tuples::tuples		( T0 p0, T1 p1, T2 p2 ) :
	m_count					(3)
{
	helper<0>::add_string	(*this, p0);
	helper<1>::add_string	(*this, p1);
	helper<2>::add_string	(*this, p2);
}

template < typename T0, typename T1, typename T2, typename T3 >
inline xray::strings::detail::tuples::tuples		( T0 p0, T1 p1, T2 p2, T3 p3 ) :
	m_count					(4)
{
	helper<0>::add_string	(*this, p0);
	helper<1>::add_string	(*this, p1);
	helper<2>::add_string	(*this, p2);
	helper<3>::add_string	(*this, p3);
}

template < typename T0, typename T1, typename T2, typename T3, typename T4 >
inline xray::strings::detail::tuples::tuples		( T0 p0, T1 p1, T2 p2, T3 p3, T4 p4 ) :
	m_count					(5)
{
	helper<0>::add_string	(*this, p0);
	helper<1>::add_string	(*this, p1);
	helper<2>::add_string	(*this, p2);
	helper<3>::add_string	(*this, p3);
	helper<4>::add_string	(*this, p4);
}

template < typename T0, typename T1, typename T2, typename T3, typename T4, typename T5 >
inline xray::strings::detail::tuples::tuples		( T0 p0, T1 p1, T2 p2, T3 p3, T4 p4, T5 p5 ) :
	m_count					(6)
{
	helper<0>::add_string	(*this, p0);
	helper<1>::add_string	(*this, p1);
	helper<2>::add_string	(*this, p2);
	helper<3>::add_string	(*this, p3);
	helper<4>::add_string	(*this, p4);
	helper<5>::add_string	(*this, p5);
}

inline u32 xray::strings::detail::tuples::size		( ) const
{
	ASSERT			(m_count > 0);

	u32				result = m_strings[0].second;
	
	for (u32 j = 1; j < m_count; ++j)
		result		+= m_strings[j].second;

	if ( result > max_concat_result_size )
	{
		error_process();
	}
	
	return			((result + 1)*sizeof(*m_strings[0].first));
}

inline void xray::strings::detail::tuples::concat	( pcstr const result ) const
{
	ASSERT			(m_count > 0);

	pstr			i = const_cast<pstr>(result);
	
	memory::copy	(i, m_strings[0].second + 1, m_strings[0].first, m_strings[0].second*sizeof(*m_strings[0].first));
	i				+= m_strings[0].second;

	for (u32 j = 1; j < m_count; ++j) {
		memory::copy(i, m_strings[j].second + 1, m_strings[j].first, m_strings[j].second*sizeof(*m_strings[j].first));
		i			+= m_strings[j].second;
	}

	*i				= 0;
}

#define TEMPLATE_SIGNATURE	template < u32 index >
#define HELPER				xray::strings::detail::tuples::helper< index >

TEMPLATE_SIGNATURE
inline u32 HELPER::length									( pcstr const string )
{
	return		( string ? ( unsigned int )strings::length(string) : 0 );
}

TEMPLATE_SIGNATURE
inline pcstr HELPER::string									( pcstr const string )
{
	return		( string );
}

TEMPLATE_SIGNATURE
template <typename T>
inline void HELPER::add_string								( xray::strings::detail::tuples& self, T p )
{
	COMPILE_ASSERT			( index < max_item_count, Error_invalid_string_index_specified );

	pcstr const cstr		= string(p);
	ASSERT					( cstr );
	self.m_strings[ index ]	= pair( cstr, length( p ) );
}

#undef HELPER
#undef TEMPLATE_SIGNATURE

#endif // #ifndef XRAY_STRINGS_DETAIL_TUPLES_INLINE_H_INLUDED