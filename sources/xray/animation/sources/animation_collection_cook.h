////////////////////////////////////////////////////////////////////////////
//	Created		: 26.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_ANIMATION_COLLECTION_COOK_H_INCLUDED
#define XRAY_ANIMATION_ANIMATION_COLLECTION_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray
{
	namespace animation
	{
		class animation_collection;

		class animation_collection_cook : public resources::translate_query_cook, public boost::noncopyable 
		{
			typedef resources::translate_query_cook	super;

		public:
									animation_collection_cook	( );
			virtual					~animation_collection_cook	( );

			virtual	void			translate_query				( resources::query_result_for_cook& parent );
			virtual void			delete_resource				( resources::resource_base* res );

		private:

			void					request_items				( configs::binary_config_ptr config_ptr, configs::binary_config_value const& collection, resources::query_result_for_cook* const parent );
			void					collection_config_loaded	( resources::queries_result& data );
			animation_collection*	new_collection				( configs::binary_config_value const& collection );
			void					sub_animations_loaded		( resources::queries_result& data, configs::binary_config_ptr config_ptr, configs::binary_config_value const& collection );

		}; // class animation_collection_cook

	} // namespace animation
} // namespace xray

#endif // #ifndef XRAY_ANIMATION_ANIMATION_COLLECTION_COOK_H_INCLUDED