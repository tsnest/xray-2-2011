////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_SOUND_SOUND_EMITTER_H_INCLUDED
#define XRAY_SOUND_SOUND_EMITTER_H_INCLUDED

#include <xray/sound/api.h>
#include <xray/sound/sound_instance_proxy.h>

namespace xray {
namespace sound {

//class sound_instance_emitter;
class world_user;
class sound_instance_proxy;
class sound_propagator_emitter;

typedef intrusive_ptr<	sound_instance_proxy,
						sound_instance_proxy,
						threading::single_threading_policy > sound_instance_proxy_ptr;

enum emitter_type
{
	single,
	composite,
	collection
};

class XRAY_SOUND_API sound_emitter : public resources::unmanaged_resource
{
public:
								sound_emitter		( );
	sound_instance_proxy_ptr	emit				
								( 
									sound_scene_ptr& scene,
									world_user& user 
								);

	sound_instance_proxy_ptr	emit_point_sound
								( 
									sound_scene_ptr& scene,
									world_user& user
								);

	sound_instance_proxy_ptr	emit_spot_sound
								( 
									sound_scene_ptr& scene,
									world_user& user,
									sound_cone_type type
								);

	// don't change the state of geometry_instance. If you want to change
	// the position, orientation, etc. of this instance, use proxy->set_matrix for these purposes.
	// Geometry instance must not be changed by user after its creation, all the operations
	// will be performed directly by emited proxy.
	sound_instance_proxy_ptr	emit_volumetric_sound
								( 
									sound_scene_ptr& scene,
									world_user& user,
									collision::geometry_instance& non_shared_instance,
									float position_to_stereo_radius
								);
	
	void						emit_and_play_once	
								(
									sound_scene_ptr& scene,
									world_user& user,
									float3 const& position,
									sound_producer const* producer = 0,
									sound_receiver const* ignorable_receiver = 0
								);
	
	virtual	sound_propagator_emitter const*	get_sound_propagator_emitter( ) const = 0;
	virtual	sound_propagator_emitter const*	get_sound_propagator_emitter( u64 address ) const = 0;

	virtual void							serialize					( memory::writer& w ) const = 0;
	virtual void							deserialize					( memory::reader& r ) = 0;
	
	virtual	emitter_type					get_type					( ) const = 0;
protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR	( sound_emitter );
protected:
	mutable u64				m_old_address;
}; // class sound_emitter

typedef resources::resource_ptr < sound_emitter, resources::unmanaged_intrusive_base > sound_emitter_ptr;

} // namespace sound
} // namespace xray

#endif // #ifndef XRAY_SOUND_SOUND_EMITTER_H_INCLUDED
