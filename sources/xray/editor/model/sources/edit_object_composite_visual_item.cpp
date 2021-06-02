////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "edit_object_composite_visual.h"
#include "model_editor.h"
#include "input_actions.h"
#include "render_model_wrapper.h"

#pragma managed( push, off )
#include <xray/collision/space_partitioning_tree.h>
#include <xray/render/facade/editor_renderer.h>
#include <xray/editor/base/collision_object_types.h>
#pragma managed( pop )

using namespace xray::editor::wpf_controls;

namespace xray{
namespace model_editor{

composite_visual_item::composite_visual_item( edit_object_composite_visual^ parent )
:m_parent					( parent ),
m_matrix					( NEW(math::float4x4) ),
m_pivot						( NEW(math::float3) ),
m_active_collision_object	( NULL ),
m_hq_collision_vertices		( NEW(resources::managed_resource_ptr) ),
m_hq_collision_indices		( NEW(resources::managed_resource_ptr) )
{
	m_model					= gcnew render_model_wrapper;
	m_matrix->identity();
	m_pivot->set			(0,0,0);
}

composite_visual_item::~composite_visual_item( )
{
	destroy_contents	( );
	delete				m_model;
	DELETE				( m_matrix );
	DELETE				( m_pivot );
	DELETE				( m_hq_collision_vertices );
	DELETE				( m_hq_collision_indices );
}

void composite_visual_item::model_name::set( System::String^ value )
{
	m_model_name		= value;
	load_model			( );
}

void composite_visual_item::destroy_contents( )
{
	if(!m_model->empty())
	{
		m_parent->get_model_editor()->collision_tree->erase	( m_active_collision_object );
		DELETE						( m_active_collision_object );

		m_model->remove_from_scene	( m_parent->get_model_editor()->editor_renderer()->scene(), 
										m_parent->get_model_editor()->scene() );
	}
}

void composite_visual_item::load_model( )
{
	destroy_contents		( );
	query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, 
														&composite_visual_item::on_model_ready), 
														g_allocator);

	resources::class_id_enum cls = resources::static_model_instance_class;
	
	unmanaged_string		s(m_model_name);
	fs_new::virtual_path_string			hq_collision_path;
	fs_new::virtual_path_string			hq_collision_indices;
	fs_new::virtual_path_string			hq_collision_vertices;

	hq_collision_path.assignf( "resources/models/%s.model/hq_collision", s.c_str() );
	hq_collision_vertices.assignf("%s/vertices", hq_collision_path.c_str());
	hq_collision_indices.assignf("%s/indices", hq_collision_path.c_str());

	resources::request r[]={
		{ s.c_str(), cls},
		{ hq_collision_path.c_str(), resources::collision_geometry_class },
		{ hq_collision_vertices.c_str(), resources::raw_data_class },
		{ hq_collision_indices.c_str(), resources::raw_data_class }
	};

	resources::query_resources	(
		r,
		boost::bind(&query_result_delegate::callback, q, _1),
		g_allocator
		);

}

void composite_visual_item::on_model_ready( resources::queries_result& data )
{
	if ( data[0].is_successful() )
	{
		*m_hq_collision_vertices		= data[2].get_managed_resource();
		*m_hq_collision_indices			= data[3].get_managed_resource();

		model_editor^ me = m_parent->get_model_editor();

		m_model->set_resource			( data[0].get_unmanaged_resource(), data[1].get_unmanaged_resource() );
		m_model->add_to_scene			( me->editor_renderer()->scene(), 
										me->scene(), 
										*m_matrix );
		me->refresh_caption				( );

		m_active_collision_object		= NEW (composite_visual_item_collision)( this );
		me->collision_tree->insert		( m_active_collision_object, *m_matrix );

		m_parent->get_model_editor()->action_focus_to_selection();
	}
}

math::aabb  composite_visual_item::get_aabb( )
{
	if(m_active_collision_object)
		return m_active_collision_object->get_aabb();
	else
		return math::create_identity_aabb().modify(*m_matrix);
}

math::float4x4 start_matrix;
math::float3 start_pivot;

void composite_visual_item::start_modify( editor_base::transform_control_base^ )
{
	start_matrix	= *m_matrix;
	start_pivot		= *m_pivot;
}

