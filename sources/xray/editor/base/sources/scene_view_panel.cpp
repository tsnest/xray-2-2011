////////////////////////////////////////////////////////////////////////////
//	Created		: 24.01.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "scene_view_panel.h"
#include "view_control_actions.h"
#include "input_keys_holder.h"
#include "gui_binder.h"
#include "action_delegate.h"
#include "input_engine.h"
#include <xray/editor/base/images/images16x16.h>
#include <xray/editor/base/collision_object_control.h>

#pragma managed( push, off )
#include <xray/collision/space_partitioning_tree.h>
#include <xray/collision/collision_object.h>
#include <xray/render/facade/scene_view_mode.h>
#include <xray/render/facade/scene_renderer.h>
#pragma managed( pop )

pcstr view_mode_names[] = 
{
	"wireframe",
	"wireframe_two_sided",
	
	"unlit",
	"lit",
	
	"normals",
	
	"specular_intencity",
	"specular_power",
	
	"miplevel",
	
	"lighting",
	"lighting_diffuse",
	"lighting_specular",
	
	"shader_complexity",
	"texture_density",
	"geometry_complexity",
	
	"emissive_only",

	"distortion_only",
	
	"ambient_occlusion_only",
	
	"unlit_with_ao",
	
	"lit_with_histogram_view_mode",
	
	"overdraw_view_mode",
	
	"indirect_lighting_mode",
};

xray::render::scene_view_mode text_to_render_mode( System::String^ mode_str )
{
	for(int mode = 0; mode<xray::render::num_view_modes; ++mode)
	{
		if( mode_str == gcnew System::String(view_mode_names[mode]) )
			return (xray::render::scene_view_mode)mode;
	}
	return xray::render::lit_view_mode;// default
}

int get_next_render_mode(int mode)
{
	++mode;
	if(mode == xray::render::num_view_modes)
		mode = (xray::render::scene_view_mode)0;
	
	return mode;
}

