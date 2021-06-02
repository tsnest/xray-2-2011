////////////////////////////////////////////////////////////////////////////
//	Created		: 08.02.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "terrain_control_cursor.h"
#include "terrain_modifier_control.h"
#include "level_editor.h"
#include "terrain_object.h"
#include "tool_terrain.h"
#include "collision_object_dynamic.h"

#pragma managed( push, off )
#	include <xray/editor/world/engine.h>
#	include <xray/render/facade/scene_renderer.h>
#	include <xray/geometry_utils.h>
#	include <xray/render/engine/model_format.h>
#	include <d3d9types.h>
#pragma managed( pop )

namespace xray {
namespace editor {

terrain_control_cursor::terrain_control_cursor( terrain_control_base^ o, render::scene_ptr const& scene ) :
	m_last_picked_position	( NEW(float3)(0,0,0)),
	m_scene					( NEW( render::scene_ptr ) (scene) ),
	m_pinned				( false ),
	m_owner					( o )
{
}

terrain_control_cursor::~terrain_control_cursor( )
{
	DELETE (m_scene);
	DELETE (m_last_picked_position);
}

void terrain_control_cursor::update_ring(	terrain_core^ terrain, 
											float3 const& picked_position_global,  
											float radius, 
											float width_, 
											u16 segments_count, 
											vector<vert_struct>& vertices)
{
	float3 cam_p, cam_d;
	float3 dir_to_camera;
	m_owner->m_level_editor->view_window()->get_camera_props(cam_p, cam_d);

	float dist						= (cam_p - picked_position_global).length();
	float width						= width_ * dist/10.0f;
	width							= math::min( width_, width );
	float height					= 0.0f;
	vector<vert_struct>::iterator	vit;
	vit								= vertices.begin();
	float segment_ang				= math::pi_x2/segments_count;

	float3 p;
	for(u32 i=0; i<segments_count; ++i)
	{
		math::sine_cosine			sincos(segment_ang*i);

		p.set						(radius*sincos.cosine, 0.0f, -radius*sincos.sine);
		float3 pg					= p+picked_position_global;
		
		height						= terrain->get_height(pg);

		p.y							= height - picked_position_global.y;
		
		pg.y						= height;

		dir_to_camera				= cam_p - pg;
		dir_to_camera.normalize_safe( float3(0,1,0) );
		dir_to_camera				*= 0.1f;

		(*vit).position				= p + dir_to_camera;
		++vit;

		p.set						((radius+width)*sincos.cosine, 0.0f, -(radius+width)*sincos.sine);
		
		pg							= p+picked_position_global;

		height						= terrain->get_height(pg);
		p.y							= height - picked_position_global.y;
		pg.y						= height;

		(*vit).position				= p + dir_to_camera;
		++vit;
	}
}

terrain_painter_cursor::terrain_painter_cursor(	terrain_modifier_control_base^ owner, render::scene_ptr const& scene)
:super				( owner, scene ),
m_shown				( false ),
m_visual_inner_added( false ),
m_visual_outer_added( false ),
m_visual_center_added( false ),
m_visual_power_added( false )
{
	m_inner_width		= 0.2f;
	m_outer_width		= 0.2f;
	m_center_width		= 0.2f;
	m_inner_radius		= 1.0f;
	m_outer_radius		= 1.0f;
	m_center_radius		= 0.0f;
	m_inner_segments	= 256; //60;
	m_outer_segments	= 256; //60;
	m_center_segments	= 8;
	
	m_inner_vertices	= NEW (vector<vert_struct>)();
	m_visual_inner_ring	= NEW (render::render_model_instance_ptr)();

	m_outer_vertices	= NEW (vector<vert_struct>)();
	m_visual_outer_ring	= NEW (render::render_model_instance_ptr)();

	m_center_vertices	= NEW (vector<vert_struct>)();
	m_visual_center_point= NEW(render::render_model_instance_ptr)();

	m_power_vertices	= NEW (vector<vert_struct>)();
	m_visual_power		= NEW(render::render_model_instance_ptr)();

	generate_geometry		( );
}

terrain_painter_cursor::~terrain_painter_cursor()
{
	if(m_shown)
		show(false);

	DELETE ( m_visual_inner_ring );
	DELETE ( m_visual_outer_ring );
	DELETE ( m_visual_center_point );
	DELETE ( m_visual_power );
	DELETE ( m_inner_vertices );
	DELETE ( m_outer_vertices );
	DELETE ( m_center_vertices );
	DELETE ( m_power_vertices );
}

void terrain_painter_cursor::on_visual_ready(xray::resources::queries_result& data)
{
	R_ASSERT(data.is_successful());

	*m_visual_inner_ring	= static_cast_resource_ptr<render::render_model_instance_ptr>(data[0].get_unmanaged_resource());
	ASSERT(*m_visual_inner_ring);
	*m_visual_outer_ring	= static_cast_resource_ptr<render::render_model_instance_ptr>(data[1].get_unmanaged_resource());
	ASSERT(*m_visual_outer_ring);
	*m_visual_center_point	= static_cast_resource_ptr<render::render_model_instance_ptr>(data[2].get_unmanaged_resource());
	ASSERT(*m_visual_center_point);
	*m_visual_power		= static_cast_resource_ptr<render::render_model_instance_ptr>(data[3].get_unmanaged_resource());
	ASSERT(*m_visual_power);

	for(u32 data_idx=0; data_idx<data.size(); ++data_idx)
	{
		const_buffer user_data_to_create	= data[data_idx].creation_data_from_user();
		pbyte data							= (pbyte)user_data_to_create.c_ptr();
		DELETE (data);
	}

	if(m_shown)
		show(true);
}

void terrain_painter_cursor::show(bool show)
{
	m_shown				= show;
	xray::render::editor::renderer& r = m_owner->m_level_editor->get_editor_renderer();

	if(m_visual_inner_ring->c_ptr())
	{
		if(m_shown)
		{
			float4x4 m		= float4x4().identity();
			if( !m_visual_inner_added )
			{
				r.scene().add_model	( *m_scene, *m_visual_inner_ring, m );
				m_visual_inner_added	= true;
			}
			if(!m_visual_outer_added)
			{
				r.scene().add_model	( *m_scene, *m_visual_outer_ring,	m );
				m_visual_outer_added	= true;
			}
			if( !m_visual_center_added )
			{
				r.scene().add_model	( *m_scene, *m_visual_center_point, m );
				m_visual_center_added	= true;
			}

			//if( !m_visual_power_added )
			//{
			//	r.add_model				( *m_visual_power, m, false, true );
			//	m_visual_power_added	= true;
			//}
		}else
		{
			if( m_visual_inner_added )
			{
				r.scene().remove_model( *m_scene, *m_visual_inner_ring );
				m_visual_inner_added	= false;
			}
			if( m_visual_outer_added )
			{
				r.scene().remove_model( *m_scene, *m_visual_outer_ring );
				m_visual_outer_added	= false;
			}
			if( m_visual_center_added )
			{
				r.scene().remove_model( *m_scene, *m_visual_center_point );
				m_visual_center_added	= false;
			}
			if( m_visual_power_added )
			{
				r.scene().remove_model( *m_scene, *m_visual_power );
				m_visual_power_added	= false;
			}
		}
	}
}


void terrain_painter_cursor::update()
{
	level_editor^ le		= m_owner->m_level_editor;

	System::Drawing::Point	mouse_pos = le->view_window()->get_mouse_position();
	if(m_last_mouse_pos==mouse_pos && !m_pinned)
		return;

	terrain_node^ terrain		= nullptr;
	collision::object const* picked_collision = NULL;
	
	if(le->view_window()->ray_query( m_owner->acceptable_collision_type(), &picked_collision, m_last_picked_position ) )
	{
		if(!m_shown)
			show		(true);

		ASSERT( picked_collision->get_type() & m_owner->acceptable_collision_type() );
	
		object_base^ o					= (static_cast_checked<collision_object_dynamic const*>(picked_collision))->get_owner_object();
		terrain							= safe_cast<terrain_node^>(o);
		update							(terrain->get_terrain_core(), *m_last_picked_position);
	}else
	{
		if(m_shown)
			show		(false);
	}

	//if(terrain)
	//{
	//System::String^ props	= System::String::Format("Terrain[{0}:{1}]: {2:f1} {3:f1} {4:f1}", terrain->m_tmp_key.x, terrain->m_tmp_key.z,
	//						m_last_picked_position->x, m_last_picked_position->y, m_last_picked_position->z);

	//le->ide()->set_status_label		(2, props);
	//}

	m_last_mouse_pos				= mouse_pos;
}

void terrain_painter_cursor::update(terrain_core^ terrain_core, float3 const& picked_position_global)
{
	if(!(*m_visual_inner_ring).c_ptr()) // not not loaded still
		return;

	math::float4x4 tr = create_translation(picked_position_global);

	xray::render::editor::renderer& r = m_owner->m_level_editor->get_editor_renderer();
	terrain_modifier_control_base^	modifier_control = safe_cast<terrain_modifier_control_base^>(m_owner);
	// tmp, for debug
	m_outer_radius					= m_owner->radius;
	m_inner_radius					= m_owner->radius * modifier_control->hardness;


	typedef vectora<xray::render::buffer_fragment>	buffer_fragments;
	buffer_fragments										fragments(g_allocator);
	fragments.resize				(1);
	xray::render::buffer_fragment& fragment = fragments[0];
	fragment.start					= 0;

	// update inner
	fragment.size					= m_inner_vertices->size() * sizeof(vert_struct);
	fragment.buffer					= &((*m_inner_vertices)[0]);
	
	update_ring(terrain_core, picked_position_global, m_inner_radius, m_inner_width, m_inner_segments, *m_inner_vertices);
	r.scene().update_model_vertex_buffer(*m_visual_inner_ring, fragments);
	r.scene().update_model			( *m_scene, *m_visual_inner_ring, tr );

	// update outer
	fragment.size					= m_outer_vertices->size() * sizeof(vert_struct);
	fragment.buffer					= &((*m_outer_vertices)[0]);
	
	update_ring(terrain_core, picked_position_global, m_outer_radius, m_outer_width, m_outer_segments, *m_outer_vertices);
	r.scene().update_model_vertex_buffer(*m_visual_outer_ring, fragments);
	r.scene().update_model			( *m_scene, *m_visual_outer_ring, tr );

	// update center
	fragment.size					= m_center_vertices->size() * sizeof(vert_struct);
	fragment.buffer					= &((*m_center_vertices)[0]);
	
	update_ring(terrain_core, picked_position_global, m_center_radius, m_center_width, m_center_segments, *m_center_vertices);
	r.scene().update_model_vertex_buffer(*m_visual_center_point, fragments);
	r.scene().update_model			( *m_scene, *m_visual_center_point, tr );

	if(m_pinned)
	{

		float3 p					= picked_position_global;
		float h						= terrain_core->get_height	(p);
		p.y							= h;

		float4x4 m					= create_scale(float3(0.5f, modifier_control->power_value(), 0.5f)) * create_translation(p);

		if( !m_visual_power_added )
		{
			r.scene().add_model		( *m_scene, *m_visual_power, m );
			m_visual_power_added	= true;
		}

		r.scene().update_model		( *m_scene, *m_visual_power, m );
	}else
	{
		if( m_visual_power_added )
		{
			r.scene().remove_model	( *m_scene, *m_visual_power );
			m_visual_power_added	= false;
		}
	}
}

void export_ring(vector<vert_struct>& dest_vertices, 
				 pbyte& dest_buffer, 
				 u32& size, 
				 float radius, 
				 float width, 
				 u16 segments, 
				 pcstr material_name )
{
	xray::memory::writer	writer(g_allocator);
	writer.external_data	= true;
	render::model_header			hdr;

	// fill hdr here !!!
	hdr.platform_id			= render::platform_uni;
	hdr.bb.identity			( );
	hdr.type				= render::mt_user_mesh_editable;

	geometry_utils::geom_vertices_type	vertices(g_allocator);
	geometry_utils::geom_indices_type	indices(g_allocator);
	geometry_utils::geom_vertices_type::const_iterator	it, it_e;
	vector<vert_struct>::iterator	vit;

	geometry_utils::create_ring	(vertices, indices, radius, width, segments );
	writer.open_chunk		( render::model_chunk_header );
	writer.write			( &hdr, sizeof(hdr) );
	writer.close_chunk		( );//model_chunk_header

	writer.open_chunk		( render::model_texture );
    writer.write_string		( material_name );
	writer.close_chunk		( ); //model_texture

	writer.open_chunk		( render::model_chunk_vertices );

	dest_vertices.resize	( vertices.size() );
	writer.write_u32		( vertices.size() );

	it						= vertices.begin();
	it_e					= vertices.end();
	vit						= dest_vertices.begin();
	bool b_inner			= true;
	for(; it!=it_e; ++it,++vit)
	{
		vert_struct& v		= *vit;
		v.position			= (*it);
		v.uv				= (b_inner) ? float2(0, 1) : float2(0, 0);

		b_inner				= !b_inner;
	}
	writer.write			( &dest_vertices.front(), dest_vertices.size()* sizeof(vert_struct) );
	writer.close_chunk		( ); //model_chunk_vertices


	writer.open_chunk		( render::model_chunk_indices );
	writer.write_u32		( indices.size() );
	writer.write			( &indices[0], indices.size()*sizeof(indices[0]) );
	writer.close_chunk		( ); //model_chunk_indices

	size					= writer.size();
	dest_buffer				= writer.pointer();
}

void export_cylinder(vector<vert_struct>& dest_vertices, 
					 pbyte& dest_buffer, 
					 u32& size, 
					 float3 dim, 
					 pcstr material_name )
{
	using namespace xray::render;

	xray::memory::writer	writer(g_allocator);
	writer.external_data	= true;
	model_header			hdr;

	// fill hdr here !!!
	hdr.platform_id			= platform_uni;
	hdr.bb.min				= float3(-1, -1, -1);
	hdr.bb.max				= float3(1, 1, 1);
	hdr.type				= mt_user_mesh_editable;

	geometry_utils::geom_vertices_type	vertices(g_allocator);
	geometry_utils::geom_indices_type	indices(g_allocator);
	geometry_utils::geom_vertices_type::const_iterator	it, it_e;
	vector<vert_struct>::iterator	vit;

	geometry_utils::create_cylinder	(vertices, indices, float4x4().identity(), dim );

	writer.open_chunk		( model_chunk_header );
	writer.write			( &hdr, sizeof(hdr) );
	writer.close_chunk		( );//model_chunk_header

	writer.open_chunk		( model_texture );
    writer.write_string		( material_name );
	writer.close_chunk		( );//model_texture

	writer.open_chunk		( model_chunk_vertices );

////vertices
	dest_vertices.resize	( vertices.size() );
	writer.write_u32		( vertices.size() );

	it						= vertices.begin();
	it_e					= vertices.end();
	vit						= dest_vertices.begin();
	
	float3 position_offset	(0.0f, 1.0f, 0.0f);

	for(; it!=it_e; ++it,++vit)
	{
		vert_struct& v		= *vit;
		v.position			= (*it) + position_offset;
	
		v.uv				= (v.position.y>0) ? float2(0, 1) : float2(0, 0);
	}
	writer.write			( &dest_vertices.front(), dest_vertices.size()* sizeof(vert_struct) );
	writer.close_chunk		( );//model_chunk_vertices


	writer.open_chunk		( model_chunk_indices );
	writer.write_u32		( indices.size() );
	writer.write			( &indices[0], indices.size()*sizeof(indices[0]) );
	writer.close_chunk		( );//model_chunk_indices

	size					= writer.size();
	dest_buffer				= writer.pointer();
}

void terrain_painter_cursor::generate_geometry()
{
	pbyte inner_p;
	pbyte outer_p;
	pbyte center_p;
	pbyte power_p;
	u32 inner_p_size;
	u32 outer_p_size;
	u32 center_p_size;
	u32 power_p_size;

	export_ring(	*m_inner_vertices,
					inner_p, inner_p_size, 
					m_inner_radius, m_inner_width, m_inner_segments,
					"editor/terrain_cursor");

	export_ring(	*m_outer_vertices,
					outer_p, outer_p_size, 
					m_outer_radius, m_outer_width, m_outer_segments,
					"editor/terrain_cursor");

	export_ring(	*m_center_vertices,
					center_p, center_p_size, 
					m_center_radius, m_center_width, m_center_segments,
					"editor/terrain_cursor");

	export_cylinder(*m_power_vertices,
					power_p, power_p_size, 
					float3(0.07f, 1.0f, 0.07f), // same as center point
					"editor/terrain_cursor");

	query_result_delegate* q		= NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &terrain_painter_cursor::on_visual_ready), g_allocator);

