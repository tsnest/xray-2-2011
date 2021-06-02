////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_VERTEX_MANAGER_FIXED_COUNT_HASH_INLINE_H_INCLUDED
#define XRAY_AI_SEARCH_VERTEX_MANAGER_FIXED_COUNT_HASH_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace vertex_manager {

#define TEMPLATE_SPECIALIZATION	template < typename AllocatorType, typename LookUpCellType >
#define manager					vertex_manager::fixed_count_hash::impl< AllocatorType, LookUpCellType >

TEMPLATE_SPECIALIZATION
inline manager::impl		( AllocatorType& allocator, u32 const hash_size, u32 const fix_size ) :
	m_allocator				( allocator ),
	m_hash_size				( hash_size ),
	m_fix_size				( fix_size ),
	m_current_path_id		( path_id_type( 0 ) )
{
	u32	bytes_count			= hash_size * sizeof( LookUpCellType* );
	m_hash					= static_cast< LookUpCellType** >( MALLOC( bytes_count, typeid( LookUpCellType* ).name() ) );
	memory::zero			( m_hash, bytes_count );

	bytes_count				= fix_size * sizeof( LookUpCellType );
	m_vertices				= static_cast< LookUpCellType* >( MALLOC( bytes_count, typeid( LookUpCellType ).name() ) );
	memory::zero			( m_vertices, bytes_count );
}

TEMPLATE_SPECIALIZATION
inline manager::~impl		( )
{
	FREE					( m_hash );
	FREE					( m_vertices );
}

TEMPLATE_SPECIALIZATION
inline void manager::on_before_search	( )
{
	m_allocator.on_before_search		( );

	m_vertex_count						= 0;
	
	++m_current_path_id;
	if ( m_current_path_id )
		return;

	m_current_path_id					= 1;
	
	memory::zero						( m_hash, m_hash_size * sizeof( LookUpCellType* ) );
	memory::zero						( m_vertices, m_fix_size * sizeof( LookUpCellType ) );
}

TEMPLATE_SPECIALIZATION
inline void manager::on_after_search	( bool const success ) const
{
	m_allocator.on_after_search			( success );
}

TEMPLATE_SPECIALIZATION
inline void manager::prepare_cell		( LookUpCellType& cell )
{
	if ( cell.m_prev )
	{
		cell.m_prev->m_next				= cell.m_next;

		if ( !cell.m_next )
			return;

		cell.m_next->m_prev				= cell.m_prev;
			return;
	}

	if ( cell.m_next )
		cell.m_next->m_prev				= 0;

	if ( !m_hash[cell.m_hash] )
		return;
	
	if ( m_hash[cell.m_hash]->m_path_id == current_path_id() )
		return;

	m_hash[cell.m_hash]					= 0;
}

TEMPLATE_SPECIALIZATION
inline typename manager::vertex_type& manager::new_vertex	( vertex_id_type const& vertex_id )
{
	vertex_type& vertex			= m_allocator.new_vertex();

	R_ASSERT					( m_vertex_count < m_fix_size, "too many vertices" );
	LookUpCellType& cell		= *( m_vertices + m_vertex_count++ );

	prepare_cell				( cell );

	cell.m_vertex				= &vertex;
	cell.m_path_id				= current_path_id();
	vertex.id()					= vertex_id;
	u32 index					= hash_index(vertex_id);
	LookUpCellType* _vertex	= m_hash[index];
	
	if ( !_vertex || ( _vertex->m_path_id != current_path_id() ) || ( _vertex->m_hash != index ) )
		_vertex					= 0;

	m_hash[index]				= &cell;
	cell.m_next					= _vertex;
	cell.m_prev					= 0;

	if ( _vertex )
		_vertex->m_prev			= &cell;

	cell.m_hash					= index;
	return						vertex;
}

TEMPLATE_SPECIALIZATION
inline void manager::add_to_opened_list	( vertex_type& vertex )
{
	vertex.opened()				= 1;
}

TEMPLATE_SPECIALIZATION
inline void manager::add_to_closed_list	( vertex_type& vertex )
{
	vertex.opened()				= 0;
}

TEMPLATE_SPECIALIZATION
inline bool manager::opened		( vertex_type const& vertex ) const
{
	return						!!vertex.opened();
}

TEMPLATE_SPECIALIZATION
inline bool manager::closed		( vertex_type const& vertex ) const
{
	return						!opened( vertex );
}

TEMPLATE_SPECIALIZATION
inline bool manager::visited	( vertex_id_type const& vertex_id ) const
{
	u32 index					= hash_index( vertex_id );
	LookUpCellType* vertex	= m_hash[index];
	if ( !vertex )
		return					false;

	if ( vertex->m_path_id != current_path_id() )
		return					false;
	
	if ( vertex->m_hash != index )
		return					false;

	for ( ; vertex; vertex = vertex->m_next )
	{
		if ( vertex->m_vertex->id() != vertex_id )
			continue;

		return					true;
	}

	return						false;
}

TEMPLATE_SPECIALIZATION
inline u32 manager::hash_index	( vertex_id_type const& vertex_id ) const
{
	return						::xray::ai::vertex_manager::hash_value( vertex_id ) % m_hash_size;
}

TEMPLATE_SPECIALIZATION
inline typename manager::vertex_type& manager::vertex	( vertex_id_type const& vertex_id ) const
{
	R_ASSERT					( visited( vertex_id ), "vertex is not visited" );

	LookUpCellType* vertex	= m_hash[hash_index( vertex_id )];

	for ( ; ; vertex = vertex->m_next )
	{
		R_ASSERT				( vertex, "vertex is null" );

		if ( vertex->m_vertex->id() != vertex_id )
			continue;

		return					*vertex->m_vertex;
	}
}

TEMPLATE_SPECIALIZATION
inline const typename manager::path_id_type& manager::current_path_id	( ) const
{
	return						m_current_path_id;
}

TEMPLATE_SPECIALIZATION
inline u32 manager::visited_vertex_count								( ) const
{
	return						m_allocator.visited_vertex_count();
}

#undef manager
#undef TEMPLATE_SPECIALIZATION

} // namespace vertex_manager
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_VERTEX_MANAGER_FIXED_COUNT_HASH_INLINE_H_INCLUDED