namespace xray{
namespace editor_base{

struct pick_cache{
	System::Drawing::Point				m_pick_xy;
	float3								m_pos;
	bool								m_cached;
	bool								m_result;
	xray::collision::object_type		m_mask;
	xray::collision::object const*		m_obj;
};

void scene_view_panel::in_constructor( )
{
	m_moved_last_frame			= false;
	m_focus_enter_frame			= 0;
	m_editor_renderer			= NULL;
	m_infinite_mode				= false;
	m_mouse_position_infinite_mode = System::Drawing::Point(0,0);
	m_near_plane				= 0.1f;
	m_far_plane					= 300.0f;
	m_selection_rate			= 1.0f;
	m_selection_color			= NEW(math::color)();
	m_mouse_in_view				= false;
	m_default_clip				= System::Drawing::Rectangle( 0 ,0 , 0, 0);

	m_inverted_view				= NEW(math::float4x4)();
	m_view_point_distance		= 70.7f;
	m_projection				= NEW(math::float4x4)();
	m_cached					= NEW(pick_cache)();

	m_cached->m_cached			= false;
	m_top_toolstrip_panel		= gcnew ToolStripPanel;
	top_panel->SuspendLayout	( );
	top_panel->Controls->Add	( m_top_toolstrip_panel );
	m_top_toolstrip_panel->Dock = System::Windows::Forms::DockStyle::Fill;
	top_panel->ResumeLayout		( );
	m_camera_effector			= NULL;
	m_axis_model				= NEW(render::static_model_ptr)();
	subscribe_on_camera_move	( gcnew camera_moved_delegate( this, &scene_view_panel::on_camera_move));
	view_panel_Resize			( this, nullptr );

	m_use_postprocess			= true;
	m_render_grid				= false;

	m_render_view_mode			= xray::render::lit_view_mode;
	create_tool_strip			("RenderOptionsStrip", 0);

	m_flying_camera_destination = NEW(math::float4x4)();
	m_flying_camera_destination_vector = NEW(math::float3)();
	m_flying_camera_timer		= NEW(timing::timer)();
	m_wsad_mode_enabled			= false;
	m_sun_light_id				= 0;
	m_camera_light_id			= 0;

}

void scene_view_panel::render_mode_btn_click( System::Object^, System::EventArgs^ )
{
	m_render_view_mode			= get_next_render_mode(m_render_view_mode);
	apply_render_view_mode		( false );
}

void scene_view_panel::apply_render_view_mode( bool text_only )
{
	if(m_render_modes_button)
		m_render_modes_button->Text	= gcnew System::String(view_mode_names[m_render_view_mode]);

	if(m_editor_renderer && !text_only)
		m_editor_renderer->scene().set_view_mode( *m_scene_view, (xray::render::scene_view_mode)m_render_view_mode);
}

void scene_view_panel::render_mode_btn_mouse_down( System::Object^ , System::Windows::Forms::MouseEventArgs^ e )
{
	if(e->Button == System::Windows::Forms::MouseButtons::Right)
	{
		System::Windows::Forms::ContextMenuStrip^ menu = gcnew System::Windows::Forms::ContextMenuStrip;
		System::Windows::Forms::ToolStripMenuItem^ item;
		
		for (int mode = 0; mode<xray::render::num_view_modes; ++mode)
		{
			System::Drawing::Image^ image	= nullptr;
			System::String^ mode_name		= gcnew System::String(view_mode_names[mode]);
			item							= gcnew System::Windows::Forms::ToolStripMenuItem(mode_name, image, gcnew System::EventHandler(this, &scene_view_panel::render_mode_menu_click));
			item->Tag						= mode;
			
			if(mode==m_render_view_mode)
				item->Checked	= true;

			menu->Items->Add	( item );
		}
		menu->Show			(MousePosition);
	}
}

void scene_view_panel::render_mode_menu_click( System::Object^ sender, System::EventArgs^ )
{
	System::Windows::Forms::ToolStripMenuItem^ menu_item = safe_cast<System::Windows::Forms::ToolStripMenuItem^>(sender);
	m_render_view_mode			= (int)menu_item->Tag;
	apply_render_view_mode		( false );
}

void scene_view_panel::in_destructor( )
{
	DELETE	( m_scene_view );
	DELETE	( m_scene );
	DELETE	( m_cached );
	DELETE	( m_inverted_view );
	DELETE	( m_projection );
	DELETE	( m_selection_color );
	DELETE	( m_axis_model );
	DELETE	( m_flying_camera_destination );
	DELETE	( m_flying_camera_destination_vector );
	DELETE	( m_flying_camera_timer );
}

float scene_view_panel::mouse_sensitivity( )
{
	return m_mouse_sensitivity;
}

void scene_view_panel::set_mouse_sensitivity( float const value )
{
	m_mouse_sensitivity = value;
}

bool scene_view_panel::ray_query(	xray::collision::object_type const mask, 
									   xray::collision::object const** object, 
										float3* position )
{
	if(!is_mouse_in_view())
		return false;

	return ray_query( mask, get_mouse_position(), object, position);
}

bool scene_view_panel::ray_query(	xray::collision::object_type const mask, 
									System::Drawing::Point screen_xy, 
									xray::collision::object const** object, 
									float3* position )
{
	if( m_cached->m_cached && 
		screen_xy == m_cached->m_pick_xy && 
		m_cached->m_mask == mask	)
	{
		if( m_cached->m_result	)
		{
			*object = m_cached->m_obj;

			if( position != NULL )
				*position = m_cached->m_pos;

			return true;
		}

		*object = NULL;
		return false;
	}

	math::float3 origin;
	math::float3 direction;
	get_screen_ray( math::int2(screen_xy.X, screen_xy.Y), origin, direction );

	bool result = ray_query( mask, origin, direction, object, &m_cached->m_pos );

	if( position != NULL)
		*position = m_cached->m_pos;

	m_cached->m_cached			= false/*true*/;	//cached object may be destroyed -(
	m_cached->m_pick_xy			= screen_xy;
	m_cached->m_obj				= *object;
	m_cached->m_result			= result;
	m_cached->m_mask			= mask;

	return				result;
}

bool scene_view_panel::ray_query(	xray::collision::object_type const mask, 
									   math::float3 origin, 
									   math::float3 direction, 
									   xray::collision::object const** object, 
									   float3* position )
{
	xray::collision::ray_objects_type		collision_results( g_allocator );

	*object = NULL;
	bool result = false;

	m_collision_tree->ray_query(
		mask,
		origin, 
		direction, 
		10000.f,  
		collision_results, 
		collision::objects_predicate_type( )
	);

	float distance = 0;
	if( collision_results.size() > 0)
	{
		xray::collision::collision_object const* obj = static_cast_checked<xray::collision::collision_object const*> (collision_results[0].object);
		ASSERT( obj, "The obj must not be NULL.");
		distance = collision_results[0].distance;

		*object = obj;
		result = true;
	}
	else
		result = false;
	
	if( position != NULL )
	{
		*position = origin + direction*distance; 
	}

	return result;
}

bool scene_view_panel::ray_query( collision::object_type const mask, 
					   math::float3 origin, 
					   math::float3 direction, 
					   collision_ray_objects^ collision_results )
{
	xray::collision::ray_objects_type	results (g_allocator);
	m_collision_tree->ray_query(
		mask,
		origin, 
		direction, 
		1000.f,  
		results, 
		xray::collision::objects_predicate_type( )
	);

	if( results.size() > 0)
	{
		for(u32 i=0; i<results.size(); ++i)
			collision_results->add(results[i]);
		return true;
	}

	return false;
}

static float3 get_triangle_normal( collision::ray_triangle_result const& result )
{
	collision::collision_object const * co	= dynamic_cast<collision::collision_object const*>( result.object );

	if( NULL == co )
		return float3( 0, 0, 0 );

	u32 triangle_id		= result.triangle_id;

	non_null< collision::geometry_instance const >::ptr geom = co->get_geometry_instance	( );
	u32 const* indices								= geom->indices		( triangle_id );
	float3 verts[3];
	verts[0]			= geom->vertices( )[ indices[0] ];
	verts[1]			= geom->vertices( )[ indices[1] ];
	verts[2]			= geom->vertices( )[ indices[2] ];

	float4x4 const m	= co->get_matrix( );
	verts[0]			= m.transform_position( verts[0] );
	verts[1]			= m.transform_position( verts[1] );
	verts[2]			= m.transform_position( verts[2] );

	return				( ( verts[1] - verts[0] ) ^ ( verts[2] - verts[1] ) ).normalize( );
}

static bool triangles_predicate ( collision::ray_triangle_result const& )
{
	return true;
}

bool scene_view_panel::ray_query(	xray::collision::object_type const mask,
									float3& position,
									float3& normal
								)
{
	System::Drawing::Point screen_point = get_mouse_position( );
	math::float3 origin;
	math::float3 direction;
	get_screen_ray( math::int2(screen_point.X, screen_point.Y), origin, direction );

	xray::collision::ray_triangles_type	results (g_allocator);
	m_collision_tree->ray_query(
		mask,
		origin, 
		direction, 
		1000.f,  
		results, 
		xray::collision::triangles_predicate_type( &triangles_predicate )
	);

	if( results.size( ) > 0 )
	{
		collision::ray_triangle_result& result = results[0];

		normal		= get_triangle_normal( result );
		position	= origin + direction * result.distance;
		
		return true;
	}

	return false;
}

bool scene_view_panel::ray_query(	xray::collision::object_type const mask,
									math::float3 origin,
									math::float3 direction,
									collision_ray_triangles^ collision_results
								)
{
	xray::collision::ray_triangles_type	results (g_allocator);
	m_collision_tree->ray_query(
		mask,
		origin, 
		direction, 
		1000.f,  
		results, 
		xray::collision::triangles_predicate_type( )
	);

	if( results.size( ) > 0 )
	{
		u32 count = results.size( );
		for( u32 i=0; i < count; ++i )
			collision_results->add( results[i] );

		return true;
	}

	return false;
}

bool scene_view_panel::frustum_query(	xray::collision::object_type const mask, 
										System::Drawing::Point start_xy, 
										System::Drawing::Point end_xy, 
										collision_objects^ objects )
{
	math::float3 origin;
	math::float3 direction;

	math::float4x4 inv_view				= get_inverted_view_matrix();
	math::float3	near_left_top		= inv_view.transform_position( screen_to_3d( math::float2( (float)start_xy.X,	(float)-start_xy.Y ),	1.f ) );
	math::float3	near_right_top		= inv_view.transform_position( screen_to_3d( math::float2( (float)end_xy.X,	(float)-start_xy.Y ),		1.f ) );
	math::float3	near_right_bottom	= inv_view.transform_position( screen_to_3d( math::float2( (float)end_xy.X,	(float)-end_xy.Y ),			1.f ) );
	math::float3	near_left_bottom	= inv_view.transform_position( screen_to_3d( math::float2( (float)start_xy.X,	(float)-end_xy.Y ),		1.f ) );

	math::float3	far_left_top		= inv_view.transform_position( screen_to_3d( math::float2( (float)start_xy.X,	(float)-start_xy.Y ),	500.f ) );
	math::float3	far_right_top		= inv_view.transform_position( screen_to_3d( math::float2( (float)end_xy.X,	(float)-start_xy.Y ),		500.f ) );
	math::float3	far_right_bottom	= inv_view.transform_position( screen_to_3d( math::float2( (float)end_xy.X,	(float)-end_xy.Y ),			500.f ) );
	math::float3	far_left_bottom		= inv_view.transform_position( screen_to_3d( math::float2( (float)start_xy.X,	(float)-end_xy.Y ),		500.f ) );

	math::plane		m_planes[ 6 ];


	if ( math::is_zero( get_plane_normal_non_normalized(near_left_top, near_right_top, near_left_bottom ).length() ) )
		return false;

	// near
	m_planes[0]	= math::create_plane( near_right_top,		near_left_top,		near_left_bottom	);
	// far							  
	m_planes[1]	= math::create_plane( far_left_bottom,		far_left_top,		far_right_top		);
	// up							  
	m_planes[2]	= math::create_plane( far_left_top,			near_left_top,		far_right_top		);
	// bottom						  
	m_planes[3]	= math::create_plane( far_right_bottom,		near_left_bottom,	far_left_bottom		);
	// left							  
	m_planes[4]	= math::create_plane( far_left_bottom,		near_left_bottom,	far_left_top		);
	// right						  
	m_planes[5]	= math::create_plane( far_right_top,		near_right_bottom,	far_right_bottom	);

	vectora<math::float3> tris(g_allocator);


	tris.push_back( near_left_top );
	tris.push_back( near_right_top );
	tris.push_back( near_left_bottom );
					
	tris.push_back( far_left_top );		
	tris.push_back( far_left_bottom );
	tris.push_back( far_right_top );
					
	tris.push_back( near_left_top );
	tris.push_back( far_left_top );	
	tris.push_back( far_right_top );
					
	tris.push_back( near_left_bottom );
	tris.push_back( far_right_bottom );
	tris.push_back( far_left_bottom );
					
	tris.push_back( near_left_bottom );
	tris.push_back( far_left_bottom );
	tris.push_back( far_left_top );
					
	tris.push_back(	near_right_bottom );
	tris.push_back(	far_right_top );
	tris.push_back(	far_right_bottom );


	math::cuboid	cuboid( m_planes );

	collision::objects_type	collision_results( g_allocator );

	bool result = false;

	m_collision_tree->cuboid_query(
		mask,
		cuboid,
		collision_results
	);

	objects->clear();

	if( collision_results.size() > 0)
	{
		xray::collision::object const* obj = collision_results[0];
		ASSERT( obj, "The obj must not be NULL.");
		objects->add( obj );

		u32 const count = collision_results.size();
		for( u32 i = 1; i< count; ++i )
		{
			collision::object const* tmp_obj = collision_results[i];
			ASSERT( tmp_obj, "The obj must not be NULL.");
			objects->add( tmp_obj );
		}
		result = true;
	}
	else
		result = false;

	return result;
}

void scene_view_panel::clear_picker_cache()
{
	m_cached->m_cached = false;
}

void scene_view_panel::get_mouse_ray( math::float3& origin, math::float3& direction )
{
	System::Drawing::Point mouse_position = get_mouse_position();
	get_screen_ray ( math::int2( mouse_position.X, mouse_position.Y), origin, direction );
}

void scene_view_panel::get_screen_ray ( math::int2 const& screen_xy, math::float3& origin, math::float3& direction )
{
	direction				= screen_to_3d ( math::float2( (float)screen_xy.x, (float)-screen_xy.y), 1 );
	direction				= get_inverted_view_matrix().transform_direction( direction );
	direction.normalize		();
	origin					= get_inverted_view_matrix().c.xyz();
}

float3 scene_view_panel::screen_to_3d( math::float2 const& screen, float const z_dist )
{
	float3 result				( screen.x, screen.y, z_dist);
	System::Drawing::Size vs	= get_view_size();
	math::float4x4 proj			= get_projection_matrix();
	result.x 					*= ( z_dist/proj.e00/(vs.Width/2));
	result.y 					*= ( z_dist/proj.e11/(vs.Height/2));

	return result;
}

float2 scene_view_panel::float3_to_screen( math::float3 const& p )
{
	System::Drawing::Size   hvs				= get_view_size();
	hvs.Width								/= 2;
	hvs.Height								/= 2;
	
	math::float4x4 view;
	view.try_invert                         ( get_inverted_view_matrix( ) );
	math::float4x4 view_projection			= math::mul4x4( view, get_projection_matrix( ) );
	math::float3 pos						= view_projection.transform( p );

	pos.x				= pos.x * hvs.Width + hvs.Width;
	pos.y				= -pos.y * hvs.Height + hvs.Height;

	return float2		( pos.x, pos.y );
}

void scene_view_panel::set_view_matrix( float2 const& add_angles, float const forward, float const right, float const up )
{
	float4x4 const view_inverted	= get_inverted_view_matrix( );
	float3 curr_angles				= view_inverted.get_angles_xyz( );

	float const x_new			= 
		math::clamp_r			(
		math::angle_normalize_signed( curr_angles.x + add_angles.x ),
		-math::pi -math::pi_d2 + math::deg2rad( 1.f ),
		math::pi + math::pi_d2 - math::deg2rad( 1.f )
		);
	
	float3 const new_angles		= float3( x_new, curr_angles.y + add_angles.y, curr_angles.z );

	float4x4 rotation			= math::create_rotation( new_angles );

	float3 const position		= 
		view_inverted.c.xyz( ) +
		view_inverted.i.xyz( )*right +
		view_inverted.j.xyz( )*up +
		view_inverted.k.xyz( )*forward;

	float4x4 const translation	= math::create_translation( position );
	set_view_matrix				( math::invert4x3 ( rotation * translation ));
}

float3 scene_view_panel::screen_to_3d_world( float2 const& screen, float z)
{
	return get_inverted_view_matrix().transform_position( screen_to_3d ( screen, z) );
}

void scene_view_panel::get_camera_props( math::float3& p, math::float3& d )
{
	p	= get_inverted_view_matrix().c.xyz();
	d	= get_inverted_view_matrix().k.xyz();
}

void scene_view_panel::set_camera_props( const math::float3& p, const math::float3& d )
{
	float4x4 m		= math::create_camera_direction( p, d, float3(0.f, 1.f, 0.f ) );
	set_view_matrix	( m );
}

bool scene_view_panel::is_mouse_in_view( )
{
	System::Drawing::Point	mouse_position	= get_mouse_position( );
	System::Drawing::Size	view_size		= get_view_size();

	return (	math::abs(mouse_position.X)*2 < view_size.Width &&
				math::abs(mouse_position.Y)*2 < view_size.Height );
	
}

void scene_view_panel::set_mouse_position( System::Drawing::Point p )
{
	Control^ control = safe_cast<Control^>( this );

	Point view_center = get_view_center();

	Point cp(p.X + view_center.X, p.Y + view_center.Y);
	control->Cursor->Position = cp;
}

System::Drawing::Point scene_view_panel::get_view_center( ) 
{
	Point result(	view_panel->DisplayRectangle.Left + view_panel->DisplayRectangle.Width/2,
					view_panel->DisplayRectangle.Top +  view_panel->DisplayRectangle.Height/2 );

	result		= view_panel->PointToScreen( result);

	return result;
}

System::Drawing::Point scene_view_panel::get_mouse_position( )
{
	System::Drawing::Point	result;
	Control^ control		= view_panel;
	System::Drawing::Size sz= control->Size;
	sz.Width				= sz.Width/2;
	sz.Height				= sz.Height/2;

	System::Drawing::Point cur = control->Cursor->Position;
	cur						= control->PointToClient(cur);
	result.X				= cur.X - sz.Width;
	result.Y				= cur.Y - sz.Height;
	return					result;
}

System::Drawing::Point scene_view_panel::get_mouse_pos_infinite( )
{
	return m_mouse_position_infinite_mode;
}

void scene_view_panel::set_infinite_mode( bool b )
{
	m_infinite_mode = b;
	if( m_infinite_mode )
	{
		view_panel->Capture			= true;
		m_mouse_campure_point		= Cursor->Position;
		while ( ShowCursor(FALSE) >= 0 );
		clip_cursor					( );
	}
	else
	{
		view_panel->Capture			= false;
		while ( ShowCursor(TRUE) <= 0 );
		unclip_cursor				( );
	}
}

System::Drawing::Size scene_view_panel::get_view_size( )
{
	return view_panel->Size;
}

math::float4x4 scene_view_panel::get_inverted_view_matrix( )
{
	return *m_inverted_view;
}

void scene_view_panel::set_view_point_distance( float const value )
{
	m_view_point_distance = value;
}

float scene_view_panel::view_point_distance( )
{
	return m_view_point_distance;
}

xray::render::editor::renderer*	scene_view_panel::editor_renderer( )
{
	 return m_editor_renderer;
}

xray::render::debug::renderer* scene_view_panel::debug_renderer( )
{
	R_ASSERT(m_editor_renderer);
	return &m_editor_renderer->debug();
}

void scene_view_panel::set_view_matrix( math::float4x4 const& m )
{
	*m_inverted_view	= math::invert4x3(m);

	m_inverted_view->i.xyz().normalize_safe	( float3(1,0,0) );
	m_inverted_view->j.xyz().normalize_safe	( float3(0,1,0) );
	m_inverted_view->k.xyz().normalize_safe	( float3(0,0,1) );

	m_moved_last_frame	= true;
}

void scene_view_panel::tick( )
{
	
	if( m_flying_camera_animating ){
		*m_flying_camera_destination_vector	= math::invert4x3(*m_flying_camera_destination).c.xyz() - get_inverted_view_matrix().c.xyz();
		float length						= m_flying_camera_destination_vector->length();

		if( math::is_zero( length ) )
		{
			m_flying_camera_animating = false;
		}
		else
		{
			*m_flying_camera_destination_vector	/= length;

			float time				= m_flying_camera_timer->get_elapsed_sec();
			float distance			= 0.f; 


			distance = m_flying_camera_speed_starting*time + 0.5f*(m_flying_camera_acceleration_starting*math::sqr(time) + m_flying_camera_acceleration_increase*math::sqr(time)*time);

			if( m_flying_camera_distance/distance < 2 )
			{
				time		= 0.5f - time;
				distance	= m_flying_camera_distance - (m_flying_camera_speed_starting*time + 0.5f*(m_flying_camera_acceleration_starting*math::sqr(time) + m_flying_camera_acceleration_increase*math::sqr(time)*time));
			}


			if( m_flying_camera_distance > distance/*m_speed*/)
			{
				float4x4 tmp_mat			= get_inverted_view_matrix();
				tmp_mat.c.xyz()				+=  (distance - m_flying_camera_distance + length)*(*m_flying_camera_destination_vector);
				set_view_matrix	( math::invert4x3( tmp_mat ) );
			}
			else
			{
				set_view_matrix	( *m_flying_camera_destination );
				m_flying_camera_animating						= false;			
			}
		}
	}
	
	if(m_camera_effector)
	{
		m_camera_effector->process_camera	( *m_inverted_view );// non-const
		m_moved_last_frame					= true;
	}

	if(m_moved_last_frame)
	{
		m_moved_last_frame	= false;
		m_camera_moved		( );				// const
	}
}

void scene_view_panel::setup_view_matrix( math::float3 const& eye_point, math::float3 const& look_at )
{
	set_view_matrix(
		math::create_camera_at(
			eye_point,
			look_at,
			float3( 0.f,  1.f, 0.f ) 
		) 
	);
}

void scene_view_panel::subscribe_on_camera_move	( camera_moved_delegate^ d )
{
	m_camera_moved += d;
}

void scene_view_panel::unsubscribe_on_camera_move	( camera_moved_delegate^ d )
{
	m_camera_moved -= d;
}

void scene_view_panel::set_camera_effector( xray::camera_effector* e )
{
	m_camera_effector				= e;
	
	if (m_camera_effector)
		m_camera_effector->on_attach	( );
}

math::float4x4 scene_view_panel::get_projection_matrix( )
{
	return *m_projection;
}


editor_base::enum_editor_cursors scene_view_panel::set_view_cursor( editor_base::enum_editor_cursors )
{
	return editor_base::enum_editor_cursors::default;
}

System::Int32 scene_view_panel::view_handle( )
{
	return view_panel->Handle.ToInt32();
}

void scene_view_panel::window_view_Activated( )
{

	if ( !Parent )
		return;

	while ( ShowCursor( TRUE ) <= 0 );
}

void scene_view_panel::window_view_Deactivate( )
{
	if (!Parent)
		return;

	while ( ShowCursor(TRUE) <= 0 );

	unclip_cursor				( );
	xray::editor_base::input_keys_holder::ref->reset	( gcnew System::Object(), gcnew System::EventArgs());
}

void scene_view_panel::clip_cursor( )
{
	if( m_default_clip.Width == 0)
	{
		using System::Drawing::Rectangle;
		Rectangle					view = view_panel->RectangleToScreen(view_panel->ClientRectangle);
		m_default_clip				= ::Cursor::Clip;
		::Cursor::Clip				= view;
	}
}

void scene_view_panel::unclip_cursor( )
{
	if( m_default_clip.Width > 0)
	{
		::Cursor::Clip = m_default_clip;

		m_default_clip.Width = 0;
		m_default_clip.Height = 0;
	}
}

void scene_view_panel::render( )
{
	if(m_render_grid && m_editor_renderer)
		render_grid	( );
}

void scene_view_panel::render_grid( )
{
	// Make a little shift down to avoid z fighting with neutral terrain.
	// TODO: This need to be done relative to view position.
	float3 cp(0.0f, -0.0f, 0.0f);
	float3 cam = (*m_inverted_view).c.xyz();

	cp.x = (float)div(math::ceil(cam.x), 10).quot;
	cp.z = (float)div(math::ceil(cam.z), 10).quot;
	cp *= 10.0f;

	math::color const clr1( 64, 64, 64 );
	math::color const clr2( 164, 164, 164 );
	math::color const* c;

	int	const	quad_count		= m_grid_dim;
	float const	line_half_len	= m_grid_step * quad_count;

	for ( int i=-quad_count; i<=quad_count; ++i)
	{
		c = ( i%10 == 0) ? &clr2 : &clr1;
		float const d = m_grid_step*i;
		// z-lines

		m_editor_renderer->debug().draw_line(	*m_scene, 
												cp+float3( d, 0, -line_half_len ), 
												cp+float3( d, 0, line_half_len ), 
												*c );

		//x-lines
		m_editor_renderer->debug().draw_line(	*m_scene, 
												cp+float3(-line_half_len, 0, d ), 
												cp+float3( line_half_len, 0, d ), 
												*c );

	}

	m_editor_renderer->debug().draw_origin	( *m_scene, float4x4().identity(), 3.0f );

}

void scene_view_panel::subscribe_on_drag_drop	( System::Windows::Forms::DragEventHandler^ e )
{
	view_panel->AllowDrop	= true;
	view_panel->DragDrop	+= e;
}

void scene_view_panel::subscribe_on_drag_enter	( System::Windows::Forms::DragEventHandler^ e )
{
	view_panel->AllowDrop	= true;
	view_panel->DragEnter	+= e;
}

void scene_view_panel::view_panel_MouseMove	( System::Object^, System::Windows::Forms::MouseEventArgs^ )
{
	if( m_infinite_mode )
	{
		Point view_center = get_view_center();

		int delta_x = Cursor->Position.X - m_mouse_campure_point.X;
		int delta_y = Cursor->Position.Y - m_mouse_campure_point.Y;

		m_mouse_position_infinite_mode.X += delta_x;
		m_mouse_position_infinite_mode.Y += delta_y;

		if( delta_x != 0 || delta_y != 0)
			Cursor->Position	= m_mouse_campure_point;
	}
}

void scene_view_panel::on_axis_model_ready( resources::queries_result& data )
{
	if ( data.is_successful() )
	{
		*m_axis_model	= static_cast_resource_ptr<render::static_model_ptr>(data[0].get_unmanaged_resource());
		m_editor_renderer->scene().add_model	( *m_scene, (*m_axis_model)->m_render_model, math::float4x4().identity() );
		on_camera_move	( );
	}
}

void scene_view_panel::on_camera_move( )
{
	if((*m_axis_model).c_ptr())
	{
		System::Drawing::Size	half_vp		= get_view_size( );
		if(half_vp.Width<=0 || half_vp.Height<=0)
			return; // minimized??

		half_vp.Width			= half_vp.Width / 2;
		half_vp.Height			= half_vp.Height / 2;

		float3 pivot_size		= float3(0.03f, 0.03f, 0.03f);
		float2 pivot_position	= float2(-(half_vp.Width-50.0f), -(half_vp.Height-50.0f));
		
		float4x4 m				= math::create_translation( screen_to_3d_world( pivot_position, 0.5f ) );
		R_ASSERT(math::valid(m.c.xyz()));
		float4x4 s				= math::create_scale( pivot_size );
		m_editor_renderer->scene().update_model	( *m_scene, (*m_axis_model)->m_render_model, s*m );
	}

	if(get_camera_light())
		update_camera_ligtht( );
}

void scene_view_panel::clear_resources( )
{
	if((*m_axis_model).c_ptr())
	{
		m_editor_renderer->scene().remove_model	( *m_scene, (*m_axis_model)->m_render_model);
		(*m_axis_model)						= 0;
	}

	if(get_camera_light())
		switch_camera_light();

	if(get_sun_active())
		switch_sun();
}

void scene_view_panel::clear_scene			( )
{
	DELETE						( m_scene_view );
	m_scene_view				= 0;

	DELETE						( m_scene );
	m_scene						= 0;

	m_editor_renderer			= 0;
}

void scene_view_panel::setup_scene( render::scene_pointer scene, 
								   render::scene_view_pointer scene_view, 
								   render::editor::renderer& renderer,
								   bool query_axis)
{
	m_editor_renderer			= &renderer;
	m_scene						= NEW( render::scene_ptr ) ( scene );
	m_scene_view				= NEW( render::scene_view_ptr ) ( scene_view );

	apply_render_view_mode		( false );

	if(query_axis)
	{
	query_result_delegate* q	= NEW(query_result_delegate)(
		gcnew query_result_delegate::Delegate(
			this, 
			&scene_view_panel::on_axis_model_ready
		),
		g_allocator
	);
	
	resources::query_resource	(
		"editor/axis",
		resources::static_model_instance_class,
		boost::bind(&query_result_delegate::callback, q, _1),
		g_allocator
	);
	}
}

void scene_view_panel::view_panel_Resize( System::Object^, System::EventArgs^ )
{
	update_projection_matrix();
}

float scene_view_panel::aspect_ratio	( )
{
	System::Drawing::Size sz	= get_view_size();

	if(sz.Width<=0 || sz.Height<=0) {
		sz.Width	= 1024;
		sz.Height	= 768;
	}

	return						((float)sz.Width/(float)sz.Height);
}

void scene_view_panel::update_projection_matrix( )
{
	System::Drawing::Size sz	= get_view_size();

	if(sz.Width<=0 || sz.Height<=0)
		return; // minimized??

	float near_plane			= m_near_plane;
	*m_projection				= math::create_perspective_projection(
		math::deg2rad( vertical_fov() ),
		aspect_ratio( ),
		math::max( near_plane, 0.001f ),
		m_far_plane
	);

	on_camera_move				( );
}

void scene_view_panel::action_focus_to_origin( )
{
	action_focus_to_point( float3(0.0f,  0.0f, 0.0f) );
}

void scene_view_panel::action_focus_to_point( math::float3 const& point )
{
	math::float3 eye_pos	= get_inverted_view_matrix( ).c.xyz( );
	setup_view_matrix		( eye_pos, point );
	set_view_point_distance	( (eye_pos-point).length() );
}

void scene_view_panel::action_focus_to_bbox	( math::aabb const& bbox )
{
	math::float4x4 const& view_inv		= *m_inverted_view;
	math::float4x4 const view			= math::invert4x3( view_inv );


	math::float4x4 const& projection	= *m_projection;

	math::float3 const extens			= bbox.extents();
	math::float3 const center			= bbox.center();
	R_ASSERT					(center.valid());

	float max_dimension			= math::max( extens.x*projection.e00, extens.y*projection.e11 );

	float distance				= max_dimension + extens.z;

	math::float3 new_pos		= view_inv.k.xyz()*(-distance)+ center;
	R_ASSERT					(new_pos.valid());
	math::float4x4 translate	= create_translation( new_pos - view_inv.c.xyz() );

	set_view_point_distance			( distance );
	R_ASSERT						(view_inv.valid());
	R_ASSERT						(translate.valid());
	math::float4x4 m				= view_inv*translate;
	R_ASSERT						(m.valid());

	set_flying_camera_destination	( math::invert4x3( m ) );
}

void scene_view_panel::remove_action_button_item( xray::editor_base::gui_binder^ binder, System::String^ action_name, System::String^ strip_key )
{
	ToolStrip^ strip			= get_tool_strip(strip_key);
	int idx						= strip->Items->IndexOfKey(action_name);
	ASSERT_U					(idx!=-1, "button doesnt exist %s", unmanaged_string(action_name).c_str());
	ToolStripButton^ button		= safe_cast<ToolStripButton^>(strip->Items[idx]);
	binder->unbind				( button );
	strip->Items->RemoveAt		( idx );
}

void scene_view_panel::add_action_button_item( xray::editor_base::gui_binder^ binder, System::String^ action_name, System::String^ strip_key, int prio )
{
	XRAY_UNREFERENCED_PARAMETER(prio);
	ToolStrip^ strip			= get_tool_strip(strip_key);
	ToolStripButton^ button		= nullptr;
	int idx						= strip->Items->IndexOfKey(action_name);
	ASSERT_U					(idx==-1, "button already exist %s", unmanaged_string(action_name).c_str());

	button						= gcnew ToolStripButton;
	button->Name				= action_name;
	strip->Items->Add			( button );

	binder->bind				( button, action_name );
	button->Image				= binder->get_image( action_name );
	button->DisplayStyle		= System::Windows::Forms::ToolStripItemDisplayStyle::Image;
	button->ImageScaling		= System::Windows::Forms::ToolStripItemImageScaling::SizeToFit;
}

void scene_view_panel::add_action_button_item( xray::editor_base::gui_binder^ binder, xray::editor_base::action^ action, System::String^ strip_key, int prio )
{
	XRAY_UNREFERENCED_PARAMETER(prio);
	ToolStrip^ strip			= get_tool_strip(strip_key);
	ToolStripButton^ button		= nullptr;
	int idx						= strip->Items->IndexOfKey( action->name( ) );
	ASSERT_U					(idx==-1, "button already exist %s", unmanaged_string(action->name( )).c_str());

	button						= gcnew ToolStripButton;
	button->Name				= action->name( );
	button->Tag					= action;
	strip->Items->Add			( button );

	binder->bind				( button, action );
	button->Image				= binder->get_image( action->name( ) );
	button->DisplayStyle		= System::Windows::Forms::ToolStripItemDisplayStyle::Image;
	button->ImageScaling		= System::Windows::Forms::ToolStripItemImageScaling::SizeToFit;
}

ToolStrip^ scene_view_panel::get_tool_strip( System::String^ name )
{
	ToolStrip^ result				= nullptr;
	int idx							= m_top_toolstrip_panel->Controls->IndexOfKey(name);
	ASSERT(idx!=-1, "toolstrip not found %s", unmanaged_string(name).c_str());
	result							= safe_cast<ToolStrip^>(m_top_toolstrip_panel->Controls[idx]);
	return							result;
}

ToolStrip^ scene_view_panel::create_tool_strip( System::String^ name, int prio )
{
	ToolStrip^ result			= nullptr;
	int idx						= m_top_toolstrip_panel->Controls->IndexOfKey(name);
	ASSERT_U					(idx==-1, "toolstrip already exist %", unmanaged_string(name).c_str());

	result						= gcnew ToolStrip;
	result->Name				= name;
	result->Tag					= prio;
	result->ImageScalingSize	= System::Drawing::Size(16,16);
	m_top_toolstrip_panel->Join	(result);

	return						result;
}

void scene_view_panel::switch_postprocess( )
{
	m_use_postprocess = !m_use_postprocess;
	editor_renderer()->scene().enable_post_process( *m_scene_view, m_use_postprocess);
}

void scene_view_panel::register_actions( input_engine^ ie, gui_binder^ binder, bool camera_light )
{

	System::String^ action_name	= "render grid";
	action_delegate^ a			= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &scene_view_panel::switch_render_grid));
	a->set_checked				( gcnew checked_delegate_managed(this, &scene_view_panel::get_render_grid));
	ie->register_action			( a, "");
	binder->register_image		( "base", action_name, toggle_draw_grid );
	add_action_button_item		( binder, action_name, "RenderOptionsStrip", 2 );

