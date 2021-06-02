////////////////////////////////////////////////////////////////////////////
//	Created		: 27.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef GAME_RESOURCES_MANAGER_H_INCLUDED
#define GAME_RESOURCES_MANAGER_H_INCLUDED

#include <xray/resources.h>
#include <xray/resources_callbacks.h>
#include <xray/intrusive_double_linked_list.h>
#include <boost/intrusive/set.hpp>
#include "resources_helper.h"

namespace xray {
namespace resources {

class resource_base_compare
{
public:
	bool	operator () (resource_base const & left, resource_base const & right) const;
};

typedef boost::intrusive::member_hook
	< 
		resource_base, 
		resource_tree_member_hook,
		& resource_base::tree_hook 
	> resource_tree_helper_option;

typedef	boost::intrusive::set< resource_base, 
							   resource_tree_helper_option, 
							   boost::intrusive::compare< resource_base_compare >,
							   boost::intrusive::constant_time_size<false> >	resource_tree_type;

class game_resources_manager 
{
public:
	void				query_finished_callback					(resource_base * resource);
	out_of_memory_treatment_enum	out_of_memory_callback		(memory_usage const & memory_needed);

	void				erase									(resource_base * resource);
	void				on_resource_leaked_callback				(resource_base * resource);
	bool				try_free								(resource_base * resource);
	
private:
	struct memory_type_and_pool
	{
		memory_type const *	type;
		u32					pool;
		memory_type_and_pool	(memory_type const * type, u32 pool) : type(type), pool(pool) {}
	};

	struct memory_type_and_pool_compare
	{
		bool operator () (memory_type_and_pool const & left, memory_type_and_pool const & right) const;
	};

	typedef intrusive_list< query_result, query_result, & query_result::m_next_out_of_memory > out_of_memory_queue;

	struct resources_info
	{
		resource_tree_type *	tree;
		u32						async_free;
		resources_info	() :	tree(NULL), async_free(0) { }
		out_of_memory_queue *	queue;
	};

	typedef	associative_vector<memory_type_and_pool, resources_info, vector, memory_type_and_pool_compare>	trees_type;
	trees_type								m_trees;

}; // class game_resources_manager

void   initialize_game_resources_manager	();
void   finilize_game_resources_manager		();

} // namespace resources
} // namespace xray

#endif // #ifndef GAME_RESOURCES_MANAGER_H_INCLUDED