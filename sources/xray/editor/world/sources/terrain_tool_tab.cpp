#include "pch.h"
#include "terrain_tool_tab.h"
#include "tool_terrain.h"
#include "window_ide.h"
#include "level_editor.h"
#include "project.h"
#include "editor_cell_manager.h"
#include "object_solid_visual.h"
#include "terrain_object.h"
#include "terrain_modifier_bump.h"
#include "terrain_modifier_raise_lower.h"
#include "terrain_modifier_flatten.h"
#include "terrain_modifier_smooth.h"
#include "terrain_modifier_painter.h"
#include "terrain_modifier_uv_relax.h"
#include "terrain_selection_control.h"
#include "terrain_painter_control_tab.h"
#include "terrain_core_form.h"
#include "editor_world.h"

#pragma managed( push, off )
#	include <xray/ai_navigation/sources/graph_generator.h>
#	include <xray/ai_navigation/world.h>
#	include <xray/geometry_utils.h>
#pragma managed( pop )

using System::Windows::Forms::ToolStripButton;
using xray::editor_base::activate_control_event;
using xray::editor_base::deactivate_control_event;
using xray::editor_base::property_changed;

namespace xray {
namespace editor {

void terrain_tool_tab::in_constructor()
{
	m_tool->m_terrain_modifier_bump->subscribe_on_activated				(gcnew activate_control_event(this, &terrain_tool_tab::on_control_activated));
	m_tool->m_terrain_modifier_bump->subscribe_on_deactivated			(gcnew deactivate_control_event(this, &terrain_tool_tab::on_control_deactivated));
	m_tool->m_terrain_modifier_bump->subscribe_on_property_changed		(gcnew property_changed(this, &terrain_tool_tab::on_control_property_changed));

	m_tool->m_terrain_modifier_raise_lower->subscribe_on_activated		(gcnew activate_control_event(this, &terrain_tool_tab::on_control_activated));
	m_tool->m_terrain_modifier_raise_lower->subscribe_on_deactivated	(gcnew deactivate_control_event(this, &terrain_tool_tab::on_control_deactivated));
	m_tool->m_terrain_modifier_raise_lower->subscribe_on_property_changed(gcnew property_changed(this, &terrain_tool_tab::on_control_property_changed));

	m_tool->m_terrain_modifier_flatten->subscribe_on_activated			(gcnew activate_control_event(this, &terrain_tool_tab::on_control_activated));
	m_tool->m_terrain_modifier_flatten->subscribe_on_deactivated		(gcnew deactivate_control_event(this, &terrain_tool_tab::on_control_deactivated));
	m_tool->m_terrain_modifier_flatten->subscribe_on_property_changed	(gcnew property_changed(this, &terrain_tool_tab::on_control_property_changed));

	m_tool->m_terrain_modifier_smooth->subscribe_on_activated			(gcnew activate_control_event(this, &terrain_tool_tab::on_control_activated));
	m_tool->m_terrain_modifier_smooth->subscribe_on_deactivated			(gcnew deactivate_control_event(this, &terrain_tool_tab::on_control_deactivated));
	m_tool->m_terrain_modifier_smooth->subscribe_on_property_changed	(gcnew property_changed(this, &terrain_tool_tab::on_control_property_changed));

	m_tool->m_terrain_modifier_detail_painter->subscribe_on_activated	(gcnew activate_control_event(this, &terrain_tool_tab::on_control_activated));
	m_tool->m_terrain_modifier_detail_painter->subscribe_on_deactivated	(gcnew deactivate_control_event(this, &terrain_tool_tab::on_control_deactivated));
	m_tool->m_terrain_modifier_detail_painter->subscribe_on_property_changed(gcnew property_changed(this, &terrain_tool_tab::on_control_property_changed));

	m_tool->m_terrain_modifier_uv_relax->subscribe_on_activated			(gcnew activate_control_event(this, &terrain_tool_tab::on_control_activated));
	m_tool->m_terrain_modifier_uv_relax->subscribe_on_deactivated		(gcnew deactivate_control_event(this, &terrain_tool_tab::on_control_deactivated));
	m_tool->m_terrain_modifier_uv_relax->subscribe_on_property_changed	(gcnew property_changed(this, &terrain_tool_tab::on_control_property_changed));

	m_tool->m_terrain_modifier_diffuse_painter->subscribe_on_activated		(gcnew activate_control_event(this, &terrain_tool_tab::on_control_activated));
	m_tool->m_terrain_modifier_diffuse_painter->subscribe_on_deactivated	(gcnew deactivate_control_event(this, &terrain_tool_tab::on_control_deactivated));
	m_tool->m_terrain_modifier_diffuse_painter->subscribe_on_property_changed(gcnew property_changed(this, &terrain_tool_tab::on_control_property_changed));

	m_tool->m_terrain_export_control->subscribe_on_activated			(gcnew activate_control_event(this, &terrain_tool_tab::on_control_activated));
	m_tool->m_terrain_export_control->subscribe_on_deactivated			(gcnew deactivate_control_event(this, &terrain_tool_tab::on_control_deactivated));
	m_tool->m_terrain_export_control->subscribe_on_property_changed		(gcnew property_changed(this, &terrain_tool_tab::on_control_property_changed));

	m_terrain_core_form								= nullptr;
	System::Windows::Forms::UserControl^ form		= nullptr;

	form									= gcnew xray::editor::terrain_painter_control_tab(m_tool);
	control_parameters_panel->Controls->Add	(form);
	form->Hide								();

	m_property_grid_host		= gcnew property_grid::host( );
//	m_property_grid_host->refresh_property_changed	+= gcnew System::EventHandler( this, &object_inspector_tab::on_refresh_value_changed );
	m_property_grid_host->Width	= 100;
	m_property_grid_host->Height= 100;
	m_property_grid_host->Dock	= System::Windows::Forms::DockStyle::Fill;

	active_control_property_grid->Controls->Add			( m_property_grid_host );

	ToolStrip^ strip				= top_toolStrip;
	editor_base::gui_binder^ binder	= m_tool->get_level_editor()->get_gui_binder();

	ToolStripButton^ button		= nullptr;
	System::String^ action_name	= nullptr;

	action_name					= "select terrain bump modifier";
	button						= gcnew ToolStripButton;
	strip->Items->Add			( button);
	binder->bind				( button, action_name );
	button->Image				= binder->get_image( action_name );
	button->DisplayStyle		= System::Windows::Forms::ToolStripItemDisplayStyle::Image;
	button->ImageScaling		= System::Windows::Forms::ToolStripItemImageScaling::SizeToFit;

	action_name					= "select terrain raise-lower modifier";
	button						= gcnew ToolStripButton;
	strip->Items->Add			( button);
	binder->bind				( button, action_name );
	button->Image				= binder->get_image( action_name );
	button->DisplayStyle		= System::Windows::Forms::ToolStripItemDisplayStyle::Image;
	button->ImageScaling		= System::Windows::Forms::ToolStripItemImageScaling::SizeToFit;

	action_name					= "select terrain flatten modifier";
	button						= gcnew ToolStripButton;
	strip->Items->Add			( button);
	binder->bind				( button, action_name );
	button->Image				= binder->get_image( action_name );
	button->DisplayStyle		= System::Windows::Forms::ToolStripItemDisplayStyle::Image;
	button->ImageScaling		= System::Windows::Forms::ToolStripItemImageScaling::SizeToFit;

	action_name					= "select terrain smooth modifier";
	button						= gcnew ToolStripButton;
	strip->Items->Add			( button );
	binder->bind				( button, action_name );
	button->Image				= binder->get_image( action_name );
	button->DisplayStyle		= System::Windows::Forms::ToolStripItemDisplayStyle::Image;
	button->ImageScaling		= System::Windows::Forms::ToolStripItemImageScaling::SizeToFit;

	action_name					= "select terrain detail painter modifier";
	button						= gcnew ToolStripButton;
	strip->Items->Add			( button);
	binder->bind				( button, action_name );
	button->Image				= binder->get_image(action_name);
	button->DisplayStyle		= System::Windows::Forms::ToolStripItemDisplayStyle::Image;
	button->ImageScaling		= System::Windows::Forms::ToolStripItemImageScaling::SizeToFit;

	action_name					= "select terrain diffuse painter modifier";
	button						= gcnew ToolStripButton;
	strip->Items->Add			( button);
	binder->bind				( button, action_name );
	button->Image				= binder->get_image( action_name );
	button->DisplayStyle		= System::Windows::Forms::ToolStripItemDisplayStyle::Image;
	button->ImageScaling		= System::Windows::Forms::ToolStripItemImageScaling::SizeToFit;

	action_name					= "select terrain selection control";
	button						= gcnew ToolStripButton;
	strip->Items->Add			( button );
	binder->bind				( button, action_name );
	button->Image				= binder->get_image( action_name );
	button->DisplayStyle		= System::Windows::Forms::ToolStripItemDisplayStyle::Image;
	button->ImageScaling		= System::Windows::Forms::ToolStripItemImageScaling::SizeToFit;


	action_name					= "select terrain detail uv relaxer";
	button						= gcnew ToolStripButton;
	strip->Items->Add			( button);
	binder->bind				( button, action_name );
	button->Image				= binder->get_image( action_name );
	button->DisplayStyle		= System::Windows::Forms::ToolStripItemDisplayStyle::Image;
	button->ImageScaling		= System::Windows::Forms::ToolStripItemImageScaling::SizeToFit;
}

void terrain_tool_tab::in_destructor()
{
}

void terrain_tool_tab::on_control_activated( editor_base::editor_control_base^ c )
{
//.	active_control_property_grid->SelectedObject = c;
	xray::editor::wpf_controls::property_container^ container = gcnew xray::editor::wpf_controls::property_container;
	xray::editor_base::object_properties::initialize_property_container( c, container );
	m_property_grid_host->selected_object		= container;
}

void terrain_tool_tab::on_control_deactivated( editor_base::editor_control_base^ )
{
//	active_control_property_grid->SelectedObject = nullptr;
	m_property_grid_host->selected_object		= nullptr;
}

void terrain_tool_tab::on_control_property_changed( editor_base::editor_control_base^ /*c*/ )
{
//	active_control_property_grid->Refresh();
	m_property_grid_host->update();
}

void terrain_tool_tab::button1_Click_1(System::Object^, System::EventArgs^)
{
	if(m_terrain_core_form!=nullptr)
	{
		m_terrain_core_form->Show			();
		return;
	}

	m_terrain_core_form					= gcnew terrain_core_form(m_tool);
	m_terrain_core_form->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &terrain_tool_tab::core_form_closing);
	m_terrain_core_form->Show			( m_tool->get_level_editor()->ide()->wnd() );
}

void terrain_tool_tab::core_form_closing(System::Object^, System::Windows::Forms::FormClosingEventArgs^)
{
	ASSERT						(m_terrain_core_form);
	m_terrain_core_form			= nullptr;
}


void terrain_tool_tab::button3_Click( System::Object^, System::EventArgs^)
{
	terrain_core^ core			= m_tool->get_terrain_core();

	for each(terrain_node^ n in core->m_nodes.Values)
		n->initialize_ph_collision();
	return;
}

void terrain_tool_tab::button4_Click(System::Object^, System::EventArgs^)
{
	terrain_core^ core			= m_tool->get_terrain_core();

	for each(terrain_node^ n in core->m_nodes.Values)
		n->destroy_ph_collision();
	return;
}

void terrain_tool_tab::button2_Click(System::Object^, System::EventArgs^)
{
	terrain_core^ core					= m_tool->get_terrain_core();

	key_vert_id_dict^ selected_quads	= core->selected_quads();
	if( selected_quads->Count==0)
	{
		m_tool->get_level_editor()->ShowMessageBox( "no terrain cells selected",
													MessageBoxButtons::OK,
													MessageBoxIcon::Exclamation );
		return;
	}

	xray::geometry_utils::geometry_collector		collector(debug::g_mt_allocator);
	key_vert_id_dict::Enumerator^ key_en = selected_quads->GetEnumerator();

	while(key_en->MoveNext())
	{
		vert_id_list^ src_list			= key_en->Current.Value;
		terrain_node^ current_terrain	= core->m_nodes[key_en->Current.Key];
		terrain_quad		quad;
		for each (u16 quad_idx in src_list)
		{
			if(!current_terrain->get_quad(quad, quad_idx))
				continue; // ??

			float3 p				= current_terrain->position_g(quad.index_lb);
			p.z *=-1;
			collector.add_vertex	( p );

			p						= current_terrain->position_g(quad.index_lt);
			p.z *=-1;
			collector.add_vertex	( p );

			p						= current_terrain->position_g(quad.index_rt);
			p.z *=-1;
			collector.add_vertex	( p );

			p						= current_terrain->position_g(quad.index_lb);
			p.z *=-1;
			collector.add_vertex	( p );

			p						= current_terrain->position_g(quad.index_rt);
			p.z *=-1;
			collector.add_vertex	( p );

			p						= current_terrain->position_g(quad.index_rb);
			p.z *=-1;
			collector.add_vertex	( p );
		}
	}
	System::Windows::Forms::SaveFileDialog		saveFileDialog;
	
	if( System::Windows::Forms::DialogResult::OK == saveFileDialog.ShowDialog() )
	{
		System::String^ fn	= saveFileDialog.FileName->Replace("\\", "/");
		fn					+=".obj";
		System::String^		message;

		if( collector.write_obj_file( unmanaged_string(fn).c_str(), 100.0f ))
			message = "Selection succesfully exported to " + gcnew System::String(fn);
		else
			message = "Error while exporting selection to " + gcnew System::String(fn);

		m_tool->get_level_editor()->ShowMessageBox( message,
													MessageBoxButtons::OK,
													MessageBoxIcon::Exclamation );

	}
}

void terrain_tool_tab::on_activate()
{
	m_tool->get_level_editor()->set_active_tool( m_tool );
}

void terrain_tool_tab::on_deactivate()
{
	m_tool->get_level_editor()->set_active_tool( nullptr );
}


void terrain_tool_tab::button5_Click( System::Object^ , System::EventArgs^)
{
	button5->Enabled		= false;
	//System::Windows::Forms::Cursor^ cursor		= m_tool->get_level_editor()->ide()->Cursor;
	//m_tool->get_level_editor()->ide()->Cursor	= Cursors::WaitCursor;
	bool use_wait_cursor_value	= m_tool->get_level_editor()->ide()->UseWaitCursor;
	m_tool->get_level_editor()->ide()->UseWaitCursor	= true;
	Application::DoEvents	( );

	xray::ai::navigation::graph_generator* graph_generator = m_tool->get_level_editor()->get_editor_world().get_ai_navigation_world().get_graph_generator();
	graph_generator->clear_geometry();

	cells_storage^ storage = m_tool->get_level_editor()->get_project()->m_editor_cell_manager->get_cells_storage();

	for each( cell_objects^ c in storage->Values ) 
	{
		for each ( object_base^ obj in c->m_objects_list ) 
		{
			object_solid_visual^ solid_obj = dynamic_cast<object_solid_visual^>(obj);
			if ( solid_obj )
			{
				graph_generator->add_geometry( &(*(*solid_obj->m_hq_collision)), solid_obj->get_transform() );
			}
		}
		terrain_node^ node = c->m_terrain_node;
		if ( node ) 
		{
			for ( int x = 0; x < 64; ++x ) 
			{
				for ( int z = 0; z < 64; ++z ) 
				{
					float h0, h1, h2, h3;
					h0 = node->m_vertices.m_vertices[ node->vertex_id( x    , z     ) ].height;
					h1 = node->m_vertices.m_vertices[ node->vertex_id( x + 1, z     ) ].height;
					h2 = node->m_vertices.m_vertices[ node->vertex_id( x + 1, z + 1 ) ].height;
					h3 = node->m_vertices.m_vertices[ node->vertex_id( x    , z + 1 ) ].height;
					
					float4x4 const& transform = node->get_transform();
					float3 v0 = transform.transform( float3( float(x)  , h0, float(0-z) ) );
					float3 v1 = transform.transform( float3( float(x+1), h1, float(0-z) ) );
					float3 v2 = transform.transform( float3( float(x+1), h2, float(0-z-1) ) );
					float3 v3 = transform.transform( float3( float(x)  , h3, float(0-z-1) ) );

					graph_generator->add_geometry_triangle( v0, v1, v3 );
					graph_generator->add_geometry_triangle( v1, v2, v3 );
				}
			}
		}
	}

	//xray::ai::navigation::graph_generator::cuboid_type cuboid;
	//cuboid.push_back( float3( 10, 10, -10 ) );
	//cuboid.push_back( float3( 20, 10, -10 ) );
	//cuboid.push_back( float3( 20, 10, -20 ) );
	//cuboid.push_back( float3( 10, 10, -20 ) );

	//cuboid.push_back( float3( 10, -1, -10 ) );
	//cuboid.push_back( float3( 20, -1, -10 ) );
	//cuboid.push_back( float3( 20, -1, -20 ) );
	//cuboid.push_back( float3( 10, -1, -20 ) );

	//graph_generator->add_restricted_area( cuboid );
	graph_generator->generate();
	m_tool->get_level_editor()->get_project()->set_changed();

	button5->Enabled		= true;
//	m_tool->get_level_editor()->ide()->Cursor	= cursor;
	m_tool->get_level_editor()->ide()->UseWaitCursor	= use_wait_cursor_value;
	Application::DoEvents	( );
}

} //namespace xray
} //namespace editor