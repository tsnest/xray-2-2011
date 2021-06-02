////////////////////////////////////////////////////////////////////////////
//	Created		: 04.10.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_wire_set.h"
#include "tool_misc.h"
#include "editor_world.h"
#include "level_editor.h"
#include "project_items.h"
#include "project.h"

#pragma managed( push, off )
#	include <xray/physics/world.h>
#	include <xray/physics/soft_body.h>
#	include <xray/physics/soft_body.h>
#	include <xray/render/facade/debug_renderer.h>
#	include <xray/render/facade/editor_renderer.h>
#	include <xray/render/facade/scene_renderer.h>
#	include <xray/collision/collision_object.h>
#	include <xray/collision/space_partitioning_tree.h>
#	include <xray/editor/base/collision_object_types.h>
#	include <xray/render/engine/model_format.h>
#	include <d3d9types.h>
#pragma managed( pop )

using namespace xray::editor::wpf_controls;
using namespace xray::editor::wpf_controls::control_containers;

namespace xray{
namespace editor{

class collision_object_wire_point: public collision::collision_object
{
private:
	typedef collision::collision_object	super;

public:
	collision_object_wire_point		( collision::geometry_instance * geometry, object_wire_set^ o, int point_idx )
		:super		( g_allocator, xray::editor_base::collision_object_type_touch, geometry ),
		m_owner		( o ),
		m_point_idx( point_idx )
		{}

private:
	int							m_point_idx;
	gcroot<object_wire_set^>	m_owner;

public:
	virtual bool			touch	( ) const
	{
	if( !m_owner->owner_project_item->get_selected() )
		m_owner->get_project( )->select_id( m_owner->id( ), xray::editor::enum_selection_method_set );

	m_owner->m_item->m_selected_point = m_point_idx;
	m_owner->owner_tool()->get_level_editor()->set_transform_object( m_owner->m_item );
	return true;
	}
}; //collision_object_wire_point


wire_item::wire_item( object_wire_set^ parent )
:m_parent				( parent ),
m_captured_points		(NULL),
m_captured_points_count	( 0 )
{
	m_offset1 = NEW(float3)(0,0,0);
	m_start_transform = NEW(float4x4)();

	fragments = 10;
	stiftness = 0.5f;
	iterations = 200;
	margin = 0.1f;
	timescale = 1.0f;
	wire_width = 0.05f;
}

wire_item::~wire_item( )
{
	DELETE		( m_offset1 );
	DELETE		( m_start_transform );
}

float3 wire_item::current_point()
{
	return (m_selected_point==0) ? float3(0,0,0) : *m_offset1;
}

void wire_item::start_modify( editor_base::transform_control_base^ )
{
	(*m_start_transform) = create_translation(current_point()) * m_parent->get_transform();
}

void wire_item::execute_preview( editor_base::transform_control_base^ control )
{
	float4x4 const current_transform		= control->calculate( *m_start_transform );

	if(m_selected_point==0)
	{
		float4x4 const world_transform_p1	= create_translation(*m_offset1) * m_parent->get_transform();
		m_parent->set_transform				( current_transform );

		float4x4 ti							= math::invert4x3( current_transform );
		float4x4 const trelative			= world_transform_p1 * ti;
		*m_offset1							= trelative.c.xyz();
	}else
	{
		float4x4 ti							= math::invert4x3( m_parent->get_transform() );
		float4x4 const trelative			= current_transform * ti;
		*m_offset1							= trelative.c.xyz();
	}
	m_parent->update_bbox					( );
	m_parent->update_collision				( );
}

void wire_item::end_modify( editor_base::transform_control_base^ )
{
}

float4x4 wire_item::get_ancor_transform( )
{
	math::float4x4 ret_t = create_translation(current_point()) * m_parent->get_transform();
	return			ret_t;
}

u32 wire_item::get_collision( System::Collections::Generic::List<editor_base::collision_object_wrapper>^% r )
{
	editor_base::collision_object_wrapper w;
	w.m_collision_object			= m_parent->m_start_point_collision;
	r->Add							( w );

	editor_base::collision_object_wrapper w2;
	w2.m_collision_object			= m_parent->m_end_point_collision;
	r->Add							( w2 );
	
	return r->Count;
}

object_wire_set::object_wire_set( tool_misc^ tool )
:super(tool),
m_start_point_collision	( NULL ),
m_end_point_collision	( NULL )
{
	image_index	= xray::editor_base::misc_tree_icon;
	m_visual	= NEW(render::render_model_instance_ptr)();
};

object_wire_set::~object_wire_set( )
{
	DELETE(m_visual);
}

void object_wire_set::set_visible(bool bvisible)
{
	if((*m_visual).c_ptr() && get_visible()!=bvisible)
	{
		render::scene_ptr const& scene				= get_editor_world().scene();
		if(bvisible)
		{
			get_editor_renderer().scene().add_model	( scene, *m_visual, *m_transform );
		}else
		{
			get_editor_renderer().scene().remove_model	( scene, *m_visual );
			*m_visual									= NULL;
		}
	}
	super::set_visible(bvisible);
}

void object_wire_set::load_contents( )
{
	if(m_start_point_collision && m_end_point_collision)
		return;

	float4x4 m					= get_transform	( );

	collision::geometry_instance* geom	= &*collision::new_box_geometry_instance( &debug::g_mt_allocator, 
											create_scale(float3(0.15f,0.15f,0.15f)) * m );

	m_start_point_collision		= NEW(collision_object_wire_point)(geom, this, 0);


	collision::geometry_instance* geom2	= &*collision::new_box_geometry_instance( &debug::g_mt_allocator, 
											create_translation(*m_item->m_offset1) * create_scale( float3(0.15f,0.15f,0.15f) ) * m );
	m_end_point_collision		= NEW(collision_object_wire_point)(geom2, this, 1);

	collision::space_partitioning_tree* tree = get_level_editor()->get_collision_tree();
	tree->insert				(	m_start_point_collision, geom->get_matrix() );

	tree->insert				(	m_end_point_collision, geom2->get_matrix() );

	if(m_item->m_captured_points_count)
		query_create_visual	( );

}

void object_wire_set::unload_contents( bool )
{
	destroy_collision		( );

	if((*m_visual).c_ptr())
	{
		render::scene_ptr const& scene				= get_editor_world().scene();
		get_editor_renderer().scene().remove_model	( scene, *m_visual );
		*m_visual									= NULL;
	}
};

void object_wire_set::save( configs::lua_config_value t )
{
	super::save				( t );
	t["offset1"]			= *m_item->m_offset1;
	t["stiftness"]			= m_item->stiftness;
	t["iterations"]			= m_item->iterations;
	t["margin"]				= m_item->margin;
	t["timescale"]			= m_item->timescale;
	t["fragments_count"]	= m_item->fragments;

	t["wire_width"]			= m_item->wire_width;

	t["game_object_type"]	= "wire_set";
	t["points_count"]		= m_item->m_captured_points_count;
	if(m_item->m_captured_points)
	{
		for(u32 i=0; i<m_item->m_captured_points_count;++i)
			t["points"][i] = m_item->m_captured_points[i];
	}
};

void object_wire_set::load_props( configs::lua_config_value const& t )
{
	m_item				= gcnew wire_item( this );

	*m_item->m_offset1	= t["offset1"];
	m_item->stiftness	= t["stiftness"];
	m_item->iterations	= t["iterations"];
	m_item->margin		= t["margin"];
	m_item->timescale	= t["timescale"];
	m_item->fragments	= t["fragments_count"];
	m_item->wire_width	= t["wire_width"];


	super::load_props	( t );
	m_item->m_captured_points_count = t["points_count"];

	if(m_item->m_captured_points_count!=0)
	{
		m_item->m_captured_points = ALLOC(float3, m_item->m_captured_points_count);
		float3* p			= m_item->m_captured_points;
		for(u32 i=0; i<m_item->m_captured_points_count; ++i)
		{
			float3 pt	= t["points"][i];
			*p			= pt;
			++p;
		}
	}

}

void object_wire_set::render( )
{
	render::scene_ptr const& scene				= get_editor_world().scene();
	render::debug::renderer& debug_renderer		= get_debug_renderer();
	float4x4 m									= get_transform();
	
	math::color clr_selected	(255,255,0,255);
	math::color clr_normal		(0,255,0,255);
	math::color& clr			= m_project_item->get_selected() ? clr_selected : clr_normal;

	if(m_item->m_physic_body)
	{
		u32 count = m_item->m_physic_body->get_fragments_count();
		for(u32 i=0; i<count; ++i)
		{
			float3 pt0, pt1;
			m_item->m_physic_body->get_fragment	( i, pt0, pt1 );
			debug_renderer.draw_line			( scene, pt0, pt1, math::color(255,0,0,255) );
		}
	}else
	if( (*m_visual).c_ptr()==NULL && m_item->m_captured_points )
	{
		for(u32 i=0; i<=m_item->m_captured_points_count-1; ++i)
		{
			float3 pt0					= m_item->m_captured_points[i] * m;
			float3 pt1					= m_item->m_captured_points[i+1] * m;
			debug_renderer.draw_line	( scene, pt0, pt1, clr );
		}
	}

	float half_size				(0.15f);

	float3 pt0 = m.c.xyz();
	float3 pt1 = *m_item->m_offset1 * m;

	math::color center_lines_clr (255,255,0,100);
	debug_renderer.draw_line( scene, pt0, pt1, center_lines_clr );

	debug_renderer.draw_cross( scene, pt0, half_size, clr );
	debug_renderer.draw_cross( scene, pt1, half_size, clr );

}

void object_wire_set::on_visual_loaded( resources::queries_result& data )
{
	R_ASSERT							(data.is_successful());
	const_buffer user_data_to_create	= data[0].creation_data_from_user();
	pbyte creation_data					= (pbyte)user_data_to_create.c_ptr();
	DELETE								( creation_data );

	*m_visual								= static_cast_resource_ptr<render::render_model_instance_ptr>(data[0].get_unmanaged_resource());
	if(get_visible())
	{
		render::scene_ptr const& scene			= get_editor_world().scene();
		get_editor_renderer().scene().add_model	( scene, *m_visual, *m_transform );
	}
}

void object_wire_set::rebuild( button^ )
{
	if((*m_visual).c_ptr())
	{
		render::scene_ptr const& scene				= get_editor_world().scene();
		get_editor_renderer().scene().remove_model	( scene, *m_visual );
		*m_visual									= NULL;
	}

	physics::world& w = get_editor_world().get_physics_world();

	if(m_item->m_physic_body)
	{
		w.remove_soft_body				( m_item->m_physic_body );
		physics::destroy_soft_body_rope	( &w, m_item->m_physic_body );
	}

	float4x4 m				= get_transform( );
	static physics::rope_construction_info info;

	//info.mass					= m_item->mass;
	info.stiftness				= m_item->stiftness;
	info.iterations				= m_item->iterations;
	info.margin					= m_item->margin;
	info.timescale				= m_item->timescale;
	info.fragments_count		= m_item->fragments;

	info.p0						= m.c.xyz();
	info.p1						= *m_item->m_offset1 * m;



	m_item->m_physic_body	= physics::create_soft_body_rope( &w, info );
	w.add_soft_body			( m_item->m_physic_body );
}

void object_wire_set::capture( button^ )
{
	if(m_item->m_physic_body)
	{
		physics::world& w = get_editor_world().get_physics_world();
		
		if(m_item->m_captured_points)
			FREE(m_item->m_captured_points);

		float4x4 ti				= math::invert4x3( get_transform( ) );

		m_item->m_captured_points_count	= m_item->m_physic_body->get_nodes_count();
		m_item->m_captured_points		= ALLOC(float3, m_item->m_captured_points_count);
		float3* p						= m_item->m_captured_points;
		for(u32 i=0; i<m_item->m_captured_points_count; ++i)
		{
			float3 pt;
			m_item->m_physic_body->get_node( i, pt );

			float3 const trelative				= pt * ti;

			*p = trelative;
			++p;
		}

		w.remove_soft_body				( m_item->m_physic_body );
		physics::destroy_soft_body_rope	( &w, m_item->m_physic_body );
		m_item->m_physic_body			= NULL;

		query_create_visual				( );
	}
}

static void create_wire_visual_source( memory::writer& writer,
								float3* points,
								u32 points_count,
								pcstr material_name, 
								float wire_width)
{
	render::model_header	hdr;

	//// fill hdr here !!!
	hdr.platform_id			= render::platform_uni;
	hdr.bb.min				= float3(-1, -1, -1);
	hdr.bb.max				= float3(1, 1, 1);
	hdr.type				= render::mt_user_mesh_wire;

	writer.open_chunk		( render::model_chunk_header );
	writer.write			( &hdr, sizeof(hdr) );
	writer.close_chunk		( );//model_chunk_header

	writer.open_chunk		( render::model_texture );
    writer.write_string		( material_name );
	
	writer.write_float		( wire_width ); // wire width
	
	writer.close_chunk		( );//model_texture

	writer.open_chunk		( render::model_chunk_vertices );

	writer.write_u32		( points_count );

	for(u32 i=0; i<points_count; ++i)
		writer.write_float3(points[i]);

	writer.close_chunk		( );//model_chunk_vertices}
}

void object_wire_set::query_create_visual( )
{
	memory::writer	writer(g_allocator);
	writer.external_data	= true;

	pcstr material_name = "editor/wire";

	create_wire_visual_source( writer, 
								m_item->m_captured_points,
								m_item->m_captured_points_count,
								material_name, m_item->wire_width );


	query_result_delegate* q		= NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &object_wire_set::on_visual_loaded), g_allocator);

