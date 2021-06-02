////////////////////////////////////////////////////////////////////////////
//	Created		: 19.08.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_PARTICLE_H_INCLUDED
#define OBJECT_PARTICLE_H_INCLUDED

#include "object_base.h"
#include <xray/render/facade/particles.h>
#include <xray/render/engine/base_classes.h>

namespace xray {

namespace particle	{
	typedef	resources::resource_ptr<
		resources::unmanaged_resource,
		resources::unmanaged_intrusive_base
	> particle_system_instance_ptr;

	typedef	resources::resource_ptr<
		resources::unmanaged_resource,
		resources::unmanaged_intrusive_base
	> particle_system_ptr;
} // namespace particle

namespace editor {

ref class tool_particle;

ref class object_particle :public object_base
{
	typedef object_base		super;
public:
					object_particle			( tool_particle^ t, render::scene_ptr const& scene );
	virtual			~object_particle		( );
	virtual void	destroy_collision		( )										override;

	virtual	void	set_transform			( float4x4 const& transform )			override;
	virtual	void	on_selected				( bool selected )						override;
	virtual	void	set_visible				( bool bvisible )						override;
	virtual void	save					( configs::lua_config_value t )			override;
	virtual void	set_library_name		( System::String^ ln )					override;

	virtual enum_terrain_interaction get_terrain_interaction( )						override	{return enum_terrain_interaction::tr_vertical_snap;}

			void	update_particle_system	( bool pause );

	virtual void	load_contents			( )	override	{}
	virtual void	unload_contents			( bool ) override	{}

protected:
	void			on_resource_loaded		( resources::queries_result& data );
	void			initialize_collision	( );

	System::String^							m_reference_name;

public:
	particle::particle_system_instance_ptr*	m_particle_system_instance_ptr;
	render::scene_ptr*				m_scene;
	
	tool_particle^					m_tool_particle;
	bool							m_selected;
public:
}; // class object_particle

} // namespace editor
} // namespace xray


#endif // #ifndef OBJECT_PARTICLE_H_INCLUDED
