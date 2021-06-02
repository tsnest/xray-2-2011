////////////////////////////////////////////////////////////////////////////
//	Created		: 7.12.2009
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "scene_graph_node_style.h"

using xray::editor::scene_graph_node_style;
using xray::editor::scene_graph_nodes_style_mgr;
using xray::editor::controls::hypergraph::view_mode;
using xray::editor::controls::hypergraph::point_align;
using xray::editor::controls::hypergraph::connection_type;
using xray::editor::controls::hypergraph::connection_point_style;
////////////////////////////////////////////////////////////////////////////
//- class scene_graph_nodes_style_mgr -------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
scene_graph_nodes_style_mgr::scene_graph_nodes_style_mgr()
{
	m_library_cfg = NEW(configs::lua_config_ptr)();
	m_library = gcnew System::Collections::ArrayList();
	load_library();
}
scene_graph_nodes_style_mgr::~scene_graph_nodes_style_mgr()
{
	DELETE(m_library_cfg);
}

void scene_graph_nodes_style_mgr::assign_style(node^ n, System::String^ style_name)
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

void scene_graph_nodes_style_mgr::load_library()
{
	query_result_delegate* rq = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &scene_graph_nodes_style_mgr::on_library_loaded), g_allocator);
	xray::resources::query_resource(
		"resources/library/node_styles/scene_graph_node_styles.library",
		xray::resources::lua_config_class,
		boost::bind(&query_result_delegate::callback, rq, _1),
		g_allocator
		);

}

void scene_graph_nodes_style_mgr::on_library_loaded(xray::resources::queries_result& data)
{
	R_ASSERT(data.is_successful());
	(*m_library_cfg) = static_cast_checked<configs::lua_config*>(data[0].get_unmanaged_resource().c_ptr());

	configs::lua_config::const_iterator	i = (*m_library_cfg)->begin();
	configs::lua_config::const_iterator	const e = (*m_library_cfg)->end();
	for (; i!=e; ++i) 
	{
		configs::lua_config_value t	= *i;
		scene_graph_node_style^ style = gcnew scene_graph_node_style();
		style->load(t);
		m_library->Add(style);
	}
}
////////////////////////////////////////////////////////////////////////////
//- class scene_graph_node_style ------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
void scene_graph_node_style::load(xray::configs::lua_config_value const& t)
{
	name = gcnew System::String(t["name"]);
	System::Drawing::Color color = System::Drawing::Color::FromName(gcnew System::String(t["active_color"]));
	m_color_node_active = color;
	color = System::Drawing::Color::FromName(gcnew System::String(t["highlight_color"]));
	m_color_node_highlighted = color;
	color = System::Drawing::Color::FromName(gcnew System::String(t["inactive_color"]));
	m_color_node_inactive = color;
	System::String^ m = gcnew System::String(t["view_mode"]);
	if(m=="full")
		mode = view_mode::full;
	else if(m=="brief")
		mode = view_mode::brief;
	
	System::String^ al = gcnew System::String(t["horiz_align"]);
	if(al=="r")
		fmt->Alignment = System::Drawing::StringAlignment::Far;
	else if(al=="c")
		fmt->Alignment = System::Drawing::StringAlignment::Center;
	else
		fmt->Alignment = System::Drawing::StringAlignment::Near;

	al = gcnew System::String(t["vert_align"]);
	if(al=="b")
		fmt->LineAlignment = System::Drawing::StringAlignment::Far;
	else if(al=="c")
		fmt->LineAlignment = System::Drawing::StringAlignment::Center;
	else
		fmt->LineAlignment = System::Drawing::StringAlignment::Near;

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
		else if(d=="inout")
			new_style->dir = connection_type::inout;
		
		System::String^ a = gcnew System::String((*b)["align"]);
		if(a=="left")
			new_style->al = point_align::left;
		else if(a=="top")
			new_style->al = point_align::top;
		else if(a=="right")
			new_style->al = point_align::right;
		else if(a=="bottom")
			new_style->al = point_align::bottom;
		else
			new_style->al = point_align::none;

		System::Drawing::Point tmp1 = System::Drawing::Point((*b)["x"], (*b)["y"]);
		System::Drawing::Size tmp2 = System::Drawing::Size((*b)["width"], (*b)["height"]);
		new_style->base_rect = System::Drawing::Rectangle(tmp1.X, tmp1.Y, tmp2.Width, tmp2.Height);
		color = System::Drawing::Color::FromName(gcnew System::String((*b)["active_color"]));
		new_style->active_color = color;
		color = System::Drawing::Color::FromName(gcnew System::String((*b)["highlight_color"]));
		new_style->highlight_color = color;
		color = System::Drawing::Color::FromName(gcnew System::String((*b)["inactive_color"]));
		new_style->inactive_color = color;
		add_new_style(new_style);
	}
}