	if(camera_light)
	{
		action_name				= "camera light";
		a						= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &scene_view_panel::switch_camera_light));
		a->set_checked			( gcnew checked_delegate_managed(this, &scene_view_panel::get_camera_light));
		ie->register_action		( a, "");
		binder->register_image	( "base", action_name, turn_camera_light );
		add_action_button_item	( binder, action_name, "RenderOptionsStrip", 3 );

		action_name				= "sun";
		a						= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &scene_view_panel::switch_sun));
		a->set_checked			( gcnew checked_delegate_managed(this, &scene_view_panel::get_sun_active));
		ie->register_action		( a, "");
		binder->register_image	( "base", action_name, sun_light );
		add_action_button_item	( binder, action_name, "RenderOptionsStrip", 3 );
		m_sun_light_id			= 1;
	}

	{
	action_name				= "toggle WSAD mode";
	a						= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &scene_view_panel::switch_wsad_mode) );
	a->set_checked			( gcnew checked_delegate_managed(this, &scene_view_panel::get_wsad_mode));
	ie->register_action		( a, "");
	binder->register_image	( "base", action_name,	camera_wasd_move );
	add_action_button_item	( binder, action_name, "RenderOptionsStrip", 11);
	}

	action_name				= "toggle post process";
	a						= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &scene_view_panel::switch_postprocess));
	a->set_checked			( gcnew checked_delegate_managed(this, &scene_view_panel::get_postprocess));
	ie->register_action		( a, "");
	binder->register_image	( "base", action_name, toggle_post_process );
	add_action_button_item	( binder, action_name, "RenderOptionsStrip", 0 );


	m_render_modes_button	= gcnew ToolStripButton;
	apply_render_view_mode	(  true );
	
	get_tool_strip("RenderOptionsStrip")->Items->Add			( m_render_modes_button );

	m_render_modes_button->DisplayStyle	= System::Windows::Forms::ToolStripItemDisplayStyle::Text;
	m_render_modes_button->Click		+= gcnew System::EventHandler(this, &scene_view_panel::render_mode_btn_click);
	m_render_modes_button->MouseDown	+= gcnew System::Windows::Forms::MouseEventHandler(this, &scene_view_panel::render_mode_btn_mouse_down);

	ie->register_action( gcnew	camera_view_wsad( "WSAD shift forward", this, camera_view_wsad::move_action::sh_forward ), "W(View)" );
	ie->register_action( gcnew	camera_view_wsad( "WSAD shift backward", this, camera_view_wsad::move_action::sh_backward ), "S(View)" );
	ie->register_action( gcnew	camera_view_wsad( "WSAD shift left", this, camera_view_wsad::move_action::sh_left ), "A(View)" );
	ie->register_action( gcnew	camera_view_wsad( "WSAD shift right", this, camera_view_wsad::move_action::sh_right ), "D(View)" );

	ie->register_action( gcnew	camera_view_wsad( "WSAD rotate up", this, camera_view_wsad::move_action::rot_up ), "Up(View)" );
	ie->register_action( gcnew	camera_view_wsad( "WSAD rotate down", this, camera_view_wsad::move_action::rot_down ), "Down(View)" );
	ie->register_action( gcnew	camera_view_wsad( "WSAD rotate left", this, camera_view_wsad::move_action::rot_left ), "Left(View)" );
	ie->register_action( gcnew	camera_view_wsad( "WSAD rotate right", this, camera_view_wsad::move_action::rot_right ), "Right(View)" );

	switch_sun				( );
}

