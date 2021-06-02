////////////////////////////////////////////////////////////////////////////
//	Created		: 09.08.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCE_QUALITY_FUNCTIONALITY_H_INCLUDED
#define RESOURCE_QUALITY_FUNCTIONALITY_H_INCLUDED

#include <xray/resources_resource_children.h>
#include <boost/intrusive/rbtree.hpp>

namespace xray {
namespace resources {

typedef boost::intrusive::set_member_hook< boost::intrusive::link_mode<boost::intrusive::auto_unlink> >		resource_tree_member_hook;

class	resource_base;
class	query_result_for_cook;

class XRAY_CORE_API resource_quality : public resource_children
{
public:	
						resource_quality				(class_id_enum class_id, u32 quality_levels_count, resource_flags_enum flags);

	u32					quality_levels_count			() const { return m_quality_levels_count; }
	bool				is_resource_with_quality		() const { return m_quality_levels_count != 1; }
	u32					child_quality					(resource_base * child) const;
	u32					current_quality_level			() const;
	u32					target_quality_level			() const { return m_target_quality_level; }
	float				current_satisfaction			() const { return m_current_satisfaction; }
	float				target_satisfaction				() const { return m_target_satisfaction; }
	void				increase_quality				(u32 target_quality, float target_satisfaction, query_result_for_cook * parent_query);
	float				satisfaction					(u32 quality_level, resource_base const * resource_user = NULL, u32 positional_users_count = 0) const;
	class_id_enum		get_class_id					() const { return m_class_id; }

protected:
	float				satisfaction_with				(u32 quality_level, math::float4x4 const & user_matrix, u32 users_count) const;
	virtual void		decrease_quality				(u32 new_best_quality) { XRAY_UNREFERENCED_PARAMETER(new_best_quality); NOT_IMPLEMENTED(); }
	virtual void		increase_quality_to_target		(query_result_for_cook * parent_query) { XRAY_UNREFERENCED_PARAMETER(parent_query); NOT_IMPLEMENTED(); }
	virtual	bool		is_increasing_quality			() const { NOT_IMPLEMENTED(return false); }

	virtual void		link_child_resource				(resource_base * child, u32 quality = resource_link::not_a_quality_link);
	virtual void		unlink_child_resource			(resource_base * child);
	void				set_target_satisfaction			(float target_satisfaction);
private:
	u32						positional_users_count		(resource_base const * resource_user) const;
	u32					calculate_max_child_quality_level	() const;
	
	float				update_satisfaction				(u64 update_tick);
	float					update_satisfaction_for_resource_with_quality	();	
	float					calculate_best_satisfaction_based_on_size		() const;

	bool				is_quality_resource				() const { return has_flags(resource_flag_is_quality_resource); }
	bool				is_on_quality_branch			() const;

	void				set_last_fail_of_increasing_quality	(float time_offset) { m_last_fail_of_increasing_quality = time_offset; }
	float				last_fail_of_increasing_quality	() const { return m_last_fail_of_increasing_quality; } 
	void				set_target_quality				(u32 quality_level, float target_satisfaction);

protected:
	memory_usage_type	m_memory_usage_self;
	u64					m_current_satisfaction_update_tick;
	resource_base *		m_next_in_increase_quality_queue;
	u32					m_quality_levels_count;
	float				m_current_satisfaction;
	float				m_target_satisfaction;
	float				m_last_fail_of_increasing_quality;
	u32					m_current_quality_level;
	u32					m_target_quality_level;
	class_id_enum		m_class_id;

	friend	class		compare_by_current_satisfaction;
	friend	class		compare_by_target_satisfaction;
	friend	class		quality_decreasing_functionality;
	friend	class		quality_increase_functionality;
	friend	class		game_resources_manager;
};

float	min_satisfaction_of_unreferenced_resource();
float	max_satisfaction_of_unreferenced_resource();

} // namespace resources
} // namespace xray

#endif // #ifndef RESOURCE_QUALITY_FUNCTIONALITY_H_INCLUDED