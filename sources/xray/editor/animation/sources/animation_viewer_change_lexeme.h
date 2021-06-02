////////////////////////////////////////////////////////////////////////////
//	Created		: 23.02.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_VIEWER_CHANGE_LEXEME_H_INCLUDED
#define ANIMATION_VIEWER_CHANGE_LEXEME_H_INCLUDED

using namespace System::Collections::Generic;

namespace xray {
namespace animation_editor {
	
	ref class animation_viewer_hypergraph;
	ref class animation_viewer_graph_node_base;
	ref class animation_node_interval;

	public ref class animation_viewer_change_lexeme: public xray::editor_base::command
	{
	public:
						animation_viewer_change_lexeme	(animation_viewer_hypergraph^ h, animation_viewer_graph_node_base^ n, List<animation_node_interval^>^ lst, u32 cycle_index);
		virtual			~animation_viewer_change_lexeme	();

		virtual bool	commit						() override;
		virtual void	rollback					() override;

	private:
		animation_viewer_hypergraph^ 	m_hypergraph;
		pstr							m_new_cfg_string;
		pstr							m_old_cfg_string;
		u32								m_node_id;
		u32								m_old_cycle_index;
		u32								m_new_cycle_index;
	}; // class animation_viewer_change_lexeme
} // namespace namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_VIEWER_CHANGE_LEXEME_H_INCLUDED