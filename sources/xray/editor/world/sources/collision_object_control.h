////////////////////////////////////////////////////////////////////////////
//	Created		: 13.03.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef CONTROL_COLLISION_OBJECT_H_INCLUDED
#define CONTROL_COLLISION_OBJECT_H_INCLUDED

#include "collision_object.h"

namespace xray {
namespace editor {

class collision_object_transform_control : public collision_object
{
public:
	collision_object_transform_control	(	collision::geometry const* geometry,
											transform_control_base^ owner_control, 
											enum_transform_axis axis );

	virtual	void							set_matrix			( xray::math::float4x4 const& matrix );

	virtual	void							set_matrix_original		( math::float4x4 const& matrix );
	virtual	math::float4x4 const&			get_matrix_original		( ) const;

			enum_transform_axis				get_axis			( ) const;
			transform_control_base^			get_owner			( ) const { return m_owner; };

private:
	gcroot<transform_control_base^>			m_owner;
	enum_transform_axis						m_axis;

	math::float4x4							m_transform_original;
	math::float4x4							m_transform_general;
}; //collision_object_transform_control


}// namespace editor
}// namespace xray

#endif // #ifndef CONTROL_COLLISION_OBJECT_H_INCLUDED