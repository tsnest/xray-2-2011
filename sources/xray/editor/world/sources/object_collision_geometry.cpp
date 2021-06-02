////////////////////////////////////////////////////////////////////////////
//	Created		: 15.08.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_collision_geometry.h"
#include "object_collision_geometry_mesh.h"
#include "tool_misc.h"
#include "editor_world.h"
#include "level_editor.h"
#include "project.h"
#include "object_collision_geometry_add_remove_mesh_commands.h"
#include <xray/editor/base/collision_object_types.h>

#pragma managed( push, off )
#	include <xray/geometry_utils.h>
#	include <xray/geometry_primitives.h>
#	include <xray/render/facade/debug_renderer.h>
#	include <xray/collision/space_partitioning_tree.h>
#	include <xray/collision/geometry.h>
#	include <xray/collision/collision_object.h>
#	include <xray/render/engine/base_classes.h>
#	include <xray/render/facade/scene_renderer.h>
#	include <xray/render/facade/editor_renderer.h>
#pragma managed( pop )

using namespace System;
using xray::editor::wpf_controls::control_containers::control_container;
using xray::editor::wpf_controls::property_descriptor;
using xray::editor::wpf_controls::property_container;
using xray::editor_base::transform_control_translation;
using xray::editor_base::transform_control_rotation;

