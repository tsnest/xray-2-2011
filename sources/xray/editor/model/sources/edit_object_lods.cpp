////////////////////////////////////////////////////////////////////////////
//	Created		: 04.03.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "edit_object_base.h"
#include "lod_view_panel.h"

namespace xray{
namespace model_editor{

typedef xray::editor::wpf_controls::hypergraph::properties_node	hypergraph_node;
typedef xray::editor::wpf_controls::hypergraph::link_id	hypergraph_link;

void lod_descriptor::load_settings( configs::lua_config_value const& v )
{
	configs::lua_config_value surfaces_list = v["surfaces"];
	configs::lua_config_iterator it = surfaces_list.begin();
	configs::lua_config_iterator it_e = surfaces_list.end();

	for( ;it!=it_e; ++it)
	{
		System::String^ sname	= gcnew System::String( (pcstr)it.key() );
		System::String^ svalue	= gcnew System::String( (pcstr)*it );

		if(!m_edit_object->has_surface(sname))
			continue;
		
		edit_surface^ s			= m_edit_object->get_surface( sname );

		if(svalue->Length && m_edit_object->has_surface(svalue))
			s->next_lod			= m_edit_object->get_surface( svalue );

		m_edit_object->set_surface_lod( s, this, false );
	}
}

void lod_descriptor::save_settings( configs::lua_config_value& v )
{
	configs::lua_config_value surfaces_list = v["surfaces"];
	for each (edit_surface^ s in m_surfaces)
	{
		edit_surface^ snext		= s->next_lod;
		unmanaged_string sname	(s->name);

		surfaces_list[sname.c_str()]	= (snext==nullptr) ? "" : unmanaged_string(snext->name).c_str();
	}
}

void edit_object_mesh::set_surface_lod( edit_surface^ s, lod_descriptor^ d, bool bmodify )
{
	for each(lod_descriptor^ d in m_lods)
	{
		if(d->m_surfaces.Contains(s))
			d->m_surfaces.Remove(s);
	}
	d->m_surfaces.Add	(s);

	if(bmodify)
		set_modified	( );
}

bool edit_object_mesh::lod_link_create_query( System::String^ input_node, System::String^ output_node )
{
	if(!m_surfaces.ContainsKey(input_node) || !m_surfaces.ContainsKey(output_node))
		return false;

	edit_surface^ input_surface		= m_surfaces[input_node];
	edit_surface^ output_surface	= m_surfaces[output_node];
	
	if(output_surface->next_lod)
		return false;

	output_surface->next_lod		= input_surface;
	
	set_modified					( );

	return							true;
}

void edit_object_mesh::fill_lod_view( )
{
	xray::editor::wpf_controls::hypergraph::hypergraph_control^ hg = m_lod_view_panel->hypergraph_host1->hypergraph;
	
	double const lod_column_width		= 150;

	int lods_count		= m_lods->GetLength(0);
	double lod_column	= 0;

	for( int lod_idx=0; lod_idx<lods_count; ++lod_idx)
	{
		double h				= 0;
		lod_descriptor^ d		= m_lods[lod_idx];

		hypergraph_node^ n		= d->m_hypergraph_node;
		if(n==nullptr)
		{
			n					= hg->create_properties_node(d->name);
			n->position			= System::Windows::Point(lod_column, h);
			n->Width			= lod_column_width-5;
			n->Height			= 20;
			d->m_hypergraph_node= n;
		}
		h						+= n->Height + 5;
		

		for each ( edit_surface^ s in d->m_surfaces )
		{
			hypergraph_node^ n	= s->m_hypergraph_node;
			
			if(n==nullptr)
			{
				n							= hg->create_properties_node(s->name);
				n->Width					= 100;
				n->Height					= 40;
				s->m_hypergraph_node		= n;
			}

			n->input_link_place_enabled		= false;//true;
			n->output_link_place_enabled	= false;//true;
			n->position						= System::Windows::Point(lod_column, h);

			h								+= n->Height;
		}
		lod_column			+= lod_column_width; 
	}
	
	for each ( edit_surface^ s in m_surfaces.Values )
	{
		if(s->next_lod)
		{
			System::String^ input_node_name		= s->next_lod->name;
			System::String^ output_node_name	= s->name;
			hg->create_link						( output_node_name, input_node_name, "output", "input" );
		}
	}
}

void edit_object_mesh::clear_lod_view( )
{
	xray::editor::wpf_controls::hypergraph::hypergraph_control^ hg = m_lod_view_panel->hypergraph_host1->hypergraph;
	int lods_count				= m_lods->GetLength(0);

	for( int lod_idx=0; lod_idx<lods_count; ++lod_idx)
	{
		lod_descriptor^ d	= m_lods[lod_idx];
		if(d->m_hypergraph_node)
			hg->remove_node	(d->name);

		for each ( edit_surface^ s in d->m_surfaces )
		{
			if(s->m_hypergraph_node)
				hg->remove_node(s->name);
		}
	}
}

void edit_object_mesh::create_lod_view( )
{
	m_lod_view_panel			= gcnew lod_view_panel;

	xray::editor::wpf_controls::hypergraph::hypergraph_control^ hg = m_lod_view_panel->hypergraph_host1->hypergraph;

	hg->link_creating			+= gcnew System::EventHandler<link_event_args^>(this, &edit_object_mesh::lod_view_on_link_creating);
	hg->selecting				+= gcnew System::EventHandler<selection_event_args^>(this, &edit_object_mesh::lod_view_on_node_selecting);
	hg->context_menu_opening	+= gcnew System::Action(this, &edit_object_mesh::lod_view_context_menu_opening);
	hg->menu_item_click			+= gcnew System::EventHandler<menu_event_args^>(this, &edit_object_mesh::lod_view_menu_item_click);

	m_delete_link_menu_item		= hg->add_context_menu_item( "Delete link" );
	m_move_to_lod_menu_item		= hg->add_context_menu_item( "Change LOD group" );
	m_move_to_lod0_menu_item	= hg->add_context_menu_item( "Change LOD group/LOD 0" );
	m_move_to_lod1_menu_item	= hg->add_context_menu_item( "Change LOD group/LOD 1" );
	m_move_to_lod2_menu_item	= hg->add_context_menu_item( "Change LOD group/LOD 2" );
}

void edit_object_mesh::lod_view_menu_item_click( System::Object^, menu_event_args^ args )
{
	if(args->menu_item==m_delete_link_menu_item)
	{
	}else
	if(args->menu_item==m_move_to_lod0_menu_item)
	{
		selection_change_lod(m_lods[0]);
	}else
	if(args->menu_item==m_move_to_lod1_menu_item)
	{
		selection_change_lod(m_lods[1]);
	}else
	if(args->menu_item==m_move_to_lod2_menu_item)
	{
		selection_change_lod(m_lods[2]);
	}
}

void edit_object_mesh::lod_view_context_menu_opening( )
{
	xray::editor::wpf_controls::hypergraph::hypergraph_control^ hg = m_lod_view_panel->hypergraph_host1->hypergraph;

	bool lod_selected[] = {false, false, false};

	int idx=0;
	for each(lod_descriptor^ d in m_lods)
	{
		for each (edit_surface^ s in d->m_surfaces)
		{
			if(s->Selected)
			{
				lod_selected[idx] = true;
				break;
			}
		}
		++idx;
	}
	bool mnu0_visible = (lod_selected[1]||lod_selected[2]) && !lod_selected[0];
	bool mnu1_visible = (lod_selected[0]||lod_selected[2]) && !lod_selected[1];
	bool mnu2_visible = (lod_selected[0]||lod_selected[1]) && !lod_selected[2];

	m_move_to_lod0_menu_item->Visibility = mnu0_visible ? System::Windows::Visibility::Visible : System::Windows::Visibility::Collapsed;
	m_move_to_lod1_menu_item->Visibility = mnu1_visible ? System::Windows::Visibility::Visible : System::Windows::Visibility::Collapsed;
	m_move_to_lod2_menu_item->Visibility = mnu2_visible ? System::Windows::Visibility::Visible : System::Windows::Visibility::Collapsed;

	bool selected_links = (hg->selected_links->Count!=0);
	m_delete_link_menu_item->Visibility = selected_links ? System::Windows::Visibility::Visible : System::Windows::Visibility::Collapsed;
}

void edit_object_mesh::lod_view_on_link_creating( System::Object^, link_event_args^ args )
{
//	xray::editor::wpf_controls::hypergraph::hypergraph_control^ hg = m_lod_view_panel->hypergraph_host1->hypergraph;
	
	if(args->link_id->input_node_id==args->link_id->output_node_id)
		return;

	if(args->link_id->input_node_id==args->link_id->output_node_id)
		return;

	//if( lod_link_create_query(args->link_id->input_node_id, args->link_id->output_node_id) )
	//	hg->create_link(	args->link_id );
}

void edit_object_mesh::lod_view_on_node_selecting( System::Object^, selection_event_args^ args )
{
	xray::editor::wpf_controls::hypergraph::hypergraph_control^ hg = m_lod_view_panel->hypergraph_host1->hypergraph;

	string_list^ nodes	= args->selected_node_ids;
	links_list^ links	= args->selected_link_ids;
	
	hg->deselect_all_nodes( );
	hg->deselect_all_links( );

	for each(edit_surface^ s in m_surfaces.Values)
	{
		s->Selected = nodes->Contains(s->name);
	}

	for each (lod_descriptor^ d in m_lods)
	{
		if(nodes->Contains(d->name))
		{
			for each(edit_surface^ s in d->m_surfaces )
				s->Selected			= true;

			hg->select_node( d->name );
		}
	}

	for each(hypergraph_link^ l in links)
	{
		hg->select_link( l );
	}

	refresh_surfaces_visibility	( );
}

void edit_object_mesh::selection_change_lod( lod_descriptor^ new_descriptor )
{
	for each( edit_surface^ s in m_surfaces.Values)
	{
		if(!s->Selected)
			continue;

		set_surface_lod				( s, new_descriptor, true );
	}

	fill_lod_view( );
}

} //namespace model_editor
} //namespace xray
