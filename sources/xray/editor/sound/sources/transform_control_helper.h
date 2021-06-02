//-------------------------------------------------------------------------------------------
//	Created		: 29.06.2011
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2011
//-------------------------------------------------------------------------------------------
#ifndef SE_TRANSFORM_CONTROL_HELPER_H_INCLUDED
#define SE_TRANSFORM_CONTROL_HELPER_H_INCLUDED

using namespace System;

namespace xray {
namespace sound_editor {

	ref class sound_scene_document;

	public ref class se_transform_control_helper : public editor_base::transform_control_helper
	{
		typedef editor_base::transform_control_helper	super;
	public:
							se_transform_control_helper	( sound_scene_document^ doc );
		virtual float4x4	get_inverted_view_matrix	( ) override;
		virtual float4x4	get_projection_matrix		( ) override;
		virtual void		get_mouse_ray				( float3& origin, float3& direction ) override;

	private:
		sound_scene_document^ m_document;
	}; // ref class se_transform_control_helper

	public ref class se_transform_value_object: public xray::editor_base::transform_control_object
	{
	public:
						se_transform_value_object	(math::float4x4* m, Action<bool>^ d);
		virtual			~se_transform_value_object	();
		virtual void	start_modify				(xray::editor_base::transform_control_base^) override;
		virtual void	execute_preview				(xray::editor_base::transform_control_base^) override;
		virtual void	end_modify					(xray::editor_base::transform_control_base^) override;
		virtual float4x4 get_ancor_transform		() override;
				float4x4 get_start_transform		();

	private:
		Action<bool>^	m_delegate;
		math::float4x4*	m_value_transform;
		math::float4x4*	m_start_transform;
	}; // se_transform_value_object
} // namespace sound_editor
} // namespace xray
#endif // #ifndef SE_TRANSFORM_CONTROL_HELPER_H_INCLUDED