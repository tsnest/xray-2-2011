////////////////////////////////////////////////////////////////////////////
//	Created		: 09.03.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TEXTURE_OPTIONS_COOKER_H_INCLUDED
#define TEXTURE_OPTIONS_COOKER_H_INCLUDED

#pragma managed(push)
#pragma unmanaged
#include <xray/resources.h>
#include <xray/resources_cook_classes.h>
#pragma managed(pop)

namespace xray
{
	namespace editor
	{
		struct texture_options;

		struct texture_options_cooker: public resources::translate_query_cook
		{
		public:
			texture_options_cooker	();

			void					on_sub_resources_loaded		(resources::queries_result & result);
			virtual void			translate_query				(resources::query_result & parent);
			virtual void			delete_resource				(resources::unmanaged_resource * resource);
			void					on_fs_iterator_ready		(resources::fs_iterator iter, texture_options* options, resources::query_result_for_cook* parent);
		}; // class texture_options_cooker
	} // namespace editor
} // namespace xray

#endif // #ifndef TEXTURE_OPTIONS_COOKER_H_INCLUDED