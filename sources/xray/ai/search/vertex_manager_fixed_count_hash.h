////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_VERTEX_MANAGER_FIXED_COUNT_HASH_H_INCLUDED
#define XRAY_AI_SEARCH_VERTEX_MANAGER_FIXED_COUNT_HASH_H_INCLUDED

namespace xray {
namespace ai {
namespace vertex_manager {

struct fixed_count_hash
{
	typedef u32						path_id_type;
	typedef u32						hash_id_type;

	template < typename VertexIdType >
	struct vertex_helper
	{
		struct vertex_impl
		{
			typedef VertexIdType	vertex_id_type;

			vertex_id_type			m_id;
			bool					m_opened;

			inline	vertex_id_type const& id( ) const	{ return m_id; }
			inline	vertex_id_type&	id		( )			{ return m_id; }
			inline	bool&			opened	( )			{ return m_opened; }
			inline bool const&		opened	( )	const	{ return m_opened; }
		};
	};

	template < typename VertexType >
	struct look_up_cell_helper
	{
		template < typename final_type >
		struct look_up_cell_impl
		{
			typedef VertexType		vertex_type;

			vertex_type*			m_vertex;
			final_type*				m_next;
			final_type*				m_prev;
			path_id_type			m_path_id;
			hash_id_type			m_hash;
		};
	};

	template < typename AllocatorType, typename LookUpCellType >
	class impl : private boost::noncopyable
	{
	public:
		typedef AllocatorType							allocator_type;
		typedef	typename allocator_type::vertex_type	vertex_type;
		typedef	typename vertex_type::vertex_id_type	vertex_id_type;
		typedef	path_id_type							path_id_type;

	public:
		inline						impl				( allocator_type& allocator, u32 const hash_size, u32 const fix_size );
		inline						~impl				( );
		
		inline void					on_before_search	( );
		inline void					on_after_search		( bool const success ) const;
		
		inline vertex_type&			new_vertex			( vertex_id_type const& vertex_id );
		inline void					add_to_opened_list	( vertex_type& vertex );
		inline void					add_to_closed_list	( vertex_type& vertex );

	public:
		inline bool					opened				( vertex_type const& vertex ) const;
		inline bool					closed				( vertex_type const& vertex ) const;
		inline bool					visited				( vertex_id_type const& vertex_id ) const;
		inline u32					hash_index			( vertex_id_type const& vertex_id ) const;
		inline vertex_type&			vertex				( vertex_id_type const& vertex_id ) const;

	public:
		inline path_id_type const&	current_path_id		( ) const;
		inline u32					visited_vertex_count( ) const;

	private:
		inline void					prepare_cell		( LookUpCellType& cell );

	private:
		allocator_type&				m_allocator;
		LookUpCellType*				m_vertices;
		LookUpCellType**			m_hash;
		u32							m_vertex_count;
		u32							m_hash_size;
		u32							m_fix_size;
		path_id_type				m_current_path_id;
	};
}; // struct fixed_count_hash

} // namespace vertex_manager
} // namespace ai
} // namespace xray

#include <xray/ai/search/vertex_manager_fixed_count_hash_inline.h>

#endif // #ifndef XRAY_AI_SEARCH_VERTEX_MANAGER_FIXED_COUNT_HASH_H_INCLUDED