////////////////////////////////////////////////////////////////////////////
//	Created		: 04.11.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "view_control_actions.h"
#include "scene_view_panel.h"

using namespace System;


namespace xray {
namespace editor_base {

view_action_base::view_action_base( System::String^ name, scene_view_panel^ w )
:action_continuous		( name ),
m_view_window			( w )
{
}

view_action_base::~view_action_base( )
{
}

void view_action_base::init_capture( )
{
	m_view_window->set_infinite_mode	( true );
	m_mouse_last_pos					= m_view_window->get_mouse_pos_infinite();
}

void view_action_base::release( )
{
	m_view_window->set_infinite_mode( false );
}


mouse_action_view_move_xy::mouse_action_view_move_xy( System::String^ name, scene_view_panel^ w )
:super( name, w )
{
}

bool mouse_action_view_move_xy::execute( )
{ 
	// Get Mouse current position
	Drawing::Point mouse_position = m_view_window->get_mouse_pos_infinite( );

	// Factors for moving in the direction axises   
	float right = 0.f;
	float up = 0.f;

	// Mouse move start point 
	float2 drag_start	(	(float)m_mouse_last_pos.X ,	(float)m_mouse_last_pos.Y	);

	// Mouse move current point
	float2 drag			(	(float)mouse_position.X,	(float)mouse_position.Y	);

	// Convert screen coordinates to 3d space coordinates by a given z coordinate. 
	float3 drag_start_3d	= m_view_window->screen_to_3d	( drag_start, 80.0f * m_view_window->mouse_sensitivity() );
	float3 drag_3d			= m_view_window->screen_to_3d	( drag, 80.0f * m_view_window->mouse_sensitivity() );

	// Factor for moving by x axis is equal to delta of the 3d points. That is 
	// the camera will be moved right an/or left instead of moving the world around it.
	right	=	-	(drag_3d.x - drag_start_3d.x);
	// The same with y 
	up		=		(drag_3d.y - drag_start_3d.y);

	// Save the current position to calculate next delta relative to the current position.
	m_mouse_last_pos = mouse_position;

	if( ! ( math::is_zero(right) && math::is_zero(up) ) )
	{
		// Set view matrix without any rotation; just moving it right/left or up/down.
		m_view_window->set_view_matrix			( float2( 0.f, 0.f ), 0, right, up );
	}

	return true; 
}

bool mouse_action_view_move_xy::capture( ) 
{
	// Initialize starting parameters 
	super::init_capture( );
	return true;
}

void mouse_action_view_move_xy::release( )
{
	super::release					( );
}

mouse_action_view_move_z::mouse_action_view_move_z( System::String^ name, scene_view_panel^ w )
:super				( name, w ),
m_continuse_drugging( false )
{	
}

bool mouse_action_view_move_z::execute( ) 
{ 
	Drawing::Point mouse_position = m_view_window->get_mouse_pos_infinite( );

	if(m_mouse_last_pos == mouse_position)
		return true;

	float forward		= (float)( mouse_position.X-m_mouse_last_pos.X )  + ( mouse_position.Y-m_mouse_last_pos.Y );
	m_mouse_last_pos	= mouse_position;

	float sens	= (2.0f/15.0f) * m_view_window->mouse_sensitivity();
	forward		*= sens;


	float dist = m_view_window->view_point_distance()-forward;
	if(math::negative(dist))
		dist = 5.0f;

	m_view_window->set_view_point_distance( dist );

	if( math::is_zero(forward) || math::is_zero(dist) )
		return true;

	// Set new view matrix my moving it forward or back according to factor.
	m_view_window->set_view_matrix			( math::float2( 0.f, 0.f ), forward, 0, 0 );

	// draw rotation origin
	math::float4x4 m	= m_view_window->get_inverted_view_matrix();
	math::float3 dir	= m.k.xyz();
	dir *= m_view_window->view_point_distance();

	float3 cp = m.c.xyz() + dir;
	m_view_window->debug_renderer()->draw_origin( m_view_window->scene(), create_translation(cp), 2.0f );

	return true;
}

bool mouse_action_view_move_z::capture( ) 
{
	super::init_capture				( );
	return true;
}

void mouse_action_view_move_z::release( )
{
	super::release					( );
}

mouse_action_view_rotate_around::mouse_action_view_rotate_around( System::String^ name, scene_view_panel^ w )
:mouse_action_view_rotate_base( name, w )
{
}

bool mouse_action_view_rotate_around::execute( )
{ 
	Drawing::Point mouse_position = m_view_window->get_mouse_pos_infinite( );

	float dx			= float( mouse_position.Y-m_mouse_last_pos.Y );
	float dy			= float( mouse_position.X-m_mouse_last_pos.X );

	m_mouse_last_pos		= mouse_position;

	float4x4 const inv_view = m_view_window->get_inverted_view_matrix();
	float3 cp				= (inv_view.c + inv_view.k * m_view_window->view_point_distance() ).xyz();

	m_view_window->debug_renderer()->draw_origin( m_view_window->scene(), create_translation(cp), 2.0f );

	if( math::is_zero(dx) && math::is_zero(dy) )
		return true;

	dx						= 0.5f * math::deg2rad(dx);
	dy						= 0.5f * math::deg2rad(dy*0.75f);
	math::clamp				( dx, -math::pi_d6, math::pi_d6);

	rotate_view_matrix		( float3( dx, dy, 0 ), m_view_window->view_point_distance() );
	return					true;
}

bool mouse_action_view_rotate_around::capture( )
{
	if(!m_view_window->is_mouse_in_view())
		return false;

	super::init_capture		( );
	return true;
}

void mouse_action_view_rotate_around::release( )
{
	super::release			( );
}

void mouse_action_view_rotate_base::rotate_view_matrix( float3 const& raw_angles_add, float const focus_distance )
{
	float4x4 inv_view_new				= m_view_window->get_inverted_view_matrix();
	float3 const angles_prev			= inv_view_new.get_angles_xyz();
	float3 	origin						= inv_view_new.c.xyz() + inv_view_new.k.xyz()*focus_distance;

	inv_view_new.c.xyz()				= float3(0,0,0);
	float4x4 rotation_y					= math::create_rotation_y( -raw_angles_add.y );

	float4x4 rotation_x					= math::create_rotation( inv_view_new.i.xyz(), -raw_angles_add.x );

	inv_view_new						= inv_view_new * rotation_x * rotation_y;
	inv_view_new.c.xyz()				= origin - inv_view_new.k.xyz()*focus_distance;
	
	m_view_window->set_view_matrix		( math::invert4x3(inv_view_new) );
}

mouse_action_view_move_y_reverse::mouse_action_view_move_y_reverse( System::String^ name, scene_view_panel^ w )
:super	( name, w )
{
}

bool mouse_action_view_move_y_reverse::execute( )
{ 
	Drawing::Point mouse_position	= m_view_window->get_mouse_pos_infinite();

	// Factors for moving in the direction axises   
	float up = 0.f;

	// Mouse move start point 
	float2 drag_start	(	(float)m_mouse_last_pos.X ,	(float)m_mouse_last_pos.Y	);

	// Mouse move current point
	float2 drag			(	(float)mouse_position.X,	(float)mouse_position.Y	);

	// Calculate Delta 
	up		=		-(drag.y - drag_start.y)*0.05f;

	// Save the current position to calculate next delta relative to the current position.
	m_mouse_last_pos.Y = mouse_position.Y;

	if( math::is_zero(up) )
		return true;

	// Set view matrix without any rotation; just moving it up/down by world y axis.
	float4x4 m				= m_view_window->get_inverted_view_matrix() * math::create_translation( float3( 0, up, 0));
	m_view_window->set_view_matrix( math::invert4x3( m ) );

	return true;
}

bool mouse_action_view_move_y_reverse::capture( )
{
	super::init_capture				( );
	return true;
}

void mouse_action_view_move_y_reverse::release( ) 
{
	super::release					( );
	m_view_window->set_infinite_mode( false );
}

mouse_action_view_move_xz_reverse::mouse_action_view_move_xz_reverse( System::String^ name, scene_view_panel^ w )
:super	( name, w )
{
}

bool mouse_action_view_move_xz_reverse::execute( )
{ 
	Drawing::Point mouse_position	= m_view_window->get_mouse_pos_infinite();

	// Factors for moving in the direction axises   
	float right = 0.f;
	float forward = 0.f;

	// Calculate deltas
	right				= (mouse_position.X - m_mouse_last_pos.X)*0.05f;
	forward				= -(mouse_position.Y - m_mouse_last_pos.Y)*0.05f;

	// Save the current position to calculate next delta relative to the current position.
	m_mouse_last_pos	= mouse_position;

	if( math::is_zero(right) && math::is_zero(forward) )
		return true;

	math::float4x4 inv_view	= m_view_window->get_inverted_view_matrix();

	// Set view matrix without any rotation; just moving it right/left or forward/back by world axes.
	float3 right_vector = inv_view.i.xyz();

	float3 forward_vector	= float3( 0, 1, 0 ) ^ right_vector;
	forward_vector.normalize();

	forward_vector			*= -forward;
	right_vector			*= right;
	
	float4x4 m							= inv_view * math::create_translation( forward_vector+right_vector );
	m_view_window->set_view_matrix		( math::invert4x3( m ) );

	return true; 
}

bool mouse_action_view_move_xz_reverse::capture( ) 
{
	super::init_capture	( );
	return true;
}

void mouse_action_view_move_xz_reverse::release( ) 
{
	super::release		( );
}

mouse_action_view_rotate::mouse_action_view_rotate( System::String^ name, scene_view_panel^ w )
:mouse_action_view_rotate_base	( name, w )
//m_start_angle_x					( 0 )
//m_shift_x						( 0 )
{
}

bool mouse_action_view_rotate::execute( ) 
{ 
	Drawing::Point mouse_position	= m_view_window->get_mouse_pos_infinite();

	// Compute rotation factors from mouse move.
	float angle_x			= .5f*math::deg2rad( float( mouse_position.Y - m_mouse_last_pos.Y /*+ m_shift_x*/ ) );
	float angle_y			= .5f*math::deg2rad( float( mouse_position.X - m_mouse_last_pos.X )*.75f );

	if( math::is_zero(angle_x) && math::is_zero(angle_y) )
		return true;


 //   if( angle_x > math::pi_d2 - m_start_angle_x )
	//{
	//	angle_x = math::pi_d2 - m_start_angle_x;
	//	m_shift_x = math::rad2deg(angle_x/.5f)-(mouse_position.Y - m_mouse_last_pos.Y);
	//}

	//if( angle_x < -math::pi_d2 - m_start_angle_x )
	//{
	//	angle_x = -math::pi_d2 - m_start_angle_x;
	//	m_shift_x = math::rad2deg(angle_x/.5f)-(mouse_position.Y - m_mouse_last_pos.Y);
	//}

	m_mouse_last_pos				= mouse_position;

	// set rotated view by angle_x angle_y around rotation origin point.
	math::float4x4 inv_view			= m_view_window->get_inverted_view_matrix();
	rotate_view_matrix				( float3( angle_x, angle_y, 0 ), 0.0f );

	return							true;
}

bool mouse_action_view_rotate::capture( ) 
{
	super::init_capture				( );
//	m_start_angle_x					= m_angles_captured->x;
//	m_shift_x						= 0;
	
	return true;
}

void mouse_action_view_rotate::release( )
{
	super::release					( );
}

//mouse_action_view_roll::mouse_action_view_roll( System::String^ name, scene_view_panel^ w )
//:super(name, w)
//{}
//
//bool mouse_action_view_roll::capture( )
//{
//	super::init_capture	( );
//
//	m_start_angle_z		= m_view_window->get_inverted_view_matrix( ).get_angles( math::rotation_zxy ).z;
//	m_mouse_last_pos	= m_view_window->get_mouse_pos_infinite( );
//
//	return true;
//}
//
//bool mouse_action_view_roll::execute( )
//{
//	Drawing::Point mouse_position	= m_view_window->get_mouse_pos_infinite( );
//	
//	float angle_z					= 0.5f * math::deg2rad( float( mouse_position.Y - m_mouse_last_pos.Y ) );
//
//	math::float4x4 inv_view			= m_view_window->get_inverted_view_matrix( );
//
//	float4x4	rotation_z_camrea	= math::create_rotation( inv_view.k.xyz( ), angle_z );
//
//	math::float3 position			= inv_view.c.xyz( );
//	inv_view.c.xyz( )				= math::float3( 0,0,0 );
//	math::float4x4 m				= inv_view * rotation_z_camrea;
//	m.c.xyz( )						= position;
//
//	m_view_window->set_view_matrix	(	math::invert4x3 ( m ) );
//
//	m_mouse_last_pos				= mouse_position;
//
//	return		true;
//}
//
//void mouse_action_view_roll::release( )
//{
//	super::release		( );
//}

collision_objects::collision_objects( )
{
	m_list	= NEW(xray::collision::objects_type)(g_allocator);
}

collision_objects::~collision_objects( )
{
	DELETE	(m_list);
}

u32	collision_objects::count( )
{
	return m_list->size();
}

const xray::collision::object* collision_objects::get( u32 idx )
{
	return m_list->at(idx);
}

void collision_objects::add( xray::collision::object const* o )
{
	m_list->push_back( o );
}

void collision_objects::clear( )
{
	m_list->clear( );
}

collision_ray_objects::collision_ray_objects( )
{
	m_list	= NEW(xray::collision::ray_objects_type)(g_allocator);
}

collision_ray_objects::~collision_ray_objects( )
{
	DELETE(m_list);
}

u32 collision_ray_objects::count( )
{
	return m_list->size();
}

const xray::collision::ray_object_result& collision_ray_objects::get( u32 idx )
{
	return m_list->at(idx);
}
void collision_ray_objects::add( xray::collision::ray_object_result& o )
{
 m_list->push_back(o);
}

void collision_ray_objects::clear( )
{
	m_list->clear();
}

collision_ray_triangles::collision_ray_triangles( )
{
	m_list	= NEW(xray::collision::ray_triangles_type)(g_allocator);
}

collision_ray_triangles::~collision_ray_triangles( )
{
	DELETE(m_list);
}

u32 collision_ray_triangles::count( )
{
	return m_list->size();
}

const xray::collision::ray_triangle_result& collision_ray_triangles::get( u32 idx )
{
	return m_list->at(idx);
}
void collision_ray_triangles::add( xray::collision::ray_triangle_result& o )
{
	m_list->push_back(o);
}

void collision_ray_triangles::clear( )
{
	m_list->clear();
}

camera_view_wsad::camera_view_wsad( System::String^ name, scene_view_panel^ w, move_action a )
:super		( name, w),
m_action	( a )
{}

bool camera_view_wsad::capture( )
{
	if( m_view_window->get_wsad_mode() )
	{
		m_timer			= NEW(timing::timer)();
		m_timer->start	();
		return true;
	}else
		return false;
}

bool camera_view_wsad::execute( )
{
	math::float4x4 m = m_view_window->get_inverted_view_matrix();
	
	float dt = m_timer->get_elapsed_sec();
	math::clamp(dt, 0.0f, 10.0f);

	dt = 1-1/(math::exp((dt*2.0f-0.5f)*5.0f)+1.0f);

	float ang				= dt * math::deg2rad(10.0f);
	float dist				= dt * 2.0f; 


	switch(m_action)
	{
	case move_action::sh_forward:
		{
			m.c.xyz()		= m.c.xyz() + m.k.xyz()*dist;
			m_view_window->set_view_matrix(math::invert4x3(m));
		}break;
	case move_action::sh_backward:
		{
			m.c.xyz()		= m.c.xyz() - m.k.xyz()*dist;
			m_view_window->set_view_matrix(math::invert4x3(m));
		}break;
	case move_action::sh_left:
		{
			m.c.xyz()		= m.c.xyz() - m.i.xyz()*dist;
			m_view_window->set_view_matrix(math::invert4x3(m));
		}break;
	case move_action::sh_right:
		{
			m.c.xyz()		= m.c.xyz() + m.i.xyz()*dist;
			m_view_window->set_view_matrix(math::invert4x3(m));
		}break;
	case move_action::rot_up:
		{
			rotate_view_matrix		( float3( -ang, 0.0f, 0.0f ), 0.0f );
		}break;
	case move_action::rot_down:
		{
			rotate_view_matrix		( float3( ang, 0.0f, 0.0f ), 0.0f );
		}break;
	case move_action::rot_left:
		{
			rotate_view_matrix		( float3( 0.0f, -ang, 0.0f ), 0.0f );
		}break;
	case move_action::rot_right:
		{
			rotate_view_matrix		( float3( 0.0f, ang, 0.0f ), 0.0f );
		}break;
	}
	//dx						= 0.5f * math::deg2rad(dx);
	//dy						= 0.5f * math::deg2rad(dy*0.75f);
	//math::clamp				( dx, -math::pi_d6, math::pi_d6);

	//rotate_view_matrix		( float3( dx, dy, 0 ), m_view_window->view_point_distance() );
	return false;
}

void camera_view_wsad::release( )
{
	DELETE	( m_timer );
}

} // namespace editor_base
} // namespace xray
