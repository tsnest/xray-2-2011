////////////////////////////////////////////////////////////////////////////
//	Created		: 02.04.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "editor_resource_manager.h"
#include <xray/editor/base/managed_delegate.h>

using xray::resources::class_id;
using xray::resources::query_callback;
using xray::resources::query_result;
using xray::resources::query_result_for_cook;
using xray::resources::queries_result;
using xray::resources::query_result_for_user;
using xray::resources::unmanaged_resource_ptr;
using xray::resources::resource_ptr;
using xray::resources::user_data_variant;
using xray::memory::base_allocator;	 	

static xray::command_line::key	s_use_editor_resource_manager	("use_editor_resource_manager",	"editor_rm", "resources", "if set, use editor resource manager");

namespace xray{
namespace editor{
namespace editor_resource_manager{

#pragma region |   ManagerImpl   |
//implement all functionality for editor_resource_manager
private ref class manager_impl
{

	#pragma region | InnerTypes |

	private:
		//keeps resource we've got
		ref class resource_holder
		{
		public:
			resource_holder(query_result_for_user* data)
			{
				xray::resources::managed_resource_ptr	res					= data->get_managed_resource();
				if(res.c_ptr() != NULL)				m_resource				= NEW(xray::resources::managed_resource_ptr)(res);
				unmanaged_resource_ptr				unmanaged_res			= data->get_unmanaged_resource();
				if(unmanaged_res.c_ptr() != NULL)	m_unmanaged_resource	= NEW(unmanaged_resource_ptr)(unmanaged_res);
				m_resource_path												= gcnew String(data->get_requested_path());
				m_is_disposed												= false;
			}
			!resource_holder()
			{
				if(m_resource != NULL)				DELETE(m_resource);
				if(m_unmanaged_resource != NULL)	DELETE(m_unmanaged_resource);
				m_resource							= NULL;
				m_unmanaged_resource				= NULL;
			}

			~resource_holder()
			{
				if(!m_is_disposed)
				{
					this->!resource_holder();
					m_is_disposed			= true;
				}
			}
		private:
			Boolean						m_is_disposed;

		public:
			xray::resources::managed_resource_ptr*				m_resource;
			unmanaged_resource_ptr*		m_unmanaged_resource;
			String^						m_resource_path;

		public:
			long		reference_count()
			{
				long ret = 0;
				if(m_unmanaged_resource != NULL)
					ret = math::max(ret, m_unmanaged_resource->c_ptr()->reference_count());
				if(m_resource != NULL)
					ret = math::max(ret, m_resource->c_ptr()->reference_count());
				return ret;
			}

		};

		//keeps query
		ref class query_holder
		{
		public:
			query_holder( const query_callback*	  set_callback,
						  memory::base_allocator* set_allocator,
						  user_data_variant const * *	set_user_data,
						  u32					  set_priority,
						  query_result_for_cook * set_parent):
			  m_is_disposed(false), allocator(set_allocator), priority(set_priority), parent(set_parent), user_data(set_user_data)
			{
				callback					= NEW(query_callback)(*set_callback);
				m_is_disposed				= false;
			}
			!query_holder()
			{
 				if(callback != NULL)		DELETE(callback);
 				callback					= NULL;
			}
			~query_holder()
			{
				if(!m_is_disposed)
				{
					this->!query_holder();
					m_is_disposed			= true;
				}
			}

		private:
			Boolean					m_is_disposed;

		public:
			query_callback *			callback;
			memory::base_allocator *	allocator;
			user_data_variant const * *	user_data;
			u32							priority;
			query_result_for_cook *		parent;
		};

	#pragma endregion

	#pragma region | Initialize |

	private:
		manager_impl				():
		   m_delete_evristik_start(1), m_delete_evristik_count(1)
		{
			m_list_recently_used			= gcnew LinkedList<resource_holder^>();
			m_dictionary_recently_used		= gcnew Dictionary<String^, LinkedListNode<resource_holder^>^>();
			m_dictionary_queried			= gcnew Dictionary<Int32, query_holder^>();
			m_dictionary_requery_ids		= gcnew Dictionary<Int32, Int32>();
		}
		~manager_impl				()
		{
			for(;m_list_recently_used->Count > 0;)
			{
				LinkedListNode<resource_holder^>^ to_del_node	= m_list_recently_used->First;
				resource_holder^ to_del_holder					= to_del_node->Value;
				m_list_recently_used->Remove					(to_del_node);
				m_dictionary_recently_used->Remove				(to_del_holder->m_resource_path);
				delete											to_del_holder;
			}

			delete m_list_recently_used;
			delete m_dictionary_recently_used;
		}

	public:
		static void initialize		()
		{
			if(manager == nullptr)
				manager = gcnew manager_impl();
		}

		static void finalize		()
		{
			if(manager != nullptr)
			{
				delete manager;
				manager = nullptr;
			}
		}

	#pragma endregion

	#pragma region |   Fields   |

	public:
		static manager_impl^	manager;

	private:
		LinkedList<resource_holder^>^								m_list_recently_used;
		Dictionary<String^, LinkedListNode<resource_holder^>^>^		m_dictionary_recently_used;
		Dictionary<Int32, query_holder^>^							m_dictionary_queried;
		Dictionary<Int32, Int32>^									m_dictionary_requery_ids;

		Boolean					m_is_sync_call;
		Boolean					m_is_sync_called;
		query_holder^ 			m_synk_query;
		Int32					m_delete_evristik_start;
		Int32					m_delete_evristik_count;

	#pragma endregion

	#pragma region |   Methods  |

	private:
	void				inner_query_callback							(xray::resources::queries_result& data)
	{
		query_holder^ query;
		if(m_is_sync_call)
		{
			query = m_synk_query;
		}
		else
		{
			query = m_dictionary_queried[data.unique_id()];
			m_dictionary_queried->Remove(data.unique_id());
		}

		if(m_dictionary_requery_ids->ContainsKey(data.unique_id()))
		{
			data.set_unique_id(m_dictionary_requery_ids[data.unique_id()]);
			m_dictionary_requery_ids->Remove(data.unique_id());
		}

		if(data.is_failed())
		{
			Boolean is_not_enough_memory = false;
			int count_resources_without_memory = 0;

			for(u32 i = 0; i < data.size(); ++i)
			{
				if(data[i].get_error_type() == query_result_for_user::error_type_out_of_memory)
				{
					is_not_enough_memory = true;
					++count_resources_without_memory;
				}
			}
			if(is_not_enough_memory)
			{
				if(m_delete_evristik_count < count_resources_without_memory)
					m_delete_evristik_count = count_resources_without_memory;

				Boolean			is_has_cleaned_memory = false;

				LinkedListNode<resource_holder^>^ current_node;
				current_node	= m_list_recently_used->First;
				
				int delete_count = 0;
				LOG_INFO("editor_resource_manager: clean max %d resources", m_delete_evristik_count);
				for(;current_node != nullptr;)
				{
					LinkedListNode<resource_holder^>^ to_del_node	= current_node;
					current_node									= current_node->Next;

					if(to_del_node->Value->reference_count() == 1)
					{
						//delete node with resource
						resource_holder^ to_del_holder					= to_del_node->Value;
						m_list_recently_used->Remove					(to_del_node);
						m_dictionary_recently_used->Remove				(to_del_holder->m_resource_path);
						is_has_cleaned_memory							= true;
						delete											to_del_holder;

						if( ++delete_count >= m_delete_evristik_count )
							break;
					}
				}

				if(is_has_cleaned_memory)
				{
					request* requests = (request*)MALLOC(sizeof(request)*data.size(), "alloc memory for requests");
					for(u32 i = 0; i < data.size(); ++i)
					{
						requests[i].path	= data[i].get_requested_path();
						requests[i].id		= data[i].get_class_id();
					}

					query_result_delegate* rq = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &manager_impl::inner_query_callback));
 					int ret_id = xray::resources::query_resources(
 						requests,
						data.size(),
						boost::bind(&query_result_delegate::callback, rq, _1),
  						query->allocator,
						query->user_data,
						query->priority,
	 					query->parent
 					);

					m_dictionary_queried->Add(ret_id, query);
					m_dictionary_requery_ids->Add(ret_id, data.unique_id());
					FREE(requests);
					m_delete_evristik_count *= 2;
					return;
				}
			}
			(*(query->callback))(data);
			delete query;
			return;
		}

