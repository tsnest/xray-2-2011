////////////////////////////////////////////////////////////////////////////
//	Created		: 13.03.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef CONTROL_COLLISION_OBJECT_H_INCLUDED
#define CONTROL_COLLISION_OBJECT_H_INCLUDED

#include <xray/collision/collision_object.h>
#include <xray/editor/base/transform_control_defines.h>
#include "transform_control_base.h"


namespace xray {
namespace editor_base {

ref class transform_control_base;

class collision_object_transform_control : public collision::collision_object
{
public:
					collision_object_transform_control	(
										collision::geometry_instance* geometry,
										transform_control_base^ owner_control, 
										enum_transform_axis axis
									);

	virtual	void					set_matrix			( math::float4x4 const& matrix );
	virtual	void					set_matrix_original	( math::float4x4 const& matrix );

			enum_transform_axis		get_axis			( ) const;
	inline	transform_control_base^	get_owner			( ) const { return m_owner; };

	inline	float4x4 const&		get_original_transform	( ) const { return m_transform_original; }

private:
	typedef collision::collision_object		super;

private:
	math::float4x4					m_transform_original;
	math::float4x4					m_transform_general;

	gcroot<transform_control_base^>	m_owner;
	enum_transform_axis				m_axis;
}; //collision_object_transform_control


}// namespace editor_base
}// namespace xray

#endif // #ifndef CONTROL_COLLISION_OBJECT_H_INCLUDED
