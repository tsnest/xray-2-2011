#include "pch.h"
#include "bsp_tree_panel.h"
#include "bsp_tree.h"
#include "bsp_tree_node.h"
#include "sectors_generator.h"
#include <xray/resources_fs_iterator.h>
#include <xray/fs/fs_path_string_utils.h>

namespace xray
{
namespace model_editor
{

System::Void bsp_tree_panel::bsp_tree_view_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	if ( e->Node )
	{
		bsp_tree_node const* node = static_cast<bsp_tree_node_wrapper>( e->Node->Tag ).node;
		m_bsp_tree->set_selected_node( node );
	}
}

System::Void bsp_tree_panel::min_square_trackbar_Scroll(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	float const value = min_square_trackbar->Value / 1000.0f;
	bsp_tree_node::ms_min_portal_square = value;
	lbl_min_square->Text				= System::String::Format( "Min square threshold: {0}", bsp_tree_node::ms_min_portal_square );
}

System::Void bsp_tree_panel::slimness_threshold_trackbar_Scroll(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	float const value =  slimness_threshold_trackbar->Value / 1000.0f;
	bsp_tree_node::ms_slimness_threshold = value;
	lbl_slimness_threshold->Text		= System::String::Format( "Min slimness threshold: {0}", bsp_tree_node::ms_slimness_threshold );
}

System::Void bsp_tree_panel::chb_aabb_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	bsp_tree_node::ms_draw_aabb = chb_aabb->Checked;
}

System::Void bsp_tree_panel::chb_solid_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	bsp_tree_node::ms_draw_solid = chb_solid->Checked;

}

System::Void bsp_tree_panel::serializeNodeToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{
	R_ASSERT( m_model_name != nullptr );
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	if ( bsp_node_save_dialog->ShowDialog() == System::Windows::Forms::DialogResult::OK	)
	{
		unmanaged_string filename( bsp_node_save_dialog->FileName->Replace( '\\', '/' ) );
		fs_new::virtual_path_string logical_path;
		bool converted = resources::convert_physical_to_virtual_path( &logical_path, filename.c_str(), resources::sources_mount );
		R_ASSERT( converted );
		configs::lua_config_ptr config = configs::create_lua_config();
		m_bsp_tree->serialise_selected_node( config );
		config->save_as( filename.c_str(), configs::target_sources );
	}
}

System::Void bsp_tree_panel::serialiseWholeTreeToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	if ( bsp_node_save_dialog->ShowDialog() == System::Windows::Forms::DialogResult::OK	)
	{
		unmanaged_string filename( bsp_node_save_dialog->FileName->Replace( '\\', '/' ) );
		fs_new::virtual_path_string logical_path;
		bool converted = resources::convert_physical_to_virtual_path( &logical_path, filename.c_str(), resources::sources_mount );
		R_ASSERT( converted );
		configs::lua_config_ptr config = configs::create_lua_config();
		m_bsp_tree->serialise_whole_tree( config );
		config->save_as( filename.c_str(), configs::target_sources );
	}
}

System::Void bsp_tree_panel::loadWholeTreeToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{	
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	R_ASSERT( m_model_name != nullptr );
	if ( bsp_node_open_dialog->ShowDialog() == System::Windows::Forms::DialogResult::OK	)
	{
		unmanaged_string filename( bsp_node_open_dialog->FileName->Replace( '\\', '/' ) );
		fs_new::virtual_path_string logical_path;
		bool converted = resources::convert_physical_to_virtual_path( &logical_path, filename.c_str(), resources::sources_mount );
		R_ASSERT( converted );
		
		query_result_delegate* qr = NEW(query_result_delegate)(
			gcnew query_result_delegate::Delegate(this,	&bsp_tree_panel::on_restore_file_ready), g_allocator) ;

		resources::query_resource(
			logical_path.c_str(),
			resources::lua_config_class,
			boost::bind(&query_result_delegate::callback, qr, _1),
			g_allocator
			);
	}
}

void bsp_tree_panel::set_model_name( System::String^ model_name )
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

void bsp_tree_panel::on_restore_file_ready( resources::queries_result& data )
{
	configs::lua_config_ptr config = 
		data.is_successful() ? 
		static_cast_resource_ptr<configs::lua_config_ptr>( data[0].get_unmanaged_resource() ) :
		configs::create_lua_config();
	m_bsp_tree->restore( config );
	this->tree_view->Nodes->Clear();
	m_bsp_tree->fill_tree_view( this->tree_view );
}

System::Void bsp_tree_panel::chb_double_sided_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	bsp_tree_node::ms_draw_double_sided = chb_double_sided->Checked;
}

