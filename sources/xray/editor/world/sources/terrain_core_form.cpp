#include "pch.h"
#include "terrain_core_form.h"
#include "terrain_object.h"
#include "tool_terrain.h"
#include "terrain_import_form.h"
#include "level_editor.h"
#include "project.h"
#include "editor_world.h"
#include "terrain_copy_nodes_form.h"

using namespace System::Drawing;
using namespace System::Windows::Forms;


namespace xray {
namespace editor {

int node_size = 64;


void terrain_core_form::m_scale_up_down_ValueChanged(System::Object^, System::EventArgs^)
{
	current_scale = System::Convert::ToInt32(m_scale_up_down->Value);
}

void terrain_core_form::in_constructor()
{
	do_edit_bkg_mode					( false );
	m_ext_value							= false;
	current_scale						= 1;
	m_scale_up_down->Value				= current_scale;
	m_terrain_core						= nullptr;
	m_first_node						= Point(-5, 5);

	selected_nodes_pos					= terrain_node_key(0,0);
	selected_nodes_size					= terrain_node_key(1,1);

	System::Drawing::Color		fg_clr = System::Drawing::Color::FromArgb(80, 255, 0, 0);
	System::Drawing::Color		bg_clr = System::Drawing::Color::FromArgb(80, 0, 255, 0);
	m_node_brush		= gcnew System::Drawing::Drawing2D::HatchBrush(
		System::Drawing::Drawing2D::HatchStyle::Divot,
		fg_clr,
		bg_clr);

	update_status_string	( );
	m_import_form			= gcnew terrain_import_form;
}

void terrain_core_form::terrain_core_form_Activated(System::Object^, System::EventArgs^)
{
	m_terrain_core			= m_tool->get_terrain_core();

	map_backgrounds% bkg	= m_tool->get_level_editor()->get_project()->get_map_backgrounds();
	pcstr p					= m_terrain_core->get_editor_world().engine().get_resources_path();

	for each (project_background_item^ itm in bkg)
	{
		if(!itm->m_bitmap)
			itm->load( p );
	}

	global_map_panel->Invalidate();
}

void terrain_core_form::terrain_core_form_Deactivate(System::Object^, System::EventArgs^)
{
	m_terrain_core	= nullptr; // invalidate
}

terrain_node_key	capture_start_node;

void get_drawing_rectangle(System::Drawing::Rectangle% result, Point first, terrain_node_key lt, terrain_node_key sz, int nsz)
{
	result.X		= (lt.x - first.X) * nsz	- 5;
	result.Y		= (first.Y - lt.z) * nsz	- 5;
	result.Width	= (sz.x) * nsz		+ 10;
	result.Height	= (sz.z) * nsz		+ 10;
}

void terrain_core_form::global_map_panel_MouseMove(System::Object^, System::Windows::Forms::MouseEventArgs^)
{
	update_status_string		();
	terrain_node_key key		= get_node_key_mouse();

	if(m_mouse_rect_capture)
	{
		terrain_node_key prev_pos	= selected_nodes_pos;
		terrain_node_key prev_size	= selected_nodes_size;

		int pt_start_x, pt_start_y;
		int size_x, size_y;

		pt_start_x					= math::min(capture_start_node.x, key.x);
		pt_start_y					= math::max(capture_start_node.z, key.z);

		size_x						= math::abs(capture_start_node.x - key.x) +1;
		size_y						= math::abs(capture_start_node.z - key.z) +1;

		selected_nodes_pos			= terrain_node_key( pt_start_x, pt_start_y );
		
		selected_nodes_size			= terrain_node_key( size_x, size_y );

		if(!prev_pos.eq(selected_nodes_pos) || !prev_size.eq(selected_nodes_size))
		{
			int nsz					= (int)node_size / current_scale;
			System::Drawing::Rectangle		r;
			get_drawing_rectangle	(r, m_first_node, prev_pos, prev_size, nsz);

			System::Drawing::Rectangle		r2;
			get_drawing_rectangle	(r2, m_first_node, selected_nodes_pos, selected_nodes_size, nsz);

			global_map_panel->Invalidate(r);
			global_map_panel->Invalidate(r2);
		}
	}else
	{
		//m_selected_node_lt		= key;
		//m_selected_node_rb		= key;
	}
	update_status_string ();
}

void terrain_core_form::update_status_string()
{
	terrain_node_key key		= get_node_key_mouse();
	current_x_label->Text		= System::String::Format( "{0}", key.x );
	current_y_label->Text		= System::String::Format( "{0}", key.z );
}

void terrain_core_form::global_map_panel_MouseLeave(System::Object^, System::EventArgs^)
{

}

void terrain_core_form::global_map_panel_MouseDown(System::Object^, System::Windows::Forms::MouseEventArgs^ e)
{
	terrain_node_key key	= get_node_key_mouse();
	if(e->Button==System::Windows::Forms::MouseButtons::Left)
	{
		m_mouse_rect_capture	= true;
		capture_start_node		= key;
	
		if(!key.eq(selected_nodes_pos)  /*|| !key.eq(selected_nodes_rb)*/)
		{
			int nsz					= (int)node_size / current_scale;
			System::Drawing::Rectangle		r;
			get_drawing_rectangle	(r, m_first_node, selected_nodes_pos, selected_nodes_size, nsz);

			selected_nodes_pos		= key;
			selected_nodes_size		= terrain_node_key(1,1);

			System::Drawing::Rectangle		r2;
			get_drawing_rectangle	(r2, m_first_node, selected_nodes_pos, selected_nodes_size, nsz);

			global_map_panel->Invalidate(r);
			global_map_panel->Invalidate(r2);
		}
		
		update_status_string ();
	}
	update_status_string ();
}


void terrain_core_form::global_map_panel_MouseUp(System::Object^, System::Windows::Forms::MouseEventArgs^ e)
{
	if(e->Button==System::Windows::Forms::MouseButtons::Left)
		m_mouse_rect_capture	= false;

	update_status_string ();
}

void terrain_core_form::global_map_menu_Opening(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e)
{
	if(!m_terrain_core)
	{
		e->Cancel				= true;
		return;
	}

	System::Windows::Forms::ContextMenuStrip^ menu	= safe_cast<System::Windows::Forms::ContextMenuStrip^>(sender);
	menu->Items->Clear								( );
	System::Windows::Forms::ToolStripItem^ item		= nullptr;

	if(num_size_x==1 && num_size_y==1)
	{
		item				= menu->Items->Add("Focus to");
		item->Click			+= gcnew System::EventHandler(this, &terrain_core_form::focus_to_node);
	}

	if(m_edit_nodes_mode)
	{
		bool b_add_nodes			= false;
		bool b_clear_nodes			= false;

		int lt_x = num_pos_x;
		int lt_y = num_pos_y;
		for(int ix = 0; ix<num_size_x; ++ix)
			for(int iy = 0; iy<num_size_y; ++iy)
			{
				terrain_node_key key(lt_x+ix, lt_y-iy);
				if(m_terrain_core->m_nodes.ContainsKey(key))
					b_clear_nodes		= true;

				if(!m_terrain_core->m_nodes.ContainsKey(key))
					b_add_nodes		= true;
			}


		if(b_clear_nodes)
		{
			if(num_size_x==1 && num_size_y==1)
				item				= menu->Items->Add("Clear node");
			else
				item				= menu->Items->Add("Clear nodes");

			item->Click			+= gcnew System::EventHandler(this, &terrain_core_form::clear_node);
		}
		
		if(b_add_nodes)
		{
			if(num_size_x==1 && num_size_y==1)
				item				= menu->Items->Add("Add node");
			else
				item				= menu->Items->Add("Add nodes");

			item->Click			+= gcnew System::EventHandler(this, &terrain_core_form::add_node);
		}
	} //if(m_edit_nodes_mode)

	if(m_edit_backgroungs_mode)
	{
		if(num_size_x==1 && num_size_y)
		{
			bool b_add_bk			= true;
			bool b_remove_bk		= false;

			map_backgrounds% bkg			= m_tool->get_level_editor()->get_project()->get_map_backgrounds();
			for each (project_background_item^ itm in bkg)
			{
				if(itm->m_position_lt.X==num_pos_x && itm->m_position_lt.Y==num_pos_y)
				{
					b_remove_bk		= true;
					b_add_bk		= false;
				}
			}
			if(b_add_bk || b_remove_bk)
				item				= menu->Items->Add("-");

			if(b_add_bk)
			{
				item				= menu->Items->Add("Add Background");
				item->Click			+= gcnew System::EventHandler(this, &terrain_core_form::add_backgroung_image);
			}
			if(b_remove_bk)
			{
				item				= menu->Items->Add("Remove Background");
				item->Click			+= gcnew System::EventHandler(this, &terrain_core_form::remove_backgroung_image);
			}
		}
	}//	if(m_edit_backgroungs_mode)
}

terrain_node_key terrain_core_form::get_node_key_mouse()
{
	terrain_node_key result;
	int	nsz			= (int)node_size;
	if(nsz==0)		
		return		result;
	
	nsz								/= current_scale;

	System::Drawing::Size area_sz	= global_map_panel->Size;
	Point pt_local			= global_map_panel->PointToClient(MousePosition);
	
	int xx			= (pt_local.X)/nsz;
	int zz			= (pt_local.Y)/nsz;

	result.x		= xx+m_first_node.X;
	result.z		= m_first_node.Y-zz;
	return			result;
}


void terrain_core_form::global_map_panel_Paint(System::Object^, System::Windows::Forms::PaintEventArgs^ e)
{
	if(!m_terrain_core)
		return;
	
	System::Drawing::Graphics^ g			= e->Graphics;

	int	nsz							= (int)(node_size / current_scale);

	System::Drawing::Pen^ grid_pen			= System::Drawing::Pens::Red;
	System::Drawing::Brush^ font_brush		= System::Drawing::Brushes::White;

	System::Drawing::Pen^ selected_nodes_pen = gcnew System::Drawing::Pen(System::Drawing::Color::Black);
	selected_nodes_pen->Width		= 3.0f;
	System::Drawing::Font^ font				= Font;

	// draw grid
	System::Drawing::Point	pt_from, pt_to;

	System::Drawing::Size	area_sz			= global_map_panel->Size;

	// vert liles
	int num				= 0;
	for(int ix=0; ix<area_sz.Width; ix+= nsz, num++)
	{
		pt_from		= Point(ix, 0);
		pt_to		= Point(ix, area_sz.Height);
		g->DrawLine	(grid_pen, pt_from, pt_to);

		pt_from.X		+= nsz/3;
		g->DrawString(System::String::Format("{0}", num+m_first_node.X),
						font,font_brush,
						pt_from); // impl thru graphics->MeasureString

	}

	// horz liles
	num				= m_first_node.Y;
	for(int iy=0; iy<area_sz.Height; iy+= nsz, num--)
	{
		pt_from		= Point(0, iy);
		pt_to		= Point(area_sz.Width, iy);
		g->DrawLine (grid_pen, pt_from, pt_to);
		
		pt_from.Y		+= nsz/3;
		g->DrawString(System::String::Format("{0}", num),
						font,font_brush,
						pt_from); // impl thru graphics->MeasureString
	}

	draw_background					( g );

	// filled nodes
	System::Drawing::Rectangle	r;
	r.Width			= nsz;
	r.Height		= nsz;
	int max_x		= m_first_node.X + area_sz.Width/nsz;
	int max_y		= m_first_node.Y - area_sz.Height/nsz;

	for(int ix=m_first_node.X; ix<=max_x; ++ix)
		for(int iy=m_first_node.Y; iy>=max_y; --iy)
		{
			terrain_node_key key(ix, iy);
			if(m_terrain_core->m_nodes.ContainsKey(key))
			{
				r.X				= (ix-m_first_node.X)*nsz;
				r.Y				= (m_first_node.Y-iy)*nsz;
				g->FillRectangle(m_node_brush, r);
				
			}
		}

	// draw selection region
	System::Drawing::Rectangle		selected_rect;
	selected_rect.X			= ( num_pos_x - m_first_node.X ) * nsz;
	selected_rect.Y			= ( m_first_node.Y - num_pos_y ) * nsz;
	selected_rect.Width		= ( num_size_x ) * nsz;
	selected_rect.Height	= ( num_size_y ) * nsz;
	g->DrawRectangle		( selected_nodes_pen, selected_rect );

	// draw current camera position
	float3 camera_position;
	float3 camera_direction;
	m_tool->get_level_editor()->view_window()->get_camera_props( camera_position, camera_direction );
	
	System::Drawing::Point cam_point = point_to_client(camera_position);
	
	System::Drawing::Point cam_dir_point = cam_point;
	cam_dir_point.X						+= (int)(camera_direction.x * 20);
	cam_dir_point.Y						-= (int)(camera_direction.z * 20);

	g->DrawEllipse		(grid_pen, cam_point.X-3, cam_point.Y-3, 6, 6 );
	g->DrawLine			(grid_pen, cam_point, cam_dir_point );
}

System::Drawing::Point terrain_core_form::point_to_client( float3 p )
{
	System::Drawing::Point	result;
	int nsz					= (int)node_size;
	nsz						/= current_scale;

	System::Drawing::Point point_lt(m_first_node.X * nsz, m_first_node.Y * nsz);
	p /= (float)current_scale;

	result.X	= (int)p.x - point_lt.X;
	result.Y	= point_lt.Y - (int)p.z;
	return		result;
}

void terrain_core_form::add_node(System::Object^, System::EventArgs^)
{
	int sel_x = num_pos_x;
	int sel_y = num_pos_y;
	for(int ix = 0; ix<num_size_x; ++ix)
	{
		for(int iy = 0; iy<num_size_y; ++iy)
		{
			terrain_node_key key(sel_x+ix, sel_y-iy);
			if(!m_terrain_core->m_nodes.ContainsKey(key))
				m_terrain_core->create_node		( key );
		}
	}
	global_map_panel->Invalidate	();
}

void terrain_core_form::clear_node(System::Object^, System::EventArgs^)
{
	int lt_x = num_pos_x;
	int lt_y = num_pos_y;

	for(int ix = 0; ix<num_size_x; ++ix)
		for(int iy = 0; iy<num_size_y; ++iy)
		{
			terrain_node_key key(lt_x+ix, lt_y-iy);
			if(m_terrain_core->m_nodes.ContainsKey(key))
				m_terrain_core->clear_node		(key);
		}
	global_map_panel->Invalidate	();
}

void terrain_core_form::focus_to_node(System::Object^, System::EventArgs^)
{
	m_tool->focus_camera_to		( selected_nodes_pos );
}

void terrain_core_form::button_move_left_Click(System::Object^, System::EventArgs^)
{
	m_first_node.X					-=current_scale;
	global_map_panel->Invalidate	();
}

void terrain_core_form::button_move_right_Click(System::Object^, System::EventArgs^)
{
	m_first_node.X					+=current_scale;
	global_map_panel->Invalidate	();
}

void terrain_core_form::button_move_up_Click(System::Object^, System::EventArgs^)
{
	m_first_node.Y					+=current_scale;
	global_map_panel->Invalidate	();
}

void terrain_core_form::button_move_down_Click(System::Object^, System::EventArgs^)
{
	m_first_node.Y					-=current_scale;
	global_map_panel->Invalidate	();
}

void terrain_core_form::set_current_view_scale_impl(int value)
{
	if(value<1)
		value = 1;

	m_current_view_scale			= value;
	global_map_panel->Invalidate	();
}

void terrain_core_form::import_button_Click(System::Object^, System::EventArgs^)
{
	m_import_form->get_up_down_lt_x()->Value = selected_nodes_pos.x;
	m_import_form->get_up_down_lt_z()->Value = selected_nodes_pos.z;

	m_import_form->get_up_down_size_x()->Value = selected_nodes_size.x;
	m_import_form->get_up_down_size_z()->Value = selected_nodes_size.z;

	m_import_form->m_settings->m_b_import_mode = true;
	m_import_form->init_layout();
	if(m_import_form->ShowDialog() == ::DialogResult::OK)
	{
		terrain_import_export_settings^ settings	= m_import_form->m_settings;
		m_terrain_core->do_import						( settings );
	}
}

void terrain_core_form::export_button_Click(System::Object^, System::EventArgs^)
{
	m_import_form->get_up_down_lt_x()->Value = selected_nodes_pos.x;
	m_import_form->get_up_down_lt_z()->Value = selected_nodes_pos.z;

	m_import_form->get_up_down_size_x()->Value = selected_nodes_size.x;
	m_import_form->get_up_down_size_z()->Value = selected_nodes_size.z;

	m_import_form->m_settings->m_b_import_mode = false;
	m_import_form->init_layout();
	if(m_import_form->ShowDialog() == ::DialogResult::OK)
	{
		terrain_import_export_settings^ settings	= m_import_form->m_settings;
		m_terrain_core->do_export					( settings );
	}
}

void terrain_core_form::draw_background(System::Drawing::Graphics^ g)
{
	map_backgrounds% bkg = m_tool->get_level_editor()->get_project()->get_map_backgrounds();
	
	for each (project_background_item^ itm in bkg)
	{
		int width	= itm->m_bitmap->Width;
		int height	= itm->m_bitmap->Height;
		int scale	= itm->m_scale;

		System::Drawing::Point lt = point_to_client(float3(itm->m_position_lt.X * (float)node_size, 
															0.0f, 
															itm->m_position_lt.Y * (float)node_size) );

		g->DrawImage		( itm->m_bitmap, lt.X, lt.Y, (width*scale)/current_scale, (height*scale)/current_scale );

		if( m_edit_backgroungs_mode )
		{
			System::Drawing::Pen^ border_pen	= gcnew System::Drawing::Pen( System::Drawing::Color::Blue, 6.0f/current_scale);
			g->DrawRectangle			( border_pen, lt.X, lt.Y, width/current_scale, height/current_scale );
			cli::array<System::Drawing::Point>^ pt_array = gcnew cli::array<System::Drawing::Point >(3);
			pt_array[0] = System::Drawing::Point(lt.X, lt.Y);
			pt_array[1] = System::Drawing::Point(lt.X+30/current_scale, lt.Y);
			pt_array[2] = System::Drawing::Point(lt.X, lt.Y+30/current_scale);

			System::Drawing::Brush^ corner_brush = gcnew System::Drawing::SolidBrush(System::Drawing::Color::Blue);
			g->FillPolygon( corner_brush, pt_array );
		}
	}
}

void terrain_core_form::terrain_core_form_FormClosing(System::Object^, System::Windows::Forms::FormClosingEventArgs^)
{
	map_backgrounds% bkg = m_tool->get_level_editor()->get_project()->get_map_backgrounds();
	for each (project_background_item^ itm in bkg)
	{
		if(itm->m_bitmap)
			itm->unload();
	}
}

bool  terrain_core_form::select_bkg_texture_name( System::String^% selected )
{
	pcstr p							= m_tool->get_level_editor()->get_editor_world().engine().get_resources_path();
	System::String^ full_path		= project_background_item::get_root_path(p);
	System::Windows::Forms::OpenFileDialog					openFileDialog;
	openFileDialog.Filter			= "bitmap files|*.bmp";
	openFileDialog.FilterIndex		= 1;
	openFileDialog.InitialDirectory	= full_path;
	openFileDialog.FileName			= selected;

	if( openFileDialog.ShowDialog() == System::Windows::Forms::DialogResult::OK )
	{
		if(!openFileDialog.FileName->ToLower()->StartsWith(full_path))
		{
			m_tool->get_level_editor()->ShowMessageBox("Texture path must be under [" +full_path+ "] directory",
														MessageBoxButtons::OK,
														MessageBoxIcon::Exclamation );
			return false;
		}

		selected						= openFileDialog.FileName->Substring(full_path->Length);
		return true;
	}

	return false;
}


void terrain_core_form::num_ValueChanged( System::Object^, System::EventArgs^ )
{
	if(m_ext_value)	return;
	
	global_map_panel->Invalidate();
}

void terrain_core_form::do_edit_bkg_mode( bool what_to_do )
{
	m_edit_backgroungs_mode		= what_to_do;
	m_edit_nodes_mode			= !what_to_do;
	edit_bkg_button->FlatStyle	= (what_to_do)?System::Windows::Forms::FlatStyle::Flat : System::Windows::Forms::FlatStyle::Standard;
	global_map_panel->Invalidate();
	map_bkg_info_panel->Visible = what_to_do;
}

void terrain_core_form::edit_bkg_button_Click(System::Object^, System::EventArgs^ )
{
	do_edit_bkg_mode( !m_edit_backgroungs_mode );
	update_bkg_panel				( );
}

void terrain_core_form::focus_button_Click(System::Object^, System::EventArgs^ )
{
	focus_to_node(nullptr, nullptr);
}


void terrain_core_form::add_backgroung_image(System::Object^, System::EventArgs^)
{
	map_backgrounds% bkg			= m_tool->get_level_editor()->get_project()->get_map_backgrounds();
	System::String^ sel				= "";

	if( select_bkg_texture_name( sel ) )
	{
		project_background_item^ itm	= gcnew project_background_item;
		pcstr p							= m_tool->get_level_editor()->get_editor_world().engine().get_resources_path();
		itm->m_texture_name				= sel;
		itm->m_position_lt				= System::Drawing::Point(num_pos_x, num_pos_y);
		itm->m_scale					= 1;
		itm->load						( p );
		bkg.Add							( itm );
		global_map_panel->Invalidate	( );
	}
}

project_background_item^ terrain_core_form::focused_bkg_item()
{
	map_backgrounds% bkg			= m_tool->get_level_editor()->get_project()->get_map_backgrounds();
	for each (project_background_item^ itm in bkg)
	{
		if(itm->m_position_lt.X==num_pos_x && itm->m_position_lt.Y==num_pos_y)
			return itm;
	}
	return nullptr;
}

void terrain_core_form::remove_backgroung_image( System::Object^, System::EventArgs^ )
{
	project_background_item^ itm = focused_bkg_item ();
	if(itm!=nullptr)
	{
		map_backgrounds% bkg			= m_tool->get_level_editor()->get_project()->get_map_backgrounds();
		bkg.Remove						(itm);
		global_map_panel->Invalidate	( );
		update_bkg_panel				( );
	}
}

void terrain_core_form::insert_bkg_button_Click( System::Object^  sender, System::EventArgs^  e)
{
	add_backgroung_image( sender, e );
	update_bkg_panel	( );
}

void terrain_core_form::remove_bkg_button_Click( System::Object^  sender, System::EventArgs^  e)
{
	remove_backgroung_image	( sender, e );
	update_bkg_panel		( );
}

void terrain_core_form::select_bkg_file_button_Click( System::Object^, System::EventArgs^ )
{
	project_background_item^ itm = focused_bkg_item ();
	if(itm!=nullptr)
	{
		if( terrain_core_form::select_bkg_texture_name( itm->m_texture_name ) )
		{
			itm->m_bitmap					= nullptr;
			pcstr p							= m_tool->get_level_editor()->get_editor_world().engine().get_resources_path();
			itm->load						( p );
			global_map_panel->Invalidate	( );
			update_bkg_panel				( );
		}
	}
}

void terrain_core_form::bkg_scale_up_down_ValueChanged( System::Object^, System::EventArgs^ )
{
	project_background_item^ itm = focused_bkg_item ();
	if(itm!=nullptr)
	{
		itm->m_scale					= (int)bkg_scale_up_down->Value;
		global_map_panel->Invalidate	( );
		update_bkg_panel				( );
	}
}

void terrain_core_form::update_bkg_panel( )
{
	if(m_edit_backgroungs_mode)
	{
		project_background_item^ itm = focused_bkg_item ();
		if(itm!=nullptr)
		{
			bkg_file_label->Text			= itm->m_texture_name;
			bkg_scale_up_down->Value		= itm->m_scale;
			insert_bkg_button->Enabled		= false;
			select_bkg_file_button->Enabled = true;
			bkg_scale_up_down->Enabled		= true;
			remove_bkg_button->Enabled		= true;
		}else
		{
			bkg_file_label->Text			= "";
			insert_bkg_button->Enabled		= true;
			select_bkg_file_button->Enabled = false;
			bkg_scale_up_down->Enabled		= false;
			remove_bkg_button->Enabled		= false;
		}
	
	}
}

void terrain_core_form::spin_num_changed(System::Object^, System::EventArgs^)
{
	global_map_panel->Invalidate	();
}

void terrain_core_form::copy_nodes_button_Click( System::Object^, System::EventArgs^ )
{
	terrain_copy_nodes_form			f;
	f.m_terrain_core				= m_terrain_core;
	f.from_x_numericUpDown->Value	= selected_nodes_pos.x;
	f.from_y_numericUpDown->Value	= selected_nodes_pos.z;

	f.size_x_numericUpDown->Value	= selected_nodes_size.x;
	f.size_y_numericUpDown->Value	= selected_nodes_size.z;

	f.ShowDialog();
}

void terrain_core_form::reset_colors_button_Click(System::Object^, System::EventArgs^)
{
	m_terrain_core->reset_all_colors();
}


} // namespace editor
} // namespace xray
