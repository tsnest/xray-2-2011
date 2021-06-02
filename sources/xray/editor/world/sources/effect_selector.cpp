#include "pch.h"
#include "effect_selector.h"
#include "effect_selector_source.h"
#include "effect_selector_item_comparator.h"

namespace xray{
namespace editor{

void effect_selector::effect_selector_Load( Object^, System::EventArgs^ )
{
	effect_selector_source^ source			= gcnew effect_selector_source( m_tree_view );
	source->is_for_texture					= is_for_texture;
	source->existing						= existing;
	source->effect_stage					= selecting_stage;
	m_tree_view->source						= source;
	m_tree_view->items_loaded				+= gcnew System::EventHandler(this, &effect_selector::effect_items_loaded);
	m_tree_view->TreeViewNodeSorter			= gcnew effect_selector_item_comparator();
	m_tree_view->NodeMouseDoubleClick		+= gcnew System::Windows::Forms::TreeNodeMouseClickEventHandler(this, &effect_selector::m_tree_view_NodeMouseDoubleClick);
	source->refresh							( );
}

void effect_selector::effect_items_loaded( Object^, System::EventArgs^ )
{
	if( selected_item != nullptr && selected_item != "")
		m_tree_view->track_active_node(selected_item);
}

void effect_selector::m_tree_view_NodeMouseDoubleClick( Object^, TreeNodeMouseClickEventArgs^ )
{
	ok_button_Click( nullptr, nullptr );
}

void effect_selector::ok_button_Click( Object^, System::EventArgs^ )
{
	if(m_tree_view->selected_node)
	{
		selected_item		= m_tree_view->selected_node->FullPath;
		this->DialogResult	= System::Windows::Forms::DialogResult::OK;
	}
}

void effect_selector::m_cancel_button_Click( Object^, System::EventArgs^ )
{
	Close( );
}

}// namespace editor
}// namespace xray