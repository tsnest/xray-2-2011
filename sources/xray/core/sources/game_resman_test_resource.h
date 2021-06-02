////////////////////////////////////////////////////////////////////////////
//	Created		: 13.09.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef GAME_RESMAN_TEST_RESOURCE_H_INCLUDED
#define GAME_RESMAN_TEST_RESOURCE_H_INCLUDED

namespace xray {
namespace resources {

class		test_resource;
typedef 	resource_ptr<test_resource, unmanaged_intrusive_base>	test_resource_ptr;

class test_resource : public positional_unmanaged_resource
{
public:
				test_resource		(u32 allocator, pcstr name, u32 size, u32 quality_levels_count = 1);
			   ~test_resource		();	

	u32			size				() const { return m_size; }
	pcstr		name				() const { return m_name.c_str(); }
	u32			int_name			() const { return m_int_name; }
	u32			allocator			() const { return m_allocator; }

	log_string_type	log_string		() const { return name(); }

	u32			children_count		() const;
	bool		add_child			(test_resource_ptr child);
	bool		has_child			(test_resource_ptr child) const;

private:
	test_resource_ptr::child_type	m_children[3];
	fixed_string<128>				m_name;
	u32								m_size;
	u32								m_int_name;
	u32								m_allocator;
};

static u32 const min_resource_size	=	sizeof(test_resource);
static u32 const max_resource_size	=	min_resource_size * 4;

} // namespace resources
} // namespace xray

#endif // #ifndef GAME_RESMAN_TEST_RESOURCE_H_INCLUDED