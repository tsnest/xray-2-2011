////////////////////////////////////////////////////////////////////////////
//	Module 		: quad_tree_inline.h
//	Created 	: 12.05.2005
//  Modified 	: 12.05.2005
//	Author		: Dmitriy Iassenev
//	Description : quad tree template class inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_QUAD_TREE_INLINE_H_INCLUDED
#define CS_GTL_QUAD_TREE_INLINE_H_INCLUDED

#define TEMPLATE_SPECIALIZATION\
	template <\
		typename _object_type,\
		typename _quad_node_type,\
		template <typename> class _quad_list_item_type,\
		template <typename> class _storage_type\
	>

#define _quad_tree\
	gtl::quad_tree<\
		_object_type,\
		_quad_node_type,\
		_quad_list_item_type,\
		_storage_type\
	>

TEMPLATE_SPECIALIZATION
inline	_quad_tree::quad_tree				(aabb3f const& box, float const min_cell_size, u32 const max_node_count, u32 const max_list_item_count)
{
	m_leaf_count				= 0;
	m_radius					= _max(box.max.x - box.min.x, box.max.z - box.min.z)*.5f;
	m_center.add				(box.min,box.max);
	m_center.mul				(.5f);

	ASSERT						(!fp_zero(min_cell_size), "minimum cell size is zero");
	ASSERT						(m_radius > min_cell_size, "radius is less or equal than minimum cell size");
	m_max_depth					= i_floor(log(2.f*m_radius/min_cell_size)/log(2.f) + .5f);

	m_nodes						= cs_new<quad_node_storage>(max_node_count);
	m_list_items				= cs_new<list_item_storage>(max_list_item_count);
	m_root						= 0;
}

TEMPLATE_SPECIALIZATION
_quad_tree::~quad_tree						()
{
	gtl::destroy				(m_nodes);
	gtl::destroy				(m_list_items);
}

TEMPLATE_SPECIALIZATION
inline	void _quad_tree::clear				()
{
	m_nodes->clear				();
	m_list_items->clear			();
	m_root						= 0;
	m_leaf_count				= 0;
}

TEMPLATE_SPECIALIZATION
inline	size_t _quad_tree::size				() const
{
	return						m_leaf_count;
}

TEMPLATE_SPECIALIZATION
inline	u32	_quad_tree::neighbour_index		(vec3f const& position, vec3f& center, float const distance) const
{
	if (position.x <= center.x)
		if (position.z <= center.z) {
			center.x			-= distance;
			center.z			-= distance;
			return				0;
		}
		else {
			center.x			-= distance;
			center.z			+= distance;
			return				1;
		}
	else
		if (position.z <= center.z) {
			center.x			+= distance;
			center.z			-= distance;
			return				2;
		}
		else {
			center.x			+= distance;
			center.z			+= distance;
			return				3;
		}
}

TEMPLATE_SPECIALIZATION
inline	void _quad_tree::insert				(_object_type* object)
{
	vec3f						center = m_center;
	float						distance = m_radius;
	_quad_node_type				**node = &m_root;
	for (int depth = 0; ; ++depth) {
		if (depth == m_max_depth) {
			_quad_list_item_type*list_item = m_list_items->object();
			list_item->m_object	= object;
			list_item->m_next	= (_quad_list_item_type*)((void*)(*node));
			*node				= (_quad_node_type*)((void*)list_item);
			++m_leaf_count;
			return;
		}

		if (!*node)
			*node				= m_nodes->object();

		distance				*= .5f;
		u32						index = neighbour_index(object->position(),center,distance);
		ASSERT					(index < 4, "navigation graph is corrupted");
		
		node					= (*node)->m_neighbours + index;
	}
}

