////////////////////////////////////////////////////////////////////////////
//	Created		: 02.04.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EDITOR_RESOURCE_MANAGER_H_INCLUDED
#define EDITOR_RESOURCE_MANAGER_H_INCLUDED

#include <xray/editor/base/managed_delegate.h>

using namespace System;
using namespace System::Collections::Generic;

using xray::resources::request;
using xray::resources::resource_ptr;
using xray::resources::class_id;
using xray::resources::creation_request;
using xray::resources::query_callback;
using xray::resources::query_result_for_cook;
using xray::resources::user_data_variant;
using xray::memory::base_allocator;	 

namespace xray
{
	namespace editor
	{
		typedef query_result_delegate::Delegate managed_query_callback;

		namespace editor_resource_manager
		{
			void		initialize					();
			void		finalize					();
			long		query_resource				(	pcstr						request_path, 
														class_id					class_id, 
	 													query_callback const &		callback, 
	 													base_allocator *			allocator,
														user_data_variant const *	user_data	=	NULL,
	 													int							priority	=	0,
	 													query_result_for_cook *		parent		=	NULL);

			long		query_resources				(	request const *				requests, 
														u32							count, 
														query_callback const &		callback, 
														base_allocator *			allocator,
														user_data_variant const *	user_data[]	=	NULL,
														int							priority	=	0,
														query_result_for_cook *		parent		=	NULL);

			void		query_resources_and_wait	(	request const *				requests, 
														u32							count, 
														query_callback const &		callback, 
														base_allocator *			allocator,
														user_data_variant const *	user_data[]	=	NULL,
														int							priority	=	0,
														query_result_for_cook *		parent		=	NULL);

			void		query_resource_and_wait		(	pcstr						request_path, 
														class_id					class_id, 
														query_callback const &		callback, 
														base_allocator *			allocator,
														user_data_variant const *	user_data	=	NULL,
														int							priority	=	0,
														query_result_for_cook *		parent		=	NULL);

			long		query_create_resources		(	creation_request const* 	requests, 
											 			u32							count, 
											 			query_callback const &		callback, 
											 			base_allocator *			allocator,
														user_data_variant const *	user_data[]	=	NULL,
											 			int							priority	=	0,
											 			query_result_for_cook *		parent		=	NULL);

			long		query_create_resource		(	pcstr						name,
														const_buffer				src_data, 
											 			class_id					class_id, 
											 			query_callback const &		callback, 
											 			base_allocator *			allocator,
														user_data_variant const *	user_data	=	NULL,
											 			int							priority	=	0,
											 			query_result_for_cook *		parent		=	NULL);

			long		query_resource				(	pcstr						request_path, 
														class_id					class_id, 
	 													managed_query_callback^		callback, 
	 													base_allocator *			allocator,
														user_data_variant const *	user_data	=	NULL,
	 													int							priority	=	0,
	 													query_result_for_cook *		parent		=	NULL);

			long		query_resources				(	request const *				requests, 
														u32							count, 
														managed_query_callback^ 	callback, 
														base_allocator *			allocator,
														user_data_variant const *	user_data[]	=	NULL,
														int							priority	=	0,
														query_result_for_cook *		parent		=	NULL);

			void		query_resources_and_wait	(	request const *				requests, 
														u32							count, 
														managed_query_callback^ 	callback, 
														base_allocator *			allocator,
														user_data_variant const *	user_data[]	=	NULL,
														int							priority	=	0,
														query_result_for_cook *		parent		=	NULL);

			void		query_resource_and_wait		(	pcstr						request_path, 
														class_id					class_id, 
														managed_query_callback^ 	callback, 
														base_allocator *			allocator,
														user_data_variant const *	user_data[]	=	NULL,
														int							priority	=	0,
														query_result_for_cook *		parent		=	NULL);

			long		query_create_resources		(	creation_request const* 	requests, 
											 			u32							count, 
											 			managed_query_callback^ 	callback, 
											 			base_allocator *			allocator,
														user_data_variant const *	user_data[]	=	NULL,
											 			int							priority	=	0,
											 			query_result_for_cook *		parent		=	NULL);

			long		query_create_resource		(	pcstr						name,
														const_buffer				src_data, 
											 			class_id					class_id, 
											 			managed_query_callback^ 	callback, 
											 			base_allocator *			allocator,
														user_data_variant const *	user_data	=	NULL,
											 			int							priority	=	0,
											 			query_result_for_cook *		parent		=	NULL);
													
		} // namespace editor_resource_manager
	} // namespace editor
} // namespace xray

#endif // #ifndef EDITOR_RESOURCE_MANAGER_H_INCLUDED