void composite_visual_item::execute_preview( editor_base::transform_control_base^ control )
{
	if(current_mode()==super::mode::edit_transform)
	{
		float4x4 const XF		= start_matrix;

		float4x4 const P		= create_translation(*m_pivot) * XF;
		float4x4 P_i			= math::invert4x3( P );
		
		math::float4x4 const m	= control->calculate( P );
		
		float4x4 m_i			= P_i * m;
		float4x4 mr				= XF * m_i;

		set_matrix				( mr );

//		math::float4x4 m	= control->calculate( start_matrix );
//		*m_matrix			= m;
		on_matrix_changed		( );
	}else
	{
		float3 const current_pivot	= start_pivot;
		float4x4 const& XF			= *m_matrix;
		float4x4 XF_i				= math::invert4x3( XF );

		float4x4 const P			= create_translation(current_pivot) * XF;

		float4x4 const m			= control->calculate( P );
		
		float4x4 const mi					= m * XF_i;
		*m_pivot					= mi.c.xyz();
	}
}

void composite_visual_item::end_modify( editor_base::transform_control_base^ )
{}

float4x4 composite_visual_item::get_ancor_transform( )
{
	return create_translation(*m_pivot) * *m_matrix;
}

u32 composite_visual_item::get_collision( System::Collections::Generic::List<editor_base::collision_object_wrapper>^% result_list )
{
	editor_base::collision_object_wrapper	wrapper;
	wrapper.m_collision_object				= m_active_collision_object;
	result_list->Add						( wrapper );

	return result_list->Count;
}

void composite_visual_item::set_selected( bool value )
{
	if(value)
	{
		m_parent->get_model_editor()->m_transform_control_helper->m_object = this;
	}else
	{
		if(get_selected())
			m_parent->get_model_editor()->m_transform_control_helper->m_object = nullptr;
	}
	m_parent->get_model_editor()->m_transform_control_helper->m_changed = true;
}

bool composite_visual_item::get_selected( )
{
	return (m_parent->get_model_editor()->m_transform_control_helper->m_object == this);
}

Float3 composite_visual_item::position::get( )
{
	return Float3(m_matrix->c.xyz());
}

void composite_visual_item::position::set( Float3 p )
{
	float4x4 m				= *m_matrix;
	m.c.xyz()				= float3(p);
	set_matrix				( m );
}

Float3 composite_visual_item::rotation::get( )
{
	float3 angles_rad		= m_matrix->get_angles_xyz();
	return Float3( angles_rad*(180.0f/math::pi) );
}

void composite_visual_item::set_matrix( float4x4 const& m )
{
	*m_matrix				= m;
	R_ASSERT(m_matrix->valid());
	m_parent->set_modified	( );
	on_matrix_changed		( );
}

void composite_visual_item::rotation::set( Float3 p )
{
	float3 angles_rad		= float3(p) * (math::pi/180.0f);

	set_matrix( create_rotation( angles_rad ) * create_translation( m_matrix->c.xyz() ) );
}

Float3 composite_visual_item::scale::get( )
{
	return Float3(m_matrix->get_scale());
}

void composite_visual_item::scale::set( Float3 p )
{
	float const min_scale = 0.05f;
	float const max_scale = 1000.0f;

	math::clamp( p.x, min_scale, max_scale );
	math::clamp( p.y, min_scale, max_scale );
	math::clamp( p.z, min_scale, max_scale );

	float4x4 m			= *m_matrix;

 	m.set_scale			( p );
	set_matrix			( m );
}

void composite_visual_item::on_matrix_changed( )
{
	model_editor^ me = m_parent->get_model_editor();

	if( !m_model->empty() ) 
	{
		me->collision_tree->move	( m_active_collision_object, *m_matrix );
		m_model->move				( me->editor_renderer()->scene(), me->scene(), *m_matrix );
	}
}

composite_visual_item_collision::composite_visual_item_collision( composite_visual_item^ parent )
:super( g_allocator, 
	   xray::editor_base::collision_object_type_touch|xray::editor_base::collision_object_type_dynamic, 
	   &*collision::new_triangle_mesh_geometry_instance( g_allocator, float4x4().identity(),  
														parent->m_model->get_collision_geometry().c_ptr()) )
{
	m_parent = parent;
}

bool composite_visual_item_collision::touch( ) const
{
	m_parent->set_selected( true );
	return true;
}

}
}
