////////////////////////////////////////////////////////////////////////////
//	Created		: 26.07.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_SYSTEM_H_INCLUDED
#define PARTICLE_SYSTEM_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace particle {

class particle_event;
class particle_emitter;
class particle_system;
class particle_action;
class particle_system_instance;
class particle_world;

typedef	resources::resource_ptr<
	resources::unmanaged_resource,
	resources::unmanaged_intrusive_base
> particle_system_instance_ptr;

struct particle_system_lod
{
	typedef platform_pointer<particle_emitter, xray::platform_pointer_default>::type		particle_emitter_ptr_type;
	typedef platform_pointer<particle_system, xray::platform_pointer_default>::type			particle_system_ptr_type;
	
	particle_emitter_ptr_type	m_emitters_array;
	u32							m_num_emitters;
	
	bool						m_visibility;
	float						m_total_length;
	
	particle_system_ptr_type	m_parent;
	
	void set_defaults()
	{
		m_emitters_array		= 0;
		m_num_emitters			= 0;
		m_visibility			= true;
		m_total_length			= 1.0f;
		m_parent				= 0;
	}
}; // struct particle_system_lod

class particle_system :
	public resources::unmanaged_resource,
	private boost::noncopyable
{
	typedef platform_pointer<particle_system_lod, xray::platform_pointer_default>::type particle_system_lod_ptr_type;

public:
	virtual ~particle_system			();
	
	template< class ConfigValueType >	void	load_from_config				(ConfigValueType const& cfg, bool mt_alloc);
	template< class ConfigValueType >	void	load_lod_from_config			(particle_system_lod& lod, ConfigValueType const& cfg, bool mt_alloc);
	template< class ConfigValueType >	u32		get_num_total_emitters			(ConfigValueType const& lod_config);
	
	particle_system_lod_ptr_type		m_lods;
	u32									m_num_lods;
	
protected:
	friend class particle_world;
	friend class particle_system_cook;
	friend class particle_system_wrapper_cook;
	friend class particle_system_instance_cook;
	friend class particle_system_instance;

	u32					action_to_index					(particle_system_lod& lod, particle_action* action);
	particle_action*	index_to_action					(particle_system_lod& lod, u32 index);

	void				set_defaults					();
	
	u32					save_binary						(xray::mutable_buffer& buffer, bool cals_size = false);
	void				load_binary						(xray::mutable_buffer& buffer );
	
	u32					save_lod_actions_binary			(particle_system_lod& lod, xray::mutable_buffer& buffer, bool cals_size = false);
	void				load_lod_actions_binary			(particle_system_lod& lod, xray::mutable_buffer& buffer);
	
	virtual void		clear_guid_list					() {}
	virtual void		add_action_to_guid_list			(pcstr, particle_action*)  {}
	virtual void		add_emitter_to_guid_list		(pcstr, particle_emitter*) {}
	virtual void		add_group_to_guid_list			(pcstr, particle_emitter*) {}
	virtual void		add_root_to_guid_list			(pcstr, particle_system_lod*) {}
	virtual void		exchange_emitters_in_guid_list	(particle_emitter*, particle_emitter*) {}
	
	// TODO: divide particle_system_impl and particle_system?
	template< class ConfigValueType >	particle_action* construct_action				(particle_system_lod& ps_lod, 
																						 particle_emitter* emitter, 
																						 ConfigValueType action_config, 
																						 bool mt_alloc, 
																						 u32& out_emitter_index);

	template< class ConfigValueType >	void			 parse_emitter					(particle_system_lod& ps_lod, 
																						 particle_emitter* emitter, 
																						 ConfigValueType const& emitter_config, 
																						 bool mt_alloc, 
																						 u32& out_emitter_index);
	
	template< class ConfigValueType >	void			 parse_group					(particle_system_lod& ps_lod, 
																						 ConfigValueType const& group_config, 
																						 bool mt_alloc, 
																						 u32& out_emitter_index);
	
	template< class ConfigValueType >	u32				 get_num_emitters_in_emitter	(ConfigValueType const& emitter_config);
	template< class ConfigValueType >	u32				 get_num_emitters_in_group		(ConfigValueType const& group_config);
	
}; // class particle_system

} // namespace particle
} // namespace xray

#include "particle_system_inline.h"

#endif // #ifndef PARTICLE_SYSTEM_H_INCLUDED