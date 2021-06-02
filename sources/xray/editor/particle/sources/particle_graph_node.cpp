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
#include "particle_system_config_wrapper.h"
#include "particle_graph_node_collection.h"
#include "particle_editor.h"
#include "particle_hypergraph.h"
#include "particle_node_properties.h"
#include "particle_graph_document.h"
#include "add_delete_group_nodes.h"
#include <xray/configs.h>
#include "images/images30x25.h"
#include "particle_sources_panel.h"
#include <xray/strings_stream.h>


using namespace System;
using namespace System::Reflection;
using namespace System::Windows::Forms;

using xray::particle_editor::particle_graph_node;
using xray::editor::controls::hypergraph::connection_point;
using xray::editor::controls::hypergraph::point_align;
using xray::editor::controls::hypergraph::view_mode;

namespace xray {
namespace particle_editor {

void					particle_graph_node::in_constructor		()
{
	m_node_config					= CRT_NEW(xray::configs::lua_config_value)((*xray::configs::create_lua_config()).get_root()["node_config"]);
	
	this->AllowDrop					= true;
	m_drag_start_distance			= 15.0f;
	m_is_on_graph					= true;
	m_children						= gcnew particle_graph_node_collection(this);
	m_active_node					= nullptr;
	m_parent_node					= nullptr;
	m_highlighted					= false;
	this->BackColor					= System::Drawing::Color::Transparent;
	this->ForeColor					= System::Drawing::Color::Transparent;
	m_parent_position_offset		= Point( 0, 0 );
	properties						= gcnew particle_node_properties(this);

	id								= System::Guid::NewGuid().ToString();
	image_type						= image_node_disabled;
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
			document_base^ parent_document = safe_cast<document_editor_base^>(safe_cast<DockContent^>(ParentForm)->DockPanel->Parent)->active_document; 
			particle_graph_document^ document = safe_cast<particle_graph_document^>(parent_document);
			if (document != nullptr)
				document->hypergraph->hide_available_nodes(this);
		}

	}
	
	if(is_on_graph){
		xray::editor::controls::hypergraph::node::OnMouseMove(e);
	}
}

void					particle_graph_node::OnMouseDown		(MouseEventArgs^ e)
{
	
	if (e->Button == Windows::Forms::MouseButtons::Middle)
		return;

	if(is_on_graph){		
		xray::editor::controls::hypergraph::node::OnMouseDown(e);
		safe_cast<particle_hypergraph^>(parent_hypergraph)->show_available_nodes(this);		
	}
	else
	{
		document_base^ parent_document = safe_cast<document_editor_base^>(safe_cast<DockContent^>(ParentForm)->DockPanel->Parent)->active_document; 
		particle_graph_document^ document = safe_cast<particle_graph_document^>(parent_document);
		if (document != nullptr)
			document->hypergraph->show_available_nodes(this);
	}
	m_is_mouse_down				= true;
	m_last_mouse_down_position	= e->Location;

	
}

