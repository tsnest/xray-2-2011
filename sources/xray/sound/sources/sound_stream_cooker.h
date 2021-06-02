////////////////////////////////////////////////////////////////////////////
//	Created		: 22.04.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_STREAM_COOKER_H_INCLUDED
#define SOUND_STREAM_COOKER_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace sound {

class sound_world;

class sound_stream_cooker :	public resources::translate_query_cook,
							public boost::noncopyable
{
	typedef resources::translate_query_cook	super;
public:
							sound_stream_cooker		( );
	virtual					~sound_stream_cooker	( );

	virtual void			delete_resource			( resources::unmanaged_resource* res);
	virtual void			translate_query			( resources::query_result& parent );

private:
			void			on_sub_resources_loaded	( resources::queries_result& data ); // raw ogg file loaded
			void			on_raw_properties_loaded( resources::queries_result& result );
}; // class sound_stream_cooker


} // namespace sound
} // namespace xray

#endif // #ifndef SOUND_STREAM_COOKER_H_INCLUDED