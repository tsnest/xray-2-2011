////////////////////////////////////////////////////////////////////////////
//	Created		: 06.01.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_SHADER_CONSTANT_HOST_H_INCLUDED
#define XRAY_RENDER_CORE_SHADER_CONSTANT_HOST_H_INCLUDED

#include <xray/render/core/shader_constant_source.h>
#include <xray/render/core/shader_constant_slot.h>

namespace xray {
namespace render {

class shader_constant_host {
public:
	inline									shader_constant_host( shared_string const& name, enum_constant_type type );
			void							set					( float value );
			void							set					( math::float2 const& value );
			void							set					( math::float3 const& value );
			void							set					( math::float4 const& value );
			void							set					( math::float4x4 const& value );
			void							set					( int value );
			void							set					( math::int2 const& value );
			void							reset_update_markers( );
	inline	shader_constant_source const&	source				( ) const;
	inline	shader_constant_slot const&		shader_slot			( u32 slot_id ) const;
	inline	shader_constant_slot&			shader_slot			( u32 slot_id );
	inline	shared_string const&			name				( ) const;
	inline	enum_constant_type				type				( ) const;

public:
	shader_constant_source	m_source;
	shader_constant_slot	m_shader_slots[3];
	shared_string			m_name;	// HLSL-name
	u32						m_update_markers[3];
	enum_constant_type		m_type;				
}; // class shader_constant_host

	inline	bool							operator ==			( shader_constant_host const& left, shared_string const& right );
	inline	bool							operator ==			( shared_string const& left, shader_constant_host const& right );
	inline	bool							operator <			( shader_constant_host const& left, shared_string const& other );
	inline	bool							operator <			( shared_string const& left, shader_constant_host const& right );
	inline	bool							operator ==			( shader_constant_host const& left, shader_constant_host const& right );
	inline	bool							operator <			( shader_constant_host const& left, shader_constant_host const& right );

} // namespace render
} // namespace xray

#include <xray/render/core/shader_constant_host_inline.h>

#endif // #ifndef XRAY_RENDER_CORE_SHADER_CONSTANT_HOST_H_INCLUDED