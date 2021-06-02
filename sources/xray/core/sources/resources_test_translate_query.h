////////////////////////////////////////////////////////////////////////////
//	Created		: 29.04.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCES_TEST_TRANSLATE_QUERY_H_INCLUDED
#define RESOURCES_TEST_TRANSLATE_QUERY_H_INCLUDED

namespace xray {
namespace resources {

class test_translate_query_cook : public translate_query_cook, public test_cook_base
{
public:
	test_translate_query_cook(reuse_enum reuse_type, bool translate_in_current_thread) : 
			test_cook_base(generate_if_no_file_test_unset),
			m_translate_in_current_thread(translate_in_current_thread),
			translate_query_cook(test_resource_class, reuse_type, translate_in_current_thread ? threading::current_thread_id() : use_resource_manager_thread_id) {}

			void					on_sub_resources_loaded	(queries_result & children)
			{
				query_result_for_cook * const parent	=	children.get_parent_query();
				
				mutable_buffer raw_file_data				(NULL, 0);
				test_unmanaged_resource * const out_result	=	MT_NEW(test_unmanaged_resource)
																(parent->get_requested_path(),
																 raw_file_data,
																 creation_translate_query);

				out_result->test_consistency				();
				parent->set_unmanaged_resource				(out_result, nocache_memory, sizeof(test_unmanaged_resource));
				parent->finish_query						(result_success);
			}

	virtual	void					translate_query			(query_result_for_cook &	parent)
	{
		request sub_query[] = {	
			{ "test/test.db/folder1/2-866.no_inline",		raw_data_class	},
     		{ "test/test.db/folder2/1-1260.no_inline",		raw_data_class	},
		};

		query_resources					(sub_query, 
										 boost::bind(& test_translate_query_cook::on_sub_resources_loaded, this, _1), 
										 parent.get_user_allocator(), 
										 NULL,
										 & parent);
	}

	virtual void					delete_resource	(resource_base * resource)
	{
		test_unmanaged_resource * test_resource	=	static_cast_checked<test_unmanaged_resource *>(resource);
		MT_DELETE									(test_resource);
	}

private:
	bool							m_translate_in_current_thread;
};

} // namespace resources
} // namespace xray

#endif // #ifndef RESOURCES_TEST_TRANSLATE_QUERY_H_INCLUDED