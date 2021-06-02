//-------------------------------------------------------------------------------------------
//	Created		: 16.04.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
//-------------------------------------------------------------------------------------------
#include "pch.h"
#include "dialog_node_visitors.h"
/*
#include "dialog.h"
#include "dialog_phrase.h"

#pragma unmanaged
using xray::dialog_editor::dialog_node_gatherer_visitor;
using xray::dialog_editor::dialog_node_giver_visitor;
using xray::dialog_editor::dialog;
using xray::dialog_editor::dialog_phrase;
using namespace xray::resources;
//-------------------------------------------------------------------------------------------
//- class dialog_node_gatherer_visitor ------------------------------------------------------
//-------------------------------------------------------------------------------------------
dialog_node_gatherer_visitor::dialog_node_gatherer_visitor():m_visited_counter(0)
{

}

void dialog_node_gatherer_visitor::visit(dialog_phrase* n)
{
	XRAY_UNREFERENCED_PARAMETER(n);
}

void dialog_node_gatherer_visitor::visit(dialog* n)
{
	request rq = request();
	rq.path = n->text();
	rq.id = dialog_class;
//	rq.id = game_dialog_class;
	m_request[m_visited_counter] = rq;
	++m_visited_counter;
}

void dialog_node_gatherer_visitor::request_subresources(xray::resources::query_result_for_cook* parent)
{
	if(m_visited_counter!=0)
	{
		query_resources(m_request, 
			m_visited_counter, 
			boost::bind(&dialog_node_gatherer_visitor::on_loaded, this, _1),
			parent->get_user_allocator(), 
			NULL,
			0,
			parent); 
	}
	else
	{
		parent->finish_query(cook_base::result_success);
		DELETE(this);
	}
}

void dialog_node_gatherer_visitor::on_loaded(xray::resources::queries_result& data)
{
	R_ASSERT(!data.is_failed());	

	if(data.is_failed())
		return;

	R_ASSERT(data.size()==m_visited_counter);	

	query_result_for_cook* parent = data.get_parent_query();
	dialog_node_base_ptr prnt_dlg_ptr = static_cast_resource_ptr<dialog_node_base_ptr>(parent->get_unmanaged_resource());
	dialog* prnt_dlg = dynamic_cast<dialog*>(prnt_dlg_ptr.c_ptr());
	if(data.size() > 0)
	{
		dialog_node_giver_visitor* visitor = NEW(dialog_node_giver_visitor)(data);
		const vector<dialog_node_base_ptr>* vec = prnt_dlg->get_phrases();
		vector<dialog_node_base_ptr>::const_iterator i = vec->begin();
		for(; i!=vec->end(); ++i)
			(*i)->accept(visitor);

		DELETE(visitor);
	}
	parent->finish_query(cook_base::result_success);
	DELETE(this);
}
//-------------------------------------------------------------------------------------------
//- class dialog_node_giver_visitor ---------------------------------------------------------
//-------------------------------------------------------------------------------------------
void dialog_node_giver_visitor::visit(dialog_phrase* n)
{
	XRAY_UNREFERENCED_PARAMETER(n);
}

void dialog_node_giver_visitor::visit(dialog* n)
{
	dialog_node_base_ptr sub_dlg = static_cast_resource_ptr<dialog_node_base_ptr>(m_resources[m_visited_counter].get_unmanaged_resource());
	sub_dlg->set_id(n->id());
	for(dialog_link* lnk = n->m_links_list.pop_front(); lnk!=NULL; lnk = n->m_links_list.pop_front())
		sub_dlg->add_link(lnk);

	query_result_for_cook* parent = m_resources.get_parent_query();
	dialog_node_base_ptr prnt_dlg_ptr = static_cast_resource_ptr<dialog_node_base_ptr>(parent->get_unmanaged_resource());
	dialog* prnt_dlg = dynamic_cast<dialog*>(prnt_dlg_ptr.c_ptr());
	fix_links(prnt_dlg, n, dynamic_cast<dialog*>(sub_dlg.c_ptr()));
	dialog_node_base_ptr* p = prnt_dlg->get_node_by_id(n->id());
	R_ASSERT(p!=NULL);
	n->clear();
	DELETE(p->c_ptr());
	*p = sub_dlg.c_ptr();
	++m_visited_counter;
}

void dialog_node_giver_visitor::fix_links(dialog* prnt_dlg, dialog* old_dlg, dialog* new_dlg)
{
	const vector<dialog_node_base_ptr>* vec = prnt_dlg->get_phrases();
	vector<dialog_node_base_ptr>::const_iterator i = vec->begin();
	for(; i!=vec->end(); ++i)
		for(dialog_link* lnk = (i->c_ptr())->m_links_list.front(); lnk!=NULL; lnk = (i->c_ptr())->m_links_list.get_next_of_object(lnk))
			if(lnk->child()==old_dlg)
				lnk->change_child(new_dlg);

}
*/