		m_delete_evristik_count = m_delete_evristik_start;

		for(u32 i = 0; i < data.size(); ++i)
		{
			String^ resource_name					= gcnew String(data[i].get_requested_path());
			if(!m_dictionary_recently_used->ContainsKey(resource_name))
			{
				LinkedListNode<resource_holder^>^ node	= gcnew LinkedListNode<resource_holder^>(gcnew resource_holder(&data[i]));
				m_list_recently_used->AddLast			(node);
				m_dictionary_recently_used->Add			(resource_name, node);
			}
		}

		if(m_is_sync_call)
		{
			LOG_INFO("editor_resource_manager: synk call");
			m_is_sync_called = true;
		}

		(*(query->callback))(data);
		delete query;
	}

	public:
	long				query_resources									(	request const *				requests, 
																			u32							count, 
																			query_callback const &		callback, 
	 																		base_allocator *			allocator,
																			user_data_variant const *	user_data[],
	 																		int							priority,
																			query_result_for_cook *		parent		)
	{

		Boolean is_all_requests_used = true;
		for(u32 i = 0; i < count; ++i)
		{
			String^ request_string = gcnew String(requests[i].path);

			//if resource in DRU => call it back and return
			if(!m_dictionary_recently_used->ContainsKey(request_string))
			{
 				is_all_requests_used = false;
				break;
			}
		}

		//If all resources are queried before, just call resources::query_resources
		if(is_all_requests_used)
		{
			LOG_INFO("editor_resource_manager: reuse resources");
			return xray::resources::query_resources(
 				requests,
 				count,
				callback,
 				allocator,
				user_data,
				priority,
				parent
 			);
		}

		//else query it for resource manager
		m_is_sync_call		= true;
		m_synk_query		= gcnew query_holder(&callback, allocator, user_data, priority, parent);

		query_result_delegate* rq = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &manager_impl::inner_query_callback));
 		int ret_id = xray::resources::query_resources(
 			requests,
 			count,
			boost::bind(&query_result_delegate::callback, rq, _1),
 			allocator,
			user_data,
			priority,
			parent
 		);

