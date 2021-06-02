////////////////////////////////////////////////////////////////////////////
//	Created		: 14.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef FILE_TREE_VIEW_H_INCLUDED
#define FILE_TREE_VIEW_H_INCLUDED

#include "tree_view_event_args.h"
#include "tree_node_type.h"
#include "tree_view_select_action.h"
#include "tree_view_selection_event_args.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections::Generic;
using namespace System::Collections::ObjectModel;
using namespace System::Windows;
using namespace System::Windows::Forms;

typedef System::Collections::Generic::IList<Object^> GObjIList;

namespace xray {
namespace editor {
namespace controls {

interface class		tree_view_source;
ref class			tree_node;
ref class			tree_view_search_panel;
ref class			tree_view_filter_panel;
ref class			tree_view_selection_event_args;

public ref class tree_view : public TreeView
{

#pragma region |   Events   |

public:
	event EventHandler^										items_loaded;
	event EventHandler<tree_view_selection_event_args^>^	selected_items_changed;

#pragma endregion

#pragma region | Initialize |

public:
	tree_view();
	~tree_view();

#pragma endregion

#pragma region |   Fields   |


private:
	tree_view_source^				m_source;
	bool							m_b_container_created;
	bool							m_b_right_clicked;
	bool							m_b_in_selection_mouse_down;

	List<tree_node^>^				m_selected_nodes;
	tree_node^						m_last_selected_node;
	tree_node_type					m_type_of_selected_nodes;

	tree_view_filter_panel^			m_filter_panel;
	tree_view_search_panel^			m_search_panel;

	tree_node^						m_root;

public:
	Panel^							m_tree_view_container;


#pragma endregion

#pragma region | Properties |

public:
	[BrowsableAttribute(false)]
	property tree_view_source^ source
	{
		tree_view_source^	get	( );
		void				set	( tree_view_source^ value );
	}

	[BrowsableAttribute(false)]
	[DesignerSerializationVisibility(DesignerSerializationVisibility::Hidden)]
	property tree_node^				root
	{
		tree_node^	get( ){ return m_root; }
	}
	property Boolean				auto_expand_on_filter
	{
		Boolean		get( );
		void		set( Boolean value );
	}
	property Boolean				is_multiselect;
	property Boolean				is_selectable_groups;
	property Boolean				is_selection_or_groups_or_items;
	[BrowsableAttribute(false)]
	property ReadOnlyCollection<tree_node^>^		selected_nodes{
		ReadOnlyCollection<tree_node^>^ get( ){return m_selected_nodes->AsReadOnly();}
	}
	
	[BrowsableAttribute(false)]
	[DesignerSerializationVisibility(DesignerSerializationVisibility::Hidden)]
	property tree_node^		selected_node
	{
		tree_node^	get( )
		{
			return ( m_selected_nodes->Count > 0 ) ? m_selected_nodes[0] : nullptr; 
		}
	}

	property Boolean				filter_visible{
		Boolean				get( );
		void				set( Boolean value );
	}

	property Boolean				keyboard_search_enabled;

	property TreeNodeCollection^	nodes
	{
		TreeNodeCollection^ get( ){return TreeView::Nodes;}
	}


#pragma endregion

#pragma region |  Methods   |


public:
	void			select_all				( );
	void			deselect_all			( );
	void			select_node				( tree_node^ node, bool value, bool exclusive );
	void			track_active_node		( String^ full_path );
	void			track_active_nodes		( List<System::String^>^ full_paths );
	tree_node^		get_node				( String^ full_path );
	void			refresh					( );
	tree_node^		add_item				( String^ item_path );
	tree_node^		add_item				( String^ item_path, Int32 image_index );
	tree_node^		add_item				( tree_node^ destination, String^ item_name );
	tree_node^		add_item				( tree_node^ destination, String^ item_name, Int32 image_index );
	tree_node^		add_group				( String^ group_path );
	tree_node^		add_group				( String^ group_path, Int32 image_index_collapsed, Int32 image_index_expanded );
	tree_node^		add_group				( tree_node^ destination, String^ group_path );
	tree_node^		add_group				( tree_node^ destination, String^ group_path, Int32 image_index_collapsed, Int32 image_index_expanded );
	void			remove_item				( String^ item_path );
	void			remove_group			( String^ group_path );
	void			remove_item				( tree_node^ item_node );
	void			remove_group			( tree_node^ group_node );
	void			change_item_context		( String^ file_path, Object^ context );
	void			on_items_loaded			( );
	void			clear					( );

	void			add_items				( GObjIList^ items );
	void			add_items				( array<Object^>^ items );

internal:
	TreeNode^		process_item_path		( array<String^>^ segments );
	TreeNode^		process_item_path		( array<String^>^ segments, Boolean create_path );
	TreeNode^		process_item_path		( array<String^>^ segments, Boolean create_path, Int32 image_index_collapsed, Int32 image_index_expanded );
	TreeNode^		process_item_path		( String^ path, String^ separator );
	TreeNode^		process_item_path		( String^ path );

protected:
	virtual	void	OnKeyDown				( KeyEventArgs^  e ) override;
	virtual void	OnKeyUp					( KeyEventArgs^  e ) override;
	virtual void	OnMouseDown				( MouseEventArgs^ e ) override;
	virtual void	OnMouseUp				( MouseEventArgs^ e ) override;
	virtual void	OnNodeMouseClick		( TreeNodeMouseClickEventArgs^  e ) override;
	virtual void	OnAfterCollapse			( TreeViewEventArgs^  e ) override;
	virtual void	OnAfterExpand			( TreeViewEventArgs^  e ) override;
			bool	on_before_select		( tree_node^ node, tree_view_select_action action );
			void	on_after_select			( tree_node^ node );
	virtual void	OnBeforeSelect			( TreeViewCancelEventArgs^ e ) override;
	virtual void	OnParentChanged			( EventArgs^ e ) override;
			void	on_selected_items_changed( List<tree_node^>^ old_selection );
			void	select_node_impl		( tree_node^ node );
			void	deselect_node_impl		( tree_node^ node );

private:
	tree_node^		add_item				( TreeNode^ destination, String^ item_name, Int32 image_index );
	void			select_sub_nodes		( tree_node^ node );
	void			deselect_nodes			( List<tree_node^>^ node );


#pragma endregion

}; // class tree_view

}//namespace controls
}//namespace editor
}//namespace xray

#endif // #ifndef FILE_TREE_VIEW_H_INCLUDED