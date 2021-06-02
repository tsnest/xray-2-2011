////////////////////////////////////////////////////////////////////////////
//	Created		: 04.11.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TRANSFORM_CONTROL_HELPER_H_INCLUDED
#define TRANSFORM_CONTROL_HELPER_H_INCLUDED

namespace xray {

namespace particle_editor {

ref class particle_editor;

public ref class pe_transform_control_helper : public editor_base::transform_control_helper
{
	typedef editor_base::transform_control_helper	super;
public:
						pe_transform_control_helper( particle_editor^ pe );
	virtual float4x4	get_inverted_view_matrix	( ) override;
	virtual float4x4	get_projection_matrix		( ) override;
	virtual void		get_mouse_ray				( float3& origin, float3& direction) override;

private:
	particle_editor^ m_particle_editor;
};

} // namespace editor
} // namespace xray

#endif // #ifndef TRANSFORM_CONTROL_HELPER_H_INCLUDED