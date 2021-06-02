////////////////////////////////////////////////////////////////////////////
//	Created		: 09.11.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_EMITTER_INLINE_H_INCLUDED
#define PARTICLE_EMITTER_INLINE_H_INCLUDED

namespace xray {
namespace particle {

template< class ConfigValueType >
void particle_emitter::load( ConfigValueType const& prop_config )
{
	m_delay				= read_config_value(prop_config, "Delay", m_delay);
	m_duration			= read_config_value(prop_config, "Duration", m_duration);
	m_duration_variance	= read_config_value(prop_config, "DurationVariance", m_duration_variance);
	m_num_loops			= read_config_value(prop_config, "EmitterLoops", m_num_loops);
	m_num_loops			= read_config_value(prop_config, "Loops", m_num_loops); // in group too
	m_world_space		= read_config_value(prop_config, "WorldSpace", m_world_space); // in group too
	m_max_num_particles	= read_config_value(prop_config, "MaxParticles", m_max_num_particles);
	m_visibility		= read_config_value(prop_config, "Enabled", m_visibility); // in group too
	
	xray::fs_new::virtual_path_string mtl_path = read_config_value<pcstr>(prop_config, "Material", m_material_name); // in group too
	
	//if (mtl_path!=m_material_name)
	//{
		strings::copy(m_material_name, mtl_path.c_str());
	//	load_material(m_material_name);
	//}
	
	if (prop_config.value_exists("LifeTime"))
		m_particle_lifetime_curve.load(prop_config["LifeTime"]);
	
	if (prop_config.value_exists("Rate"))
		m_particle_spawn_rate_curve.load(prop_config["Rate"]);
	
	if (prop_config.value_exists("BurstList"))
	{
		ConfigValueType const& burst_list_config = prop_config["BurstList"];
		
		u32 num_burst_entries					 = burst_list_config.size();
		
		if (num_burst_entries==0)
		{
			// Delete all.
			if (m_num_burst_entries)
			{
				m_num_burst_entries  = 0;
				burst_entry* pointer = &*m_burst_entries;
				MT_FREE(pointer);
			}
		}
		else if (num_burst_entries==1)
		{
			if (m_num_burst_entries==0) // Append first.
			{
				//((xray::configs::lua_config_value*)&burst_list_config)->save_as("f:/1.lua");
				m_burst_entries = MT_ALLOC(burst_entry, num_burst_entries);
				m_burst_entries[0].count		  = burst_list_config["Burst[0]"]["Count"];
				m_burst_entries[0].count_variance = burst_list_config["Burst[0]"]["CountVariance"];
				m_burst_entries[0].time			  = burst_list_config["Burst[0]"]["Time"];
				m_num_burst_entries		 = num_burst_entries;
			}
			else
			{
				for (u32 i=0; i<m_num_burst_entries; i++) // Update one.
				{
					fixed_string<32> n;
					n.assignf("Burst[%d]",i);
					if (burst_list_config.value_exists(n.c_str()))
					{
						pcstr c = n.c_str();
						ConfigValueType const& value = burst_list_config[c];
						burst_entry& be = m_burst_entries[i];
						be.count			= read_config_value(value, "Count", (u32)xray::math::abs(be.count));
						be.count_variance	= read_config_value(value, "CountVariance", be.count_variance);
						be.time				= read_config_value(value, "Time", be.time);
					}
				}
			}
		}
		else // Append new all.
		{
			m_burst_entries = MT_ALLOC(burst_entry, num_burst_entries);
			for (u32 i=0; i<num_burst_entries; i++)
			{
				fixed_string<32> n;
				n.assignf("Burst[%d]",i);
				pcstr c = n.c_str();
				
				if (burst_list_config.value_exists(c))
				{
					m_burst_entries[i].count		  = read_config_value(burst_list_config[c], "Count", (u32)xray::math::abs(m_burst_entries[i].count));
					m_burst_entries[i].count_variance = read_config_value(burst_list_config[c], "CountVariance", m_burst_entries[i].count_variance);
					m_burst_entries[i].time			  = read_config_value(burst_list_config[c], "Time", m_burst_entries[i].time);
				}
			}	
			m_num_burst_entries = num_burst_entries;
		}
	}
}

} // namespace particle
} // namespace xray

#endif // #ifndef PARTICLE_EMITTER_INLINE_H_INCLUDED