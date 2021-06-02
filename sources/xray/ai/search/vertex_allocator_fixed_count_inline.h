////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_FIXED_COUNT_INLINE_H_INCLUDED
#define XRAY_AI_SEARCH_FIXED_COUNT_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace vertex_allocator {

#define TEMPLATE_SPECIALIZATION template< typename vertex_type >
#define allocator				vertex_allocator::fixed_count::impl< vertex_type >

TEMPLATE_SPECIALIZATION
inline allocator::impl			( u32 const max_vertex_count )
{
	m_vertices					= static_cast< vertex_type* >( MALLOC( max_vertex_count * sizeof( vertex_type ), typeid( vertex_type ).name() ) );
	m_vertices_end				= m_vertices + max_vertex_count;
	vertex_type* iter			= m_vertices;
	
	for ( ; iter != m_vertices_end; ++iter )
		new(iter)vertex_type	( );
}

TEMPLATE_SPECIALIZATION
inline allocator::~impl			( )
{
	vertex_type* iter			= m_vertices;
	for ( ; iter != m_vertices_end; ++iter )
		iter->~vertex_type		( );

	FREE						( m_vertices );
}

TEMPLATE_SPECIALIZATION
inline void allocator::on_before_search	( )
{
	m_vertex_current			= m_vertices;
}

TEMPLATE_SPECIALIZATION
inline void allocator::on_after_search	( bool const success )
{
	XRAY_UNREFERENCED_PARAMETER			( success );
}

TEMPLATE_SPECIALIZATION
inline typename allocator::vertex_type& allocator::new_vertex	( )
{
	R_ASSERT					( m_vertex_current < m_vertices_end, "too many vertices" );
	vertex_type& result			= *m_vertex_current;
	++m_vertex_current;
	return						result;
}

TEMPLATE_SPECIALIZATION
inline u32 allocator::visited_vertex_count						( ) const
{
	return					u32( m_vertex_current - m_vertices );
}

#undef allocator
#undef TEMPLATE_SPECIALIZATION

} // namespace vertex_allocator
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_FIXED_COUNT_INLINE_H_INCLUDED