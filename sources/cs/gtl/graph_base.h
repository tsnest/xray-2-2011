////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_base.h
//	Created 	: 14.01.2004
//  Modified 	: 19.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph base class template
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_GRAPH_BASE_H_INCLUDED
#define CS_GTL_GRAPH_BASE_H_INCLUDED

#include <cs/gtl/intrusive_ptr.h>

namespace fs {
	namespace readers {class base;}
	namespace writers {class base;}
	typedef ::gtl::intrusive_ptr<readers::base>	reader;
	typedef ::gtl::intrusive_ptr<writers::base>	writer;
};

// TODO:
//		1. add container_type for holding graph vertices
//		2. add container_type for holding graph edges in graph_vertex
//		3. add container_type for holding back references in graph_vertex
//		4. specialize removals for different containers (back() instead of begin()) in graph
//		5. specialize removals for different containers (back() instead of begin()) in graph_vertex for edges
//		6. specialize removals for different containers (back() instead of begin()) in graph_vertex for vertices
//		7. add edge_id type
//		8. add safe save using chunks
//		9. specialize save for different graphs
//			a. if graph is discharged enough - save edges by vertex id
//			b. if graph is not discharged - save edges by a generic way

namespace gtl { namespace detail {

template <
	typename _edge_weight_type,
	typename _vertex_id_type,
	typename _graph_vertex_type,
	typename _graph_edge_type,
	typename inherited
>
class graph_base : 
//	public 
//		pure_serialize_object<
//			fs::reader,
//			fs::writer
//		>,
	public 
		inherited
{
public:
	typedef _graph_vertex_type						vertex_type;
	typedef _graph_edge_type						edge_type;

public:
	typedef cs_map<_vertex_id_type,vertex_type*>	VERTICES;
	typedef typename vertex_type::EDGES				EDGES;

public:
	typedef typename VERTICES::const_iterator		const_vertex_iterator;
	typedef typename VERTICES::iterator				vertex_iterator;
	typedef typename EDGES::const_iterator			const_edge_iterator;
	typedef typename EDGES::iterator				edge_iterator;

private:
	VERTICES					m_vertices;

protected:
	inline	void				add_vertex			(_vertex_id_type const& vertex_id, vertex_type* new_vertex);

public:
	inline						graph_base			();
	virtual						~graph_base			();
//	virtual void				save				(fs::writer stream);
//	virtual void				load				(fs::reader stream);
	inline	bool				operator==			(graph_base const& obj) const;
	inline	void				clear				();
	inline	void				remove_vertex		(_vertex_id_type const& vertex_id);
	inline	void				add_edge			(_vertex_id_type const& vertex_id0, _vertex_id_type const& vertex_id1, _edge_weight_type const& edge_weight);
	inline	void				add_edge			(_vertex_id_type const& vertex_id0, _vertex_id_type const& vertex_id1, _edge_weight_type const& edge_weight0, _edge_weight_type const& edge_weight1);
	inline	void				remove_edge			(_vertex_id_type const& vertex_id0, _vertex_id_type const& vertex_id1);
	inline	size_t				vertex_count		() const;
	inline	size_t				edge_count			() const;
	inline	bool				empty				() const;
	inline	VERTICES const&		vertices			() const;
	inline	vertex_type const*	vertex				(_vertex_id_type const& vertex_id) const;
	inline	vertex_type*			vertex				(_vertex_id_type const& vertex_id);
	inline	edge_type const*		edge				(_vertex_id_type const& vertex_id0, _vertex_id_type const& vertex_id1) const;
	inline	edge_type*			edge				(_vertex_id_type const& vertex_id0, _vertex_id_type const& vertex_id1);
	inline	graph_base const&	header				() const;
};

};};

#include <cs/gtl/graph_base_inline.h>

#endif // #ifndef CS_GTL_GRAPH_BASE_H_INCLUDED