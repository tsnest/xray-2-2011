////////////////////////////////////////////////////////////////////////////
//	Created		: 23.04.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef OGG_SOURCE_COOK_H_INCLUDED
#define OGG_SOURCE_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace sound {

class ogg_source_cook :	public resources::translate_query_cook,
							private boost::noncopyable					
{
	typedef resources::translate_query_cook		super;
public:
								ogg_source_cook		( );

	virtual	void				translate_query			( resources::query_result_for_cook&	parent );

	virtual void				delete_resource			( resources::resource_base* resource );

private:

			void				on_ogg_file_loaded		( resources::queries_result& data, 
														  resources::query_result_for_cook* parent_query );

#ifndef MASTER_GOLD
			void				on_source_options_loaded( resources::queries_result& data, 
														  resources::query_result_for_cook* parent_query );

			void				on_fs_iterators_ready	( resources::queries_result& data, 
															resources::query_result_for_cook* parent_query );
#endif

}; // class ogg_source_cook

} // namespace sound
} // namespace xray

#endif // #ifndef OGG_SOURCE_COOK_H_INCLUDED