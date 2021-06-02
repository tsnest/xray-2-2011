////////////////////////////////////////////////////////////////////////////
//	Module 		: quad_tree.h
//	Created 	: 12.05.2005
//  Modified 	: 12.05.2005
//	Author		: Dmitriy Iassenev
//	Description : quad tree template class
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_QUAD_TREE_H_INCLUDED
#define CS_GTL_QUAD_TREE_H_INCLUDED

#include <cs/gtl/quad_node.h>
#include <cs/gtl/quad_list_item.h>
#include <cs/gtl/quad_fixed_storage.h>

namespace gtl {

template <
	typename _object_type,
	typename _quad_node_type = detail::quad_node,
	template <typename> class _quad_list_item_type = detail::quad_list_item,
	template <typename> class _storage_type = detail::fixed_storage
>
class quad_tree {
public:
	typedef _object_type						_object_type;
	typedef _quad_node_type						_quad_node_type;
	typedef _quad_list_item_type<_object_type>	_quad_list_item_type;
	typedef _storage_type<_quad_node_type>		quad_node_storage;
	typedef _storage_type<_quad_list_item_type>	list_item_storage;

public:
	typedef cs_vector<_object_type*>			OBJECTS;

private:
	vec3f						m_center;
	float						m_radius;
	int							m_max_depth;
	_quad_node_type*				m_root;
	quad_node_storage*			m_nodes;
	list_item_storage*			m_list_items;
	size_t						m_leaf_count;

protected:
	inline		u32				neighbour_index	(vec3f const& position, vec3f& center, float const distance) const;
	inline		void			nearest			(vec3f const& position, float const radius, OBJECTS& objects, _quad_node_type* node, vec3f const& center, float distance, int const depth) const;
	inline		_object_type*	remove			(_object_type const* object, _quad_node_type* &node, vec3f const& center, float const distance, int const depth);
	inline		void			all				(OBJECTS& objects, _quad_node_type* node, int const depth) const;

public:
	inline						quad_tree		(aabb3f const& box, float const min_cell_size, u32 const max_node_count, u32 const max_list_item_count);
	inline						~quad_tree		();
	inline		void			clear			();
	inline		void			insert			(_object_type* object);
	inline		_object_type*	remove			(_object_type const* object);
	inline		_object_type*	find			(vec3f const& position);
	inline		void			nearest			(vec3f const& position, float const radius, OBJECTS& objects, bool const clear = true) const;
	inline		void			all				(OBJECTS& objects, bool const clear = true) const;
	inline		size_t			size			() const;
	inline		size_t			node_count		() const;
	inline		size_t			leaf_count		() const;
};

};

#include <cs/gtl/quad_tree_inline.h>

#endif // #ifndef CS_GTL_QUAD_TREE_H_INCLUDED