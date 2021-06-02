////////////////////////////////////////////////////////////////////////////
//	Created		: 06.12.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef FS_IMPL_FINDING_ASYNC_CALLBACKS_DATA_H_INCLUDED
#define FS_IMPL_FINDING_ASYNC_CALLBACKS_DATA_H_INCLUDED

namespace xray {
namespace fs {

struct	async_callbacks_data
{
	u32						callbacks_count;
	u32						callbacks_called_count;
	bool					result;
	bool					all_queries_done;
	find_results_struct *	find_results;

	async_callbacks_data	(find_results_struct * find_results) 
		: find_results(find_results), callbacks_count(0), callbacks_called_count(0), result(true), all_queries_done(false) 
	{
		R_ASSERT						(find_results);
	}

	void  on_callback_may_destroy_this	()
	{
		++callbacks_called_count;
		try_finish_may_destroy_this		();
	}

	void  try_finish_may_destroy_this	()
	{
		if ( !all_queries_done )
			return;
		if ( callbacks_called_count != callbacks_count )
			return;

		find_results->callback				(find_results->pin_iterator);
		
		async_callbacks_data * this_ptr	=	this;
		XRAY_DELETE_IMPL					(& memory::g_resources_helper_allocator, this_ptr);
	}
};

} // namespace fs
} // namespace xray

#endif // #ifndef FS_IMPL_FINDING_ASYNC_CALLBACKS_DATA_H_INCLUDED