void					particle_graph_node::OnMouseUp			(MouseEventArgs^ e)
{
	if (e->Button == Windows::Forms::MouseButtons::Middle)
		return;

	if(is_on_graph){
		xray::editor::controls::hypergraph::node::OnMouseUp(e);	
		safe_cast<particle_hypergraph^>(parent_hypergraph)->hide_available_nodes(this);
		process_node_selection();		
	}
	else{
		document_base^ parent_document = safe_cast<document_editor_base^>(safe_cast<DockContent^>(ParentForm)->DockPanel->Parent)->active_document; 
		particle_graph_document^ document = safe_cast<particle_graph_document^>(parent_document);
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
	return (enabled && !is_parent(this, node) && node->parent_node != this);
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
		safe_cast<particle_hypergraph^>(parent_hypergraph)->comm_engine->run(
			gcnew particle_graph_commands::add_group(safe_cast<particle_hypergraph^>(parent_hypergraph), config, id)
		);
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

static int lod_nodes_offset		= 5;
static int width_between_nodes	= 30;
static int height_between_nodes	 = 0;


void					particle_graph_node::OnPaint			(PaintEventArgs^ e)
{
	
	images30x25 mini_image = images30x25_emitter;
	if (image_type == image_node_emitter)
		if(is_root_node())
			mini_image = images30x25_root;
		else
			mini_image = images30x25_emitter;
	else if (image_type == image_node_render)
		mini_image = images30x25_render;
	else if (image_type == image_node_color)
		mini_image = images30x25_color;
	else if (image_type == image_node_size)
		mini_image = images30x25_size;
	else if (image_type == image_node_velocity)
		mini_image = images30x25_velocity;
	else if (image_type == image_node_rotation)
		mini_image = images30x25_rotation;
	else if (image_type == image_node_rotaion_rate)
		mini_image = images30x25_rotaion_rate;
	else if (image_type == image_node_location)
		mini_image = images30x25_location;
	else if (image_type == image_node_event)
		mini_image = images30x25_event;
	else if (image_type == image_node_misc)
		mini_image = images30x25_misc;
	if(m_parent != nullptr){

		ImageList^ particle_node_images = safe_cast<particle_graph_node_style_mgr^>(parent_hypergraph->nodes_style_manager)->images;
		ImageList^ particle_node_mini_images = safe_cast<particle_graph_node_style_mgr^>(parent_hypergraph->nodes_style_manager)->mini_images;
		
		if (!enabled)
			e->Graphics->DrawImage(particle_node_images->Images[image_node_disabled], Drawing::Point(0,0));			
		
		if(m_parent->selected_nodes->Contains(this))
			e->Graphics->DrawImage(particle_node_images->Images[image_node_selected], Drawing::Point(0,0));			
		
		e->Graphics->DrawImage(particle_node_mini_images->Images[mini_image], Drawing::Point(-2,0));
	}
	else
	{
		particle_editor^ editor = safe_cast<particle_editor^>(m_parent_editor);
		e->Graphics->DrawImage(editor->node_style_mgr->mini_images->Images[mini_image], Drawing::Point(-2,0));		
	}
	e->Graphics->DrawString		(	node_text,
									gcnew Drawing::Font("Arial", 7,FontStyle::Bold ),
									gcnew SolidBrush(Drawing::Color::Black),
									Drawing::Point(25,7)
								);

	if (parent_lod_name != nullptr)
	{
		e->Graphics->DrawString		(	parent_lod_name,
										gcnew Drawing::Font("Arial", 5,FontStyle::Bold ),
										gcnew SolidBrush(Drawing::Color::White),
										Drawing::Point(Width - 25,6)
									);
	}
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
	if((m_children == nullptr) || (m_children->Count == 0) || !enabled){
		Visible = true;
		return;
	}
	if ((m_parent_node->m_active_node != this)){
		Visible = true;
		return;
	}

	for each(String^ n_id in m_children) {
		particle_graph_node^ n = m_children->nodes_dictionary[n_id];
		n->show_children();		
	}
	Visible = true;
}

void particle_graph_node::enabled::set(bool value){
	(*properties->config)["Enabled"] = (value);

	if (parent_document != nullptr){
		configs::lua_config_value outgoing	= configs::create_lua_config()->get_root();
		outgoing["Enabled"] = (value);
		parent_document->particle_system_property_changed(this, outgoing);
	}

	if (value){
		process_node_selection();
		parent_hypergraph->Invalidate(true);
	}
	else{
		hide_children();
		this->Visible = true;
		parent_hypergraph->Invalidate(true);
	}					
}

void					particle_graph_node::hide_children()
{	
	if((m_children == nullptr)||(m_children->Count == 0)){
		Visible = false;
		return;
	}

	for each(String^ n_id in m_children) {
		particle_graph_node^ n = m_children->nodes_dictionary[n_id];
		n->hide_children();		
	}
	Visible = false;
}

void					particle_graph_node::process_node_selection(){
	if (m_parent_node == nullptr || !enabled || children->Count == 0)
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

void					particle_graph_node::get_initialize_config(configs::lua_config_value config){
	
	config.add_super_table	(	"library/particle_configs/particle_source.library", 
								unmanaged_string("particle_entity_types." + particle_entity_type ).c_str() );

	config["properties"].assign_lua_value(*(properties->config));
	config["id"] = unmanaged_string(id).c_str();

	configs::lua_config_value children			= config["children"];
	configs::lua_config_value children_order	= config["children_order"];

	Int32 index = 0;
	for each(String^ n_id in m_children) {
		particle_graph_node^ node = m_children->nodes_dictionary[n_id];
		children_order[index++] = unmanaged_string(n_id).c_str();
		node->get_initialize_config(children[unmanaged_string(n_id).c_str()]);
	}	
}

static void get_final_lua_config_value(	xray::configs::lua_config_value const& from_value, 
										xray::configs::lua_config_value final_value)
{
 	using namespace xray::configs;
	lua_config_value supertable_ids = from_value.get_super_table_ids_table();

	bool defaults_supertable_exists = false;
	lua_config_value::iterator supertable_ids_begin		= supertable_ids.begin();
	lua_config_value::iterator supertable_ids_end		= supertable_ids.end();
	for(;supertable_ids_begin != supertable_ids_end;++supertable_ids_begin)
	{
		String^ supertable_path = gcnew String(*supertable_ids_begin);
		if (supertable_path->Contains("particle_source.library")){
			defaults_supertable_exists = true;
			break;
		}
	}
	if (!defaults_supertable_exists)
	{
		lua_config_value super_tables = from_value.get_super_tables_table();
		lua_config_value::iterator begin	= super_tables.begin();
		lua_config_value::iterator end		= super_tables.end();
		for(;begin != end;++begin){
			get_final_lua_config_value(*begin, final_value);
		}
	}
	else if (from_value.has_super_tables())
	{
		final_value.copy_super_tables(from_value);	
	}

	lua_config_value::iterator begin	= from_value.begin();
	lua_config_value::iterator end		= from_value.end();
	int index = 0;
	for(;begin != end;++begin){
		if ((*begin).get_type() == t_table_indexed || (*begin).get_type() == t_table_named)
			continue;

		if((from_value).get_type() == configs::t_table_indexed)
		{
			final_value[index].assign_lua_value(from_value[index]);
			index++;
		}
		else{
			final_value[(pcstr)(begin.key())].assign_lua_value(from_value[(pcstr)(begin.key())]);		
		}

	}
}	


static void get_config_with_default_inheritance(configs::lua_config_value base_config, configs::lua_config_value result_config)
{
	using namespace configs;
	if (base_config.get_type() != t_table_indexed &&  base_config.get_type() != t_table_named)
		return;
	lua_config_value::iterator begin	= base_config.begin();
	lua_config_value::iterator end		= base_config.end();
	int index = 0;
	for(;begin != end;++begin){
		if ((*begin).get_type() != t_table_indexed && (*begin).get_type() != t_table_named)
			continue;
		if((base_config).get_type() == configs::t_table_indexed)
		{
			get_config_with_default_inheritance(*begin, result_config[index]);
			index++;
		}
		else
			get_config_with_default_inheritance(*begin, result_config[(pcstr)begin.key()]);
	}
	get_final_lua_config_value(base_config, result_config);	
}



void					particle_graph_node::load(configs::lua_config_value config)
{
	
	m_node_config->assign_lua_value(config);

	
	configs::lua_config_value children_order_incoming	= (*m_node_config)["children_order"];

	configs::lua_config_value children_order = configs::create_lua_config()->get_root();
	
	configs::lua_config_value::iterator begin	= children_order_incoming.begin();
	configs::lua_config_value::iterator end		= children_order_incoming.end();

	configs::lua_config_value children			= (*m_node_config)["children"];

	SuspendLayout();

	for(;begin != end;++begin)
	{
		configs::lua_config_value child_node_config = children[(pcstr)(*begin)];
		particle_graph_node^ new_node	= parent_editor->deserialize_particle_node_from_config( child_node_config );
				
		new_node->m_style				= this->m_style;		
		this->children->Add				(new_node);
		new_node->load					( child_node_config );
		this->recalc_children_position_offsets();
	}

	

	properties->config->assign_lua_value((*m_node_config)["properties"]);
	
	configs::lua_config_value val = 
		m_parent_editor->particles_library_config["particle_nodes"][(pcstr)unmanaged_string(node_config_field).c_str()]["properties"];
	
	properties->load_properties( &val, "particle_node_properties." + node_config_field);
	
	enabled = enabled;
	
	ResumeLayout(false);	
}

void					particle_graph_node::clone_from_config(configs::lua_config_value config)
{
	configs::lua_config_value children_order_incoming	= (*m_node_config)["children_order"];

	configs::lua_config_value children_order = configs::create_lua_config()->get_root();
	
	configs::lua_config_value::iterator begin	= children_order_incoming.begin();
	configs::lua_config_value::iterator end		= children_order_incoming.end();

	configs::lua_config_value children			= (*m_node_config)["children"];

	SuspendLayout();

	for(;begin != end;++begin)
	{
		configs::lua_config_value child_node_config = children[(pcstr)(*begin)];
		particle_graph_node^ new_node	= parent_editor->deserialize_particle_node_from_config( child_node_config );
		
		//new_node->id = Guid::NewGuid().ToString();
		
		new_node->m_style				= this->m_style;

		new_node->node_config->assign_lua_value(child_node_config);	
		new_node->id = Guid::NewGuid().ToString();

		this->children->Add				(new_node);
		new_node->clone_from_config		( child_node_config );
		new_node->parent_lod_name		= parent_document->lod_name;
		this->recalc_children_position_offsets();
	}

	properties->config->assign_lua_value((*m_node_config)["properties"]);

	if (!is_root_node()){		
		configs::lua_config_value val = 
			m_parent_editor->particles_library_config["particle_nodes"][(pcstr)unmanaged_string(node_config_field).c_str()]["properties"];

		properties->load_properties( &val, "particle_node_properties." + node_config_field);
	}

	ResumeLayout(false);


}


void					particle_graph_node::save(configs::lua_config_value config)
{
	config.assign_lua_value(m_node_config->copy());	
}

void					particle_graph_node::save_with_only_default_inheritance(configs::lua_config_value config)
{	
	get_config_with_default_inheritance(*m_node_config, config);	
//	config.save_as("d:/result_config");
}

System::Void particle_graph_node::hypergraph_node_MouseClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e){
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
}


void particle_graph_node::add_children(particle_graph_node^ node){
	children->Add(node);
}

void particle_graph_node::remove_children(particle_graph_node^ node){
	children->Remove(node);
}

void particle_graph_node::recalc_children_position_offsets(){
	for each(String^ n_id in m_children){
		particle_graph_node^ n = m_children->nodes_dictionary[n_id];
		n->recalc_parent_position_offset();
	}
	m_on_position_changed(this);
}

void particle_graph_node::recalc_parent_position_offset(){
	int group_offset = 0;
	int self_index		= m_parent_node->children->IndexOf(this->id);
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
	int horizontal_offset = width_between_nodes;
		
	if (parent_document->lod_name == parent_lod_name && parent_document->lod_name != "LOD 0")
		horizontal_offset += lod_nodes_offset;

	m_parent_position_offset = Point(	m_parent_node->size.Width + horizontal_offset,
										self_index*(height_between_nodes + size.Height) + group_offset);

}

void particle_graph_node::recalculate_position(node^ node){

	R_ASSERT(node == m_parent_node);
	
	position = Drawing::Point(node->position.X + m_parent_position_offset.X, node->position.Y + m_parent_position_offset.Y);	
}

void particle_graph_node::set_parent_node_impl(particle_graph_node^ node){
	if (m_parent_node == node)
		return;	
	
	using namespace xray::editor::controls::hypergraph;

	if (node != nullptr)
		node->m_on_position_changed += gcnew hypergraph::position_changed(this, &particle_graph_node::recalculate_position);
	else
		m_parent_node->m_on_position_changed -= gcnew hypergraph::position_changed(this, &particle_graph_node::recalculate_position);
	
	m_parent_node = node;
	
}

particle_graph_document^				particle_graph_node::parent_document::get()
{
	if (is_on_graph){
		particle_hypergraph^ parent_hyp = safe_cast<particle_hypergraph^>(Parent);
		particle_graph_document^ parent_doc = safe_cast<particle_graph_document^>((parent_hyp->Parent)->Parent);
		return parent_doc;
	}
	return nullptr;
	//return safe_cast<particle_graph_document^>(parent_editor->multidocument_base->active_document);
}

void	particle_graph_node::show_properties									(){
	parent_editor->show_properties(properties->container);
//	String^ path = get_node_parents_path(this);
}

static String^ get_node_parents_path_and_list(particle_graph_node^ node,  List<String^>^ lst)
{ 
	String^ node_path = gcnew String("");
	while( node->parent_node != nullptr )
	{	

		lst->Insert(0, node->id);
		lst->Insert(0, "children");
		node_path = ".children[\"" + node->id + "\"]" + node_path;
		node = node->parent_node;				
	}
	return node_path;

}

static configs::lua_config_value get_value_from_list_path(configs::lua_config_value value_config, 
														   List<String^>^	list_path)
{
	for each(String^ field in list_path)
	{
		if (value_config.value_exists(unmanaged_string(field).c_str()))
			value_config = value_config[unmanaged_string(field).c_str()];
		else
			value_config = value_config[unmanaged_string(field).c_str()].create_table();
	}
	return value_config;
}

static void inherit_table	(
								 configs::lua_config_value& root_config,
								 configs::lua_config_value& from_config,
								 configs::lua_config_value& to_config,
								 String^ supertable_path
								 )
{
	pcstr field_id = from_config.get_field_id();
	if (strings::equal(field_id, "children_order"))
		to_config.assign_lua_value(from_config.copy());
	else
		to_config.add_super_table			(
		root_config,
		unmanaged_string( supertable_path ).c_str( )
		);	

	using configs::lua_config_value;
	lua_config_value::iterator i		= from_config.begin( );
	lua_config_value::iterator const e	= from_config.end( );
	for (;i != e;++i){
		if ( i.key( ).get_type( ) != configs::t_string )
			continue;

		if ( ((*i).get_type() != configs::t_table_indexed) && ((*i).get_type() != configs::t_table_named) )
			continue;

		pcstr const key					= static_cast<pcstr>( i.key() );
		String^ new_supertable_path		= supertable_path + String::Format("[\"{0}\"]", gcnew String(key) );
		lua_config_value lod_current	= from_config[ key ];
		lua_config_value lod_next		= to_config[ key ];
		lod_next.create_table			( );
		inherit_table					(
			root_config,
			lod_current,
			lod_next,
			new_supertable_path
			);
	}
}


void particle_graph_node::check_for_reload_lower_lodes()
{
	configs::lua_config_ptr config = parent_editor->particle_configs["particles/" + parent_document->source_path]->c_ptr();
	if (!config->value_exists("documents_needs_to_reload"))
		(*config)["documents_needs_to_reload"].create_table();
	
	int current_lod_num = int::Parse(parent_document->lod_name->Substring(parent_document->lod_name->Length - 1, 1 ));

	for each(document_base^ doc in parent_editor->multidocument_base->opened_documents)
	{
		particle_graph_document^ particle_doc = safe_cast<particle_graph_document^>(doc);
		if (particle_doc->source_path == parent_document->source_path)
		{
			int doc_lod_num = int::Parse(particle_doc->lod_name->Substring(particle_doc->lod_name->Length - 1, 1 ));
			if (doc_lod_num > current_lod_num)
				(*config)["documents_needs_to_reload"][unmanaged_string(particle_doc->lod_name).c_str()] = true;
		}
	}
	//config->save_as("d:/after_check");
}

void	particle_graph_node::on_new_node_added									(particle_graph_node^ node, int index){
	List<String^>^				list_path			= gcnew List<String^>();
	List<String^>^				parent_list_path	= gcnew List<String^>();
	String^						string_path			= get_node_parents_path_and_list(node, list_path);
	String^						current_lod_name	= parent_document->lod_name;
	int							current_lod_num		= int::Parse(current_lod_name->Substring(current_lod_name->Length - 1, 1 ));
	configs::lua_config_ptr		config				= parent_editor->particle_configs["particles/" + parent_document->source_path]->c_ptr();
	configs::lua_config_value&	root				= config->get_root( );
	get_node_parents_path_and_list(this, parent_list_path);
	current_lod_num++;
			
	while (config->value_exists(unmanaged_string(gcnew String("LOD " + current_lod_num)).c_str()))
	{
		configs::lua_config_value parent_value_config = (*config)[unmanaged_string(gcnew String("LOD " + current_lod_num)).c_str()];	
		//parent_value_config.save_as("d:/parent_conf");
		parent_value_config	= get_value_from_list_path(parent_value_config, parent_list_path);
		//parent_value_config.save_as("d:/parent_conf");
		configs::lua_config_value new_config_value = parent_value_config["children"][unmanaged_string(node->id).c_str()].create_table();
		inherit_table(root, *node->node_config, new_config_value, String::Format("[\"LOD {0}\"]", (current_lod_num-1)) + string_path);
		//parent_value_config.save_as("d:/parent_conf");
		if (index == this->children->Count){
			parent_value_config["children_order"][parent_value_config["children_order"].size() + 1] = unmanaged_string(node->id).c_str();		
			//parent_value_config.save_as("d:/parent_conf");
		}
		else
		{

			String^ next_node_id = this->children[index];

			configs::lua_config_value parent_children = parent_value_config["children_order"];
			
			List<String^>^ children_order_list = gcnew List<String^>();

			configs::lua_config_value::iterator i		= parent_value_config["children_order"].begin( );
			configs::lua_config_value::iterator const e	= parent_value_config["children_order"].end( );
			for (;i != e;++i){
				String^ node_id = gcnew String(*i);
				if (node_id == next_node_id)
					children_order_list->Add(node->id);
				children_order_list->Add(gcnew String(*i));
			}

			int index = 0;
			parent_value_config["children_order"].clear();
			for each(String^ node_id in children_order_list)
			{
				parent_value_config["children_order"][++index] = unmanaged_string(node_id).c_str();
			}
		
		}		
		current_lod_num++;
	}
	check_for_reload_lower_lodes();
}

void	particle_graph_node::on_node_removed									(particle_graph_node^ node){
	List<String^>^				parent_list_path	= gcnew List<String^>();
	String^						current_lod_name	= parent_document->lod_name;
	int							current_lod_num		= int::Parse(current_lod_name->Substring(current_lod_name->Length - 1, 1 ));
	configs::lua_config_ptr		config				= parent_editor->particle_configs["particles/" + parent_document->source_path]->c_ptr();
	get_node_parents_path_and_list(this, parent_list_path);
	current_lod_num++;
	while (config->value_exists(unmanaged_string(gcnew String("LOD " + current_lod_num)).c_str()))
	{
		configs::lua_config_value parent_value_config = (*config)[unmanaged_string(gcnew String("LOD " + current_lod_num)).c_str()];	
		parent_value_config	= get_value_from_list_path(parent_value_config, parent_list_path);
		parent_value_config["children"].erase(unmanaged_string(node->id).c_str());
		
		List<String^>^ children_order_list = gcnew List<String^>();

		configs::lua_config_value::iterator i		= parent_value_config["children_order"].begin( );
		configs::lua_config_value::iterator const e	= parent_value_config["children_order"].end( );
		for (;i != e;++i){
			children_order_list->Add(gcnew String(*i));
		}
		children_order_list->Remove(node->id);

		int index = 0;
		parent_value_config["children_order"].clear();
		for each(String^ node_id in children_order_list)
		{
			parent_value_config["children_order"][++index] = unmanaged_string(node_id).c_str();
		}

		current_lod_num++;
	}

	check_for_reload_lower_lodes();
}

// static void swap_nodes(	particle_graph_node^ parent_node,
// 						configs::lua_config_ptr	config,
// 						List<String^>^ parent_list_path, 
// 						String^ insert_after_node_id,
// 						String^ moved_node_id, 						
// 						String^ moved_node_next_id, 
// 						int lod_num)
//  {	
// 	
// 	 
// 	if (config->value_exists(unmanaged_string(gcnew String("LOD " + lod_num)).c_str()))
// 	{
//  		configs::lua_config_value parent_value_config = (*config)[unmanaged_string(gcnew String("LOD " + lod_num)).c_str()];	
// 
//  		parent_value_config	= get_value_from_list_path(parent_value_config, parent_list_path);
// 
//  		List<String^>^ children_order_list			= gcnew List<String^>();
//  		configs::lua_config_value::iterator i		= parent_value_config["children_order"].begin( );
//  		configs::lua_config_value::iterator const e	= parent_value_config["children_order"].end( );
//  		for (;i != e;++i){
//  			children_order_list->Add(gcnew String(*i));
//  		}	
// 		int moved_node_index = children_order_list->IndexOf(moved_node_id);
// 		int moved_node_next_index = children_order_list->IndexOf(moved_node_next_id);
//  		for (int i = moved_node_index; i<moved_node_next_index;i++)
//  		{
// 			swap_nodes(parent_node,
//  				config,
//  				parent_list_path, 
//  				insert_after_node_id,
//  				children_order_list[i],
//  				children_order_list[i+1],
//  				lod_num + 1);
// 			
// 			String^ current_node_id = children_order_list[i];
// 
// 			children_order_list->Remove(current_node_id);
// 			children_order_list->Insert(children_order_list->IndexOf(insert_after_node_id) + 1,current_node_id);
// 		}
// 		
// 		int index = 0;
// 		parent_value_config["children_order"].clear();
// 		for each(String^ node_id in children_order_list)
// 		{
// 			parent_value_config["children_order"][++index] = unmanaged_string(node_id).c_str();
// 		}
// 		lod_num++;
// 	
//  	}	
// 	
// }

void particle_graph_node::on_change_node_index(String^ moved_node_id, String^ moved_to_node_id )
{
	bool moving_up = children->IndexOf(moved_to_node_id) < children->IndexOf(moved_node_id);
	
	List<String^>^				parent_list_path	= gcnew List<String^>();
	String^						current_lod_name	= parent_document->lod_name;
	int							current_lod_num		= int::Parse(current_lod_name->Substring(current_lod_name->Length - 1, 1 ));
	configs::lua_config_ptr		config				= parent_editor->particle_configs["particles/" + parent_document->source_path]->c_ptr();
	get_node_parents_path_and_list(this, parent_list_path);
	current_lod_num++;
	
	String^ moved_node_next_id = nullptr;
	//int moved_to_node_index = children->IndexOf(moved_to_node_id);
	//int children_count = children->Count;
	if (children->IndexOf(moved_node_id) < children->Count - 1)
		moved_node_next_id = children[children->IndexOf(moved_node_id) + 1];

	String^ insert_before_node_id = nullptr;
	if (moving_up)
		insert_before_node_id = moved_to_node_id;
	else
		if (moved_node_next_id != children[children->Count - 1])
			insert_before_node_id = children[children->IndexOf(moved_node_next_id) + 1];

	while (config->value_exists(unmanaged_string(gcnew String("LOD " + current_lod_num)).c_str()))
	{
		configs::lua_config_value parent_value_config = (*config)[unmanaged_string(gcnew String("LOD " + current_lod_num)).c_str()];	

		parent_value_config	= get_value_from_list_path(parent_value_config, parent_list_path);

		List<String^>^ children_order_list			= gcnew List<String^>();
		configs::lua_config_value::iterator i		= parent_value_config["children_order"].begin( );
		configs::lua_config_value::iterator const e	= parent_value_config["children_order"].end( );
		for (;i != e;++i){
			children_order_list->Add(gcnew String(*i));
		}	

		List<String^>^ children_order_list_to_move	= gcnew List<String^>();

		int i1 = children_order_list->IndexOf(moved_node_id);
		int i2 = (!!moved_node_next_id) ? (children_order_list->IndexOf( moved_node_next_id ) - 1): (children_order_list->Count - 1);
		
		int index = i1;

		for (;index<=i2; index++)
		{
			children_order_list_to_move->Add(children_order_list[index]);
		}
		children_order_list->RemoveRange(i1, children_order_list_to_move->Count);

		int insert_index = (!!insert_before_node_id) ? children_order_list->IndexOf(insert_before_node_id) : children_order_list->Count;

		children_order_list->InsertRange(insert_index, children_order_list_to_move);
		
		index = 0;
		parent_value_config["children_order"].clear();
		for each(String^ node_id in children_order_list)
		{
			parent_value_config["children_order"][++index] = unmanaged_string(node_id).c_str();
		}
		current_lod_num++;
	}
	
	
	//swap_nodes(this, config,parent_list_path, insert_after_node_id, moved_node_id, children[children->IndexOf(moved_node_id) + 1], current_lod_num);
	check_for_reload_lower_lodes();

}

void particle_graph_node::notify_render_on_property_changed(cli::array<System::String^>^ names)
{
	for each (String^ string_path in names){
		configs::lua_config_value changes_config = configs::create_lua_config()->get_root();
		array<String^>^ str_fields = string_path->Split('/');

		configs::lua_config_value changes_config_value = changes_config;
		configs::lua_config_value node_config_value = (*this->properties->config);
		int checked_length = str_fields->Length - 1;
		if (str_fields->Length > 1 && str_fields[str_fields->Length - 1] == "")
		{
			checked_length--;
		}

		for (int i = 0; i<= checked_length; i++)
		{
			//node_config_value.save_as("d:/ncv");
			String^ current_field = str_fields[i];
			node_config_value = node_config_value[unmanaged_string(current_field).c_str()];
			//node_config_value.save_as("d:/ncv1");

			bool is_table_value =	(node_config_value.get_type() == configs::t_table_named)||
									(node_config_value.get_type() == configs::t_table_indexed);

			if ( i < checked_length )
				changes_config_value = changes_config_value[unmanaged_string(current_field).c_str()].create_table();
			else if( is_table_value && node_config_value.empty() )
				changes_config_value[unmanaged_string(current_field).c_str()].assign_lua_value(configs::create_lua_config()->get_root());
			else
				changes_config_value[unmanaged_string(current_field).c_str()].assign_lua_value(node_config_value.copy());
		}
		parent_document->particle_system_property_changed(this, changes_config);
	}
}

}//namespace particle_editor
}//namespace xray