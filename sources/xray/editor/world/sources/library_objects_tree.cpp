#include "pch.h"
#include "library_objects_tree.h"
#include "tool_base.h"
#include "level_editor.h"
#include "project_items.h"
#include "editor_world.h"

namespace xray{
namespace editor {

ref class drag_drop_data
{
public:
	lib_item_base^			m_lib_item_base;
	tree_node^				m_over_node;
	tree_node^				m_src_node;
	System::DateTime		m_dt;

	void drag_over_node(tree_node^ n)
	{
		if(n!=m_over_node)
		{
			m_over_node		= n;
			m_dt			= System::DateTime::Now;
		}else
		{
			System::TimeSpan	delta = System::DateTime::Now - m_dt;
			if(delta.TotalMilliseconds>800)
			{
				if(m_over_node)
					m_over_node->Expand();
				
				m_dt		= System::DateTime::Now;
			}
		}
	}
};

controls::tree_node^  library_objects_tree::get_selected_node( )
{
	System::Collections::ObjectModel::ReadOnlyCollection<tree_node^>^ selection = tree_view->selected_nodes;
	if(selection->Count!=0)
	{
		R_ASSERT(selection->Count==1, "multiselect not allowed here");
		return selection[0];
	}else
		return nullptr;
}

void library_objects_tree::post_constructor( )
{
	show_empty_folders			= true;
	sync_tool_selected_name		= false;
	allow_folder_creation		= false;
	allow_items_reparent		= false;
	allow_items_rename			= false;
	allow_items_delete			= false;
	allow_folder_selection		= false;
	tree_view->ImageList		= m_tool->get_level_editor()->get_editor_world().images16x16();
}

void library_objects_tree::current_path::set( System::String^ value )
{
	System::String^ prev_path	= m_current_path;
	m_current_path				= value;
	m_current_path_changed		( m_current_path, prev_path );
}

void library_objects_tree::sync_tool_selected_name::set( bool value )
{
	if(m_sync_tool_selected_name && !value)
		m_current_path_changed	-= gcnew current_path_changed(this, &library_objects_tree::update_tool_selected_name);
	else
	if(!m_sync_tool_selected_name && value)
		m_current_path_changed	+= gcnew current_path_changed(this, &library_objects_tree::update_tool_selected_name);

	m_sync_tool_selected_name	= value;
}

void library_objects_tree::update_tool_selected_name( System::String^ new_path, System::String^ prev_path )
{
	XRAY_UNREFERENCED_PARAMETERS	( prev_path );
	m_tool->select_library_name		( new_path );
}

void library_objects_tree::menu_strip_Opening( System::Object^, System::ComponentModel::CancelEventArgs^ )
{
	delete_item_menu_item->Enabled		= allow_items_delete && (nullptr!=get_selected_node());
	new_folder_menu_item->Enabled		= allow_folder_creation;
	rename_menu_item->Enabled			= allow_items_rename;
}

bool library_objects_tree::fill_objects_list( System::String^ initial )
{
	tree_view->nodes->Clear		( );
	m_tool->m_library->fill_tree( tree_view->nodes, m_tool, show_empty_folders, filter );

	if(initial!=nullptr)
	{
		tree_node^ n		= find_hierrarchy( tree_view->nodes, initial );
		
		if(n!=nullptr)
		{
			n->Selected				= true;
			n->EnsureVisible		( );
			current_path			= initial;
			return					true;
		}else
		{
			System::String^ last_name = System::IO::Path::GetFileName( initial );
			for each (lib_item^ l in m_tool->m_library->m_tool_library)
			{
				if(l->m_name == last_name)
				{
					System::String^ full_name	= l->full_path_name();
					tree_node^ n				= find_hierrarchy( tree_view->nodes, full_name );
					if(n!=nullptr)
					{
						n->Selected				= true;
						n->EnsureVisible		( );
						current_path			= full_name;
						return					true;
					}
					break;
				}
			}
			current_path				= "";
		}
	}else
	{
		current_path	= "";
	}

	return	false;
}

void library_objects_tree::tree_view_AfterSelect( System::Object^, System::Windows::Forms::TreeViewEventArgs^  e)
{
	if( e->Node==nullptr )
	{
		current_path	= nullptr;
	}else
	if(allow_folder_selection && dynamic_cast<lib_folder^>(e->Node->Tag)!=nullptr )
	{
		current_path	= e->Node->FullPath;
	}else
	if(dynamic_cast<lib_item^>(e->Node->Tag)!=nullptr)
	{
		current_path	= e->Node->FullPath;
	}else
	{
		current_path	= "";
	}
}

void library_objects_tree::tree_view_AfterLabelEdit( System::Object^, System::Windows::Forms::NodeLabelEditEventArgs^ e )
{
	System::Object^ tag			= e->Node->Tag;
	System::String^	new_name	= e->Label;
	
	if(new_name)
	{
		lib_item^ li		= dynamic_cast<lib_item^>(tag);
		if(li)
		{
			System::String^ tmp			= li->m_name;
			li->m_name					= new_name;
			System::String^ fullpath	= li->full_path_name();
			li->m_name					= tmp;
			if(	m_tool->m_library->get_folder_item_by_full_path(fullpath, false) || 
				m_tool->m_library->get_library_item_by_full_path(fullpath, false) )
				e->Node->BeginEdit ( );
			else
				li->m_name				= new_name;

		}else
		{
			lib_folder^ lf = dynamic_cast<lib_folder^>(tag);
			if(lf)
			{
				System::String^ tmp			= lf->m_name;
				lf->m_name					= new_name;
				System::String^ fullpath	= lf->full_path_name();
				lf->m_name					= tmp;
				if(	m_tool->m_library->get_folder_item_by_full_path(fullpath, false) || 
					m_tool->m_library->get_library_item_by_full_path(fullpath, false) )
					e->Node->BeginEdit ( );
				else
					lf->m_name				= new_name;

			}
		}
	}

	m_tool->save_library();
}

void library_objects_tree::rename_menu_item_Click( System::Object^, System::EventArgs^ )
{
	tree_node^ selected = get_selected_node();
	if(selected)
		selected->BeginEdit		( );
}

void library_objects_tree::new_folder_menu_item_Click( System::Object^, System::EventArgs^ )
{
	tree_node_collection^	root_nodes	= tree_view->nodes;

	tree_node^ selected = get_selected_node();
	if(selected)
	{
		if(dynamic_cast<lib_folder^>(selected->Tag))
			root_nodes = selected->nodes;
	}

	tree_node^ new_node				= gcnew xray::editor::controls::tree_node("NewFolder");
	new_node->ImageIndex			= xray::editor_base::node_group;
	new_node->SelectedImageIndex	= xray::editor_base::node_group;
	new_node->Name					= "NewFolder";
	root_nodes->Add					( new_node );

	System::String^ new_folder_name = new_node->FullPath;
	lib_folder^ lf				= m_tool->m_library->add_new_folder( new_folder_name );
	new_node->Tag				= lf;
	new_node->Name				= lf->m_name;
	new_node->Text				= lf->m_name;
	new_node->EnsureVisible		( );
	new_node->BeginEdit			( );
}

void library_objects_tree::delete_item_menu_item_Click( System::Object^, System::EventArgs^ )
{
	tree_node^ selected = get_selected_node();
	
	R_ASSERT(selected);

	// feeee, andy->andy refactor this!
	lib_folder^ lf	= dynamic_cast<lib_folder^>(selected->Tag);
	lib_item^ li	= dynamic_cast<lib_item^>(selected->Tag);

	if(lf || li)
	{
		// confirmation
		::DialogResult result = m_tool->get_level_editor()->ShowMessageBox( "Delete '" + selected->FullPath + "' confirmation",
														MessageBoxButtons::YesNo,
														MessageBoxIcon::Exclamation );

		if ( result == ::DialogResult::Yes )
		{
			if(li)
				m_tool->m_library->delete_library_item	( li );
			else
			if(lf)
				m_tool->m_library->delete_folder_item	( lf );
			else
			{
				UNREACHABLE_CODE();
			}

			m_tool->save_library					( );
			m_tool->on_library_changed				( );
			m_tool->select_library_name				( "" );
		}
	}else
		UNREACHABLE_CODE();
}

void library_objects_tree::tree_view_ItemDrag( System::Object^, System::Windows::Forms::ItemDragEventArgs^  e)
{
	tree_node^ selected		= get_selected_node();
	
	if(!selected)
		return;

	if(allow_items_reparent && e->Button == ::MouseButtons::Right )
	{
		drag_drop_data^	data	= gcnew drag_drop_data;

		data->m_lib_item_base	= safe_cast<lib_item_base^>(selected->Tag);

		data->m_src_node		= selected;
		DoDragDrop				( data, DragDropEffects::Move );
	}else
	if ( e->Button == ::MouseButtons::Left )
	{
		add_object_to_scene^ data	= gcnew add_object_to_scene;
		lib_item^ li				= dynamic_cast<lib_item^>(selected->Tag);
		if(li)
		{
			data->m_library_item_name	= li->full_path_name();
			data->m_tool				= m_tool;
			DoDragDrop					( data, DragDropEffects::Link );
		}
	}

}

void library_objects_tree::tree_view_DragOver( System::Object^, System::Windows::Forms::DragEventArgs^  e)
{
	drag_drop_data^ data	= dynamic_cast<drag_drop_data^>(e->Data->GetData(drag_drop_data::typeid));
	if(data)
	{
		e->Effect = DragDropEffects::Move;
	}else
	{
		e->Effect = DragDropEffects::None;
		return;
	}

	TreeNode^ dest_node		= tree_view->GetNodeAt(tree_view->PointToClient(System::Drawing::Point(e->X, e->Y)));

	if(dest_node)
	{
		lib_item^ li	= dynamic_cast<lib_item^>(dest_node->Tag);
		if(li)
			e->Effect	= DragDropEffects::None;
	}

	data->drag_over_node	(safe_cast<tree_node^>(dest_node));
}

void library_objects_tree::tree_view_DragDrop( System::Object^, System::Windows::Forms::DragEventArgs^  e)
{
	drag_drop_data^ data		= dynamic_cast<drag_drop_data^>(e->Data->GetData(drag_drop_data::typeid));

	lib_folder^	new_parent		= nullptr;
	tree_node_collection^ nodes	= tree_view->nodes;

	TreeNode^ dest_node		= tree_view->GetNodeAt(tree_view->PointToClient(System::Drawing::Point(e->X, e->Y)));
	if(dest_node)
	{
		nodes		= dest_node->Nodes;
		new_parent	= dynamic_cast<lib_folder^>(dest_node->Tag);
	}
	
	if(data->m_lib_item_base->m_parent_folder == new_parent)
		return;

	data->m_lib_item_base->m_parent_folder	= new_parent;
	data->m_src_node->Remove				( );
	nodes->Add								( data->m_src_node );
	dest_node->Expand						( );
	m_tool->save_library					( );
}

void library_objects_tree::tree_view_MouseDoubleClick( System::Object^, System::Windows::Forms::MouseEventArgs^ )
{
	tree_node^ selected		= get_selected_node();
	
	if(!selected)
		return;

	lib_item_base^ item_base = safe_cast<lib_item_base^>(selected->Tag);

	m_double_clicked			( item_base );

}
} //namespace editor
} //namespace xray
