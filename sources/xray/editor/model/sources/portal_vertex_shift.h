////////////////////////////////////////////////////////////////////////////
//	Created		: 23.06.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PORTAL_VERTEX_SHIFT_H_INCLUDED
#define PORTAL_VERTEX_SHIFT_H_INCLUDED

namespace xray {
namespace model_editor {
ref class edit_portal;
ref class portal_vertex_shift: editor_base::transform_control_object {
public:
	delegate void					on_modified_delegate();
									portal_vertex_shift			( edit_portal^ portal, on_modified_delegate^ on_modified, on_modified_delegate^ on_end_modify );
	virtual							~portal_vertex_shift		( );
	virtual void					start_modify				( editor_base::transform_control_base^ control ) override;
	virtual void					execute_preview				( editor_base::transform_control_base^ control ) override;
	virtual void					end_modify					( editor_base::transform_control_base^ control ) override;
	virtual float4x4				get_ancor_transform			( ) override;
	virtual u32						get_collision				( System::Collections::Generic::List<editor_base::collision_object_wrapper>^% ) override;
	virtual bool					translation_ignore_rotation	( ) override { return false; }
private:
	void							update						( math::float4x4 const& m );
	typedef System::Collections::Generic::IList<int> indices_ilist;
	indices_ilist^					m_indices;
	edit_portal^					m_portal;
	math::float4x4*					m_identity_martrix;
	math::float4x4*					m_ancor_transform;
	math::float4x4*					m_previous_transform;
	on_modified_delegate^			m_on_modified;
	on_modified_delegate^			m_on_end_modify;
}; // class portal_vertex_shift

} // namespace model_editor
} // namespace xray

#endif // #ifndef PORTAL_VERTEX_SHIFT_H_INCLUDED