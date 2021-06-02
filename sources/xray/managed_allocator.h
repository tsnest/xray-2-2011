#ifndef RESOURCES_MANAGED_ALLOCATOR_H_INCLUDED
#define RESOURCES_MANAGED_ALLOCATOR_H_INCLUDED

#include <xray/memory_base_allocator.h>
#include <xray/managed_allocator_base.h>
#include <xray/memory_debug_parameters.h>

namespace xray		{
namespace memory	{

class	managed_node;
enum	managed_node_type;
// notes about defragmentations
// to improve speed of defragmentations:
// 1. increase reserved_space to make defragmentations less frequent
// 2. more optimal selection of which blocks to move
//    i.e.:	don't move large blocks on small distances if it can be avoided
// 3. use faster memory copy routine
class managed_allocator : public managed_allocator_base, public memory::base_allocator
{
	typedef managed_allocator_base	super;

public:
	// note: when defragmentation starts, it first marks some nodes of device managers as unmovable,
	// so they can read in parallel with defragmentation
	static const u32	max_parallel_with_defragmentation_file_operations_allowed	=	10;

public:
						// temp_arena_size parameter is also max allocation request 
						// more reserved_size makes defragmentations less frequent
						managed_allocator		(u32 			temp_arena_size,
												 u32 			reserved_size, 
#if XRAY_PLATFORM_WINDOWS_64
												 u32 			granularity = 128
#else // #if XRAY_PLATFORM_WINDOWS_64
												 u32 			granularity = 128
#endif // #if XRAY_PLATFORM_WINDOWS_64
												 );

						~managed_allocator 	() {}

	bool				can_allocate					(u32 buffer_size) const;
	managed_node *		allocate					 	(u32 size);

	void				deallocate						(managed_node *	ptr);
	void				defragment						(u32				sufficient_contigous_space);
	bool				is_defragmenting				() const { return m_defragmenting; }

	void				test_defragment					();
	
	void				notify_unmovable_changed		(managed_node *		node);

	virtual	size_t		total_size						() const { return managed_allocator_base::total_size(); }
	virtual	size_t		allocated_size					() const { return managed_allocator_base::total_size() - 
																		  managed_allocator_base::get_free_size(); }

	virtual	size_t		usable_size_impl				(pvoid pointer) const;

private:
	virtual	void		initialize_impl					(pvoid arena, u64 size, pcstr arena_id);
	virtual	void		finalize_impl					() {}

	virtual	pvoid		call_malloc						(size_t size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION);
	virtual	pvoid		call_realloc					(pvoid pointer, size_t new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION);
	virtual	void		call_free						(pvoid pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION);

private:
	enum arena_t										{ main_arena, temp_arena };
	static 
	managed_node *		find_node_of_type				(managed_node *		start_node,
														 managed_node_type	type);

	managed_node *		allocate_in_node_defrag			(size_t const		full_size, 
														 managed_node *		node,
														 size_t const		offset_in_node,
														 managed_node *		prev_free_node);

	managed_node *		deallocate_defrag				(managed_node *		ptr);
	managed_allocator_base & get_node_arena				(managed_node *		node);

	void				log_defragmenter_state			() const;
	void				log_defragmenter_nodes			(managed_allocator_base const & alloc) const;

	void				free_unpinned					(arena_t			arena);
	void				transfer_resource				(managed_node *		dest_node, 
														 managed_node *		src_node);

	void				add_unpin_looked_node			(managed_node *		node);

	managed_node *		allocate_duplicate_in_main_arena(managed_node *		src_node,
														 managed_node *		dest_node,
														 managed_node * &	depend_node);
	managed_node *		get_next_defragmentation_node	(managed_node *		cur_node);

	void				set_dependence					(managed_node *		depend_node, 
														 managed_node *		wait_for_free);
	bool				is_waiting_for_unpin			(managed_node * 	node) const;

private:
	struct unmovable
	{
		managed_node *	node;
		enum			{ undefined, waited, ignored }	state;
	};
	
	struct unmovables_less
	{
		bool operator () (unmovable const& u1, unmovable const& u2) const
		{
			return						u1.node < u2.node;
		}
	};

	managed_node *		to_next_ignored_unmovable		(u32 &					index);
	u32					calculate_place_pos_and_free_space(buffer_vector<u32>&	looses,
														 u32 const				start_index,
														 pbyte					place_pos,
														 u32 &					result_index);

	void				calculate_unmovables_state		(u32 					start_index,
														 pbyte					place_pos,
														 u32					sufficient_contigous_space);

	threading::mutex &	get_defragmentation_mutex		() { return m_defragmentation_mutex; }

	typedef fixed_vector<unmovable, 1024 + max_parallel_with_defragmentation_file_operations_allowed>	unmovables_t;
	unmovables_t										m_unmovables;
	threading::mutex									m_unmovables_mutex;
	threading::mutex									m_defragmentation_mutex;

	struct mode_state
	{
		enum			change_place_t					{ dont_change_place, change_place };
		managed_node *	node;
		managed_node *	place_node;
		pbyte			place_pos;
		u32				unmovable_index;
		u32				unmovable_place_index;
	};

	void				correct_place_pos_and_node		(mode_state * 						mode);

	void				init_mode						(mode_state *						mode);

	size_t 				get_contigous_space_defrag		(u32								unmovable_index);

	void				move_next_mode_node				();
	void				handle_unmovable				(u32 const	sufficient_contigous_space,
														 bool &		call_continue_in_main_cycle);

	void				calculate_nodes_place_pos_and_unmovable_state (u32 sufficient_contigous_space);

	//----------------------------------------------------
	// testing
	//----------------------------------------------------
	bool				check_valid_place_node			() const;
	void				test							();
	void				test_speed						();
	void				remove_test_resource			(u32 index);
#if !XRAY_PLATFORM_PS3
	void				test_unmovable_init				(u32 start_ms);
	void				test_unmovable_unlock_expired	(u32 cur_ms);
	void				test_unmovables_list_valid		() const;
#endif // #if !XRAY_PLATFORM_PS3
	void				log_test_resources				() const;



	bool												m_defragmenting;
	u32													m_reserved_size;
	mode_state											m_main_mode;
	mode_state											m_no_temp_mode;
	mode_state *										m_mode;

	size_t												m_num_moved_bytes;
	u64													m_whole_moved_bytes;

	u32													m_defrag_iteration;
	mutable u32											m_log_iteration;

	buffer_vector<unmovable> *							m_defrag_unmovables;

	timing::timer										m_defragment_timer;
	managed_allocator_base								m_temp_arena;

	pcstr												m_arena_id;

	friend class										managed_node_owner;
};

} // namespace memory
} // namespace xray

#endif // #ifndef RESOURCES_MANAGED_ALLOCATOR_H_INCLUDED