		if(!m_is_sync_called)
			m_dictionary_queried->Add(ret_id, m_synk_query);

		m_is_sync_call		= false;
		m_is_sync_called	= false;

		return ret_id;
	}

	#pragma endregion

}; // class editor_resource_manager

#pragma endregion

#pragma region | PublicInterface |

void		initialize					()
{
	manager_impl::initialize();
}

void		finalize					()
{
	manager_impl::finalize();
}

long		query_resource				(	pcstr						request_path, 
											class_id					class_id, 
	 										query_callback const &		callback, 
	 										base_allocator *			allocator,
											user_data_variant const *	user_data,
	 										int							priority,
	 										query_result_for_cook *		parent)
{
	request		request		=	{ request_path, class_id };
	user_data_variant const * user_data_array[]	=	{ user_data };
	if(s_use_editor_resource_manager.is_set())
		return manager_impl::manager->query_resources(& request, 1, callback, allocator, user_data_array, priority, parent);
	else
		return resources::query_resource(request_path, class_id, callback, allocator, user_data, priority, parent);
}			

long		query_resources				(	request const *				requests, 
											u32							count, 
											query_callback const &		callback, 
											base_allocator *			allocator,
											user_data_variant const *	user_data[],
											int							priority,
											query_result_for_cook *		parent)
{
	if(s_use_editor_resource_manager.is_set())
		return manager_impl::manager->query_resources(requests, count, callback, allocator, user_data, priority, parent);
	else
		return resources::query_resources(requests, count, callback, allocator, user_data, priority, parent);
}			

void		query_resources_and_wait(		request const *				requests, 
											u32							count, 
											query_callback const &		callback, 
											base_allocator *			allocator,
											user_data_variant const *	user_data[],
											int							priority,
											query_result_for_cook *		parent)
{
	resources::query_resources_and_wait(		requests,
												count,
												callback,
												allocator,
												user_data,
												priority,
												parent		);
}

