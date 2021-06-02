////////////////////////////////////////////////////////////////////////////
//	Created		: 28.03.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OGG_ENCODED_SOUND_INTERFACE_COOK_H_INCLUDED
#define OGG_ENCODED_SOUND_INTERFACE_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace sound {

class ogg_encoded_sound_interface_cook :	
	public resources::translate_query_cook,
	private boost::noncopyable
{
	typedef resources::translate_query_cook	super;
public:
					ogg_encoded_sound_interface_cook 	( );
	virtual			~ogg_encoded_sound_interface_cook 	( );

	virtual void	translate_query			( resources::query_result_for_cook& parent );	
	virtual void	delete_resource			( resources::resource_base* res);
private:
			void	on_sub_resources_loaded	( resources::queries_result& data ); // raw ogg file loaded
}; // class ogg_encoded_sound_interface_cook

} // namespace sound
} // namespace xray

#endif // #ifndef OGG_ENCODED_SOUND_INTERFACE_COOK_H_INCLUDED