	const_buffer creation_buffer( writer.pointer(), writer.size() );
	resources::query_create_resource(
		unmanaged_string(get_full_name()).c_str(),
		creation_buffer,
		resources::user_mesh_class,
		boost::bind(&query_result_delegate::callback, q, _1),
		g_allocator
	);
}

wpf_controls::property_container^ object_wire_set::get_property_container( )
{
	property_container^ result = super::get_property_container();

	editor_base::object_properties::initialize_property_container	( m_item, result );

	control_container^ container	= result->add_dock_container( false );
	container->category				= "Test";
	button^ but = container->add_button( "Rebuild", gcnew System::Action<button^>( this, &object_wire_set::rebuild ) );
	but->width	= 60;
	but = container->add_button( "Capture", gcnew System::Action<button^>( this, &object_wire_set::capture ) );
	but->width	= 60;

	return result;
}

void object_wire_set::destroy_collision( )
{
	if(m_start_point_collision && m_end_point_collision)
	{
		collision::space_partitioning_tree* tree = get_level_editor()->get_collision_tree();

		tree->erase				( m_start_point_collision );
		tree->erase				( m_end_point_collision );
		DELETE					( m_start_point_collision );
		DELETE					( m_end_point_collision );
		m_start_point_collision = NULL;
		m_end_point_collision	= NULL;
	}
}

