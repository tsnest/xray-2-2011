////////////////////////////////////////////////////////////////////////////
//	Created		: 24.03.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_patrol_graph.h"
#include "tool_misc.h"
#include "level_editor.h"
#include "window_ide.h"
#include "editor_world.h"
#include "le_transform_control_helper.h"
#include "object_inspector_tab.h"
#include "project_tab.h"
#include "tools_tab.h"
#include "project.h"
#include "project_items.h"
#include "input_actions.h"

#include "object_patrol_graph_complete_graph_action.h"
#include "object_patrol_graph_delete_part_action.h"
#include "object_patrol_graph_link_nodes_action.h"
#include "object_patrol_graph_change_edge_direction_action.h"
#include "object_patrol_graph_add_look_point_action.h"

#include "object_patrol_graph_part_collision.h"
#include "object_patrol_graph_node.h"
#include "object_patrol_graph_edge.h"
#include "object_patrol_graph_look_point.h"

#include <xray/editor/base/collision_object_types.h>
#include <xray/geometry_primitives.h>

#pragma managed( push, off )
#	include <xray/render/facade/debug_renderer.h>
#	include <xray/collision/space_partitioning_tree.h>
#	include <xray/collision/geometry.h>
#pragma managed( pop )

typedef xray::editor::object_patrol_graph_node				node;
typedef xray::editor::object_patrol_graph_edge				edge;
typedef xray::editor::object_patrol_graph_look_point		look_point;

typedef xray::editor::object_patrol_graph_link_nodes_action				link_nodes_action;
typedef xray::editor::object_patrol_graph_complete_graph_action			complete_graph_action;
typedef xray::editor::object_patrol_graph_delete_part_action			delete_part_action;
typedef xray::editor::object_patrol_graph_change_edge_direction_action	change_edge_direction_action;
typedef xray::editor::object_patrol_graph_add_look_point_action			add_look_point_action;

using	xray::editor_base::gui_binder;
using	xray::editor_base::action_delegate;
using	xray::editor_base::execute_delegate_managed;
using	xray::editor_base::enabled_delegate_managed;
using	xray::editor_base::checked_delegate_managed;