TEMPLATE_SPECIALIZATION
inline	_object_type* _quad_tree::find		(vec3f const& position)
{
	vec3f						center = m_center;
	float						distance = m_radius;
	_quad_node_type*				node = m_root;
	for (int depth = 0; ; ++depth) {
		if (!node)
			return				0;

		distance				*= .5f;
		u32						index = neighbour_index(position,center,distance);
		ASSERT					(index < 4);

		if (depth == m_max_depth) {
			_quad_list_item_type*	leaf = ((_quad_list_item_type*)((void*)(node)));
			for ( ; leaf; leaf = leaf->m_next)
				if (leaf->m_object->position().similar(position))
					return		leaf->m_object;
			return				0;
		}

		node					= node->m_neighbours[index];
	}
}

TEMPLATE_SPECIALIZATION
inline	void _quad_tree::nearest			(vec3f const& position, float const radius, OBJECTS& objects, bool const clear) const
{
	if (clear)
		objects.clear			();
	nearest						(position,radius,objects,m_root,m_center,m_radius,0);
}

TEMPLATE_SPECIALIZATION
inline	void _quad_tree::nearest			(vec3f const& position, float const radius, OBJECTS& objects, _quad_node_type* node, vec3f const& center, float distance, int const depth) const
{
	if (!node)
		return;

	if (depth == m_max_depth) {
		float					radius_sqr = _sqr(radius);
		_quad_list_item_type*	leaf = ((_quad_list_item_type*)((void*)(node)));
		for ( ; leaf; leaf = leaf->m_next) {
			vec3f				position0 = leaf->m_object->position();
			if (_sqr(position0.x - position.x) + _sqr(position0.z - position.z) <= radius_sqr)
				objects.push_back	(leaf->m_object);
		}
		return;
	}

	distance					*= .5f;
	vec3f						next_center = center;
	u32							index = neighbour_index(position,next_center,distance);
	ASSERT						(index < 4, "algorithm error");
	if (_abs(position.z - center.z) < radius) {
		if (_abs(position.x - center.x) < radius) {
			if (_sqr(position.z - center.z) + _sqr(position.x - center.x) < _sqr(radius)) {
				nearest			(position,radius,objects,node->m_neighbours[0],next_center.set(center.x - distance,center.y,center.z - distance),distance,depth + 1);
				nearest			(position,radius,objects,node->m_neighbours[1],next_center.set(center.x - distance,center.y,center.z + distance),distance,depth + 1);
				nearest			(position,radius,objects,node->m_neighbours[2],next_center.set(center.x + distance,center.y,center.z - distance),distance,depth + 1);
				nearest			(position,radius,objects,node->m_neighbours[3],next_center.set(center.x + distance,center.y,center.z + distance),distance,depth + 1);
				return;
			}

			nearest				(position,radius,objects,node->m_neighbours[index],next_center,distance,depth + 1);

			if (position.z > center.z) {
				if (index&  1)
					nearest		(position,radius,objects,node->m_neighbours[index == 1 ? 0 : 2],next_center.set(center.x + (index == 1 ? -1 : 1)*distance,center.y,center.z - distance),distance,depth + 1);
			}
			else
				if (!(index&  1))
					nearest		(position,radius,objects,node->m_neighbours[!index ? 1 : 3],next_center.set(center.x + (!index ? -1 : 1)*distance,center.y,center.z + distance),distance,depth + 1);

			if (position.x > center.x) {
				if (index > 1)
					nearest		(position,radius,objects,node->m_neighbours[index == 2 ? 0 : 1],next_center.set(center.x - distance,center.y,center.z + (index == 2 ? -1 : 1)*distance),distance,depth + 1);
			}
			else
				if (index < 2)
					nearest		(position,radius,objects,node->m_neighbours[!index ? 2 : 3],next_center.set(center.x + distance,center.y,center.z + (!index ? -1 : 1)*distance),distance,depth + 1);

			return;
		}
		else {
			nearest				(position,radius,objects,node->m_neighbours[index],next_center,distance,depth + 1);
			
			if (position.z > center.z) {
				if (index&  1)
					nearest		(position,radius,objects,node->m_neighbours[index == 1 ? 0 : 2],next_center.set(center.x + (index == 1 ? -1 : 1)*distance,center.y,center.z - distance),distance,depth + 1);
			}
			else
				if (!(index&  1))
					nearest		(position,radius,objects,node->m_neighbours[!index ? 1 : 3],next_center.set(center.x + (!index ? -1 : 1)*distance,center.y,center.z + distance),distance,depth + 1);

			return;
		}
	}
	else {
		nearest					(position,radius,objects,node->m_neighbours[index],next_center,distance,depth + 1);

		if (_abs(position.x - center.x) < radius)
			if (position.x > center.x) {
				if (index > 1)
					nearest		(position,radius,objects,node->m_neighbours[index == 2 ? 0 : 1],next_center.set(center.x - distance,center.y,center.z + (index == 2 ? -1 : 1)*distance),distance,depth + 1);
			}
			else
				if (index < 2)
					nearest		(position,radius,objects,node->m_neighbours[!index ? 2 : 3],next_center.set(center.x + distance,center.y,center.z + (!index ? -1 : 1)*distance),distance,depth + 1);
	}
}

