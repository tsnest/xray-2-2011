////////////////////////////////////////////////////////////////////////////
//	Created		: 11.10.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "portal_generation_view.h"
#include "bsp_tree.h"
#include "bsp_tree_node.h"
#include "bsp_tree_control.h"
namespace xray
{
namespace model_editor
{
using namespace xray::editor::wpf_controls;

void portal_generation_view::in_constructor()
{
	m_property_grid_host		= gcnew property_grid::host( );
	m_property_grid_host->Dock	= System::Windows::Forms::DockStyle::Fill;
	top_panel->SuspendLayout	( );
	top_panel->Controls->Add	( m_property_grid_host );
	top_panel->ResumeLayout		( false );
	m_bsp_tree_control			= gcnew bsp_tree_control();
	m_bsp_tree_control->Dock	= System::Windows::Forms::DockStyle::Fill;
	bottom_panel->SuspendLayout	();
	bottom_panel->Controls->Add	( m_bsp_tree_control );
	bottom_panel->ResumeLayout	();
}

System::Void portal_generation_view::bsp_tree_view_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	if ( e->Node )
	{
		bsp_tree_node const* node = static_cast<bsp_tree_node_wrapper>( e->Node->Tag ).node;
		m_bsp_tree->set_selected_node( node );
	}
}

System::Void portal_generation_view::serializeNodeToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{
	R_ASSERT( m_model_name != nullptr );
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	if ( bsp_node_save_dialog->ShowDialog() == System::Windows::Forms::DialogResult::OK	)
	{
		{
			unmanaged_string bsp_tree_filename( bsp_node_save_dialog->FileName->Replace( '\\', '/' ) );
			configs::lua_config_ptr bsp_tree_config = configs::create_lua_config();
			m_bsp_tree->serialise_selected_node( bsp_tree_config );
			bsp_tree_config->save_as( bsp_tree_filename.c_str(), xray::configs::target_sources  );
		}
		{
			unmanaged_string mesh_filename( bsp_node_save_dialog->FileName->Replace( ".lua", ".mesh" )->Replace( '\\', '/' ) );
			configs::lua_config_ptr mesh_config = configs::create_lua_config();
			m_bsp_tree->serialise_triangles_mesh( mesh_config );
			mesh_config->save_as( mesh_filename.c_str(), xray::configs::target_sources  );
		}
	}
}

System::Void portal_generation_view::serialiseWholeTreeToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	if ( bsp_node_save_dialog->ShowDialog() == System::Windows::Forms::DialogResult::OK	)
	{
		{
			unmanaged_string filename( bsp_node_save_dialog->FileName->Replace( '\\', '/' ) );
			configs::lua_config_ptr config = configs::create_lua_config();
			m_bsp_tree->serialise_whole_tree( config );
			config->save_as( filename.c_str(), xray::configs::target_sources  );
		}
		{
			unmanaged_string mesh_filename( bsp_node_save_dialog->FileName->Replace( ".lua", ".mesh" )->Replace( '\\', '/' ) );
			configs::lua_config_ptr mesh_config = configs::create_lua_config();
			m_bsp_tree->serialise_triangles_mesh( mesh_config );
			mesh_config->save_as( mesh_filename.c_str(), xray::configs::target_sources  );
		}
	}
}

System::Void portal_generation_view::loadWholeTreeToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{	
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	R_ASSERT( m_model_name != nullptr );
	if ( bsp_node_open_dialog->ShowDialog() == System::Windows::Forms::DialogResult::OK	)
	{
		m_bsp_tree->reset();
		{
			unmanaged_string filename( bsp_node_open_dialog->FileName->Replace( '\\', '/' ) );
			fs_new::virtual_path_string logical_path;
			bool success = resources::convert_physical_to_virtual_path( 
				&logical_path, 
				fs_new::native_path_string::convert(filename.c_str()).make_lowercase(),
				resources::sources_mount 
			);

			R_ASSERT( success );

			query_result_delegate* qr = NEW(query_result_delegate)(
				gcnew query_result_delegate::Delegate(this,	&portal_generation_view::on_tree_restore_file_ready), g_allocator) ;
			resources::query_resource(
				logical_path.c_str(),
				resources::binary_config_class,
				boost::bind(&query_result_delegate::callback, qr, _1),
				g_allocator
			);
		}
		{
			unmanaged_string mesh_filename( bsp_node_open_dialog->FileName->Replace( ".lua", ".mesh" )->Replace( '\\', '/' ) );
			fs_new::virtual_path_string mesh_logical_path;
			bool success = resources::convert_physical_to_virtual_path( 
				&mesh_logical_path, 
				fs_new::native_path_string::convert(mesh_filename.c_str()).make_lowercase(), 
				resources::sources_mount 
				);

			R_ASSERT( success );

			query_result_delegate* qr = NEW(query_result_delegate)(
				gcnew query_result_delegate::Delegate(this,	&portal_generation_view::on_mesh_restore_file_ready), g_allocator) ;
			resources::query_resource(
				mesh_logical_path.c_str(),
				resources::binary_config_class,
				boost::bind(&query_result_delegate::callback, qr, _1),
				g_allocator
			);
		}
	}
}

void portal_generation_view::set_model_name( System::String^ model_name )
{
	m_model_name = model_name;
	R_ASSERT( m_model_name != nullptr );
	fs_new::virtual_path_string  logical_path;
	logical_path.assignf	( "resources/models/%s.model", unmanaged_string( m_model_name ).c_str() );
	fs_new::native_path_string  physical_path;
	bool converted = resources::convert_virtual_to_physical_path ( &physical_path, logical_path, resources::sources_mount );
	R_ASSERT( converted );
	fs_new::native_path_string  absolute_physical_path;
	converted = fs_new::convert_to_absolute_path( &absolute_physical_path, physical_path, assert_on_fail_true );
	R_ASSERT( converted );
	bsp_node_save_dialog->InitialDirectory = gcnew System::String( absolute_physical_path.c_str() );
	bsp_node_save_dialog->RestoreDirectory = true;
	bsp_node_open_dialog->InitialDirectory = bsp_node_save_dialog->InitialDirectory;
	bsp_node_open_dialog->RestoreDirectory = bsp_node_save_dialog->RestoreDirectory;
}

void portal_generation_view::on_tree_restore_file_ready( resources::queries_result& data )
{
	R_ASSERT( data.is_successful() );
	configs::binary_config_ptr config = static_cast_resource_ptr<configs::binary_config_ptr>( data[0].get_unmanaged_resource() );
	m_bsp_tree->restore( config );
	if ( m_bsp_tree->is_created() )
	{
		bsp_tree_view->Nodes->Clear();
		m_bsp_tree->fill_tree_view( bsp_tree_view );
	}
}

void portal_generation_view::on_mesh_restore_file_ready	( resources::queries_result& data )
{
	R_ASSERT( data.is_successful() );
	configs::binary_config_ptr config = 
		static_cast_resource_ptr<configs::binary_config_ptr>( data[0].get_unmanaged_resource() );
	m_bsp_tree->restore_triangles_mesh( config );
	if ( m_bsp_tree->is_created() )
	{
		bsp_tree_view->Nodes->Clear();
		m_bsp_tree->fill_tree_view( bsp_tree_view );
	}
}

void portal_generation_view::set_property_container( xray::editor::wpf_controls::property_container^ cont )
{
	m_property_grid_host->selected_object	= cont;
}


}
}
