////////////////////////////////////////////////////////////////////////////
//	Created		: 15.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_SEARCH_RESTRICTOR_GENERIC_H_INCLUDED
#define XRAY_AI_SEARCH_SEARCH_RESTRICTOR_GENERIC_H_INCLUDED

#include <xray/ai/search/search_restrictor_base.h>

namespace xray {
namespace ai {
namespace search_restrictor {

template < typename vertex_id_type, typename const_edge_iterator, typename distance_type >
class generic :
	public base< vertex_id_type, const_edge_iterator, distance_type >,
	private boost::noncopyable
{
public:
	inline						generic				(
									vertex_id_type const& start_vertex_id,
									vertex_id_type const& target_vertex_id,
									distance_type const& max_range = std::numeric_limits< distance_type >::max(),
									u32 const max_iteration_count = u32(-1),
									u32 const max_visited_vertex_count = u32(-1)
								);

public:
	inline bool						target_reached	( vertex_id_type const& vertex_id ) const;

public:
	inline vertex_id_type const&	start_vertex_id	( u32 const index = u32(-1) ) const;
	inline vertex_id_type const&	target_vertex_id( ) const;

private:
	typedef base< vertex_id_type, const_edge_iterator, distance_type >	super;

private:
	vertex_id_type const&		m_start_vertex_id;
	vertex_id_type const&		m_target_vertex_id;
}; // class generic

} // namespace search_restrictor
} // namespace ai
} // namespace xray

#include <xray/ai/search/search_restrictor_generic_inline.h>

#endif // #ifndef XRAY_AI_SEARCH_SEARCH_RESTRICTOR_GENERIC_H_INCLUDED