void scene_view_panel::scene_view_panel_Enter( System::Object^, System::EventArgs^)
{
	m_focus_enter_frame = 1;
}

void scene_view_panel::scene_view_panel_Leave( System::Object^, System::EventArgs^)
{
	m_focus_enter_frame = 0;
}

bool scene_view_panel::context_fit( )
{
	bool is_fit = false;
	u32 frame	= 5;
	is_fit = (m_focus_enter_frame!=0 && frame > m_focus_enter_frame+2);

	Form^ frm = Form::ActiveForm;

	if( xray::editor::wpf_controls::dot_net_helpers::is_type<WeifenLuo::WinFormsUI::Docking::FloatWindow^>( frm ) )
	{
		WeifenLuo::WinFormsUI::Docking::FloatWindow^ float_window = (WeifenLuo::WinFormsUI::Docking::FloatWindow^)frm; 
		if( float_window->DockPanel->ActiveContent != this )
			is_fit = false;
	}

	return is_fit;
}

void scene_view_panel::set_flying_camera_destination( float4x4 view_tranform )
{
	*m_flying_camera_destination			= view_tranform;

	*m_flying_camera_destination_vector	= math::invert4x3(*m_flying_camera_destination).c.xyz() - get_inverted_view_matrix().c.xyz();

	m_flying_camera_distance			= m_flying_camera_destination_vector->length();


	m_flying_camera_acceleration_starting	= m_flying_camera_distance * m_flying_camera_distance_factor;
	m_flying_camera_speed_starting			= 200 * m_flying_camera_acceleration_starting; 
	m_flying_camera_acceleration			= m_flying_camera_acceleration_starting;
	m_flying_camera_speed					= m_flying_camera_speed_starting;

	float time				= 0.5f * 0.5f;

	m_flying_camera_acceleration_increase = ( 2*(m_flying_camera_distance/2-m_flying_camera_speed_starting*time) - m_flying_camera_acceleration_starting*math::sqr(time) ) / (math::sqr(time)*time);

	m_flying_camera_animating				= true;

	m_flying_camera_timer->start			();
	tick								();
	return;
}