	using resources::creation_request;
	creation_request requests[] =
	{
		creation_request( "inner_p", const_buffer(inner_p, inner_p_size),		resources::user_mesh_class ),
		creation_request( "outer_p", const_buffer(outer_p, outer_p_size),		resources::user_mesh_class ),
		creation_request( "center_p", const_buffer(center_p, center_p_size),	resources::user_mesh_class ),
		creation_request( "strength_p", const_buffer(power_p, power_p_size),	resources::user_mesh_class ),
	};
	resources::query_create_resources(	requests,
										boost::bind(&query_result_delegate::callback, q, _1),
										g_allocator);
}



terrain_selector_cursor::terrain_selector_cursor( terrain_control_base^ owner, render::scene_ptr const& scene )
:super			( owner, scene ),
m_shown			( false ),
m_visual_added	( false )
{
	m_outer_width		= 0.2f;
	m_outer_radius		= 1.0f;
	m_outer_segments	= 256; //60;

	m_outer_vertices	= NEW ( vector<vert_struct> )();
	m_visual_outer_ring	= NEW ( render::render_model_instance_ptr )();

	generate_geometry		( );
}

terrain_selector_cursor::~terrain_selector_cursor()
{
	if(m_shown)
		show(false);

	DELETE (m_visual_outer_ring);
	DELETE (m_outer_vertices);
}

