////////////////////////////////////////////////////////////////////////////
//	Created		: 02.08.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_SCENE_COOK_H_INCLUDED
#define SOUND_SCENE_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace sound {

class sound_world;

class sound_scene_cook :	public resources::unmanaged_cook,
							private boost::noncopyable
{
public:
							sound_scene_cook		( sound_world& world );

	virtual	mutable_buffer	allocate_resource		(	resources::query_result_for_cook& in_query,
														const_buffer raw_file_data,
														bool file_exist
													);
	virtual void			deallocate_resource		( pvoid buffer );

	virtual void			create_resource			(	resources::query_result_for_cook& in_out_query,
														const_buffer raw_file_data,
														mutable_buffer in_out_unmanaged_resource_buffer
													);
	virtual void			destroy_resource		( resources::unmanaged_resource* resource );
private:
	sound_world&			m_sound_world;
}; // class sound_scene_cook

} // namespace sound
} // namespace xray

#endif // #ifndef SOUND_SCENE_COOK_H_INCLUDED