namespace xray{
namespace editor{

////////////////			I N I T I A L I Z E				////////////////

object_collision_geometry::object_collision_geometry ( tool_misc^ tool ):
	super					( tool ),
	m_collision_meshes		( gcnew List<object_collision_geometry_mesh^>( ) ),
	m_collision_anti_meshes	( gcnew List<object_collision_geometry_mesh^>( ) )
{
	image_index				= xray::editor_base::misc_tree_icon;
}
object_collision_geometry::~object_collision_geometry ( )
{
	for each ( object_collision_geometry_mesh^ mesh in m_collision_anti_meshes )
		delete mesh;

	for each ( object_collision_geometry_mesh^ mesh in m_collision_meshes )
		delete mesh;
}

////////////////		P U B L I C   M E T H O D S			////////////////

void object_collision_geometry::load_contents ( )
{
	if( !m_collision->initialized( ) )
		init_collision( );
}
void object_collision_geometry::unload_contents ( Boolean is_destroy )
{
	XRAY_UNREFERENCED_PARAMETER( is_destroy );

	if( m_collision->initialized( ) )
		m_collision->destroy( &debug::g_mt_allocator );
}

void object_collision_geometry::load_props ( configs::lua_config_value const& config_value )
{
	super::load_props			( config_value );

	mode									= (object_collision_geometry_mode)(u32)config_value["mode"];

	configs::lua_config_value meshes		= config_value["meshes"];
	
	u32 count = meshes.size( );
	for( u32 i = 0; i < count; ++i )
	{
		object_collision_geometry_mesh^ mesh		= gcnew object_collision_geometry_mesh( this );

		mesh->load						( meshes[i] );
		m_collision_meshes->Add			( mesh );
		mesh->activate					( true );
	}

	configs::lua_config_value anti_meshes	= config_value["anti_meshes"];
	
	count = anti_meshes.size( );
	for( u32 i = 0; i < count; ++i )
	{
		object_collision_geometry_mesh^ mesh		= gcnew object_collision_geometry_mesh( this, true );

		mesh->load						( anti_meshes[i] );
		m_collision_anti_meshes->Add	( mesh );
		mesh->activate					( true );
	}
}

void object_collision_geometry::save ( configs::lua_config_value config_value )
{
	super::save				( config_value );

	config_value["mode"]	= (u32)mode;

	config_value["game_object_type"]	= "collision_geometry";

	configs::lua_config_value meshes	= config_value["meshes"];
	u32 count = m_collision_meshes->Count;
	for( u32 i = 0; i < count; ++i )
	{
		configs::lua_config_value mesh = meshes[i];
		m_collision_meshes[i]->save( mesh );
	}

	configs::lua_config_value anti_meshes	= config_value["anti_meshes"];
	count = m_collision_anti_meshes->Count;
	for( u32 i = 0; i < count; ++i )
	{
		configs::lua_config_value mesh = anti_meshes[i];
		m_collision_anti_meshes[i]->save( mesh );
	}
}

void object_collision_geometry::render ( )
{
	super::render							( );
	render::scene_ptr scene					= get_editor_world( ).get_level_editor( )->scene( );

	//octahedron
	{
		geometry_utils::geom_vertices_type	vertices(g_allocator);
		geometry_utils::geom_indices_type	indices(g_allocator);
		
		geometry_utils::create_octahedron	( vertices, indices, create_translation( get_position( ) ), float3( .5f, .5f, .5f ) );
		u32 vcount 							= vertices.size( );
		u32 icount 							= indices.size( );
		xray::render::debug_vertices_type	draw_vertices( g_allocator );
		draw_vertices.resize				( vcount );
		xray::render::debug_indices_type	draw_indices( g_allocator );
		draw_indices.resize					( icount );
		
		math::color color( 100, 100, 200, 200 );

		for( u32 i = 0; i < vcount; ++i )
		{
			xray::render::vertex_colored& v = draw_vertices[i];
			v.position						= vertices[i];
			v.color							= color;
		}

		for(u32 i=0; i<icount; ++i)
		{
			draw_indices[i] = indices[i];
		}

		get_debug_renderer().draw_triangles	( scene, draw_vertices, draw_indices );

		math::color clr_line ( 80, 80, 160, 200 );
		for(u32 i=0; i<vcount; ++i)
		{
			xray::render::vertex_colored& v = draw_vertices[i];
			v.color							= clr_line;
		}


		icount 							= geometry_utils::octahedron_solid::pair_count*2;
		draw_indices.resize				( icount );

		for(u32 i=0; i<icount; ++i)
		{
			draw_indices[i] = geometry_utils::octahedron_solid::pairs[i];
		}

		get_debug_renderer().draw_lines		( scene, draw_vertices, draw_indices );
	}

	u32 count = m_collision_anti_meshes->Count;
	for( u32 i = 0; i < count; ++i )
		m_collision_anti_meshes[i]->render( scene, get_debug_renderer( ) );

	count = m_collision_meshes->Count;
	for( u32 i = 0; i < count; ++i )
	{
		m_collision_meshes[i]->render( scene, get_debug_renderer( ) );
	}
}

property_container^	object_collision_geometry::get_property_container	( )
{
	property_container^	ret_container	= super::get_property_container( );



	//collision meshes
	control_container^ container		= ret_container->add_uniform_container( );
	container->category			= "collision meshes";

	container->add_button		( "sphere", gcnew Action<button^>( this, &object_collision_geometry::add_sphere_mesh_clicked ) );
	container->add_button		( "box", gcnew Action<button^>( this, &object_collision_geometry::add_box_mesh_clicked ) );
	//container->add_button		( "cyliner", gcnew Action<button^>( this, &object_collision_geometry::add_cylinder_mesh_clicked ) );
	container->add_button		( "capsule", gcnew Action<button^>( this, &object_collision_geometry::add_capsule_mesh_clicked ) );
	container->add_button		( "truncated sphere", gcnew Action<button^>( this, &object_collision_geometry::add_truncated_sphere_mesh_clicked ) );

	u32 i=0;
	for each ( object_collision_geometry_mesh^ mesh in m_collision_meshes )
	{
		property_container^	sub				= mesh->get_property_container( );
		
		String^ name						= String::Format( "{0}", ++i );
		property_descriptor^ desc			= ret_container->properties->add_container	( name, "collision meshes", "no description", sub );
		mesh->last_property_descriptor		= desc;
		sub->inner_value					=	( gcnew property_descriptor( name, mesh, "type" ) )->set_compo_box_style(
													gcnew cli::array<String^>( 5 ){ "sphere", "box", "", /*"cylinder",*/ "capsule", "truncated sphere" }
												);

		desc->add_button( " x ", "", gcnew Action<button^>( this, &object_collision_geometry::remove_mesh_clicked ) )->tag = mesh;
			
		desc->selection_changed		+= gcnew Action<Boolean>( mesh, &object_collision_geometry_mesh::set_selected );
	}

	//collision anti meshes
	container					= ret_container->add_uniform_container( );
	container->category			= "collision anti meshes";

	container->add_button		( "sphere", gcnew Action<button^>( this, &object_collision_geometry::add_sphere_antimesh_clicked ) );
	container->add_button		( "box", gcnew Action<button^>( this, &object_collision_geometry::add_box_antimesh_clicked ) );
	//container->add_button		( "cyliner", gcnew Action<button^>( this, &object_collision_geometry::add_cylinder_antimesh_clicked ) );
	container->add_button		( "capsule", gcnew Action<button^>( this, &object_collision_geometry::add_capsule_antimesh_clicked ) );
	container->add_button		( "truncated sphere", gcnew Action<button^>( this, &object_collision_geometry::add_truncated_sphere_antimesh_clicked) );

	i=0;
	for each ( object_collision_geometry_mesh^ mesh in m_collision_anti_meshes )
	{
		property_container^	sub		= gcnew property_container;
		editor_base::object_properties::initialize_property_container( mesh, sub );
		
		String^ name						= String::Format( "{0}", ++i );
		property_descriptor^ desc			= ret_container->properties->add_container	( name, "collision anti meshes", "no description", sub );
		mesh->last_property_descriptor		= desc;
		sub->inner_value					=	( gcnew property_descriptor( name, mesh, "type" ) )->set_compo_box_style(
													gcnew cli::array<String^>( 5 ){ "sphere", "box", "", /*"cylinder",*/"capsule", "truncated sphere" }
												);

		desc->add_button			( " x ", "", gcnew Action<button^>( this, &object_collision_geometry::remove_antimesh_clicked ) )->tag = mesh;
		desc->selection_changed		+= gcnew Action<Boolean>( mesh, &object_collision_geometry_mesh::set_selected );
	}

	return ret_container;
}

void object_collision_geometry::set_transform ( float4x4 const& transform )
{
	super::set_transform( transform );

	u32 count = m_collision_meshes->Count;
	for ( u32 i = 0; i < count; ++i )
		m_collision_meshes[i]->update_collision( );
}

aabb object_collision_geometry::get_aabb ( )
{
	if( m_selected_mesh == nullptr )
		return super::get_aabb( );

	return m_selected_mesh->get_aabb( );
}

void object_collision_geometry::on_selected ( bool bselected )
{
	m_is_selected	= bselected;
	m_selected_mesh	= nullptr;

	if( !bselected )
	{
		for each ( object_collision_geometry_mesh^ mesh in m_collision_anti_meshes )
			mesh->last_property_descriptor = nullptr;

		for each ( object_collision_geometry_mesh^ mesh in m_collision_meshes )
			mesh->last_property_descriptor = nullptr;
	}
}

bool object_collision_geometry::attach_transform ( transform_control_base^ transform )
{
	if( xray::editor::wpf_controls::dot_net_helpers::is_type<transform_control_translation^>( transform ) ||
		xray::editor::wpf_controls::dot_net_helpers::is_type<transform_control_rotation^>( transform ) )
		return true;

	return false;
}
////////////////		I N T E R N A L   M E T H O D S			////////////////

object_collision_geometry_mesh^	object_collision_geometry::get_mesh ( Boolean is_anti_mesh, Int32 index )
{
	if( is_anti_mesh )
		return m_collision_anti_meshes[index];
	else
		return m_collision_meshes[index];
}
Int32 object_collision_geometry::get_mesh_index ( object_collision_geometry_mesh^ mesh, Boolean is_anti_mesh )
{
	if( is_anti_mesh )
		return m_collision_anti_meshes->IndexOf( mesh );
	else
		return m_collision_meshes->IndexOf( mesh );
}
space_partitioning_tree* object_collision_geometry::get_collision_tree ( )
{
	return owner_tool( )->get_level_editor( )->get_collision_tree( );
}

void object_collision_geometry::set_modified ( )
{
	
}

void object_collision_geometry::select_mesh ( object_collision_geometry_mesh^ obj )
{
	if( !m_is_selected )
		owner_tool( )->get_level_editor( )->get_project( )->select_object( owner_project_item, enum_selection_method_set );

	if( m_selected_mesh == obj )
		return;

	if( m_selected_mesh != nullptr )
		m_selected_mesh->last_property_descriptor->is_selected = false;

	owner_tool( )->get_level_editor( )->set_transform_object( obj );
	
	m_selected_mesh = obj;
}

Int32 object_collision_geometry::add_collision_mesh ( collision::primitive_type type )
{
	u32 index = m_collision_meshes->Count;
	create_mesh( type, m_collision_meshes, index );
	return index;
}

void object_collision_geometry::add_collision_mesh	( collision::primitive_type type, Float3% position, Float3% rotation, Float3% scale, Int32 index )
{
	object_collision_geometry_mesh^ mesh = create_mesh( type, m_collision_meshes, index );
	mesh->position	= position;
	mesh->rotation	= rotation;
	mesh->scale		= scale;
}

void object_collision_geometry::remove_collision_mesh ( Int32 index )
{
	object_collision_geometry_mesh^ mesh = m_collision_meshes[index];
	remove_mesh( mesh, m_collision_meshes );
}
void object_collision_geometry::remove_collision_mesh ( Int32 index, Float3% position, Float3% rotation, Float3% scale )
{
	object_collision_geometry_mesh^ mesh = m_collision_meshes[index];
	position	= mesh->position; 
	rotation	= mesh->rotation;
	scale		= mesh->scale;
	remove_mesh( mesh, m_collision_meshes );
}

Int32 object_collision_geometry::add_anti_mesh ( collision::primitive_type type )
{
	u32 index = m_collision_anti_meshes->Count;
	create_mesh( type, m_collision_anti_meshes );
	return index;
}

void object_collision_geometry::add_anti_mesh ( collision::primitive_type type, Float3% position, Float3% rotation, Float3% scale, Int32 index )
{
	object_collision_geometry_mesh^ mesh = create_mesh( type, m_collision_anti_meshes, index );
	mesh->position	= position;
	mesh->rotation	= rotation;
	mesh->scale		= scale;
}

void object_collision_geometry::remove_anti_mesh ( Int32 index )
{
	object_collision_geometry_mesh^ mesh = m_collision_anti_meshes[index];
	remove_mesh( mesh, m_collision_anti_meshes );
}
void object_collision_geometry::remove_anti_mesh ( Int32 index, Float3% position, Float3% rotation, Float3% scale )
{
	object_collision_geometry_mesh^ mesh = m_collision_anti_meshes[index];
	position	= mesh->position; 
	rotation	= mesh->rotation;
	scale		= mesh->scale;
	remove_mesh( mesh, m_collision_anti_meshes );
}

////////////////		P R I V A T E   M E T H O D S			////////////////

void object_collision_geometry::init_collision ( )
{
	ASSERT( !m_collision->initialized( ) );

	collision::geometry_instance* geom			= &*collision::new_sphere_geometry_instance( &debug::g_mt_allocator, float4x4().identity(), 0.5f );
	m_collision->create_from_geometry	( true, this, geom, xray::editor_base::collision_object_type_dynamic | xray::editor_base::collision_object_type_touch );
	m_collision->insert					( &get_transform( ) );
//	update_collision_transform			( );
}

void object_collision_geometry::update_collision ( )
{
	m_collision->destroy( &debug::g_mt_allocator );
	init_collision( );
}

object_collision_geometry_mesh^ object_collision_geometry::create_mesh ( collision::primitive_type type, List<object_collision_geometry_mesh^>^ to )
{
	return create_mesh( type, to, to->Count );
}

object_collision_geometry_mesh^ object_collision_geometry::create_mesh ( collision::primitive_type type, List<object_collision_geometry_mesh^>^ to, Int32 index )
{
	object_collision_geometry_mesh^ mesh = gcnew object_collision_geometry_mesh( this, to == m_collision_anti_meshes );
	mesh->type						= (int)type;
	mesh->position					= Float3(float3(0,0,0));
	mesh->rotation					= Float3(float3(0,0,0));
	mesh->scale						= Float3(float3(1,1,1));
	to->Insert						( index, mesh );
	set_modified					( );
	get_project( )->select_object	( m_project_item, enum_selection_method_set );
	mesh->activate					( true );
	mesh->set_selected				( true );
	return mesh;
}

void object_collision_geometry::remove_mesh ( object_collision_geometry_mesh^ mesh, List<object_collision_geometry_mesh^>^ from )
{
	from->IndexOf( mesh );
	from->Remove					( mesh );
	mesh->set_selected				( false );
	delete							mesh;
	set_modified					( );
	get_project( )->select_object	( m_project_item, enum_selection_method_set );
}

void object_collision_geometry::remove_mesh ( List<object_collision_geometry_mesh^>^ from, Int32 index )
{
	object_collision_geometry_mesh^ mesh = from[index];
	from->RemoveAt					( index );
	mesh->set_selected				( false );
	delete							mesh;
	set_modified					( );
	get_project( )->select_object	( m_project_item, enum_selection_method_set );
}

void object_collision_geometry::add_sphere_mesh_clicked( button^ )
{
	owner_tool( )->get_level_editor( )->get_command_engine( )->run(
		gcnew object_collision_geometry_add_mesh_command( this, collision::primitive_sphere, false )
	);
}

void object_collision_geometry::add_box_mesh_clicked ( button^ )
{
	owner_tool( )->get_level_editor( )->get_command_engine( )->run(
		gcnew object_collision_geometry_add_mesh_command( this, collision::primitive_box, false )
	);
}

void object_collision_geometry::add_cylinder_mesh_clicked ( button^ )
{
	owner_tool( )->get_level_editor( )->get_command_engine( )->run(
		gcnew object_collision_geometry_add_mesh_command( this, collision::primitive_cylinder, false )
	);
}

void object_collision_geometry::add_capsule_mesh_clicked ( button^ )
{
	owner_tool( )->get_level_editor( )->get_command_engine( )->run(
		gcnew object_collision_geometry_add_mesh_command( this, collision::primitive_capsule, false )
	);
}

void object_collision_geometry::add_truncated_sphere_mesh_clicked ( button^ )
{
	owner_tool( )->get_level_editor( )->get_command_engine( )->run(
		gcnew object_collision_geometry_add_mesh_command( this, collision::primitive_truncated_sphere, false )
	);
}

void object_collision_geometry::remove_mesh_clicked ( button^ button )
{
	object_collision_geometry_mesh^ mesh = safe_cast<object_collision_geometry_mesh^>( button->tag );
	owner_tool( )->get_level_editor( )->get_command_engine( )->run(
		gcnew object_collision_geometry_remove_mesh_command( this, (collision::primitive_type) mesh->type, false, m_collision_meshes->IndexOf( mesh ) )
	);
}

void object_collision_geometry::add_sphere_antimesh_clicked ( button^ )
{
	owner_tool( )->get_level_editor( )->get_command_engine( )->run(
		gcnew object_collision_geometry_add_mesh_command( this, collision::primitive_sphere, true )
	);
}

void object_collision_geometry::add_box_antimesh_clicked ( button^ )
{
	owner_tool( )->get_level_editor( )->get_command_engine( )->run(
		gcnew object_collision_geometry_add_mesh_command( this, collision::primitive_box, true )
	);
}

void object_collision_geometry::add_cylinder_antimesh_clicked ( button^ )
{
	owner_tool( )->get_level_editor( )->get_command_engine( )->run(
		gcnew object_collision_geometry_add_mesh_command( this, collision::primitive_cylinder, true )
	);
}

void object_collision_geometry::add_capsule_antimesh_clicked ( button^ )
{
	owner_tool( )->get_level_editor( )->get_command_engine( )->run(
		gcnew object_collision_geometry_add_mesh_command( this, collision::primitive_capsule, true )
	);
}

void object_collision_geometry::add_truncated_sphere_antimesh_clicked ( button^ )
{
	owner_tool( )->get_level_editor( )->get_command_engine( )->run(
		gcnew object_collision_geometry_add_mesh_command( this, collision::primitive_truncated_sphere, true )
	);
}

void object_collision_geometry::remove_antimesh_clicked ( button^ button )
{
	object_collision_geometry_mesh^ mesh = safe_cast<object_collision_geometry_mesh^>( button->tag );
	owner_tool( )->get_level_editor( )->get_command_engine( )->run(
		gcnew object_collision_geometry_remove_mesh_command( this, (collision::primitive_type) mesh->type, true, m_collision_anti_meshes->IndexOf( mesh ) )
	);
}

} // namespace editor
} // namespace xray
