////////////////////////////////////////////////////////////////////////////
//	Created		: 23.06.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "portal_vertex_shift.h"
#include "edit_portal.h"

namespace xray {
namespace model_editor {

portal_vertex_shift::portal_vertex_shift( edit_portal^ portal, on_modified_delegate^ on_modified, on_modified_delegate^ on_end_modify )
:m_portal( portal ),
m_identity_martrix( NULL ),
m_ancor_transform( NULL ),
m_previous_transform( NULL ),
m_on_modified( on_modified ),
m_on_end_modify( on_end_modify )
{
	m_indices = m_portal->get_selected_vertices( );
	R_ASSERT( m_indices->Count != 0 );
	m_identity_martrix = NEW (math::float4x4) ( );
	m_identity_martrix->identity();
	m_ancor_transform = NEW (math::float4x4) ( );
	m_ancor_transform->identity();
	*m_ancor_transform = math::create_translation( m_portal->get_untransformed_vertex( m_indices[0] ) ) * m_portal->get_transformation();
	m_previous_transform = NEW (math::float4x4) (  );
	m_previous_transform->identity();
}

portal_vertex_shift::~portal_vertex_shift( )
{
	DELETE ( m_previous_transform );
	DELETE ( m_ancor_transform );
	DELETE ( m_identity_martrix );
}

void portal_vertex_shift::start_modify( editor_base::transform_control_base^ control )
{
	if ( control->GetType() != editor_base::transform_control_translation::typeid )//only translation can affect on vertices
		return;
	ASSERT( m_portal->get_selected_vertices( )->Count != 0 );
	m_indices = m_portal->get_selected_vertices( );
	*m_ancor_transform = math::create_translation( m_portal->get_untransformed_vertex( m_indices[0] ) ) * m_portal->get_transformation();
	control->set_matrix( *m_ancor_transform );
}

void portal_vertex_shift::execute_preview( editor_base::transform_control_base^ control )
{
	if ( control->GetType() != editor_base::transform_control_translation::typeid )//only translation can affect on vertices
		return;
	math::float4x4 m = control->calculate( *m_identity_martrix );
	ASSERT(m.valid());

	bool bneed_update	= 
		!math::is_similar( m.i.xyz( ), ( *m_previous_transform).i.xyz( ) ) ||
		!math::is_similar( m.j.xyz( ), ( *m_previous_transform).j.xyz( ) ) ||
		!math::is_similar( m.k.xyz( ), ( *m_previous_transform).k.xyz( ) ) ||
		!math::is_similar( m.c.xyz( ), ( *m_previous_transform).c.xyz( ) ) ;

	if ( bneed_update )
	{
		this->update	( m );
		control->set_matrix( *m_ancor_transform );
	}
}

void portal_vertex_shift::end_modify( editor_base::transform_control_base^ control )
{
	if ( control->GetType() != editor_base::transform_control_translation::typeid )//only translation can affect on vertices
		return;
	math::float4x4 m	=	control->calculate( *m_identity_martrix );
	ASSERT(m.valid());
	this->update	( m );
	m_indices = nullptr;
	m_previous_transform->identity();
	if ( m_on_end_modify != nullptr )
		m_on_end_modify( );
}

float4x4 portal_vertex_shift::get_ancor_transform( )
{
	return *m_ancor_transform;
}

u32 portal_vertex_shift::get_collision( System::Collections::Generic::List<editor_base::collision_object_wrapper>^% collisions )
{
	return m_portal->get_collisions( collisions );
}

void portal_vertex_shift::update( math::float4x4 const& m )
{
	for each ( int index in m_indices )
	{
		m_portal->shift_vertex( index, m.c.xyz() - m_previous_transform->c.xyz() );
	}

	*m_previous_transform = m;


	*m_ancor_transform = math::create_translation( m_portal->get_untransformed_vertex( m_indices[0] ) ) * m_portal->get_transformation();
	if ( m_on_modified )
		m_on_modified( );
}

} // namespace model_editor
} // namespace xray