void terrain_selector_cursor::on_visual_ready(xray::resources::queries_result& data)
{
	R_ASSERT(data.is_successful());

	*m_visual_outer_ring	= static_cast_resource_ptr<render::render_model_instance_ptr>(data[0].get_unmanaged_resource());
	ASSERT(*m_visual_outer_ring);

	for(u32 data_idx=0; data_idx<data.size(); ++data_idx)
	{
		const_buffer user_data_to_create	= data[data_idx].creation_data_from_user();
		pbyte data							= (pbyte)user_data_to_create.c_ptr();
		DELETE (data);
	}

	if(m_shown)
		show(true);
}

void terrain_selector_cursor::show(bool show)
{
	m_shown				= show;
	xray::render::editor::renderer& r = m_owner->m_level_editor->get_editor_renderer();

	if(m_visual_outer_ring->c_ptr())
	{
		if(m_shown)
		{
			if(!m_visual_added)
			{
				float4x4 m		= float4x4().identity();
				r.scene().add_model	( *m_scene, *m_visual_outer_ring, m );
				m_visual_added	= true;
			}
		}else
		{
			if(m_visual_added)
			{
				r.scene().remove_model	( *m_scene, *m_visual_outer_ring );
				m_visual_added	= false;
			}
		}
	}
}


