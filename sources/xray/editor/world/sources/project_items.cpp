////////////////////////////////////////////////////////////////////////////
//	Created		: 20.03.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "project_items.h"
#include "project.h"
#include "tool_base.h"
#include "object_composite.h"
#include "lua_config_value_editor.h"
#include "resource_loading_dialog.h"

namespace xray{
namespace editor{

project_item_base::project_item_base( project^ p )
:	m_project	( p ), 
	m_tree_node_( nullptr ), 
	m_parent	( nullptr ), 
	m_selected	( false )
{}

project_item_base::~project_item_base( )
{
	s_id_to_object.Remove		(m_id);
}

System::String^ project_item_base::get_full_name( )
{
	System::String^ result	= nullptr;
	if(m_parent)
	{
		System::String^ parent_path		= m_parent->get_full_name();
		result							= System::String::Concat(parent_path, (parent_path->Length)?"/":"", get_name());
	}
	else if ( get_object() && get_object()->parent_object_base )
	{
		result = get_object()->get_full_name();
	}
	else
		result = (m_project->root()==this)? "" : get_name();

	return result;
}

project_item_base^ get_parent( project_item_base^ itm)
{
	if( itm->m_parent )
	{
		return itm->m_parent;
	}else
	{
		if(itm->m_tree_node_ && itm->m_tree_node_->Parent)// composite member in "edit" mode
		{
			project_item_base^ parent_itm = safe_cast<project_item_base^>(itm->m_tree_node_->Parent->Tag);
			return parent_itm;
		}

		UNREACHABLE_CODE();
		return nullptr;
	}
}

System::String^ project_item_base::get_full_path( )
{
	return get_parent(this)->get_full_name();
}

bool project_item_base::is_child_of( project_item_base^ pg )
{
	project_item_base^ parent = m_parent;
	while(parent)
	{
		if(parent==pg)
			return true;
		else
			parent	= parent->m_parent;
	}
	return false;
}

project_item_base^ project_item_base::object_by_id( u32 id )
{
	project_item_base^		object;

	if( s_id_to_object.TryGetValue(id, object) ) 
		return			object;

	ASSERT( false, "There is no object with the specified id!");
	return				nullptr;
}

project_item_base^ project_item_base::object_by_full_name( System::String^ full_name )
{
	for each(project_item_base^ item in s_id_to_object.Values)
	{
		System::String^ item_full_name = item->get_full_name();
		if (item_full_name == full_name)
			return item;
	}

	return				nullptr;
}

void project_item_base::assign_id( u32 id )
{
	ASSERT( m_id == 0, "Object ID can be assigned only once." );

	if( id == 0 )
		id = generate_id();

	ASSERT( false == s_id_to_object.ContainsKey(id), "An object with the specified id allready exists." );

	m_id				= id;
	s_id_to_object[id]	= this;
}

u32 project_item_base::generate_id( )
{
 	return ++m_last_generated_id;
}

bool project_item_base::get_selected( )
{
	return m_selected;
}

void project_item_base::set_selected( bool b )
{
	m_selected = b;

	if(m_tree_node_)
		m_tree_node_->Selected		= b;
	
	on_selected						( m_selected );
}

bool project_item_base::check_correct_existance( System::String^% name, u32 index )
{

	if(System::String::IsNullOrEmpty(name))
		name = "new";

	name = name->ToLower();

	System::String^ index_str		= System::UInt32(index).ToString();
	project_item_base^ item			= get( name );

	if( item != nullptr )
	{
		int idx			= name->LastIndexOf('(');
		int idx_en		= (idx!=-1)?name->IndexOf(')', idx):-1;

		if( idx!=-1 && idx_en!=-1)
		{
			System::String^  num = name->Substring(idx+1, idx_en-idx-1);
			if(num==index_str)
				++index;

			name = name->Substring(0, idx);
		}
		name += "(";
		name += index_str;
		name += ")";

		check_correct_existance( name, index );
		return true;
	}
	return false;
}



project_item_folder::project_item_folder( project^ p )
:super		( p ),
m_pivot		( NULL ),
m_locked	( false )
{
	m_node_type	 = controls::tree_node_type::group_item ;
	m_name		= "New";
}

project_item_folder::~project_item_folder( )
{
	DELETE		( m_pivot );
}

bool project_item_folder::get_persistent( )
{
	bool broot = (m_project->root()==this);
	if(broot)
		return true;

	for each (project_item_object^ pi in m_objects)
		if(pi->get_persistent())
			return true;

	for each (project_item_folder^ pg in m_folders)
		if(pg->get_persistent())
			return true;

	return false;
}

void project_item_folder::get_all_items( project_items_list^% result_list, bool recursive )
{
	for each (project_item_object^ pi in m_objects)
	{
		if(recursive)
			pi->get_all_items	( result_list, recursive );
		else
			result_list->Add	( pi );
	}

	for each (project_item_folder^ pg in m_folders)
	{
		if(recursive)
			pg->get_all_items( result_list, recursive );
		else
			result_list->Add	( pg );
	}

	result_list->Add		( this );
}

void project_item_folder::after_name_edit( System::String^% new_name )
{
	m_parent->check_correct_existance	( new_name, 1 );
	set_name							( new_name );
}

void project_item_folder::folder_name::set(System::String^ n)	
{
	System::String^ new_name = n;
	m_parent->check_correct_existance	( new_name, 1 );
	set_name							( new_name );
}

void project_item_folder::locked::set(bool v)
{
	m_locked = v;
	if(m_tree_node_)
	{
		m_tree_node_->ImageIndex			= m_locked? xray::editor_base::node_group_locked : xray::editor_base::node_group;
		m_tree_node_->SelectedImageIndex	= m_locked? xray::editor_base::node_group_locked : xray::editor_base::node_group;
	}

}

void project_item_folder::on_selected( bool b_select )
{
	if( b_select )
	{
		m_project->set_focused_folder	( this );
		if(m_project->track_active_item)
			m_tree_node_->EnsureVisible		( );
	}else
	{
		if( this == m_project->get_focused_folder( ) )
			m_project->set_focused_folder( nullptr );
	}
}

void project_item_folder::clear( bool b_destroy )
{
	for each (project_item_folder^ pg in m_folders)
	{
		pg->clear		( b_destroy );
		delete			pg;
	}
	m_folders.Clear();

	for each (project_item_object^ pi in m_objects)
	{
		pi->clear		( b_destroy );
		delete			pi;
	}
	m_objects.Clear		( );
	
	if(m_tree_node_)
		m_tree_node_->Remove	( );

	if(m_project->get_focused_folder()==this)
	{
		if( this->m_parent )
			m_project->set_focused_folder	( safe_cast<project_item_folder^>(this->m_parent) );
	}
}

void project_item_folder::remove( project_item_base^ itm )
{

	if( itm->get_object() )
		m_objects.Remove		( itm );
	else
		m_folders.Remove		( itm );

	m_project->on_item_removed	( itm );
}

void project_item_folder::add( project_item_base^ itm )
{
	System::String^ name		= itm->get_name();

	if(	check_correct_existance(name, 1) )
		itm->set_name			(name);

	if(itm->get_object())
	{
		R_ASSERT				( !m_objects.Contains(itm) );
		m_objects.Add			( itm );
	}else
	{
		R_ASSERT				( !m_folders.Contains(itm) );
		m_folders.Add			( itm );
	}

	itm->m_parent				= this;
	m_project->on_item_added	( itm );
}

void project_item_folder::fill_tree_view( tree_node_collection^ nodes )
{
	if(!visible_for_project_tree( ))
		return;

	tree_node_collection^ collection	= nodes;
	
	//bool add_this = (this!=m_project->root());
	//if( add_this )
	{
		m_tree_node_						= gcnew controls::tree_node( get_name( ) );
		m_tree_node_->m_node_type			= m_node_type;
		m_tree_node_->Name					= m_tree_node_->Text;
		m_tree_node_->ImageIndex			= m_locked? xray::editor_base::node_group_locked : xray::editor_base::node_group;
		m_tree_node_->SelectedImageIndex	= m_locked? xray::editor_base::node_group_locked : xray::editor_base::node_group;
		collection->Add						( m_tree_node_ );
		m_tree_node_->Tag					= this;

		collection							= m_tree_node_->nodes; 
	}

	for each (project_item_folder^ pg in m_folders)
		pg->fill_tree_view				( collection );

	for each (project_item_object^ pi in m_objects)
		pi->fill_tree_view				( collection );
}

project_item_base^ project_item_folder::get( System::String^ item_name )
{
	for each (project_item_object^ pi in m_objects)
		if(pi->get_name()==item_name)
			return pi;

	for each (project_item_folder^ pg in m_folders)
	{
		if(pg->get_name() == item_name)
			return pg;
	}
	return nullptr;
}

void project_item_folder::set_focused( )
{
	m_project->set_focused_folder( this );
}

void project_item_folder::set_name_internal( System::String^ s )
{
	m_name			= s;

	if(m_tree_node_)
		m_tree_node_->Text = m_name;
}

void project_item_folder::on_before_manual_delete	( )
{
	for each (project_item_folder^ pg in m_folders)
		pg->on_before_manual_delete();

	for each (project_item_object^ pi in m_objects)
		pi->on_before_manual_delete();	
}

project_item_object::project_item_object( project^ p )
:super			( p ),
m_object_base	( nullptr ),
m_children		( nullptr )
{
	m_node_type	 = controls::tree_node_type::single_item ;
}

project_item_object::project_item_object( project^ p, object_base^ object )
:super					( p ), 
m_object_base			( object )
{
	m_node_type	 = controls::tree_node_type::single_item ;
	object->owner_project_item = this;
}

project_item_base^ project_item_object::get( System::String^ item_name )
{
	if ( m_children == nullptr )
		return nullptr;

	for each (project_item_object^ pi in m_children)
		if(pi->get_name()==item_name)
			return pi;

	return nullptr;
}

void project_item_object::remove( project_item_base^ itm )
{
	R_ASSERT						( m_children );
	m_children->Remove	( itm );
	
	R_ASSERT						( itm->get_object() );

	if (m_object_base->GetType() == object_composite::typeid ){
		object_composite^ o = safe_cast<object_composite^>(m_object_base);
		R_ASSERT						( o->is_edit_mode() );
		o->edit_remove_object			( itm->get_object(), false );
	}

	m_project->on_item_removed( itm );
}

void project_item_object::add( project_item_base^ itm )
{
	if ( m_children == nullptr )
		m_children = gcnew project_items_list();

	System::String^ name		= itm->get_name();

	if(	check_correct_existance(name, 1) )
		itm->set_name			( name );

	R_ASSERT					( itm->get_object() );

	R_ASSERT					( !m_children->Contains(itm) );
	m_children->Add	( itm );

	itm->m_parent				= this;
	m_project->on_item_added	( itm );
}

void project_item_object::after_name_edit( System::String^% new_name)
{
	m_parent->check_correct_existance	( new_name, 1 );
	m_object_base->set_name				( new_name, true );
}

void project_item_object::clear( bool b_destroy_object )
{
	if(m_tree_node_)
		m_tree_node_->Remove	();

	if(b_destroy_object)
		m_object_base->owner_tool()->destroy_object(m_object_base);

	if ( m_children != nullptr )
	{
		for each ( project_item_object^ child in m_children )
		{
			child->clear(b_destroy_object);
			delete		child;
		}
		m_children->Clear		( );
	}
	m_object_base		= nullptr;
	
}

System::String^ project_item_object::get_name( )
{
	R_ASSERT				( m_object_base );
	return m_object_base->get_name();
}

void project_item_object::set_name( System::String^ new_name )
{
	R_ASSERT				( m_object_base );
	m_object_base->set_name	( new_name, true );
}

void project_item_object::on_selected( bool b )
{
	R_ASSERT					( m_object_base );
	m_object_base->on_selected	( b );

	if( b && m_tree_node_ )
	{
		if(m_project->track_active_item)
			m_tree_node_->EnsureVisible	( );

		project_item_folder^ pgroup = dynamic_cast<project_item_folder^>(m_parent);
		
		if( pgroup )
			m_project->set_focused_folder( pgroup );
	}
}

void project_item_object::get_all_items( project_items_list^% result_list, bool recursive )
{
	
	if (recursive && m_children != nullptr )
	{
		for each ( project_item_object^ pi in m_children )
			pi->get_all_items( result_list, recursive );
	}
	result_list->Add		( this );
}

void project_item_object::fill_tree_view( tree_node_collection^ nodes )
{
	if(!visible_for_project_tree( ))
		return;

	m_tree_node_						= gcnew controls::tree_node(get_name());
	m_tree_node_->m_node_type			= m_node_type;
	m_tree_node_->Name					= m_tree_node_->Text;
	nodes->Add							( m_tree_node_ );

	m_tree_node_->ImageIndex			= m_object_base->image_index;
	m_tree_node_->SelectedImageIndex	= m_object_base->image_index;
	m_tree_node_->Tag					= this;

	if ( m_children != nullptr && m_children->Count > 0 )
	{
		for each ( project_item_object^ child_item in m_children )
		{
			child_item->fill_tree_view(m_tree_node_->nodes);
		}
	}
}

void project_item_object::refresh_ui( )
{
	if(m_tree_node_)
	{
		m_tree_node_->Text		= gcnew System::String(get_name());
		m_tree_node_->Name		= m_tree_node_->Text;
	}
}

bool project_item_object::get_persistent( )
{
	return m_object_base->get_persistent();
}

bool project_item_object::get_selectable( )
{
	return m_object_base->get_selectable();
}

bool project_item_object::visible_for_project_tree( )
{
	return m_object_base->visible_for_project_tree();
}

void project_item_object::on_before_manual_delete	( )
{
	m_object_base->on_before_manual_delete();
}

void project_background_item::load( pcstr resources_path )
{
	System::String^ fn = get_root_path( resources_path );
	fn					+= m_texture_name;
	
	//temporary solution, will be replaced with async texture loading
	m_bitmap = gcnew System::Drawing::Bitmap( fn );
}

void project_background_item::unload( )
{
	delete		m_bitmap;
	m_bitmap	= nullptr;
}

System::String^	project_background_item::get_root_path( pcstr path )
{
	System::String^ resource_path	= gcnew System::String(path);
	resource_path					+= "\\sources\\map_bkg\\";
	System::String^ full_path		= System::IO::Path::GetFullPath(resource_path);
	return full_path->ToLower();
}

tree_node^ find_node_with_label(tree_node_collection^ nodes, System::String^ label)
{
	for each(tree_node^ n in nodes)
		if(n->Text==label)
			return n;
	return nullptr;
}

tree_node^ find_hierrarchy( tree_node_collection^ root, System::String^ name )
{
	tree_node^	result						= nullptr;
	tree_node_collection^ current_root		= root;
	cli::array<System::String^>^ tokens		= name->Split('/');

	for(int i=0; i<tokens->GetLength(0); ++i)
	{
		System::String^ folder		= tokens[i];
		tree_node^ n = find_node_with_label	( current_root, folder );
		if(n!=nullptr)
		{
			result			= n;
			current_root	= n->nodes;
		}else
			return nullptr;
	}
	return result;
}

tree_node^ make_hierrarchy( tree_node_collection^ root, System::String^ name )
{
	tree_node_collection^ current_root = root;
	editor::controls::tree_node^		result;
	cli::array<System::String^>^ tokens = name->Split('/');

	for(int i=0; i<tokens->GetLength(0); ++i)
	{
		System::String^ folder		= tokens[i];
		tree_node^ n = find_node_with_label(current_root, folder);
		if(n!=nullptr)
		{
			current_root = n->nodes;
		}else
		{

			result						= gcnew xray::editor::controls::tree_node(folder);
			result->ImageIndex			= xray::editor_base::node_group;
			result->SelectedImageIndex	= xray::editor_base::node_group;
			result->Name				= folder;
			current_root->Add			( result );
			current_root				= result->nodes;
		}
	}
	R_ASSERT		(result, "item[%s] already exist", name);
	return			result;
}

void missing_resource_registry::register_substitute( System::String^ key, System::String^ source, System::String^ dest, subst_resource_result result )
{
	R_ASSERT(nullptr ==find(key, source) );
	subst_list^ list	= nullptr;
	if(!m_dictionary.ContainsKey(key))
	{
		list = gcnew subst_list;
		m_dictionary.Add(key, list);
	}else
		list = m_dictionary[key];

	subst_record^ record = gcnew subst_record;
	record->source_name		= source;
	record->dest_name		= dest;
	record->result			= result;
	
	list->Add				( record );
}

subst_record^ missing_resource_registry::find( System::String^ key, System::String^ source )
{
	if(m_dictionary.ContainsKey(key))
	{
		subst_list^ list		= m_dictionary[key];
		subst_record^ all		= nullptr;

		for each(subst_record^ record in list)
		{
			if( record->source_name == source )
				return record;

			if(record->source_name=="#all_objects")
				all	= record;
		}
		if(all)
			return all;
	}
	return nullptr;
}

void missing_resource_registry::clear( )
{
	m_dictionary.Clear();
};

System::String^ missing_resource_registry::choose_missing_resource( tool_base^ tool, System::String^ resource_name )
{
	if(nullptr == m_dialog)
		m_dialog				= gcnew resource_loading_dialog;

	m_dialog->init_labels		( tool->name(), "resource [" + resource_name + "] loading error" );
	subst_resource_result res	= m_dialog->do_dialog( tool, resource_name );
	
	switch (res)
	{
		case subst_resource_result::subst_one:
			return			m_dialog->selected_resource_name;
			break;

		case subst_resource_result::subst_all_by_name:
			register_substitute( tool->name(), resource_name, m_dialog->selected_resource_name, res );
			return			m_dialog->selected_resource_name;
			break;

		case subst_resource_result::subst_all_any_name:
			register_substitute( tool->name(), "#all_objects", m_dialog->selected_resource_name, res );
			return			m_dialog->selected_resource_name;
			break;

		case subst_resource_result::ignore_all_by_name:
			register_substitute( tool->name(), resource_name, "", res );
			return			resource_name;
			break;

		case subst_resource_result::ignore_all_any_name:
			register_substitute( tool->name(), "#all_objects", "", res );
			return			resource_name;
			break;

		case subst_resource_result::ignore_one:
			return			resource_name;
			break;
		default:
			UNREACHABLE_CODE();
			break;
	}
	UNREACHABLE_CODE();
	return nullptr;
}

bool is_valid_object_name( System::String^ s )
{
	if( s->Contains("]")	||
		s->Contains("[")	||
		s->Contains(".")	||
		s->Contains(",")	||
		s->Contains("\\")	||
		s->EndsWith("/")	||
		s->StartsWith("/")	||
		s=="0" )
	return false;
 
	int len = s->Length;
	bool all_digit = true;
	for(int i=0; i<len; ++i)
	{
		all_digit &= System::Char::IsDigit(s[i]);
		if(!all_digit)
			break;
	}

	if(all_digit)
		return false;

	return true;
}

wpf_controls::property_container^ project_item_folder::get_property_container( )
{
	wpf_controls::property_container^ cont = gcnew wpf_controls::property_container;
	editor_base::object_properties::initialize_property_container	( this, cont );
	return cont;
}

} // namespace editor
} // namespace xray
