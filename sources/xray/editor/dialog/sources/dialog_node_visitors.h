//-------------------------------------------------------------------------------------------
//	Created		: 16.04.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
//-------------------------------------------------------------------------------------------
#ifndef DIALOG_NODE_VISITORS_H_INCLUDED
#define DIALOG_NODE_VISITORS_H_INCLUDED
/*
//#include "dialog_node_base.h"
#pragma managed(push, off)

namespace xray {
namespace dialog_editor {
	class dialog;
	class dialog_phrase;

	class dialog_node_gatherer_visitor : public boost::noncopyable
	{
	public:
				dialog_node_gatherer_visitor	();
				~dialog_node_gatherer_visitor	() {};
		void	visit							(dialog* n);
		void	visit							(dialog_phrase* n);
		void	request_subresources			(xray::resources::query_result_for_cook* parent);

	private:
		void	on_loaded						(xray::resources::queries_result& data);

	private:
		u32							m_visited_counter;
		xray::resources::request	m_request[255];
	}; // class dialog_node_gatherer_visitor

	class dialog_node_giver_visitor : public boost::noncopyable
	{
	public:
				dialog_node_giver_visitor	(xray::resources::queries_result& data)
					:m_resources(data),
					m_visited_counter(0)		{};
				~dialog_node_giver_visitor	()	{};

		void	visit						(dialog_phrase* n);
		void	visit						(dialog* n);

	private:
		void	fix_links					(dialog* prnt_dlg, dialog* old_dlg, dialog* new_dlg);

	private:
		u32									m_visited_counter;
		xray::resources::queries_result&	m_resources;
	}; // class dialog_node_giver_visitor
} // namespace dialog_editor 
} // namespace xray 
#pragma managed(pop)
*/
#endif // #ifndef DIALOG_NODE_VISITORS_H_INCLUDED