System::Void bsp_tree_panel::chb_draw_divider_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	bsp_tree_node::ms_draw_divider = chb_draw_divider->Checked;
}

System::Void bsp_tree_panel::chb_geometry_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	bsp_tree_node::ms_draw_geometry = chb_geometry->Checked;
}

System::Void bsp_tree_panel::chb_portal_blanks_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	bsp_tree_node::ms_draw_portal_blanks = chb_portal_blanks->Checked;
}


System::Void bsp_tree_panel::chb_portals_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	bsp_tree_node::ms_draw_portal = chb_portals->Checked;
}

System::Void bsp_tree_panel::chb_outer_edges_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	bsp_tree_node::ms_draw_outer_edges = chb_outer_edges->Checked;
}

System::Void bsp_tree_panel::square_difference_trackbar_Scroll(System::Object^  sender, System::EventArgs^  e)
{	
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	//float const value = square_difference_trackbar->Value / 100.0f;
	//indexed_polygon::ms_similarity_max_square_difference = value;
	//lbl_square_difference->Text			= System::String::Format( "Max relative square difference: {0}", indexed_polygon::ms_similarity_max_square_difference );
}

System::Void bsp_tree_panel::position_difference_trackbar_Scroll(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	//float const value = position_difference_trackbar->Value / 1000.0f;
	//indexed_polygon::ms_similarity_max_position_difference = value;
	//lbl_position_difference->Text		= System::String::Format( "Max vertex position difference: {0}", indexed_polygon::ms_similarity_max_position_difference );
}

System::Void bsp_tree_panel::on_bsp_tree_panel_shown(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	
	chb_aabb->Checked					= bsp_tree_node::ms_draw_aabb;
	chb_solid->Checked					= bsp_tree_node::ms_draw_solid;
	chb_double_sided->Checked			= bsp_tree_node::ms_draw_double_sided;
	chb_draw_divider->Checked			= bsp_tree_node::ms_draw_divider;
	chb_geometry->Checked				= bsp_tree_node::ms_draw_geometry;
	chb_portal_blanks->Checked			= bsp_tree_node::ms_draw_portal_blanks;
	chb_portals->Checked				= bsp_tree_node::ms_draw_portal;
	chb_outer_edges->Checked			= bsp_tree_node::ms_draw_outer_edges;
	chb_generated_portals->Checked		= bsp_tree::ms_draw_extracted_portals;
	chb_hanged_portals->Checked			= bsp_tree::ms_draw_hangeded_portals;

	//square_difference_trackbar->Value	= static_cast<int>( indexed_polygon::ms_similarity_max_square_difference * 100 );
	//lbl_square_difference->Text			= System::String::Format( "Max relative square difference: {0}", indexed_polygon::ms_similarity_max_square_difference );
	//position_difference_trackbar->Value = static_cast<int>( indexed_polygon::ms_similarity_max_position_difference * 1000 );
	//lbl_position_difference->Text		= System::String::Format( "Max vertex position difference: {0}", indexed_polygon::ms_similarity_max_position_difference );

	min_square_trackbar->Value			= static_cast<int>( bsp_tree_node::ms_min_portal_square * 1000 );
	lbl_min_square->Text				= System::String::Format( "Min square threshold: {0}", bsp_tree_node::ms_min_portal_square );

	slimness_threshold_trackbar->Value	= static_cast<int>( bsp_tree_node::ms_slimness_threshold * 1000 );
	lbl_slimness_threshold->Text		= System::String::Format( "Min slimness threshold: {0}", bsp_tree_node::ms_slimness_threshold );

	chb_delimited_edges->Checked		= sectors_generator::ms_draw_delimited_edges;
	chb_cuboids->Checked				= sectors_generator::ms_draw_cuboids;
	chb_sectors->Checked				= sectors_generator::ms_draw_sectors;
}

System::Void bsp_tree_panel::chb_generated_portals_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	bsp_tree::ms_draw_extracted_portals = chb_generated_portals->Checked;
	chb_hanged_portals->Enabled			= chb_generated_portals->Checked;
}

System::Void bsp_tree_panel::chb_hanged_portals_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	bsp_tree::ms_draw_hangeded_portals = chb_hanged_portals->Checked;
}

System::Void bsp_tree_panel::chb_delimited_edges_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	sectors_generator::ms_draw_delimited_edges = chb_delimited_edges->Checked;
}

System::Void bsp_tree_panel::chb_cuboids_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	sectors_generator::ms_draw_cuboids = chb_cuboids->Checked;
}

System::Void bsp_tree_panel::chb_sectors_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	sectors_generator::ms_draw_sectors = chb_sectors->Checked;
}

}
}