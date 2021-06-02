////////////////////////////////////////////////////////////////////////////
//	Created		: 07.07.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "collision_primitive_item.h"
#include "edit_object_base.h"
#include "model_editor.h"
#include "input_actions.h"
#include "property_grid_panel.h"

#pragma managed(push, on)
#include <xray/collision/primitives.h>
#include <xray/collision/space_partitioning_tree.h>
#include <xray/editor/base/collision_object_types.h>
#include <xray/render/facade/debug_renderer.h>
#include <xray/physics/rigid_body.h>
#pragma managed(pop)

namespace xray{
namespace model_editor{

collision_primitive_item::collision_primitive_item( edit_object_mesh^ parent )
:m_matrix		( NEW(float4x4) ),
m_primitive		( NEW(collision::primitive) ),
m_parent		( parent )
{
}

collision_primitive_item::~collision_primitive_item( )
{
	activate	( false );
	DELETE		( m_matrix );
	DELETE		( m_primitive );
}

void collision_primitive_item::render(	render::scene_ptr const& scene, 
										render::debug::renderer& r, 
										math::color const& clr )
{
	//math::color clr(104,178,35,200);//rgba
	switch (m_primitive->type)
	{
	case collision::primitive_sphere:
		{
			collision::sphere sp = m_primitive->sphere();
			r.draw_sphere_solid( scene, m_matrix->c.xyz(), sp.radius, clr );
		}break;

	case collision::primitive_box:
		{
			collision::box box = m_primitive->box();
			r.draw_cube_solid( scene, *m_matrix, box.half_side, clr );
		}break;

	case collision::primitive_cylinder:
		{
			collision::cylinder cylinder = m_primitive->cylinder();
			r.draw_cylinder_solid( scene, *m_matrix, float3(cylinder.radius, cylinder.half_length, cylinder.radius), clr );
		}break;
	}
}

void collision_primitive_item::load( configs::lua_config_value const& t, float3 const& mass_center )
{
	m_primitive->type	= (collision::primitive_type)((int)t["type"]);
	float3 position		= t["position"] + mass_center;
	float3 rotation		= t["rotation"];
	float3 scale		= t["scale"];

	*m_matrix = create_rotation(rotation)* create_translation(position);

	switch (m_primitive->type)
	{

	case collision::primitive_sphere:
		{
			m_primitive->data_ = float3(scale.x, scale.x, scale.x) ;
		}break;

	case collision::primitive_box:
		{
			m_primitive->data_ = scale;
		}break;

	case collision::primitive_cylinder:
		{
			m_primitive->data_ = scale;
		}break;
	}

}

void collision_primitive_item::save( configs::lua_config_value& t, float3 const& mass_center )
{
	t["type"]			= (int)m_primitive->type;
	t["position"]		= m_matrix->c.xyz() - mass_center;
	t["rotation"]		= m_matrix->get_angles_xyz();
	t["scale"]			= m_primitive->data_;
}

Float3 collision_primitive_item::position::get( )
{
	return Float3(m_matrix->c.xyz());
}

void collision_primitive_item::position::set( Float3 p )
{
	m_matrix->c.xyz()		= float3(p);
	m_parent->m_mass_center_item->collision_item_moved( );
	m_parent->set_modified	( );
}

Float3 collision_primitive_item::rotation::get( )
{
	float3 angles_rad		= m_matrix->get_angles_xyz();
	return Float3( angles_rad*(180.0f/math::pi) );
}

void collision_primitive_item::rotation::set( Float3 p )
{
	float3 angles_rad		= float3(p) * (math::pi/180.0f);

	*m_matrix				= create_rotation( angles_rad ) * create_translation( m_matrix->c.xyz() );
	m_parent->set_modified	( );
}

Float3 collision_primitive_item::scale::get( )
{
	return Float3(m_primitive->data_);
}

void collision_primitive_item::scale::set( Float3 p )
{
	m_primitive->data_		= float3(p);
	m_parent->set_modified	( );
}

int collision_primitive_item::type::get( )
{
	return m_primitive->type;
}

void collision_primitive_item::type::set( int t )
{
	m_primitive->type		= (collision::primitive_type)t;
	m_parent->set_modified	( );
}

collision::geometry_instance* collision_primitive_item::create_geometry_instance( )
{
	collision::geometry_instance* result = NULL; 
	switch (m_primitive->type)
	{
	case collision::primitive_sphere:
		{
			collision::sphere sp = m_primitive->sphere();
			result =  &*collision::new_sphere_geometry_instance( g_allocator, float4x4().identity(), sp.radius );

		}break;

	case collision::primitive_box:
		{
			collision::box box = m_primitive->box();
			result = &*collision::new_box_geometry_instance( g_allocator, math::create_scale(box.half_side) );
		}break;

	case collision::primitive_cylinder:
		{
			collision::cylinder cylinder = m_primitive->cylinder();
			result = &*collision::new_cylinder_geometry_instance( g_allocator, float4x4().identity(), cylinder.radius, cylinder.half_length );
		}break;
	}
	return result;
}

void collision_primitive_item::activate( bool value )
{
	if(value == (m_active_collision_object!=NULL) )
		return;

	if(value)
	{
		m_active_collision_object	= NEW (collision_primitive_item_collision)( this );
		m_parent->get_model_editor()->collision_tree->insert(
			m_active_collision_object, 
			*m_matrix
		);
	}else
	{
		set_selected				( false );
		m_parent->get_model_editor()->collision_tree->erase( m_active_collision_object );
		DELETE						( m_active_collision_object );
		m_active_collision_object	= NULL;
	}
}

math::float4x4 start_modify_matrix;

void collision_primitive_item::start_modify( editor_base::transform_control_base^ control )
{
	if(control->GetType() == editor_base::transform_control_scaling::typeid)
	{
		start_modify_matrix = math::create_scale(scale);
	}else
		start_modify_matrix = *m_matrix;
}

void collision_primitive_item::execute_preview( editor_base::transform_control_base^ control )
{
	math::float4x4 m	= control->calculate( start_modify_matrix );
	if(control->GetType() == editor_base::transform_control_scaling::typeid)
	{
		scale			= Float3(m.get_scale());
		update_collision	( );
	}else
	{
		*m_matrix		= create_rotation( m.get_angles_xyz() ) * create_translation( m.c.xyz() );
		
		m_parent->m_mass_center_item->collision_item_moved( );

		m_parent->get_model_editor()->collision_tree->move(
			m_active_collision_object,
			*m_matrix
		);
	}
	m_parent->get_collision_panel()->refresh_properties();
}

bool collision_primitive_item::is_selected( )
{
	transform_control_object^ current_object = m_parent->get_model_editor()->m_transform_control_helper->m_object;
	return (current_object == this);
}

void collision_primitive_item::set_selected( bool value )
{
	transform_control_object^ current_object = m_parent->get_model_editor()->m_transform_control_helper->m_object;

	bool is_selected_now = is_selected();
	
	if(is_selected_now == value)
		return;

	collision_primitive_item^ prev = (is_selected_now)? nullptr : dynamic_cast<collision_primitive_item^>(current_object);

	if(!m_active_collision_object)
		return;

	if(value)
	{
		if(prev)
			prev->set_selected( false );

		m_parent->get_model_editor()->m_transform_control_helper->m_object = this;
	}else
	{
		if(is_selected_now)
			m_parent->get_model_editor()->m_transform_control_helper->m_object = nullptr;
	}
	m_parent->get_model_editor()->m_transform_control_helper->m_changed = true;

	m_current_desc->is_selected = value;
}

void collision_primitive_item::end_modify( editor_base::transform_control_base^ )
{
}

float4x4 collision_primitive_item::get_ancor_transform( )
{
	return *m_matrix;
}

u32 collision_primitive_item::get_collision( System::Collections::Generic::List<editor_base::collision_object_wrapper>^% result_list )
{
	editor_base::collision_object_wrapper	wrapper;
	wrapper.m_collision_object				= m_active_collision_object;
	result_list->Add						( wrapper );

	return result_list->Count;
}

void collision_primitive_item::update_collision( )
{
	bool was_selected = is_selected();

	activate	( false );
	activate	( true );
	
	if(was_selected)
		set_selected(true);
}

collision_primitive_item_collision::collision_primitive_item_collision( collision_primitive_item^ parent )
:super( g_allocator, xray::editor_base::collision_object_type_touch, parent->create_geometry_instance() )
{
	m_parent = parent;
}

bool collision_primitive_item_collision::touch( ) const
{
	m_parent->set_selected( true );
	return true;
}

// mass center item
mass_center_item::mass_center_item( edit_object_mesh^ parent )
:m_parent		( parent )
{
}

mass_center_item::~mass_center_item( )
{
	activate	( false );
}

void mass_center_item::render(	render::scene_ptr const& scene, 
								render::debug::renderer& r )
{
	math::color clr(255,0,0,255);//rgba
	r.draw_cross(	scene, 
					float3(m_parent->m_physics_settings->mass_center), 
					0.1f, 
					clr,
					false );
}

collision::geometry_instance* mass_center_item::create_geometry_instance( )
{
	collision::geometry_instance* result = NULL; 
	result	= &*collision::new_box_geometry_instance( g_allocator, math::create_scale(float3(0.3f,0.3f,0.3f)) );

	return result;
}

void mass_center_item::activate( bool  )
{
	//if(value == (m_collision_object!=NULL) )
	//	return;

	//if(value)
	//{
	//	m_collision_object	= NEW (mass_center_item_collision)( this );
	//	m_parent->get_model_editor()->collision_tree->insert(
	//		m_collision_object, 
	//		math::create_translation(m_parent->m_physics_settings->mass_center)
	//	);
	//}else
	//{
	//	set_selected				( false );
	//	m_parent->get_model_editor()->collision_tree->erase( m_collision_object );
	//	DELETE						( m_collision_object );
	//	m_collision_object	= NULL;
	//}
}

math::float4x4 start_mass_center_modify_matrix;

void mass_center_item::start_modify( editor_base::transform_control_base^ )
{
	start_mass_center_modify_matrix = math::create_translation(m_parent->m_physics_settings->mass_center);
}

void mass_center_item::execute_preview( editor_base::transform_control_base^ control )
{
	math::float4x4 m	= control->calculate( start_mass_center_modify_matrix );

	m_parent->m_physics_settings->mass_center = Float3(m.c.xyz());

	m_parent->get_collision_panel()->refresh_properties();
}

bool mass_center_item::is_selected( )
{
	transform_control_object^ current_object = m_parent->get_model_editor()->m_transform_control_helper->m_object;
	return (current_object == this);
}

void mass_center_item::set_selected( bool value )
{
//	transform_control_object^ current_object = m_parent->get_model_editor()->m_transform_control_helper->m_object;

	bool is_selected_now = is_selected();
	
	if(is_selected_now == value)
		return;

	//mass_center_item^ prev = (is_selected_now)? nullptr : dynamic_cast<mass_center_item^>(current_object);
	//if(!m_collision_object)
	//	return;

	if(value)
	{
		//if(prev)
		//	prev->set_selected( false );

		m_parent->get_model_editor()->m_transform_control_helper->m_object = this;
	}else
	{
		if(is_selected_now)
			m_parent->get_model_editor()->m_transform_control_helper->m_object = nullptr;
	}

	m_parent->get_model_editor()->m_transform_control_helper->m_changed = true;
}

void mass_center_item::end_modify( editor_base::transform_control_base^ )
{
}

float4x4 mass_center_item::get_ancor_transform( )
{
	return	math::create_translation(m_parent->m_physics_settings->mass_center	);
}

u32 mass_center_item::get_collision( System::Collections::Generic::List<editor_base::collision_object_wrapper>^% /*result_list*/ )
{
	//editor_base::collision_object_wrapper	wrapper;
	//wrapper.m_collision_object				= m_collision_object;
	//result_list->Add						( wrapper );
	//return result_list->Count;
	return 0;
}

void mass_center_item::collision_item_moved( )
{
	if(m_parent->m_collision_primitives.Count==1)
	{
		Float3 item_position	= m_parent->m_collision_primitives[0]->position;
		m_parent->m_physics_settings->mass_center = item_position;
		m_parent->set_modified	( );
	}
}

//void mass_center_item::update_collision( )
//{
//	if(m_collision_object)
//	{
//	m_parent->get_model_editor()->collision_tree->move(
//		m_collision_object,
//		math::create_translation(m_parent->m_physics_settings->mass_center)	);
//	}
//}

//mass_center_item_collision::mass_center_item_collision( mass_center_item^ parent )
//:super( g_allocator, xray::editor_base::collision_object_type_touch, parent->create_geometry_instance() )
//{
//	m_parent = parent;
//}
//
//bool mass_center_item_collision::touch( ) const
//{
//	m_parent->set_selected	( true );
//	return					true;
//}

// rigid body construction info

rigid_body_construction_info_ref::rigid_body_construction_info_ref( edit_object_mesh^ parent )
:m_parent( parent )
{
	m_info = NEW(physics::bt_rigid_body_construction_info)();
}

rigid_body_construction_info_ref::~rigid_body_construction_info_ref()
{
	DELETE(m_info);
}

void rigid_body_construction_info_ref::save_to( configs::lua_config_ptr config )
{
	m_info->save( config->get_root() );
}
void rigid_body_construction_info_ref::load_from( configs::lua_config_ptr const config )
{
	bool load_result = m_info->load( config->get_root() );
	if(!load_result)
		m_parent->set_modified();
}
float rigid_body_construction_info_ref::mass::get()
{ return m_info->m_mass; }
void rigid_body_construction_info_ref::mass::set( float value )
{ m_info->m_mass = value; m_parent->set_modified();}

float rigid_body_construction_info_ref::linear_damping::get()
{ return m_info->m_linearDamping; }
void rigid_body_construction_info_ref::linear_damping::set( float value )
{ m_info->m_linearDamping = value; m_parent->set_modified();}

float rigid_body_construction_info_ref::angular_damping::get()
{ return m_info->m_angularDamping; }
void rigid_body_construction_info_ref::angular_damping::set( float value )
{ m_info->m_angularDamping = value; m_parent->set_modified();}

float rigid_body_construction_info_ref::friction::get()
{ return m_info->m_friction; }
void rigid_body_construction_info_ref::friction::set( float value )
{ m_info->m_friction = value; m_parent->set_modified();}

float rigid_body_construction_info_ref::restitution::get()
{ return m_info->m_restitution; }
void rigid_body_construction_info_ref::restitution::set( float value )
{ m_info->m_restitution = value; m_parent->set_modified();}

float rigid_body_construction_info_ref::linear_sleeping_threshold::get()
{ return m_info->m_linearSleepingThreshold; }
void rigid_body_construction_info_ref::linear_sleeping_threshold::set( float value )
{ m_info->m_linearSleepingThreshold = value; m_parent->set_modified();}

float rigid_body_construction_info_ref::angular_sleeping_threshold::get()
{ return m_info->m_angularSleepingThreshold; }
void rigid_body_construction_info_ref::angular_sleeping_threshold::set( float value )
{ m_info->m_angularSleepingThreshold = value; m_parent->set_modified();}

bool rigid_body_construction_info_ref::additional_damping::get()
{ return m_info->m_additionalDamping; }
void rigid_body_construction_info_ref::additional_damping::set( bool value )
{ m_info->m_additionalDamping = value; m_parent->set_modified();}

float rigid_body_construction_info_ref::additional_damping_factor::get()
{ return m_info->m_additionalDampingFactor; }
void rigid_body_construction_info_ref::additional_damping_factor::set( float value )
{ m_info->m_additionalDampingFactor = value; m_parent->set_modified();}

float rigid_body_construction_info_ref::additional_linear_damping_threshold_sqr::get()
{ return m_info->m_additionalLinearDampingThresholdSqr; }
void rigid_body_construction_info_ref::additional_linear_damping_threshold_sqr::set( float value )
{ m_info->m_additionalLinearDampingThresholdSqr = value; m_parent->set_modified();}

float rigid_body_construction_info_ref::additional_angular_damping_threshold_sqr::get()
{ return m_info->m_additionalAngularDampingThresholdSqr; }
void rigid_body_construction_info_ref::additional_angular_damping_threshold_sqr::set( float value )
{ m_info->m_additionalAngularDampingThresholdSqr = value; m_parent->set_modified();}

float rigid_body_construction_info_ref::additional_angular_damping_factor::get()
{ return m_info->m_additionalAngularDampingFactor; }
void rigid_body_construction_info_ref::additional_angular_damping_factor::set( float value )
{ m_info->m_additionalAngularDampingFactor = value; m_parent->set_modified();}

Float3 rigid_body_construction_info_ref::mass_center::get()
{ return Float3(m_info->m_render_model_offset); }

void  rigid_body_construction_info_ref::mass_center::set( Float3 p )
{	m_info->m_render_model_offset = float3(p);  
	m_parent->set_modified();
	//m_parent->m_mass_center_item->update_collision();
}

}
}