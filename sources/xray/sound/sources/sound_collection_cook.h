////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_COLLECTION_COOK_H_INCLUDED
#define SOUND_COLLECTION_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace sound {

class sound_world;
class sound_collection;

class sound_collection_cook :
	public resources::translate_query_cook,
	public boost::noncopyable
{
	typedef resources::translate_query_cook	super;

public:
					sound_collection_cook	( sound_world& world );
	virtual			~sound_collection_cook	( );

	virtual	void	translate_query			( resources::query_result_for_cook& parent );
	virtual void	delete_resource			( resources::resource_base* res );

private:
			void	request_items			( configs::binary_config_ptr config_ptr, configs::binary_config_value const& collection, resources::query_result_for_cook* const parent );
			void	collection_config_loaded( resources::queries_result& data );
sound_collection*	create_collection		( configs::binary_config_value const& collection );
			void	sub_sounds_loaded		( resources::queries_result& data, configs::binary_config_ptr config_ptr, configs::binary_config_value const& collection );

private:
	sound_world&	m_world;
}; // class sound_collection_cook

} // namespace sound
} // namespace xray

#endif // #ifndef SOUND_COLLECTION_COOK_H_INCLUDED