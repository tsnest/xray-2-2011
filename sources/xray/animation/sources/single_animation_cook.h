////////////////////////////////////////////////////////////////////////////
//	Created		: 28.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SINGLE_ANIMATION_COOK_H_INCLUDED
#define SINGLE_ANIMATION_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray
{
	namespace animation
	{
		class single_animation;

		class single_animation_cook : public resources::translate_query_cook, public boost::noncopyable 
		{
			typedef resources::translate_query_cook	super;

		public:
									single_animation_cook		( );
			virtual					~single_animation_cook		( );

			virtual	void			translate_query				( resources::query_result_for_cook& parent );
			virtual void			delete_resource				( resources::resource_base* res );

		private:
			void					request_items				( configs::binary_config_ptr config_ptr, configs::binary_config_value const& collection, resources::query_result_for_cook* const parent );
			void					on_sub_resources_loaded		( resources::queries_result& data );
			void					on_animations_loaded		( resources::queries_result& data, configs::binary_config_ptr config_ptr, configs::binary_config_value const& collection );

		}; // class single_animation_cook

	} // namespace animation
} // namespace xray

#endif // #ifndef SINGLE_ANIMATION_COOK_H_INCLUDED