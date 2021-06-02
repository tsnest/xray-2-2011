////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RESOURCES_INTRUSIVE_BASE_H_INCLUDED
#define XRAY_RESOURCES_INTRUSIVE_BASE_H_INCLUDED

#include <xray/debug/signalling_bool.h>

namespace xray {
namespace resources {

class managed_resource;
class unmanaged_resource;

class XRAY_CORE_API base_of_intrusive_base
{
public:
	base_of_intrusive_base	() : m_reference_count (0), m_flags(0) {}
	long					get_reference_count		()	const	{ return m_reference_count; }

	bool					is_captured	() const { return m_flags.has(flag_is_captured); }
	bool					is_pinned_for_safe_destroying		() const { return m_flags.has(flag_pinned_for_safe_destroying); }

private:
	template				<class Resource>
	signalling_bool			try_unregister_from_fat_or_from_name_registry	(Resource * const object, u32 count_that_allows_unregister = 0) const;

	bool					is_associated_with_fat	(managed_resource * const object) const;
	bool					is_associated_with_fat	(unmanaged_resource * const object) const;

	void					pin_reference_count_for_safe_destroying				();
	void					pin_reference_count_for_query_finished_callback		();
	bool					was_pinned_for_query_finished_callback				() const { return m_flags.has(flag_was_pinned_by_query_finished_callback); }
	void					unpin_reference_count_for_query_finished_callback	();
	void					on_capture_increment_reference_count				();
	void					on_release_decrement_reference_count				();

protected:
	threading::atomic32_type	m_reference_count;

private:
	
	enum flag_pinned_by_enum {
		flag_is_captured							=	1 << 0,
		flag_is_pinned_by_query_finished_callback	=	1 << 1,
		flag_was_pinned_by_query_finished_callback	=	1 << 2,
		flag_pinned_for_safe_destroying				=	1 << 3,
	};

	flags_type<flag_pinned_by_enum, threading::multi_threading_policy>	m_flags;

	friend	class			threading::multi_threading_policy;
	friend	class			threading::single_threading_policy;
	friend	class			managed_resource;
	friend	class			unmanaged_resource;
	friend	class			resources_manager;
	friend	class			resource_base;
	friend	class			game_resources_manager;
	friend	class			resources_to_free_collection;
	friend	class			resource_freeing_functionality;
	friend	class			releasing_functionality;
	friend	class			managed_intrusive_base;
	friend	class			unmanaged_intrusive_base;
	friend	class			query_result;
	friend	class			queries_result;
	friend  class			cook_base;
};

class resource;

class XRAY_CORE_API managed_intrusive_base : public base_of_intrusive_base
{
public:
	void		destroy		(managed_resource * object)	const;
};

class unmanaged_resource;

class XRAY_CORE_API unmanaged_intrusive_base : public base_of_intrusive_base
{
public:
	void		destroy		(unmanaged_resource* object)	const;
};

} // namespace resources
} // namespace xray

#endif // #ifndef XRAY_RESOURCES_INTRUSIVE_BASE_H_INCLUDED