////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_PRIORITY_QUEUE_BINARY_HEAP_INLINE_H_INCLUDED
#define XRAY_AI_SEARCH_PRIORITY_QUEUE_BINARY_HEAP_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace priority_queue {

#define TEMPLATE_SPECIALIZATION	template < typename vertex_manager >
#define queue					priority_queue::binary_heap::impl< vertex_manager >

TEMPLATE_SPECIALIZATION
inline queue::impl				( vertex_manager_type& manager, u32 const vertex_count ) :
	m_manager					( manager )
{
	u32 const bytes_count		= vertex_count * sizeof( vertex_type* );
	m_heap						= static_cast< vertex_type** >( MALLOC ( bytes_count, typeid( vertex_type* ).name() ) );
	memory::zero				( m_heap, bytes_count );
}

TEMPLATE_SPECIALIZATION
inline queue::~impl				( )
{
	FREE						( m_heap );
}

TEMPLATE_SPECIALIZATION
inline void queue::on_before_search	( )
{
	m_manager.on_before_search		( );

	m_heap_head						= m_heap;
	m_heap_tail						= m_heap;
}

TEMPLATE_SPECIALIZATION
inline void queue::on_after_search	( bool const success ) const
{
	m_manager.on_after_search		( success );
}

TEMPLATE_SPECIALIZATION
inline bool queue::opened_list_empty( ) const
{
	R_ASSERT						( m_heap_head <= m_heap_tail, "binary heap pointer is out of bounds" );
	return							m_heap_head == m_heap_tail;
}

TEMPLATE_SPECIALIZATION
inline void queue::add_to_opened_list	( vertex_type& vertex )
{
	R_ASSERT							( m_heap_head <= m_heap_tail, "binary heap pointer is out of bounds" );
	
	m_manager.add_to_opened_list		( vertex );

	if ( !*m_heap_head || ( (*m_heap_head)->f() < vertex.f() ) )
	{
		*m_heap_tail					= &vertex;
	}
	else
	{
		*m_heap_tail					= *m_heap_head;
		*m_heap_head					= &vertex;
	}

	std::push_heap						( m_heap_head, m_heap_tail++, predicate() );
}

TEMPLATE_SPECIALIZATION
inline void queue::decrease_opened		( vertex_type& vertex, distance_type const& value )
{
	XRAY_UNREFERENCED_PARAMETER			( value );

	R_ASSERT							( !opened_list_empty(), "opened list is empty" );
	
	vertex_type** i						= m_heap_head;
	while ( *i != &vertex )
		++i;

	std::push_heap						( m_heap_head, i + 1, predicate() );
}

TEMPLATE_SPECIALIZATION
inline void queue::move_best_to_closed	( )
{
	R_ASSERT							( !opened_list_empty(), "opened list is empty" );
	m_manager.add_to_closed_list		( **m_heap_head );
	std::pop_heap						( m_heap_head, m_heap_tail--, predicate() );
}

TEMPLATE_SPECIALIZATION
inline typename queue::vertex_type& queue::best	( ) const
{
	R_ASSERT							( !opened_list_empty(), "opened list is empty" );
	return								**m_heap_head;
}

TEMPLATE_SPECIALIZATION
inline bool queue::opened				( vertex_type const& vertex ) const
{
	return								m_manager.opened( vertex );
}

TEMPLATE_SPECIALIZATION
inline bool queue::visited				( vertex_id_type const& vertex_id ) const
{
	return								m_manager.visited( vertex_id );
}

TEMPLATE_SPECIALIZATION
inline typename queue::vertex_type& queue::new_vertex	( vertex_id_type const& vertex_id ) const
{
	return								m_manager.new_vertex( vertex_id );
}

TEMPLATE_SPECIALIZATION
inline typename queue::vertex_type& queue::vertex		( vertex_id_type const& vertex_id ) const
{
	return								m_manager.vertex(vertex_id);
}

TEMPLATE_SPECIALIZATION
inline u32 queue::visited_vertex_count					( ) const
{
	return								m_manager.visited_vertex_count();
}

#undef queue
#undef TEMPLATE_SPECIALIZATION

} // namespace priority_queue
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_PRIORITY_QUEUE_BINARY_HEAP_INLINE_H_INCLUDED