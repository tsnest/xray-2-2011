////////////////////////////////////////////////////////////////////////////
//	Created		: 14.02.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef COMPOSITE_SOUND_COOK_H_INCLUDED
#define COMPOSITE_SOUND_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace sound {

class composite_sound;
class sound_world;

class composite_sound_cook :
	public resources::translate_query_cook,
	public boost::noncopyable
{
	typedef resources::translate_query_cook	super;

public:
					composite_sound_cook	( sound_world& world );

	virtual			~composite_sound_cook	( );

	virtual	void	translate_query			( resources::query_result_for_cook& parent );
	virtual void	delete_resource			( resources::resource_base* res );

private:
			void	on_sub_resources_loaded	( resources::queries_result& data );
			void	on_sounds_loaded		( resources::queries_result& data, configs::binary_config_ptr config_ptr, configs::binary_config_value const& sounds );
composite_sound*	create_sound			( configs::binary_config_value const& composite );
private:
	sound_world&	m_world;
}; // class composite_sound_cook

} // namespace sound
} // namespace xray

#endif // #ifndef COMPOSITE_SOUND_COOK_H_INCLUDED