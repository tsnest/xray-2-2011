////////////////////////////////////////////////////////////////////////////
//	Created 	: 22.02.2011
//	Author		: Dmitriy Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OGG_SOUND_COOK_H_INCLUDED
#define OGG_SOUND_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>
#include "sound_world.h"
#include "ogg_sound.h"

namespace xray {
namespace sound {

class sound_world;

class ogg_sound_cook :	public resources::translate_query_cook,
							public boost::noncopyable
{
	typedef resources::translate_query_cook	super;
public:
							ogg_sound_cook		( );
	virtual					~ogg_sound_cook		( );

	virtual void			delete_resource			( resources::resource_base* res);
	virtual void			translate_query			( resources::query_result_for_cook& parent );

private:
			void			on_sub_resources_loaded	( resources::queries_result& data ); // raw ogg file loaded
			void			on_raw_properties_loaded( resources::queries_result& result );

}; // class ogg_sound_cook


} // namespace sound
} // namespace xray

#endif // #ifndef OGG_SOUND_COOK_H_INCLUDED