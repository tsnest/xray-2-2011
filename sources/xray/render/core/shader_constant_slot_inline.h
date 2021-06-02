////////////////////////////////////////////////////////////////////////////
//	Created		: 06.01.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_SHADER_CONSTANT_SLOT_INLINE_H_INCLUDED
#define XRAY_RENDER_CORE_SHADER_CONSTANT_SLOT_INLINE_H_INCLUDED

namespace xray {
namespace render {

inline shader_constant_slot::shader_constant_slot	( ) :
	m_class_id		( static_cast<u16>(-1) ),
	m_array_size	( 0 ),
	m_buffer_index	( slot_dest_buffer_null ),
	m_slot_index	( static_cast<u16>(-1) )
{
}

inline u16 shader_constant_slot::class_id			( ) const
{
	return			m_class_id;
}

inline u16 shader_constant_slot::array_size			( ) const
{
	return			m_array_size;
}

inline u16 shader_constant_slot::buffer_index		( ) const
{
	return			m_buffer_index;
}

inline u16 shader_constant_slot::slot_index			( ) const
{
	return			m_slot_index;
}

inline u64 shader_constant_slot::value				( ) const
{
	return			m_value;
}

inline bool operator ==								( shader_constant_slot const& left, shader_constant_slot const& right)
{
	return			left.value() == right.value();
}

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_CORE_SHADER_CONSTANT_SLOT_INLINE_H_INCLUDED