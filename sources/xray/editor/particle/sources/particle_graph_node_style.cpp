////////////////////////////////////////////////////////////////////////////
//	Created		: 20.01.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_graph_node_style.h"
#include "images/images30x25.h"
#include "images/images92x25.h"


using xray::particle_editor::particle_graph_node_style;
using xray::particle_editor::particle_graph_node_style_mgr;
using xray::editor::controls::hypergraph::view_mode;
using xray::editor::controls::hypergraph::point_align;
using xray::editor::controls::hypergraph::connection_type;
using xray::editor::controls::hypergraph::connection_point_style;


using namespace System;
using namespace System::Windows::Forms;
////////////////////////////////////////////////////////////////////////////
//- class scene_graph_nodes_style_mgr -------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
namespace xray{
namespace particle_editor{

particle_graph_node_style_mgr::particle_graph_node_style_mgr()
{
	m_library		= gcnew System::Collections::ArrayList();
	m_images		= xray::editor_base::image_loader::load_images("images92x25", "particle_editor.resources", 92, safe_cast<int>(images92x25_count), this->GetType()->Assembly);
	m_mini_images	= xray::editor_base::image_loader::load_images("images30x25", "particle_editor.resources", 30, safe_cast<int>(images30x25_count), this->GetType()->Assembly);	
}

void particle_graph_node_style_mgr::assign_style(node^ n, System::String^ style_name)
{
	node_style^ style = nullptr;
	for each(node_style^ st in m_library)
	{
		if(st->name==style_name)
		{
			style = st;
			break;
		}
	}
	if(style==nullptr)
		style = gcnew node_style();

	n->assign_style(style);
}

void particle_graph_node_style_mgr::load( configs::lua_config_value config)
{
	configs::lua_config_value t	= config;
	particle_graph_node_style^ style = gcnew particle_graph_node_style();
	style->load(t);
	m_library->Add(style);	
}

////////////////////////////////////////////////////////////////////////////
//- class scene_graph_node_style ------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
void particle_graph_node_style::load(xray::configs::lua_config_value const& t)
{
	name = gcnew System::String(t["name"]);
	int width = t["width"];
	int height = t["height"];
	full_size = System::Drawing::Size(width, height);
	m_color_node_active = System::Drawing::Color::Yellow;
	m_color_node_highlighted = System::Drawing::Color::White;
	m_color_node_inactive = System::Drawing::Color::Transparent;
	mode = view_mode::full;
	fmt->Alignment = System::Drawing::StringAlignment::Center;
	fmt->LineAlignment = System::Drawing::StringAlignment::Center;

	xray::configs::lua_config_value::iterator b = t["connection_styles"].begin();
	xray::configs::lua_config_value::iterator e = t["connection_styles"].end();
	for(; b!=e; ++b)
	{
		connection_point_style^ new_style = gcnew connection_point_style();
		new_style->name = gcnew System::String((*b)["name"]);
		System::String^ d = gcnew System::String((*b)["dir"]);
// 		if(d=="input")
// 			new_style->dir = connection_type::input;
// 		else if(d=="output")
// 			new_style->dir = connection_type::output;

		new_style->dir		= safe_cast<connection_type>(System::Enum::Parse(connection_type::typeid, d));

		System::String^ a = gcnew System::String((*b)["name_align"]);
		if(a=="right")
		{
			new_style->al = point_align::left;
			new_style->base_rect = System::Drawing::Rectangle(0, height-18, 10, 10);
		}
		else if(a=="bottom")
		{
			new_style->al = point_align::top;
			new_style->base_rect = System::Drawing::Rectangle((width-10)/2, 0, 10, 10);
		}
		else if(a=="left")
		{
			new_style->al = point_align::right;
			new_style->base_rect = System::Drawing::Rectangle(width-10, height-18, 10, 10);
		}
		else if(a=="top")
		{
			new_style->al = point_align::bottom;
			new_style->base_rect = System::Drawing::Rectangle((width-10)/2, height-5, 10, 10);
		}
		else
		{
			new_style->al = point_align::none;
			new_style->base_rect = System::Drawing::Rectangle(0, 0, 10, 10);
		}

		new_style->active_color = System::Drawing::Color::Yellow;
		new_style->highlight_color = System::Drawing::Color::White;
		new_style->inactive_color = System::Drawing::Color::Transparent;
		add_new_style(new_style);
	}
}


}//	namespace particle_editor
}//namespace xray

