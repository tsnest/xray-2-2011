#include "pch.h"
#include "options_dialog.h"
#include "options_page.h"
#include "input_engine.h"
#include "action_engine.h"

namespace xray {
namespace editor {

void options_dialog::unregister_page( System::String^ full_name )
{
	array<Char>^	chars			= {'\\','/'};
	array<String^>^ splited			= full_name->Split( chars );
	TreeNodeCollection^ curr_nodes	= tree_view->Nodes;
	TreeNode^			found_node	= nullptr;

	for( int i = 0; i < splited->Length; ++i )
	{
		array<TreeNode^>^ nodes = curr_nodes->Find( splited[i], false );
		ASSERT			( nodes->Length == 1 );

		found_node		= nodes[0];
		curr_nodes		= found_node->Nodes;
	}

	ASSERT					(found_node);
	options_page^ page		= dynamic_cast<options_page^>(found_node->Tag);
	if(page)
		delete page;

	tree_view->Nodes->Remove(found_node);
}

void options_dialog::register_page( System::String^ full_name, options_page^ page )
{ // options_dialog takes ownership on given page
	ASSERT( page );
	
	array<Char>^	chars			= {'\\','/'};
	array<String^>^ splited			= full_name->Split( chars );
	TreeNodeCollection^ curr_nodes	= tree_view->Nodes;
	TreeNode^			added_node	= nullptr;

	for( int i = 0; i < splited->Length; ++i )
	{
		array<TreeNode^>^ nodes = curr_nodes->Find( splited[i], false );
		TreeNode^ curr			= nullptr;

		ASSERT(nodes->Length==0 || nodes->Length==1);

		if( nodes->Length == 1 )
		{
			curr			= nodes[0];
			curr_nodes		= curr->Nodes;
		}else
		{
			curr			= gcnew TreeNode( splited[i] );
			curr->Name		= splited[i];
			curr_nodes->Add	( curr );
			added_node		= curr;
			curr_nodes		= curr->Nodes;
		}
	}

	ASSERT			(added_node);
	added_node->Tag	= page;
}

Void options_dialog::tree_view_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETER	( sender );

	ASSERT( m_initialized);

	TreeNode^ curr_node			= e->Node;
	options_page^ page			= nullptr;

	if( curr_node )
		page = dynamic_cast<options_page^>(curr_node->Tag);

	if( m_active_page && m_active_page != page )
	{
		m_active_page->deactivate_page();
		m_active_page	= nullptr;
	}

	if( page )
	{
		m_active_page							= page;
		m_active_page->get_control()->Dock		= System::Windows::Forms::DockStyle::Fill;
		m_active_page->activate_page			();
		m_active_page->get_control()->BringToFront();
	}

}

Void options_dialog::options_dialog_Shown(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS	( sender, e );

	if( !m_initialized )
		initialize_all		( tree_view->Nodes );

	m_initialized			= true;
	
	tree_view->SelectedNode = nullptr;

	if( tree_view->Nodes->Count > 0 )
		tree_view->SelectedNode = tree_view->Nodes[0];
}

void options_dialog::initialize_all( TreeNodeCollection^ nodes )
{
	for ( int i = 0; i< nodes->Count; ++i )
	{
		options_page^ page = dynamic_cast<options_page^>( nodes[i]->Tag ); 
		if( page )
		{
			page->deactivate_page					();
			page_platform_panel->Controls->Add	(page->get_control());
		}

		initialize_all( nodes[i]->Nodes );
	}
}

void options_dialog::make_changes( bool b_accept, TreeNodeCollection^ nodes )
{
	for ( int i = 0; i< nodes->Count; ++i )
	{
		options_page^ page = dynamic_cast<options_page^>( nodes[i]->Tag ); 
		if( page && page->changed() )
		{
			if(b_accept)
				page->accept_changes();
			else
				page->cancel_changes();
		}
		make_changes( b_accept, nodes[i]->Nodes );
	}
}


void options_dialog::cancel_button_Click	(System::Object^, System::EventArgs^)
{
	make_changes	(false,   tree_view->Nodes  );
	Close			();
}

void options_dialog::ok_button_Click	(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS	( sender, e );
	make_changes	(true,   tree_view->Nodes  );
	Close			();
}

void options_dialog::options_dialog_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS	( sender, e );
	if(m_active_page)
		m_active_page->deactivate_page	();
}

} // namespace editor
} // namespace xray