void object_wire_set::set_transform( float4x4 const& m )
{
	super::set_transform		( m );
	update_collision			( );
	update_bbox					( );
	if((*m_visual).c_ptr())
	{
		render::scene_ptr const& scene				= get_editor_world().scene();
		get_editor_renderer().scene().update_model	( scene, *m_visual, *m_transform );
	}
}

void object_wire_set::update_bbox( )
{
	float3 p1(*m_item->m_offset1);

	float3 mn(math::min(0.f, p1.x), math::min(0.f, p1.y), math::min(0.f, p1.z) );
	float3 mx(math::max(0.f, p1.x), math::max(0.f, p1.y), math::max(0.f, p1.z) );

	*m_aabbox = math::create_aabb_min_max(mn, mx).modify( *m_transform );
}

void object_wire_set::update_collision( )
{
	if(m_start_point_collision && m_end_point_collision)
	{
		collision::space_partitioning_tree* tree = get_level_editor()->get_collision_tree();

		tree->move				(	m_start_point_collision, create_translation(float3(0,0,0)) * *m_transform );
		tree->move				(	m_end_point_collision, create_translation(*m_item->m_offset1) * *m_transform );
	}
}

aabb object_wire_set::get_focus_aabb( )
{
	return math::create_identity_aabb().modify( create_translation(m_item->current_point()* *m_transform) );
}

}
}