namespace xray{
namespace editor{

//////////////////////////////			I N I T I A L I Z E				////////////////////////////////////

void object_patrol_graph::initialize_in_tool ( tool_misc^ tool )
{
	level_editor^ le = tool->get_level_editor( );

	s_actions_layer				= gcnew actions_layer( 4 );
	s_selected_patrol_graphs	= gcnew List<object_patrol_graph^>( );

	gui_binder^ gui_binder				= le->get_gui_binder( );
	le->ide( )->create_tool_strip		( "patrol_graph_tool_bar", 0);
	
	String^ action_name					= "patrol graph toggle nodes adding";
	action_delegate^ action_d			= gcnew action_delegate( action_name, gcnew execute_delegate_managed( &object_patrol_graph::toggle_node_adding_mode ) );
	action_d->set_enabled				( gcnew enabled_delegate_managed( &object_patrol_graph::is_in_edit_mode ) );
	action_d->set_checked				( gcnew checked_delegate_managed( &object_patrol_graph::is_in_node_adding_mode ) );
	s_actions_layer->register_action	( action_d, "" );
	gui_binder->register_image			( "base", action_name, xray::editor_base::preview_cube );
	le->ide( )->add_action_button_item	( gui_binder, action_d, "patrol_graph_tool_bar", 10 );

	action^ action;

	action = gcnew delete_part_action	( "patrol graph remove selected part",				le->view_window( ) );
	s_actions_layer->register_action	( action, "Del(View)" );
	gui_binder->register_image			( "base", action->name( ), xray::editor_base::preview_cube );
	le->ide( )->add_action_button_item	( gui_binder, action, "patrol_graph_tool_bar", 10 );

	action = gcnew link_nodes_action	( "patrol graph link selected nodes",				le->view_window( ) );
	s_actions_layer->register_action	( action ,"LButton+A(View)" );
	gui_binder->register_image			( "base", action->name( ), xray::editor_base::preview_cube );
	le->ide( )->add_action_button_item	( gui_binder, action, "patrol_graph_tool_bar", 10 );

	action = gcnew complete_graph_action( "patrol graph add node",							le->view_window( ) );
	s_actions_layer->register_action	( action, "LButton+A(View)" );

	action = gcnew add_look_point_action( "patrol graph add look point",					le->view_window( ) );
	s_actions_layer->register_action	( action, "LButton+D(View)" );

	action = gcnew change_edge_direction_action( "patrol graph change edge direction",		le->view_window( ) );
	s_actions_layer->register_action	( action, "S(View)" );
	gui_binder->register_image			( "base", action->name( ), xray::editor_base::preview_cube );
	le->ide( )->add_action_button_item	( gui_binder, action, "patrol_graph_tool_bar", 10 );

	le->get_input_engine( )->add_actions_layer( s_actions_layer );
}

object_patrol_graph::object_patrol_graph	( tool_misc^ tool ) :
	super									( tool ),
	m_tool_misc								( tool )
{
	m_nodes					= gcnew List<node^>( );
	m_nodes_readonly		= gcnew ReadOnlyCollection<node^>( m_nodes );
}

object_patrol_graph::~object_patrol_graph	( )
{
	if( s_selected_patrol_graphs->Count == 1 && s_selected_patrol_graphs[0] == this )
		s_selected_patrol_graphs->Remove( this );

	for( s32 i = m_nodes->Count - 1; i >= 0; --i )
	{	
		node^ node = m_nodes[i];
		node->detach( );
		delete node;
	}

	m_nodes		= nullptr;
}

//////////////////////////////		 P U B L I C   M E T H O D S		////////////////////////////////////

void object_patrol_graph::load_contents ( )
{
	if( !m_collision->initialized( ) )
		init_collision( );
}

void object_patrol_graph::unload_contents ( bool bdestroy )
{
	XRAY_UNREFERENCED_PARAMETER	( bdestroy );
	destroy_collision			( );
}

void object_patrol_graph::load_props ( configs::lua_config_value const& t )
{
	super::load_props			( t );

	configs::lua_config_value graph_value	= t["graph"];

	configs::lua_config_value nodes_value	= graph_value["nodes"];
	
	u32 nodes_count = nodes_value.size( );
	for( u32 i = 0; i < nodes_count; ++i )
		m_nodes->Add				( gcnew node( this, nodes_value[i] ) );

	for( u32 i = 0; i < nodes_count; ++i )
		m_nodes[i]->load_edges		( nodes_value[i] );
}

void object_patrol_graph::save ( configs::lua_config_value t )
{
	super::save		( t );
	configs::lua_config_value graph_value		= t["graph"];

	configs::lua_config_value nodes_value		= graph_value["nodes"];
	
	u32 nodes_count = m_nodes->Count;
	for( u32 i = 0; i < nodes_count; ++i )
		m_nodes[i]->save( nodes_value[i] );
}



void object_patrol_graph::destroy_collision	( )
{
	if ( m_collision->initialized() )
		m_collision->destroy( &debug::g_mt_allocator );
}

aabb object_patrol_graph::get_aabb ( )
{
	if( m_selected_graph_part != nullptr )
		return m_selected_graph_part->get_aabb( );
	
	return super::get_aabb( );
}



void object_patrol_graph::set_transform ( float4x4 const& transform )
{
	float3 position_prev		= object_position;
	super::set_transform		( transform );

	for each( node^ node in m_nodes )
		node->fix_up_position( object_position - position_prev );

}

node^ object_patrol_graph::create_node ( float3 position )
{
	return create_node( position, m_nodes->Count );
}

node^ object_patrol_graph::create_node ( float3 position, u32 index )
{
	node^		new_node			= gcnew node( this, Float3( position ) );
	new_node->attach				( index );

	return new_node;
}

edge^ object_patrol_graph::create_edge ( node^ source_node, node^ destination_node )
{
	edge^ new_edge = gcnew edge( this );
	new_edge->attach( source_node, destination_node );
	return new_edge;
}

edge^ object_patrol_graph::create_edge ( node^ source_node, node^ destination_node, u32 index )
{
	edge^ new_edge = gcnew edge( this );
	new_edge->attach( source_node, index, destination_node );
	return new_edge;
}

edge^ object_patrol_graph::create_edge_to_node ( node^ to_node )
{
	R_ASSERT( dot_net::is<node^>( m_selected_graph_part ) );

	edge^ edge = create_edge( safe_cast<node^>( m_selected_graph_part ), to_node );

	to_node->select( );

	return edge;
}

void object_patrol_graph::render ( )
{
	super::render			( );

	get_debug_renderer( ).draw_origin	(  get_editor_world( ).scene( ), get_transform( ), 2.0f );

	math::float3	cylinder_size		= math::float3( 0.5f, 0.88f, 0.5f );
	math::float3	cone_size			= math::float3( 0.3f, 0.3f, 0.3f );

	math::color		color				( 0, 64, 196 );
	math::color		alpha_color			( 0, 64, 196, 128 );

	if( m_is_selected )
	{
		color			= math::color		( 0, 196, 64 );
		alpha_color		= math::color		( 0, 196, 64, 128 );
	}

	math::color		color_selected		( 255, 255, 0 );	
	math::color		alpha_color_selected( 255, 255, 0, 128 );
	math::color		white_color			( 255, 255, 255 );

	s32 count		= m_nodes->Count;

	//render nodes
	for( s32 i = 0; i < count; ++i )
	{
		node^		node				= m_nodes[i];
		bool		is_selected_node	= ( node == m_selected_graph_part );
		float3		node_pos			= node->position;
		float4x4	node_transform		= math::create_translation( node_pos );
		
		get_debug_renderer( ).draw_cylinder_solid	( get_editor_world( ).scene( ), node_transform, cylinder_size, is_selected_node ? alpha_color_selected : alpha_color );

		get_debug_renderer( ).draw_lines( 
			get_editor_world( ).scene( ),
			create_scale( float3( 0.5f, 0.5f, 0.5f ) ) * create_translation( node_pos + float3( 0, 0.88f, 0 ) ),
			geometry_utils::circle::vertices,
			geometry_utils::circle::vertex_count,
			geometry_utils::circle::pairs,
			geometry_utils::circle::pair_count,
			white_color
		);

		get_debug_renderer( ).draw_lines( 
			get_editor_world( ).scene( ),
			create_scale( float3( 0.5f, 0.5f, 0.5f ) ) * create_translation( node_pos + float3( 0, -0.88f, 0 ) ),
			geometry_utils::circle::vertices,
			geometry_utils::circle::vertex_count,
			geometry_utils::circle::pairs,
			geometry_utils::circle::pair_count,
			white_color
		);
	}

	//render edges
	if( count > 1 )
	{
		render::debug_vertices_type		vertices( g_allocator );
		render::debug_indices_type		indices( g_allocator );

		render::vertex_colored			vertex;
		vertex.color					= color;

		// Fill nodes
		for( s32 vi = 0 ; vi < count; ++vi )
		{
			node^ node					= m_nodes[vi];

			vertex.position				= node->position;
			vertices.push_back			( vertex );

			// Fill links
			int links_count = node->outgoing_edges->Count;
			for( s32 i = 0; i < links_count; ++i )
			{
				edge^		edge			= node->outgoing_edges[i];

				float3		source_pos		= edge->source_node->position;
				float3		dest_pos		= edge->destination_node->position;
				float3		direction		= ( dest_pos - source_pos ).normalize( );
				float3		up				= ( direction ^ float3( 0, 1, 0 ) ) ^ direction;
				float4x4	cone_transform	=
					math::create_rotation_x( -math::pi_d2 ) *
					math::create_rotation( direction, up ) *
					math::create_translation( dest_pos - direction * ( cylinder_size.x + cone_size.y ) );

				if( edge == m_selected_graph_part )
				{
					get_debug_renderer( ).draw_line			( get_editor_world( ).scene( ), source_pos, dest_pos, color_selected );
					get_debug_renderer( ).draw_cone_solid	( get_editor_world( ).scene( ), cone_transform, cone_size, alpha_color_selected );
				}
				else
				{
					indices.push_back			( (u16)edge->source_node->index );
					indices.push_back			( (u16)edge->destination_node->index );

					get_debug_renderer( ).draw_cone_solid	( get_editor_world( ).scene( ), cone_transform, cone_size, alpha_color );
				}

				float4x4 transform = cone_transform;
				transform.c.xyz( ) -= transform.j.xyz( ) * cone_size.y;
				get_debug_renderer( ).draw_circle	( get_editor_world( ).scene( ), transform, cone_size, white_color );
			}
		}

		for( s32 count = ( ( vertices.size( ) - indices.size( ) ) / 2 + 1 ) * 2; count > 0; --count )
			indices.push_back			( 0 );

		get_debug_renderer( ).draw_lines( get_editor_world( ).scene( ), vertices, indices );
	}

	//render look points
	{
		render::debug_vertices_type		vertices( g_allocator );
		render::debug_indices_type		indices( g_allocator );

		render::vertex_colored			vertex;
		vertex.color					= color;

		float3 camera_position	= owner_tool( )->get_level_editor( )->view_window( )->get_inverted_view_matrix( ).c.xyz( );

		for( s32 vi = 0 ; vi < count; ++vi )
		{
			node^ node		= m_nodes[vi];

			u32 count		= node->look_points->Count;
			if( count == 0 )
				continue;

			vertex.position				= node->position;
			vertices.push_back			( vertex );
			u32 node_vertex_index		= vertices.size( ) - 1;
			
			s32 look_points_count		= node->look_points->Count;
			for( s32 i = 0; i < look_points_count; ++i )
			{
				look_point^	look_point		= node->look_points[i];

				vertex.position				= look_point->position;
				vertices.push_back			( vertex );
				indices.push_back			( (u16)node_vertex_index );
				indices.push_back			( (u16)( vertices.size( ) - 1 ) );

				float3 direction	= ( camera_position - vertex.position ).normalize( );
				float3 up			= ( ( direction ^ float3( 0, 1, 0 ) ) ^ direction ).normalize( );
				float3 right		= direction ^ up;
				float4x4 transform;
				transform.identity( );
				transform.i.xyz( ) = right;
				transform.j.xyz( ) = direction;
				transform.k.xyz( ) = up;
				transform.c.xyz( ) = vertex.position;

				get_debug_renderer( ).draw_sphere_solid	( get_editor_world( ).scene( ), vertex.position, 0.5f, ( look_point == m_selected_graph_part ) ? alpha_color_selected : alpha_color );
				get_debug_renderer( ).draw_circle( get_editor_world( ).scene( ), transform, float3( 0.5f, 0.5f, 0.5f ), white_color );
				get_debug_renderer( ).draw_circle( get_editor_world( ).scene( ), create_translation( vertex.position ), float3( 0.5f, 0.5f, 0.5f ), white_color );
			}
		}

		get_debug_renderer( ).draw_lines( get_editor_world( ).scene( ), vertices, indices );
	}
}

property_container^	object_patrol_graph::get_property_container ( )
{
	wpf_controls::property_container^ cont = super::get_property_container( );
	m_temp_container		= cont;

	cont->properties->remove( "general/scale" );

	return					cont;
}

void object_patrol_graph::on_selected ( bool bselected )
{
	m_is_selected = bselected;

	if( bselected )
	{
		select_patrol_graph		( );
	}
	else
	{
		deselect_patrol_graph	( );

		m_selected_graph_part			= nullptr;
		m_temp_container				= nullptr;
	}
}

List<String^>^ object_patrol_graph::get_signals_list ( )
{
	List<String^>^ ret_lst = gcnew List<String^>( );
	for each( node^ n in m_nodes )
	{
		if ( n->signal != nullptr && n->signal != "" && !ret_lst->Contains( n->signal ) )
			ret_lst->Add( n->signal );		
	}
	return ret_lst;
}

///////////////////////////		   I N T E R N A L   M E T H O D S			////////////////////////////////

void object_patrol_graph::add_node ( node^ node )
{
	m_nodes->Add	( node );
}

void object_patrol_graph::insert_node ( u32 index, node^ node )
{
	m_nodes->Insert	( index, node );
}

void object_patrol_graph::remove_node ( node^ node )
{
	m_nodes->Remove	( node );
}

node^ object_patrol_graph::get_node_at ( u32 index )
{
	R_ASSERT( index < (u32)m_nodes->Count );

	return m_nodes[index];
}

void object_patrol_graph::select_graph_part ( object_patrol_graph_part^ part )
{
	if( !m_is_selected )
		owner_tool( )->get_level_editor( )->get_project( )->select_object( owner_project_item, enum_selection_method_set );

	m_selected_graph_part = part;
	m_owner_tool->get_level_editor( )->get_object_inspector_tab( )->show_properties( part );
}

void object_patrol_graph::insert_graph_in_collision ( )
{
	for each( node^ node in m_nodes )
	{
		m_collision->m_collision_tree->insert( node->get_collision( ), node->get_matrix( ) );

		for each( edge^ edge in node->outgoing_edges )
			m_collision->m_collision_tree->insert( edge->get_collision( ), edge->get_matrix( ) );

		for each( look_point^ look_point in node->look_points )
			m_collision->m_collision_tree->insert( look_point->get_collision( ), look_point->get_matrix( ) );
	}
}

void object_patrol_graph::remove_graph_from_collision ( )
{
	for each( node^ node in m_nodes )
	{
		m_collision->m_collision_tree->erase( node->get_collision( ) );

		for each( edge^ edge in node->outgoing_edges )
			m_collision->m_collision_tree->erase( edge->get_collision( ) );

		for each( look_point^ look_point in node->look_points )
			m_collision->m_collision_tree->erase( look_point->get_collision( ) );
	}
}

void object_patrol_graph::select_patrol_graph (  )
{
	s_selected_patrol_graphs->Add		( this );
	toggle_edit_mode					( this );
}

void object_patrol_graph::deselect_patrol_graph ( )
{
	s_selected_patrol_graphs->Remove	( this );
	toggle_edit_mode					( this );
}

void object_patrol_graph::toggle_edit_mode ( object_patrol_graph^ graph )
{
	if( s_selected_patrol_graphs->Count == 1 )
	{
		graph->owner_tool( )->get_level_editor( )->get_input_engine( )->activate_actions_layer		( s_actions_layer );
	}
	else
	{
		graph->owner_tool( )->get_level_editor( )->get_input_engine( )->deactivate_actions_layer	( s_actions_layer );
	}
}

bool object_patrol_graph::is_in_edit_mode ( )
{
	return s_selected_patrol_graphs->Count == 1;
}

void object_patrol_graph::toggle_node_adding_mode ( )
{
	s_is_in_node_adding_mode = !s_is_in_node_adding_mode;

	if( s_is_in_node_adding_mode )
	{
		s_actions_layer->add_action_shortcut	( "patrol graph add node", "LButton(View)" );
	}
	else
	{
		s_actions_layer->remove_action_shortcut	( "patrol graph add node", "LButton(View)" );
	}
}

bool object_patrol_graph::is_in_node_adding_mode ( )
{
	return s_is_in_node_adding_mode;
}

//////////////////////////////		P R I V A T E   M E T H O D S		////////////////////////////////////

void object_patrol_graph::init_collision ( )
{
	ASSERT						( !m_collision->initialized( ) );
	float3 extents				( 0.3f,0.3f,0.3f );

	collision::geometry_instance* geom	= &*collision::new_box_geometry_instance( &debug::g_mt_allocator, math::create_scale( extents ) );
	m_collision->create_from_geometry( true, this, geom, editor_base::collision_object_type_dynamic | editor_base::collision_object_type_touch );
	m_collision->insert			( m_transform );
}

void object_patrol_graph::run_command ( editor_base::command^ command )
{
	m_owner_tool->get_level_editor( )->get_command_engine( )->run( command );
}


} // namespace editor
} // namespace xray
