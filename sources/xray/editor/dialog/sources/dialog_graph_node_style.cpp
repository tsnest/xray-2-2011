////////////////////////////////////////////////////////////////////////////
//	Created		: 7.12.2009
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_graph_node_style.h"

using xray::dialog_editor::dialog_graph_node_style;
using xray::dialog_editor::dialog_graph_nodes_style_mgr;
using xray::editor::controls::hypergraph::view_mode;
using xray::editor::controls::hypergraph::point_align;
using xray::editor::controls::hypergraph::connection_type;
using xray::editor::controls::hypergraph::connection_point_style;
////////////////////////////////////////////////////////////////////////////
//- class scene_graph_nodes_style_mgr -------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
dialog_graph_nodes_style_mgr::dialog_graph_nodes_style_mgr()
{
	m_library_cfg = NEW(configs::lua_config_ptr)();
	m_library = gcnew System::Collections::ArrayList();
	load_library();
}
dialog_graph_nodes_style_mgr::~dialog_graph_nodes_style_mgr()
{
	DELETE(m_library_cfg);
}

void dialog_graph_nodes_style_mgr::assign_style(node^ n, System::String^ style_name)
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

void dialog_graph_nodes_style_mgr::load_library()
{
	query_result_delegate* rq = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &dialog_graph_nodes_style_mgr::on_library_loaded), g_allocator);
	xray::resources::query_resource(
		"resources/library/node_styles/dialog_node_styles.library",
		xray::resources::lua_config_class,
		boost::bind(&query_result_delegate::callback, rq, _1),
		g_allocator
		);

}

void dialog_graph_nodes_style_mgr::on_library_loaded(xray::resources::queries_result& data)
{
	R_ASSERT(data.is_successful());
	(*m_library_cfg) = static_cast_checked<configs::lua_config*>(data[0].get_unmanaged_resource().c_ptr());

	configs::lua_config::const_iterator	i = (*m_library_cfg)->begin();
	configs::lua_config::const_iterator	const e = (*m_library_cfg)->end();
	for (; i!=e; ++i) 
	{
		configs::lua_config_value t	= *i;
		dialog_graph_node_style^ style = gcnew dialog_graph_node_style();
		style->load(*i);
		m_library->Add(style);
	}
}
////////////////////////////////////////////////////////////////////////////
//- class scene_graph_node_style ------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
void dialog_graph_node_style::load(xray::configs::lua_config_value const& t)
{
	name = gcnew System::String(t["name"]);
	int width = t["width"];
	int height = t["height"];
	full_size = System::Drawing::Size(width, height);
	if(t.value_exists("active_color"))
		m_color_node_active = System::Drawing::Color::FromName(gcnew System::String(t["active_color"]));
	else
		m_color_node_active = System::Drawing::Color::YellowGreen;

	if(t.value_exists("highlight_color"))
		m_color_node_highlighted = System::Drawing::Color::FromName(gcnew System::String(t["highlight_color"]));
	else
		m_color_node_highlighted = System::Drawing::SystemColors::ButtonHighlight;

	if(t.value_exists("inactive_color"))
		m_color_node_inactive = System::Drawing::Color::FromName(gcnew System::String(t["inactive_color"]));
	else
		m_color_node_inactive = System::Drawing::SystemColors::ButtonFace;

	if(t.value_exists("active_link_color"))
		m_color_link_active = System::Drawing::Color::FromName(gcnew System::String(t["active_link_color"]));
	else
		m_color_link_active = System::Drawing::Color::YellowGreen;

	if(t.value_exists("highlight_link_color"))
		m_color_link_highlighted = System::Drawing::Color::FromName(gcnew System::String(t["highlight_link_color"]));
	else
		m_color_link_highlighted = System::Drawing::SystemColors::ButtonHighlight;

	if(t.value_exists("inactive_link_color"))
		m_color_link_inactive = System::Drawing::Color::FromName(gcnew System::String(t["inactive_link_color"]));
	else
		m_color_link_inactive = System::Drawing::SystemColors::ButtonFace;

	mode = view_mode::full;
	font = gcnew System::Drawing::Font(L"Microsoft Sans Serif", 
								(t.value_exists("font_size"))?float(t["font_size"]):9, 
								System::Drawing::FontStyle::Regular, 
								System::Drawing::GraphicsUnit::Point, 
								static_cast<System::Byte>(204));

	connection_points_font= gcnew System::Drawing::Font(L"Microsoft Sans Serif", 
								(t.value_exists("conn_points_font_size"))?float(t["conn_points_font_size"]):9, 
								System::Drawing::FontStyle::Regular, 
								System::Drawing::GraphicsUnit::Point, 
								static_cast<System::Byte>(204));

	if(t.value_exists("horiz_align"))
	{
		System::String^ al = gcnew System::String(t["horiz_align"]);
		if(al=="r")
			fmt->Alignment = System::Drawing::StringAlignment::Far;
		else if(al=="c")
			fmt->Alignment = System::Drawing::StringAlignment::Center;
		else
			fmt->Alignment = System::Drawing::StringAlignment::Near;
	}
	else
		fmt->Alignment = System::Drawing::StringAlignment::Center;

	if(t.value_exists("vert_align"))
	{
		System::String^ al = gcnew System::String(t["vert_align"]);
		if(al=="b")
			fmt->LineAlignment = System::Drawing::StringAlignment::Far;
		else if(al=="c")
			fmt->LineAlignment = System::Drawing::StringAlignment::Center;
		else
			fmt->LineAlignment = System::Drawing::StringAlignment::Near;
	}
	else
		fmt->LineAlignment = System::Drawing::StringAlignment::Center;

	xray::configs::lua_config_value::iterator b = t["connection_styles"].begin();
	xray::configs::lua_config_value::iterator e = t["connection_styles"].end();
	for(; b!=e; ++b)
	{
		connection_point_style^ new_style = gcnew connection_point_style();
		new_style->name = gcnew System::String((*b)["name"]);
		System::String^ d = gcnew System::String((*b)["dir"]);
		if(d=="input")
			new_style->dir = connection_type::input;
		else if(d=="output")
			new_style->dir = connection_type::output;

		System::String^ a = gcnew System::String((*b)["align"]);
		if(a=="left")
		{
			new_style->al = point_align::left;
			new_style->base_rect = System::Drawing::Rectangle(0, height-15, 10, 10);
		}
		else if(a=="top")
		{
			new_style->al = point_align::top;
			new_style->base_rect = System::Drawing::Rectangle((width-10)/2, 0, 10, 10);
		}
		else if(a=="right")
		{
			new_style->al = point_align::right;
			new_style->base_rect = System::Drawing::Rectangle(width-10, height-15, 10, 10);
		}
		else if(a=="bottom")
		{
			new_style->al = point_align::bottom;
			new_style->base_rect = System::Drawing::Rectangle((width-10)/2, height-5, 10, 10);
		}
		else
		{
			new_style->al = point_align::none;
			new_style->base_rect = System::Drawing::Rectangle(0, 0, 10, 10);
		}

		new_style->active_color = m_color_node_active;
		new_style->highlight_color = m_color_node_highlighted;
		new_style->inactive_color = m_color_node_inactive;
		add_new_style(new_style);
	}
}