void scene_view_panel::load_settings( RegistryKey^ owner_editor_key )
{
	set_render_grid			( System::Convert::ToBoolean(owner_editor_key->GetValue("draw_grid", true)) );
	m_near_plane			= math::max( 0.1f, convert_to_single(owner_editor_key->GetValue("near_plane", "0.1")) );
	m_far_plane				= math::max( 2*m_near_plane, convert_to_single(owner_editor_key->GetValue("far_plane", "300.0")) );
	m_selection_rate		= convert_to_single(owner_editor_key->GetValue("selection_rate", "1.0"));

	m_grid_step				= convert_to_single(owner_editor_key->GetValue("grid_step", "1.0"));
	m_grid_dim				= System::Convert::ToUInt32(owner_editor_key->GetValue("grid_dim", "100"));

	u32 argb				= System::Convert::ToUInt32(owner_editor_key->GetValue("selection_color", math::color_rgba(85u, 0, 255, 255) ) );
	*m_selection_color		= math::color( argb );

	set_mouse_sensitivity( convert_to_single(owner_editor_key->GetValue( "mouse_sensitivity", "0.3"))	);

	System::String^ rm		= owner_editor_key->GetValue("render_mode", "lit" )->ToString();

	m_render_view_mode		= text_to_render_mode( rm );
	apply_render_view_mode	( false );
}

void scene_view_panel::save_settings( RegistryKey^ owner_editor_key )
{
	owner_editor_key->SetValue		( "draw_grid", get_render_grid() );
	owner_editor_key->SetValue		( "near_plane", convert_to_string(m_near_plane) );
	owner_editor_key->SetValue		( "far_plane", convert_to_string(m_far_plane) );
	owner_editor_key->SetValue		( "selection_rate", convert_to_string(m_selection_rate) );
	owner_editor_key->SetValue		( "selection_color", m_selection_color->m_value );
	owner_editor_key->SetValue		( "mouse_sensitivity", convert_to_string(mouse_sensitivity()) );

	owner_editor_key->SetValue		( "grid_step", convert_to_string(m_grid_step) );
	owner_editor_key->SetValue		( "grid_dim", m_grid_dim );

	System::String^ rm				= gcnew System::String(view_mode_names[m_render_view_mode]);
	owner_editor_key->SetValue		( "render_mode", rm );
}


} //namespace editor_base
} //namespace xray