void   		query_resource_and_wait	(		pcstr						request_path, 
											class_id					class_id, 
											query_callback const &		callback, 
											base_allocator *			allocator,
											user_data_variant const *	user_data,
											int							priority,
											query_result_for_cook *		parent)
{
	resources::query_resource_and_wait(			request_path,
												class_id,
												callback,
												allocator,
												user_data,
												priority,
												parent
															);
}

long   		query_create_resources	(		creation_request const* 	requests, 
											u32							count, 
											query_callback const &		callback, 
											base_allocator *			allocator,
											user_data_variant const *	user_data[],
											int							priority,
											query_result_for_cook *		parent)
{
	return resources::query_create_resources(	requests,
												count,
												callback,
												allocator,
												user_data,
												priority,
												parent		);
}

long   		query_create_resource	(		pcstr						name,
											const_buffer				src_data, 
											class_id					class_id, 
											query_callback const &		callback, 
											base_allocator *			allocator,
											user_data_variant const *	user_data,
											int							priority,
											query_result_for_cook *		parent)
{
	return resources::query_create_resource(	name,
												src_data,
												class_id,
												callback,
												allocator,
												user_data,
												priority,
												parent		);
}

long		query_resource				(	pcstr						request_path, 
											class_id					class_id, 
	 										managed_query_callback^		callback, 
	 										base_allocator *			allocator,
											user_data_variant const *	user_data,
	 										int							priority,
	 										query_result_for_cook *		parent)
{
	query_result_delegate* rq = NEW(query_result_delegate)(callback);
	return editor_resource_manager::query_resource(
		request_path, class_id, boost::bind(&query_result_delegate::callback, rq, _1), allocator, user_data, priority, parent);
}			

long		query_resources				(	request const *				requests, 
											u32							count, 
											managed_query_callback^ 	callback, 
											base_allocator *			allocator,
											user_data_variant const *	user_data[],
											int							priority,
											query_result_for_cook *		parent)
{
	query_result_delegate* rq = NEW(query_result_delegate)(callback);
	return editor_resource_manager::query_resources(
		requests, count, boost::bind(&query_result_delegate::callback, rq, _1), allocator, user_data, priority, parent);
}			

void		query_resources_and_wait	(	request const *				requests, 
											u32							count, 
											managed_query_callback^ 	callback, 
											base_allocator *			allocator,
											user_data_variant const *	user_data[],
											int							priority,
											query_result_for_cook *		parent)
{
	query_result_delegate* rq = NEW(query_result_delegate)(callback);
	return editor_resource_manager::query_resources_and_wait(
		requests, count, boost::bind(&query_result_delegate::callback, rq, _1), allocator, user_data, priority, parent);
}

void		query_resource_and_wait		(	pcstr						request_path, 
											class_id					class_id, 
											managed_query_callback^ 	callback, 
											base_allocator *			allocator,
											user_data_variant const *	user_data,
											int							priority,
											query_result_for_cook *		parent)
{
	query_result_delegate* rq = NEW(query_result_delegate)(callback);
	return editor_resource_manager::query_resource_and_wait(
		request_path, class_id, boost::bind(&query_result_delegate::callback, rq, _1), allocator, user_data, priority, parent);
}

long		query_create_resources		(	creation_request const* 	requests, 
								 			u32							count, 
								 			managed_query_callback^ 	callback, 
								 			base_allocator *			allocator,
											user_data_variant const *	user_data[],
								 			int							priority,
								 			query_result_for_cook *		parent)
{
	query_result_delegate* rq = NEW(query_result_delegate)(callback);
	return editor_resource_manager::query_create_resources(
		requests, count, boost::bind(&query_result_delegate::callback, rq, _1), allocator, user_data, priority, parent);
}

long		query_create_resource		(	pcstr						name,
											const_buffer				src_data, 
								 			class_id					class_id, 
								 			managed_query_callback^ 	callback, 
								 			base_allocator *			allocator,
											user_data_variant const *	user_data,
								 			int							priority,
								 			query_result_for_cook *		parent)
{
	query_result_delegate* rq = NEW(query_result_delegate)(callback);
	return editor_resource_manager::query_create_resource(
		name, src_data, class_id, boost::bind(&query_result_delegate::callback, rq, _1), allocator, user_data, priority, parent);
}

#pragma endregion

} // namespace editor_resource_manager
} // namespace editor
} // namespace xray