////////////////////////////////////////////////////////////////////////////
//	Created		: 13.05.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef WAV_ENCODED_SOUND_INTERFACE_COOK_H_INCLUDED
#define WAV_ENCODED_SOUND_INTERFACE_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace sound {

class wav_encoded_sound_interface_cook :	public resources::translate_query_cook,
											private boost::noncopyable
{
	typedef resources::translate_query_cook	super;
public:
					wav_encoded_sound_interface_cook 	( );
	virtual			~wav_encoded_sound_interface_cook 	( );

	virtual void	translate_query			( resources::query_result_for_cook& parent );	
	virtual void	delete_resource			( resources::resource_base* res);
private:
			void	on_file_loaded			( resources::queries_result& data ); // raw wav file loaded
}; // class wav_encoded_sound_interface_cook

} // namespace sound
} // namespace xray

#endif // #ifndef WAV_ENCODED_SOUND_INTERFACE_COOK_H_INCLUDED