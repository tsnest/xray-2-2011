////////////////////////////////////////////////////////////////////////////
//	Created		: 06.01.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_SHADER_CONSTANT_SLOT_H_INCLUDED
#define XRAY_RENDER_CORE_SHADER_CONSTANT_SLOT_H_INCLUDED

namespace xray {
namespace render {

// the 255th index is considered the NULL buffer index
// slot with NULL buffer index is disabled
enum {
	slot_dest_buffer_null = static_cast<u16>( -1),
};

class shader_constant_slot {
public:
	inline			shader_constant_slot( );
	inline	u16		class_id			( ) const;
	inline	u16		array_size			( ) const;
	inline	u16		buffer_index		( ) const;
	inline	u16		slot_index			( ) const;
	inline	u64		value				( ) const;

public:
	union {
		// The ordering is made with consideration struct alignment to 32 bits
#pragma warning( push )
#pragma warning( disable : 4201 )
		struct {
			u16	m_class_id;				// element class ( enum_constant_class )
			u16	m_array_size;			// Array size if the shader constant slot expects array of elements, otherwise 0.
			u16	m_buffer_index;			// shader constant buffer index 
			u16	m_slot_index;			// linear index ( pixel )
		};
#pragma warning( pop )
		u64		m_value;
	};
}; // class shader_constant_slot

	inline	bool	operator ==			( shader_constant_slot const& left, shader_constant_slot const& right );

} // namespace render
} // namespace xray

#include <xray/render/core/shader_constant_slot_inline.h>

#endif // #ifndef XRAY_RENDER_CORE_SHADER_CONSTANT_SLOT_H_INCLUDED