void terrain_selector_cursor::update()
{
	level_editor^ le		= m_owner->m_level_editor;

	System::Drawing::Point	mouse_pos =	le->view_window()->get_mouse_position();

	if(m_last_mouse_pos==mouse_pos && !m_pinned)
		return;

	terrain_node^ terrain		= nullptr;
	collision::object const* picked_collision = NULL;
	
	if(le->view_window()->ray_query( m_owner->acceptable_collision_type(), &picked_collision, m_last_picked_position ) )
	{
		if(!m_shown)
			show		(true);

		ASSERT( picked_collision->get_type() & m_owner->acceptable_collision_type() );
	
		object_base^ o					= (static_cast_checked<collision_object_dynamic const*>(picked_collision))->get_owner_object();
		terrain							= safe_cast<terrain_node^>(o);
		update							(terrain->get_terrain_core(), *m_last_picked_position);
	}else
	{
		if(m_shown)
			show		(false);
	}

	//if(terrain)
	//{
	//System::String^ props	= System::String::Format("Terrain[{0}:{1}]: {2:f1} {3:f1} {4:f1}", terrain->m_tmp_key.x, terrain->m_tmp_key.z,
	//						m_last_picked_position->x, m_last_picked_position->y, m_last_picked_position->z);

	//le->ide()->set_status_label		(2, props);
	//}

	m_last_mouse_pos				= mouse_pos;
}

