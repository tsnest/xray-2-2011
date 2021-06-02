////////////////////////////////////////////////////////////////////////////
//	Created		: 22.04.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include <xray/resources_cook_classes.h>

#ifndef XRAY_SOUND_SOUND_COOKER_H_INCLUDED
#define XRAY_SOUND_SOUND_COOKER_H_INCLUDED

namespace xray {
namespace sound {

class sound_world;

class sound_cooker :	public resources::translate_query_cook,
						public boost::noncopyable
{
	typedef resources::translate_query_cook	super;
public:
							sound_cooker			( );
	virtual					~sound_cooker			( ) {}

	virtual	void			translate_query			( resources::query_result & parent );
	virtual void			delete_resource			( resources::unmanaged_resource* res);

private:
			void			on_sub_resources_loaded	( resources::queries_result & data );
}; // class sound_cooker

} // namespace sound
} // namespace xray

#endif // #ifndef XRAY_SOUND_SOUND_COOKER_H_INCLUDED