TEMPLATE_SPECIALIZATION
inline	_object_type* _quad_tree::remove	(_object_type const* object)
{
	return						remove(object,m_root,m_center,m_radius,0);
}

TEMPLATE_SPECIALIZATION
inline	_object_type* _quad_tree::remove	(_object_type const* object, _quad_node_type* &node, vec3f const& center, float const distance, int const depth)
{
	ASSERT						(node);
	if (depth == m_max_depth) {
		_quad_list_item_type*	leaf = ((_quad_list_item_type*)((void*)(node)));
		_quad_list_item_type*	leaf_prev = 0;
		for ( ; leaf; leaf_prev = leaf, leaf = leaf->m_next)
			if (leaf->m_object == object) {
				if (!leaf_prev)
					node = 0;
				else
					leaf_prev->m_next = leaf->m_next;
				_object_type*	_object = leaf->m_object;
				m_list_items->remove(leaf);
				--m_leaf_count;
				return			_object;
			}
		NODEFAULT;
	}

	float						new_distance = distance*.5f;
	vec3f						new_center = center;
	u32							index = neighbour_index(object->position(),new_center,new_distance);
	ASSERT						(index < 4);
	_object_type*				_object = remove(object,node->m_neighbours[index],new_center,new_distance,depth + 1);
	if (node->m_neighbours[index] || node->m_neighbours[0] || node->m_neighbours[1] || node->m_neighbours[2] || node->m_neighbours[3])
		return					_object;
	m_nodes->remove				(node);
	return						_object;
}

TEMPLATE_SPECIALIZATION
inline	void _quad_tree::all				(OBJECTS& objects, _quad_node_type* node, int const depth) const
{
	if (!node)
		return;

	if (depth == m_max_depth) {
		_quad_list_item_type*	leaf = ((_quad_list_item_type*)((void*)(node)));
		for ( ; leaf; leaf = leaf->m_next)
			objects.push_back	(leaf->m_object);
		return;
	}

	all							(objects,node->m_neighbours[0],depth + 1);
	all							(objects,node->m_neighbours[1],depth + 1);
	all							(objects,node->m_neighbours[2],depth + 1);
	all							(objects,node->m_neighbours[3],depth + 1);
}

TEMPLATE_SPECIALIZATION
inline	void _quad_tree::all				(OBJECTS& objects, bool const clear) const
{
	if (clear)
		objects.clear			();
	all							(objects,m_root,0);
}

TEMPLATE_SPECIALIZATION
inline	size_t _quad_tree::node_count		() const
{
	return						m_nodes->count();
}

TEMPLATE_SPECIALIZATION
inline	size_t _quad_tree::leaf_count		() const
{
	return						m_list_items->count();
}

#undef TEMPLATE_SPECIALIZATION
#undef _quad_tree

#endif // #ifndef CS_GTL_QUAD_TREE_INLINE_H_INCLUDED