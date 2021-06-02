////////////////////////////////////////////////////////////////////////////
//	Created		: 21.01.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_graph_node.h"
#include "particle_graph_node_emitter.h"
#include "particle_graph_node_action.h"
#include "particle_graph_node_event.h"
#include "particle_graph_node_property.h"
#include "particle_data_source_nodes.h"
#include "particle_graph_node_collection.h"
#include "particle_editor.h"
#include "particle_hypergraph.h"
#include "particle_node_properties.h"
#include "particle_graph_document.h"
#include "add_delete_group_nodes.h"
#include <xray/configs.h>
#include <xray/editor/base/images/images30x25.h>
#include "property_holder.h"
#include "property_container.h"
#include "particle_sources_panel.h"

using namespace System;
using namespace System::Reflection;
using namespace System::Windows::Forms;

using xray::editor::particle_graph_node;
using xray::editor::controls::hypergraph::connection_point;
using xray::editor::controls::hypergraph::point_align;
using xray::editor::controls::hypergraph::view_mode;

namespace xray {
namespace editor {
	
void					particle_graph_node::in_constructor		()
{
	this->AllowDrop					= true;
	m_drag_start_distance			= 15.0f;
	m_is_on_graph					= true;
	m_children						= gcnew particle_graph_node_collection(this);
	m_active_node					= nullptr;
	m_parent_node					= nullptr;
	m_highlighted					= false;
	m_enabled						= true;
	this->BackColor					= System::Drawing::Color::Transparent;
	this->ForeColor					= System::Drawing::Color::Transparent;
	m_image_type					= xray::editor_base::image_node_disabled;
	m_parent_position_offset		= Point( 0, 0 );
	m_property_holder				= NEW(::property_holder)("particle_node_props", NULL, NULL);
	m_properties_config				= NEW(xray::configs::lua_config_value)((*xray::configs::create_lua_config())["properties"]);
	m_time_parameters_holder		= nullptr;
}

void					particle_graph_node::OnMouseMove		(MouseEventArgs^ e)
{
	if(m_is_mouse_down && (!is_on_graph || (this->m_parent_node != nullptr &&	Math::Sqrt(
		Math::Pow(e->Location.X - m_last_mouse_down_position.X, 2) +
		Math::Pow(e->Location.Y - m_last_mouse_down_position.Y, 2)) > m_drag_start_distance)))
	{
		m_cursors->make_cursors				(this);
		DoDragDrop							(this, DragDropEffects::Copy|(m_is_on_graph?DragDropEffects::Move:DragDropEffects::None));
		Windows::Forms::Cursor::Current		= Cursors::Default;
		m_is_mouse_down						= false;

		if(is_on_graph){
			safe_cast<particle_hypergraph^>(parent_hypergraph)->hide_available_nodes(this);
		}
		else{
			particle_graph_document^ document = safe_cast<particle_graph_document^>(safe_cast<document_editor_base^>(safe_cast<DockContent^>(ParentForm)->DockPanel->Parent)->active_document);
			if (document != nullptr)
				document->hypergraph->hide_available_nodes(this);
		}

	}
	
	if(is_on_graph){
		controls::hypergraph::node::OnMouseMove(e);
	}
}

void					particle_graph_node::OnMouseDown		(MouseEventArgs^ e)
{
	if(is_on_graph){		
		controls::hypergraph::node::OnMouseDown(e);
		safe_cast<particle_hypergraph^>(parent_hypergraph)->show_available_nodes(this);		
	}
	else
	{
		particle_graph_document^ document = safe_cast<particle_graph_document^>(safe_cast<document_editor_base^>(safe_cast<DockContent^>(ParentForm)->DockPanel->Parent)->active_document);
		if (document != nullptr)
			document->hypergraph->show_available_nodes(this);
	}
	m_is_mouse_down				= true;
	m_last_mouse_down_position	= e->Location;

	
}

void					particle_graph_node::OnMouseUp			(MouseEventArgs^ e)
{
	if(is_on_graph){
		controls::hypergraph::node::OnMouseUp(e);	
		safe_cast<particle_hypergraph^>(parent_hypergraph)->hide_available_nodes(this);
		process_node_selection();		
	}
	else{
		particle_graph_document^ document = safe_cast<particle_graph_document^>(safe_cast<document_editor_base^>(safe_cast<DockContent^>(ParentForm)->DockPanel->Parent)->active_document);
		if (document != nullptr)
			document->hypergraph->hide_available_nodes(this);
	}
	
	m_is_mouse_down = false;
	
}

static Boolean				is_parent(particle_graph_node^ self, particle_graph_node^ parent)
{
	while(self != parent && self != nullptr)
	{
		self = self->parent_node;
	}
	return self == parent;
}
Boolean particle_graph_node::can_accept_node(particle_graph_node^ node){
	return (enabled() && !is_parent(this, node) && node->parent_node != this);
}

void					particle_graph_node::OnDragOver			(DragEventArgs^ e)
{ 
	if(m_is_on_graph && 
		e->Data->GetData(e->Data->GetType()) != this)
	{
		System::Object^ obj = nullptr;
		if ( e->Data->GetDataPresent(particle_graph_node_action::typeid))
			obj = e->Data->GetData(particle_graph_node_action::typeid);
		else if ( e->Data->GetDataPresent(particle_graph_node_emitter::typeid))
			obj = e->Data->GetData(particle_graph_node_emitter::typeid);
		else if ( e->Data->GetDataPresent(particle_graph_node_event::typeid))
			obj = e->Data->GetData(particle_graph_node_event::typeid);
		else if ( e->Data->GetDataPresent(particle_graph_node_property::typeid))
			obj = e->Data->GetData(particle_graph_node_property::typeid);
	
		particle_graph_node^ dragged_node = safe_cast<particle_graph_node^>(obj);
		if(can_accept_node(dragged_node))
		{
			if(dragged_node->m_parent != nullptr && dragged_node->m_parent->Parent != m_parent->Parent)
			{
				e->Effect = DragDropEffects::Copy;
				return;
			}

			if(	e->AllowedEffect == (DragDropEffects::Move|DragDropEffects::Copy))
				if(!(e->KeyState == (int)Keys::ControlKey))
					e->Effect = DragDropEffects::Move;
				else
					e->Effect = DragDropEffects::Copy;

			else if(e->AllowedEffect == DragDropEffects::Copy)
				e->Effect = DragDropEffects::Copy;
		}
	}
}

void					particle_graph_node::OnDragEnter		(DragEventArgs^ e){
	XRAY_UNREFERENCED_PARAMETERS(e);
	if (m_parent != nullptr){		
		m_parent->focused_node=this;	
	}
}

void					particle_graph_node::OnDragLeave		(EventArgs^ e){
	XRAY_UNREFERENCED_PARAMETERS(e);
	if (m_parent != nullptr)
		m_parent->focused_node=nullptr;	
}


void					particle_graph_node::OnDragDrop			(DragEventArgs^ e)
{
	System::Object^ obj = nullptr;
	if ( e->Data->GetDataPresent(particle_graph_node_action::typeid))
		obj = e->Data->GetData(particle_graph_node_action::typeid);
	else if ( e->Data->GetDataPresent(particle_graph_node_emitter::typeid))
		obj = e->Data->GetData(particle_graph_node_emitter::typeid);
	else if ( e->Data->GetDataPresent(particle_graph_node_event::typeid))
		obj = e->Data->GetData(particle_graph_node_event::typeid);
	else if ( e->Data->GetDataPresent(particle_graph_node_property::typeid))
		obj = e->Data->GetData(particle_graph_node_property::typeid);

	particle_graph_node^ dropped_node = safe_cast<particle_graph_node^>(obj);

	if(e->Effect == DragDropEffects::Copy)
	{

		configs::lua_config_ptr config = configs::create_lua_config( );
		configs::lua_config_value val = (*config)["root"];

		dropped_node->save(val[0]);

		safe_cast<particle_hypergraph^>(parent_hypergraph)->comm_engine->run(gcnew particle_graph_commands::add_group(safe_cast<particle_hypergraph^>(parent_hypergraph), config, this));
	}
	else if(e->Effect == DragDropEffects::Move)
	{
		safe_cast<particle_hypergraph^>(parent_hypergraph)->move_node(dropped_node, this);
	}
}

void					particle_graph_node::OnGiveFeedback		(GiveFeedbackEventArgs^ e)
{
	e->UseDefaultCursors = false;
	m_cursors->set_cursor_for(e->Effect);
}


static int width_between_nodes	= 30;
static int height_between_nodes	 = 0;


void					particle_graph_node::OnPaint			(PaintEventArgs^ e)
{
	
	xray::editor_base::images30x25 mini_image = xray::editor_base::images30x25_emitter;
	if (m_image_type == xray::editor_base::image_node_emitter)
		if(is_root_node())
			mini_image = xray::editor_base::images30x25_root;
		else
			mini_image = xray::editor_base::images30x25_emitter;
	else if (m_image_type == xray::editor_base::image_node_render)
		mini_image = xray::editor_base::images30x25_render;
	else if (m_image_type == xray::editor_base::image_node_color)
		mini_image = xray::editor_base::images30x25_color;
	else if (m_image_type == xray::editor_base::image_node_size)
		mini_image = xray::editor_base::images30x25_size;
	else if (m_image_type == xray::editor_base::image_node_velocity)
		mini_image = xray::editor_base::images30x25_velocity;
	else if (m_image_type == xray::editor_base::image_node_rotation)
		mini_image = xray::editor_base::images30x25_rotation;
	else if (m_image_type == xray::editor_base::image_node_rotaion_rate)
		mini_image = xray::editor_base::images30x25_rotaion_rate;
	else if (m_image_type == xray::editor_base::image_node_location)
		mini_image = xray::editor_base::images30x25_location;
	else if (m_image_type == xray::editor_base::image_node_event)
		mini_image = xray::editor_base::images30x25_event;
	else if (m_image_type == xray::editor_base::image_node_misc)
		mini_image = xray::editor_base::images30x25_misc;
	if(m_parent != nullptr){

		ImageList^ particle_node_images = safe_cast<particle_graph_node_style_mgr^>(parent_hypergraph->nodes_style_manager)->images;
		ImageList^ particle_node_mini_images = safe_cast<particle_graph_node_style_mgr^>(parent_hypergraph->nodes_style_manager)->mini_images;
		
		if (!m_enabled)
			e->Graphics->DrawImage(particle_node_images->Images[xray::editor_base::image_node_disabled], Drawing::Point(0,0));			
		
		if(m_parent->selected_nodes->Contains(this))
			e->Graphics->DrawImage(particle_node_images->Images[xray::editor_base::image_node_selected], Drawing::Point(0,0));			
		
		e->Graphics->DrawImage(particle_node_mini_images->Images[mini_image], Drawing::Point(-2,0));
	}
	else
	{
		particle_editor^ editor = safe_cast<particle_editor^>(safe_cast<DockContent^>(ParentForm)->DockPanel->Parent->Parent);
		e->Graphics->DrawImage(editor->node_style_mgr->mini_images->Images[mini_image], Drawing::Point(-2,0));		
	}
e->Graphics->DrawString		(Text, gcnew Drawing::Font("Arial", 7,FontStyle::Bold ), gcnew SolidBrush(Drawing::Color::Black),  Drawing::Point(25,7));
}

void					particle_graph_node::OnKeyDown		(KeyEventArgs^ e)
{
	if(is_on_graph){	
		m_parent->on_key_down(this, e);
	}
}

void					particle_graph_node::OnKeyUp			(KeyEventArgs^ e)
{
	if(is_on_graph)
		m_parent->on_key_up(this, e);
}

particle_graph_node^	particle_graph_node::clone				()
{
	particle_graph_node^ new_node					= gcnew particle_graph_node(parent_editor);
	new_node->m_style								= m_style;	 
	new_node->recalc								();
	return new_node;
}

void					particle_graph_node::show_children()
{	
	if((m_children == nullptr) || (m_children->Count == 0) || !enabled()){
		Visible = true;
		return;
	}
	if ((m_parent_node->m_active_node != this)){
		Visible = true;
		return;
	}

	for each(particle_graph_node^ n in m_children) {
		n->show_children();		
	}
	Visible = true;
}

void					particle_graph_node::hide_children()
{	
	if((m_children == nullptr)||(m_children->Count == 0)){
		Visible = false;
		return;
	}

	for each(particle_graph_node^ n in m_children) {
		n->hide_children();		
	}
	Visible = false;
}

void					particle_graph_node::process_node_selection(){
	if (m_parent_node == nullptr || !enabled() || children->Count == 0)
		return;

//	if(m_parent_node->active_node == this)
//		return;

	if (m_parent_node->active_node != nullptr){
		m_parent_node->active_node->hide_children();
		m_parent_node->active_node->Visible = true;
	}
	m_parent_node->active_node = this;
	m_parent_node->active_node->show_children();

	parent_hypergraph->Invalidate(false);

}

static bool is_empty_table(configs::lua_config_value table){
	bool is_empty = true;

	configs::lua_config_value::iterator table_begin		= table.begin( );
	configs::lua_config_value::iterator table_end		= table.end( );

	for (;table_begin != table_end;++table_begin){
		if ( ( *table_begin ).get_type( ) == configs::t_table_indexed || ( *table_begin ).get_type( ) == configs::t_table_named ){
			if (!is_empty_table(*table_begin)){
				is_empty = false;
				break;
			}
		}
		else 
			is_empty = false;	
	}
	return is_empty;
}
	
void					particle_graph_node::load(configs::lua_config_value config)
{
	properties = gcnew particle_node_properties(this);
	
	properties->name		= gcnew String(config["name"]);
	properties->type		= gcnew String(config["type"]);
	

	if (config.value_exists("enabled")){
		m_enabled = bool(config["enabled"]);
	}

	m_image_type = xray::editor_base::images92x25(int(config["image_type"]));

	Text										= properties->name;
	
	m_config_field					= gcnew String(config["node_config_field"]);
	
	if (config.value_exists("properties") && !is_empty_table(config["properties"]))
		m_properties_config->assign_lua_value(config["properties"]);

	load_node_properties((*m_parent_editor->node_sources_panel->particle_nodes_config)[(pcstr)unmanaged_string(m_config_field).c_str()]["properties"]);
	
	
	
 	configs::lua_config_value children			= config["children"];
	configs::lua_config_value::iterator begin	= children.begin();
	configs::lua_config_value::iterator end		= children.end();

	SuspendLayout();

	for(;begin != end;++begin)
	{
		particle_graph_node^ new_node	= nullptr;
		if (gcnew String((*begin)["type"]) == "action")
			new_node = gcnew particle_graph_node_action(xray::editor_base::image_node_emitter, parent_editor);
		if (gcnew String((*begin)["type"]) == "emitter")
			new_node = gcnew particle_graph_node_emitter(xray::editor_base::image_node_emitter, parent_editor);
		if (gcnew String((*begin)["type"]) == "event")
			new_node = gcnew particle_graph_node_event(xray::editor_base::image_node_emitter, parent_editor);
		if (gcnew String((*begin)["type"]) == "property")
			new_node = gcnew particle_graph_node_property(xray::editor_base::image_node_emitter, parent_editor);
		
		new_node->m_style				= this->m_style;		
		this->children->Add				(new_node);
		new_node->load					(*begin);
	}

	ResumeLayout(false);
}



void					particle_graph_node::save(configs::lua_config_value config)
{
	config["name"]				= unmanaged_string(properties->name).c_str();
	config["type"]				= unmanaged_string(properties->type).c_str();

	
	config["enabled"]			= m_enabled;
	

	config["image_type"]		= int(m_image_type);

	if (is_root_node())
		config["node_config_field"] = "Root";
	else
		config["node_config_field"] = unmanaged_string(m_config_field).c_str();
	
//  	configs::lua_config_ptr tst = configs::create_lua_config("d:/lua.tst");
//  	(*tst)["m_conf"].assign_lua_value( *m_properties_config );
//  	tst->save();

	config["properties"].assign_lua_value(*m_properties_config);

	configs::lua_config_value children = config["children"];
	
	Int32 index = 0;
	for each(particle_graph_node^ node in this->children)
	{
		node->save(children[index++]);
	}	
}





static void copy_table_struct(configs::lua_config_value to_config, configs::lua_config_value from_config, String^ supertable_path)
{
	configs::lua_config_value::iterator from_config_begin	= from_config.begin( );
	configs::lua_config_value::iterator from_config_end		= from_config.end( );
	for (;from_config_begin != from_config_end;++from_config_begin){
		if ( from_config_begin.key( ).get_type( ) != configs::t_string )
			continue;
		pcstr begin_key = ( pcstr )from_config_begin.key( );
		if ( ( *from_config_begin ).get_type( ) == configs::t_table_indexed || ( *from_config_begin ).get_type( ) == configs::t_table_named ){
			String^ new_supertable_path = supertable_path + "." + gcnew String( begin_key ) ;
			to_config[ begin_key ].create_table( );
			
			copy_table_struct( to_config[ begin_key ], *from_config_begin, new_supertable_path);
		}
	}	
	
	to_config.add_super_table( "library/particle_configs/particle_source", unmanaged_string( supertable_path ).c_str() );	
}


static void fill_properties_table(	particle_graph_node^ particle_node,
									::property_holder* holder,
									configs::lua_config_value node_properties_config,
									configs::lua_config_value properties_config,
									configs::lua_config_value defaults_config)
{
	//node_properties_config->save("d:/node_properties_config.s");

	configs::lua_config_value::iterator props_begin		= properties_config.begin();
	configs::lua_config_value::iterator props_end		= properties_config.end();
	
// 	(*props_begin).save("d:/prb.s");
// 	defaults_config.save("d:/prb.s");
// 	properties_config.save("d:/prb.s");


	for(;props_begin != props_end;++props_begin){
		String^ prop_type	= gcnew String((*props_begin)["type"]);		
		String^ prop_name	= gcnew String((*props_begin)["name"]);
		String^ prop_desc	= gcnew String((*props_begin)["description"]);
		String^ prop_category = gcnew String((*props_begin)["category"]);
		property_holder::readonly_enum prop_readonly	= (bool)((*props_begin)["readonly"])? property_holder::property_read_only : property_holder::property_read_write;



		
		if (prop_type == "string"){
			String^ prop_value = gcnew String((node_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()]);

			holder->add_string		(prop_name, prop_category, prop_desc, prop_value, node_properties_config,
				prop_readonly,
				property_holder::do_not_notify_parent_on_change,
				property_holder::no_password_char,
				property_holder::do_not_refresh_grid_on_change);
		}
		else if (prop_type == "holder"){
			property_holder_ptr prop_value = NEW(::property_holder)((pcstr)unmanaged_string(prop_name).c_str(), NULL, NULL);

			::property_holder*			real_value = dynamic_cast<::property_holder*>(&*prop_value);

			fill_properties_table( particle_node, real_value, (node_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()], (*props_begin)["children_properties"] , (defaults_config)[(pcstr)unmanaged_string(prop_name).c_str()] );
				
			if ( (*props_begin).value_exists("special") )
				holder->add_special_property_holder(prop_name, prop_category, prop_desc, prop_value, gcnew String((*props_begin)["special"]), 
				prop_readonly,
				property_holder::do_not_notify_parent_on_change,
				property_holder::no_password_char,
				property_holder::do_not_refresh_grid_on_change);
			else
				holder->add_property_holder(prop_name, prop_category, prop_desc, prop_value, 
				prop_readonly,
				property_holder::do_not_notify_parent_on_change,
				property_holder::no_password_char,
				property_holder::do_not_refresh_grid_on_change);

		}
		else if (prop_type == "file"){
			String^ prop_value = gcnew String((node_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()]);
			String^ default_extension = gcnew String((*props_begin)["default_extension"]);
			String^ default_folder = gcnew String((*props_begin)["default_folder"]);
			String^ default_mask = gcnew String((*props_begin)["default_mask"]);
			String^ default_caption = gcnew String((*props_begin)["default_caption"]);

			holder->add_string		(prop_name, prop_category, prop_desc, prop_value, node_properties_config,
				default_extension,
				default_mask,
				default_folder,
				default_caption,
				property_holder::can_enter_text,
				property_holder::keep_extension,
				prop_readonly,				
				property_holder::do_not_notify_parent_on_change,
				property_holder::no_password_char,
				property_holder::do_not_refresh_grid_on_change);
		}
		else if (prop_type == "bool"){
			bool prop_value = (bool)((node_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()]);
		//	(node_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()].save("d:/ddd.aaa");
			holder->add_bool		(prop_name, prop_category, prop_desc, prop_value, node_properties_config, 
				prop_readonly,
				property_holder::do_not_notify_parent_on_change,
				property_holder::no_password_char,
				property_holder::do_not_refresh_grid_on_change);

		}
		else if (prop_type == "color"){
			configs::lua_config_value prop_value = (node_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()];
			if ((*props_begin).value_exists("can_have_data_source")&&particle_node->is_on_graph){

				particle_color_curve_data_source_holder^ curve_holder = gcnew particle_color_curve_data_source_holder(holder, node_properties_config ,particle_node, prop_name, prop_desc, prop_name+" Settings",(u32)((defaults_config)[(pcstr)unmanaged_string(prop_name).c_str()]));

				holder->add_string("DataSource", prop_name+" Settings",prop_desc,"Const",  
					gcnew string_getter_type(curve_holder, &particle_color_curve_data_source_holder::get_data_source_name),
					gcnew string_setter_type(curve_holder, &particle_color_curve_data_source_holder::set_data_source_name),
					gcnew Func<Int32, String^>(particle_node->parent_editor->data_sources_panel, &particle_data_sources_panel::get_color_curves),
					gcnew Func<Int32>(particle_node->parent_editor->data_sources_panel, &particle_data_sources_panel::get_color_curves_count),
					property_holder::value_editor_combo_box,
					property_holder::can_enter_text,
					property_holder::property_read_write,
					property_holder::do_not_notify_parent_on_change,
					property_holder::no_password_char,
					property_holder::do_not_refresh_grid_on_change);

				(node_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()] = (u32)((defaults_config)[(pcstr)unmanaged_string(prop_name).c_str()]);

				holder->add_color		(prop_name, prop_name+" Settings", prop_desc, color((u32)((defaults_config)[(pcstr)unmanaged_string(prop_name).c_str()])), node_properties_config, 
					prop_readonly,
					property_holder::do_not_notify_parent_on_change,
					property_holder::no_password_char,
					property_holder::do_not_refresh_grid_on_change);

				if (prop_value.get_type() == configs::t_string){
					curve_holder->set_data_source_name(gcnew String(prop_value));
				}
			}
			else{
				color prop_value = color((u32)((node_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()]));
				holder->add_color		(prop_name, prop_category, prop_desc, prop_value, node_properties_config, 
					prop_readonly,
					property_holder::do_not_notify_parent_on_change,
					property_holder::no_password_char,
					property_holder::do_not_refresh_grid_on_change);
			}
		}
		else if (prop_type == "float"){
			configs::lua_config_value prop_value = (node_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()];

			if ((*props_begin).value_exists("can_have_data_source")&&particle_node->is_on_graph){

				particle_float_curve_data_source_holder^ curve_holder = gcnew particle_float_curve_data_source_holder(holder, node_properties_config ,particle_node, prop_name, prop_desc, prop_name+" Settings",(float)((defaults_config)[(pcstr)unmanaged_string(prop_name).c_str()]));

				holder->add_string("DataSource", prop_name+" Settings",prop_desc,"Const",  
					gcnew string_getter_type(curve_holder, &particle_float_curve_data_source_holder::get_data_source_name),
					gcnew string_setter_type(curve_holder, &particle_float_curve_data_source_holder::set_data_source_name),
					gcnew Func<System::Int32, System::String^>(particle_node->parent_editor->data_sources_panel, &particle_data_sources_panel::get_float_curves),
					gcnew Func<System::Int32>(particle_node->parent_editor->data_sources_panel, &particle_data_sources_panel::get_float_curves_count),
					property_holder::value_editor_combo_box,
					property_holder::can_enter_text,
					property_holder::property_read_write,
					property_holder::do_not_notify_parent_on_change,
					property_holder::no_password_char,
					property_holder::do_not_refresh_grid_on_change);

				(node_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()] = (float)((defaults_config)[(pcstr)unmanaged_string(prop_name).c_str()]);

				holder->add_float		(prop_name, prop_name+" Settings", prop_desc, (float)((defaults_config)[(pcstr)unmanaged_string(prop_name).c_str()]), node_properties_config, 
					prop_readonly,
					property_holder::do_not_notify_parent_on_change,
					property_holder::no_password_char,
					property_holder::do_not_refresh_grid_on_change);

				if (prop_value.get_type() == configs::t_string){
					curve_holder->set_data_source_name(gcnew String(prop_value));
				}
			}
			else{
				float prop_value = (float)((node_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()]);
				holder->add_float		(prop_name, prop_category, prop_desc, prop_value, node_properties_config, 
					prop_readonly,
					property_holder::do_not_notify_parent_on_change,
					property_holder::no_password_char,
					property_holder::do_not_refresh_grid_on_change);
			}

		}
		else if (prop_type == "int"){
			int prop_value = (int)((node_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()]);

			holder->add_integer		(prop_name, prop_category, prop_desc, prop_value, node_properties_config, 
				prop_readonly,
				property_holder::do_not_notify_parent_on_change,
				property_holder::no_password_char,
				property_holder::do_not_refresh_grid_on_change);

		}		
		else if (prop_type == "float3"){
			float3 prop_value = (float3)((node_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()]);

			holder->add_vec3f(prop_name, prop_category, prop_desc, prop_value, node_properties_config, 
				prop_readonly,
				property_holder::do_not_notify_parent_on_change,
				property_holder::no_password_char,
				property_holder::do_not_refresh_grid_on_change);

		}
		else if (prop_type == "list"){
			String^ prop_value = gcnew String((node_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()]);

			configs::lua_config_value values_table = (*props_begin)["values_list"];

			System::Collections::ArrayList^ values = gcnew System::Collections::ArrayList();

			configs::lua_config_value::iterator values_begin		= values_table.begin();
			configs::lua_config_value::iterator values_end			= values_table.end();

			for(;values_begin != values_end;++values_begin){
				values->Add(gcnew String(*values_begin));
			}

			holder->add_string		(prop_name, prop_category, prop_desc, prop_value, node_properties_config, values,
				prop_readonly,
				property_holder::do_not_notify_parent_on_change,
				property_holder::no_password_char,
				property_holder::do_not_refresh_grid_on_change);

		} 
		else if (prop_type == "object_list"){   
			configs::lua_config_value values_table = (*props_begin)["values_list"];
			//(node_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()].assign_lua_value((defaults_config)[(pcstr)unmanaged_string(prop_name).c_str()])	;		   
			// 			config["default_values"].save("d:/lua.tst");
			// 			m_properties_config->save("d:/lua.tst");

			particle_object_list_holder^  obj_list_holder = gcnew particle_object_list_holder(particle_node, (node_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()],(defaults_config)[(pcstr)unmanaged_string(prop_name).c_str()],  prop_name, prop_desc, prop_name+" Settings");

			System::Collections::ArrayList^ values = gcnew System::Collections::ArrayList();

			configs::lua_config_value::iterator values_begin		= values_table.begin();
			configs::lua_config_value::iterator values_end			= values_table.end();

			for(;values_begin != values_end;++values_begin){
				values->Add(gcnew String(*values_begin));
			}
			holder->add_string		(prop_name, prop_name + " Settings", prop_desc, "POINT",
				gcnew string_getter_type(obj_list_holder, &particle_object_list_holder::get_value),
				gcnew string_setter_type(obj_list_holder, &particle_object_list_holder::set_value),
				values,
				property_holder::value_editor_combo_box,
				property_holder::cannot_enter_text,
				prop_readonly,
				property_holder::do_not_notify_parent_on_change,
				property_holder::no_password_char,
				property_holder::do_not_refresh_grid_on_change);

			obj_list_holder->set_value(gcnew String((node_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()]["default_value"]));	
		}
		else if (prop_type == "float_curve" && particle_node->is_on_graph){

			configs::lua_config_value prop_value = (node_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()];

			float_curve^ fc = nullptr;

			if (prop_value.get_type() == configs::t_string){
				fc = particle_node->parent_document->data_sources->get_float_curve_by_name(gcnew String(prop_value));
			}
			else
			{
				particle_data_source_fc_node^ node = gcnew particle_data_source_fc_node(particle_node->parent_editor);
				particle_node->parent_document->data_sources->add_data_source(node);

				node->Text = particle_node->Text+particle_node->parent_document->data_sources->float_curves->Count;

				(node_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()] = (pcstr)unmanaged_string(node->Text).c_str();

				fc = gcnew float_curve((particle_node->parent_document->data_sources->float_curves_config)[(pcstr)unmanaged_string(node->Text).c_str()]);
				node->fc = fc;
				node->is_on_graph					= false;

				fc->key_added += gcnew EventHandler<float_curve_event_args^>(node, &particle_data_source_fc_node::curve_changed);
				fc->key_altered += gcnew EventHandler<float_curve_event_args^>(node, &particle_data_source_fc_node::curve_changed);

				particle_node->parent_editor->data_sources_panel->add_new_control(node, particle_node->parent_editor->data_sources_panel->fc_page);
			}
			dynamic_cast<::property_holder*>(holder)->add_property( fc, prop_name, prop_category, prop_desc, prop_readonly,
				property_holder::do_not_notify_parent_on_change,
				property_holder::no_password_char,
				property_holder::do_not_refresh_grid_on_change);

		}
		else if (prop_type == "color_curve" && particle_node->is_on_graph){
			configs::lua_config_value prop_value = (node_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()];

			color_curve^ cc = nullptr;

			if (prop_value.get_type() == configs::t_string){
				cc = particle_node->parent_document->data_sources->get_color_curve_by_name(gcnew String(prop_value));
			}
			else{
				particle_data_source_cc_node^ node = gcnew particle_data_source_cc_node(particle_node->parent_editor);
				particle_node->parent_document->data_sources->add_data_source(node);
				node->Text = particle_node->Text+particle_node->parent_document->data_sources->color_curves->Count;
				(node_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()] = (pcstr)unmanaged_string(node->Text).c_str();

				cc = gcnew color_curve((particle_node->parent_document->data_sources->color_curves_config)[(pcstr)unmanaged_string(node->Text).c_str()]);

				node->cc = cc;
				node->is_on_graph					= false;

				cc->on_curve_changed += gcnew color_curve_changed(node, &particle_data_source_cc_node::curve_changed);
				particle_node->parent_editor->data_sources_panel->add_new_control(node, particle_node->parent_editor->data_sources_panel->cc_page);
			}
			dynamic_cast<::property_holder*>(holder)->add_property( cc, prop_name, prop_category, prop_desc, prop_readonly,
				property_holder::do_not_notify_parent_on_change,
				property_holder::no_password_char,
				property_holder::do_not_refresh_grid_on_change);
		};
	};
}

void particle_graph_node::load_node_properties(configs::lua_config_value config){
// 	if (!is_on_graph)
// 		return;
// 	


	if (m_properties_config->empty())
		copy_table_struct(*m_properties_config, config["default_values"], "particle_node_properties."+m_config_field+".default_values");


 	if ( !is_root_node()&&is_on_graph&&(is_emitter_node() || is_action_node())){
 
 		(*m_properties_config)["Delay"]		= 0.0f;
 		(*m_properties_config)["LifeTime"]	= 30.0f;
 
 		m_time_parameters_holder = gcnew particle_node_time_parameters_holder(this, *m_properties_config);
 	
 		m_property_holder->add_float("Delay", "Time Parameters", "Description", 0.0f, 
 			gcnew float_getter_type(m_time_parameters_holder, &particle_node_time_parameters_holder::get_start_time_impl),
 			gcnew float_setter_type(m_time_parameters_holder, &particle_node_time_parameters_holder::set_start_time_impl),
 			0.0f,
			700.0f,
			property_holder::property_read_write,
 			property_holder::do_not_notify_parent_on_change,
 			property_holder::no_password_char,
 			property_holder::do_not_refresh_grid_on_change);
 		m_property_holder->add_float("LifeTime", "Time Parameters", "Description", 30.0f, 
 			gcnew float_getter_type(m_time_parameters_holder, &particle_node_time_parameters_holder::get_length_time_impl),
 			gcnew float_setter_type(m_time_parameters_holder, &particle_node_time_parameters_holder::set_length_time_impl),
			0.0f,
			700.0f,
			property_holder::property_read_write,
 			property_holder::do_not_notify_parent_on_change,
 			property_holder::no_password_char,
 			property_holder::do_not_refresh_grid_on_change);
 	
 	}

	fill_properties_table( this, m_property_holder, *m_properties_config, config["properties_config"] , config["default_values"]);	
};




System::Void particle_graph_node::hypergraph_node_MouseClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e){
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
}


void particle_graph_node::add_children(particle_graph_node^ node){
	children->Add(node);
}

void particle_graph_node::remove_children(particle_graph_node^ node){
	children->Remove(node);
}

void particle_graph_node::set_enabled(bool enabled){
	m_enabled = enabled;
	if (m_enabled){
		process_node_selection();
		parent_hypergraph->Invalidate(true);
	}
	else{
		hide_children();
		this->Visible = true;
		parent_hypergraph->Invalidate(true);
	}
	
}

void particle_graph_node::recalc_children_position_offsets(){
	for each(particle_graph_node^ child_node in children){
		child_node->recalc_parent_position_offset();
	}
	m_on_position_changed(this);
}

void particle_graph_node::recalc_parent_position_offset(){
	int group_offset = 0;
	int self_index		= m_parent_node->children->IndexOf(this);
	if (self_index > (m_parent_node->children->last_action_id - 1) + m_parent_node->children->last_property_id){
		if (m_parent_node->children->last_property_id > 0)
			group_offset = group_offset + 16;
		if (m_parent_node->children->last_action_id > 0)
			group_offset = group_offset + 16;					
	}
	else if (self_index > m_parent_node->children->last_property_id - 1){
		if (m_parent_node->children->last_property_id > 0)
			group_offset = group_offset + 16;
	}
	m_parent_position_offset = Point(m_parent_node->size.Width + width_between_nodes, self_index*(height_between_nodes + size.Height) + group_offset);

}

void particle_graph_node::recalculate_position(node^ node){

	R_ASSERT(node == m_parent_node);
	
	position = Drawing::Point(m_parent_node->position.X + m_parent_position_offset.X, m_parent_node->position.Y + m_parent_position_offset.Y);	
}

void particle_graph_node::set_parent_node_impl(particle_graph_node^ node){
	if (m_parent_node == node)
		return;	
	
	if (node != nullptr)
		node->m_on_position_changed += gcnew controls::hypergraph::position_changed(this, &particle_graph_node::recalculate_position);
	else
		m_parent_node->m_on_position_changed -= gcnew controls::hypergraph::position_changed(this, &particle_graph_node::recalculate_position);
	
	m_parent_node = node;
	
}

particle_graph_document^				particle_graph_node::parent_document::get()
{
	return safe_cast<particle_graph_document^>(parent_editor->multidocument_base->active_document);
}

}//namespace editor
}//namespace xray