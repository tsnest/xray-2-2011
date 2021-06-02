////////////////////////////////////////////////////////////////////////////
//	Created		: 09.03.2011
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef TRANSFORM_CONTROL_HELPER_H_INCLUDED
#define TRANSFORM_CONTROL_HELPER_H_INCLUDED

namespace xray {
namespace animation_editor {

	ref class animation_editor;

	public ref class ae_transform_control_helper : public editor_base::transform_control_helper
	{
		typedef editor_base::transform_control_helper	super;
	public:
							ae_transform_control_helper	( animation_editor^ ae );
		virtual float4x4	get_inverted_view_matrix	( ) override;
		virtual float4x4	get_projection_matrix		( ) override;
		virtual void		get_mouse_ray				( float3& origin, float3& direction ) override;

	private:
		animation_editor^	m_animation_editor;
	}; // ref class ae_transform_control_helper
} // namespace animation_editor
} // namespace xray
#endif // #ifndef TRANSFORM_CONTROL_HELPER_H_INCLUDED