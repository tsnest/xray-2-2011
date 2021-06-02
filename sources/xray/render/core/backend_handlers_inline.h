////////////////////////////////////////////////////////////////////////////
//	Created		: 19.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BACKEND_HANDLERS_INLINE_H_INCLUDED
#define BACKEND_HANDLERS_INLINE_H_INCLUDED

namespace xray {
namespace render {

template <enum_shader_type shader_type>
template <typename T>
void constants_handler<shader_type>::set_constant( shader_constant_host const & c, T const& arg)
{
	COMPILE_ASSERT(	constant_type_traits<T>::value == true, Only_types_described_in_constant_h_are_supported );

	u32 buffer_index = c.shader_slot(shader_type).buffer_index();
	if( c.m_update_markers[shader_type] == backend::ref().get_constants_marker<shader_type>() 
		&& ( buffer_index = c.shader_slot(shader_type).buffer_index()) != slot_dest_buffer_null)
	{
		ASSERT( c.shader_slot(shader_type).array_size() == 1, "Single shader constant can't be set into an array.");

		ASSERT( (constant_class_cast_mask & c.shader_slot(shader_type).class_id()) == (constant_type_traits<T>::class_id&constant_class_cast_mask)
			,"Wrong type! The given object cant be cast to the corresponding shader constant slot.");
		ASSERT( (c.shader_slot(shader_type).class_id() & constant_class_size_mask) <= constant_type_traits<T>::size, "The given object has smaller dimension than the corresponding shader constant slot!");

		ASSERT( c.type() == constant_type_traits<T>::type, "The given object dosen't have the same type as the corresponding shader constant slot!");

		m_current->m_const_buffers[buffer_index]->set_typed( c.shader_slot(shader_type), arg);

#ifdef DEBUG
		render::vector<shader_constant_host*>::iterator it = std::find( m_unset_constants.begin(), m_unset_constants.end(), &c);
		if( it != m_unset_constants.end())
			m_unset_constants.erase( it);
#endif //DEBUG
		
	}
}

template <enum_shader_type shader_type>
template <typename T>
void constants_handler<shader_type>::set_constant_array( shader_constant_host const & c, T const * arg, u32 array_size)
{
	COMPILE_ASSERT(	constant_type_traits<T>::value == true, Only_types_described_in_constant_h_are_supported );

	u32 buffer_index;
	if( c.m_update_markers[shader_type] == backend::ref().get_constants_marker<shader_type>() 
		&& ( buffer_index = c.shader_slot(shader_type).buffer_index()) != slot_dest_buffer_null)
	{
		// == may be changed to < 
		//ASSERT( c.shader_slot(shader_type).array_size() == array_size);
		ASSERT( c.shader_slot(shader_type).array_size() >= array_size);

		ASSERT( c.shader_slot(shader_type).class_id() == constant_type_traits<T>::class_id,"Wrong type! The given object can't be cast to the corresponding shader constant slot.");
		ASSERT( (c.shader_slot(shader_type).class_id() & constant_class_size_mask) == constant_type_traits<T>::size, "The given object has smaller dimension than the corresponding shader constant slot!");

		ASSERT( c.type() == constant_type_traits<T>::type, "The given object dosen't have the same type as the corresponding shader constant slot!");
		
		m_current->m_const_buffers[buffer_index]->set_typed( c.shader_slot(shader_type), arg, array_size);

#ifdef DEBUG
		render::vector<shader_constant_host*>::iterator it = std::find( m_unset_constants.begin(), m_unset_constants.end(), &c);
		if( it != m_unset_constants.end())
			m_unset_constants.erase( it);
#endif //DEBUG

	}
}

} // namespace render
} // namespace xray

#endif // #ifndef BACKEND_HANDLERS_INLINE_H_INCLUDED