void terrain_selector_cursor::update(terrain_core^ terrain_core, float3 const& picked_position_global)
{
	if(!(*m_visual_outer_ring).c_ptr()) // not not loaded still
		return;

	xray::render::editor::renderer& r = m_owner->m_level_editor->get_editor_renderer();
	
	// tmp, for debug
	m_outer_radius					= m_owner->radius;


	typedef vectora<xray::render::buffer_fragment>	buffer_fragments;
	buffer_fragments								fragments(g_allocator);
	fragments.resize				(1);
	xray::render::buffer_fragment& fragment = fragments[0];
	fragment.start					= 0;

	// update outer
	fragment.size					= m_outer_vertices->size() * sizeof(vert_struct);
	fragment.buffer					= &((*m_outer_vertices)[0]);
	
	update_ring(terrain_core, picked_position_global, m_outer_radius, m_outer_width, m_outer_segments, *m_outer_vertices);
	r.scene().update_model_vertex_buffer(*m_visual_outer_ring, fragments);
	math::float4x4 tr				= create_translation(picked_position_global);
	r.scene().update_model			( *m_scene, *m_visual_outer_ring, tr );
}



void terrain_selector_cursor::generate_geometry()
{
	pbyte outer_p;
	u32 outer_p_size;

	export_ring(	*m_outer_vertices,
					outer_p, outer_p_size, 
					m_outer_radius, m_outer_width, m_outer_segments,
					"editor/terrain_cursor");


	query_result_delegate* q		= NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &terrain_selector_cursor::on_visual_ready), g_allocator);

	resources::creation_request requests[] =
	{
		resources::creation_request("outer_p", const_buffer(outer_p, outer_p_size), resources::user_mesh_class),
	};
	resources::query_create_resources(	requests,
										boost::bind(&query_result_delegate::callback, q, _1),
										g_allocator);
}

} // namespace editor
} // namespace xray
