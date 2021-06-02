////////////////////////////////////////////////////////////////////////////
//	Created		: 07.04.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_NODE_PROPERTIES_INLINE_H_INCLUDED
#define PARTICLE_NODE_PROPERTIES_INLINE_H_INCLUDED

namespace xray {
namespace editor {

inline void particle_node_properties::set_new_start_time(int value){
	math::clamp(value, 0 , 700 - m_length_time);
	if (m_start_time == value)
		return;
	m_start_time = value;
	m_length_time_pos = m_start_time + m_length_time;
	if (m_length_time_pos > 700){
		m_length_time_pos = 700;
		m_start_time = m_length_time_pos - m_length_time;
	}	
}

inline void particle_node_properties::set_start_time_impl(int value){
	if (!m_transaction_active){
		start_transaction();
		set_new_start_time(value);
		end_transaction();
	}
	else{
		set_new_start_time(value);
	}

	on_property_changed(m_owner);
}
inline void particle_node_properties::set_length_time_impl(int value){

	math::clamp(value,16,700);
	if (m_length_time == value)
		return;
	m_length_time = value;
	m_length_time_pos = m_start_time + m_length_time;
	if (m_length_time_pos > 700){
		m_length_time_pos = 700;
		m_length_time = m_length_time_pos - m_start_time;
	}
	on_property_changed(m_owner);
}

inline void	particle_node_properties::set_start_length_offsets(int offset){
	int	new_start_value = m_start_time + offset;
	int old_start_value = m_start_time;

	math::clamp(new_start_value, 0 , m_length_time_pos - 16);
	
	offset = new_start_value - old_start_value;
	if (offset == 0)
		return;

	m_start_time +=offset;
	m_length_time -= offset;
	on_property_changed(m_owner);
}

inline void				particle_node_properties::start_transaction(){
	m_transaction_active = true;	
}

inline void				particle_node_properties::end_transaction(){
	m_transaction_active = false;
	// Команда комманд_энджина(my_old_version, this)
}


} // namespace editor
} // namespace xray

#endif // #ifndef PARTICLE_NODE_PROPERTIES_INLINE_H_INCLUDED