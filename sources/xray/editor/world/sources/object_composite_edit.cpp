////////////////////////////////////////////////////////////////////////////
//	Created		: 30.09.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_composite.h"
#include "object_scene.h"
#include "tool_composite.h"
#include "project_items.h"
#include "level_editor.h"
#include "project.h"
#include "logic_editor_panel.h"

namespace xray{
namespace editor{

void object_composite::edit_add_object( object_base^ o )
{
	if(o==this)					return;

	R_ASSERT					( is_edit_mode() );
	if( -1!=index_of(o) )
	{
		LOG_WARNING				("object [%s] already a member of compound [%s]", o->get_name(), get_name());
		return;
	}
	
	if(o->owner_tool()==owner_tool())
	{
		LOG_WARNING				("compound in compound not supported yet" );
		return;
	}

	R_ASSERT					( o->owner_project_item != nullptr );

	// make unique name
	System::String^ new_object_name						= o->get_name();
	if(	owner_project_item->check_correct_existance(new_object_name, 1) )
		o->set_name		( new_object_name, false );

	if( o->owner_project_item->m_parent!=nullptr )
		o->owner_project_item->m_parent->remove( o->owner_project_item );

	if( o->owner_project_item->m_tree_node_ )
		o->owner_project_item->m_tree_node_->Remove( );

	o->owner_project_item->m_tree_node_				= nullptr;
	owner_project_item->m_children->Add				( o->owner_project_item );
	o->owner_project_item->m_parent					= owner_project_item;

	o->owner_project_item->fill_tree_view				( owner_project_item->m_tree_node_->nodes );

	add_object					( o, NULL );
	
	if(!o->get_visible())
		o->set_visible			( true );

	o->load_contents			( );
	
	setup_object_props(m_objects[index_of(o)]);
}

void object_composite::edit_remove_object( object_base^ o, bool destroy )
{
	R_ASSERT					( is_edit_mode() );
	int object_idx				= index_of( o );
	if(-1==object_idx )
	{
		LOG_WARNING	("object [%s] is not a member of compound [%s]", o->get_name(), get_name());
		return;
	}

	m_objects->RemoveAt	( object_idx );
	o->owner_project_item->m_parent	= nullptr;
	o->set_parent				( nullptr );
	if( destroy )
	{
		o->owner_tool()->destroy_object(o);
	}else
	{	// just detach from compound
		// put this object into scene
	}

	initialize_collision	( );
}

void close_scene_logic_view ( object_scene^ scene )
{
	for each( object_scene^ sub_sene in scene->scenes )
	{
		close_scene_logic_view( sub_sene );
	}
	if ( scene->own_logic_view != nullptr ){
		scene->own_logic_view->Close();
		//scene->own_logic_view = nullptr;
	}
}

void object_composite::switch_to_edit_mode( bool value )
{
	editor_base::command_engine^ ce  = m_tool_composite->get_level_editor()->get_command_engine();
	ce->clear_stack		( );
	if(value)
	{
		R_ASSERT(nullptr == owner_project_item->m_children);
		owner_project_item->m_children = gcnew project_items_list;

		for each (composite_object_item^ itm in m_objects)
		{
			project_item_object^ itm_object = itm->m_object->owner_project_item;

			R_ASSERT			(itm_object->m_tree_node_ == nullptr );

			R_ASSERT			( ! owner_project_item->m_children->Contains(itm_object) );
			owner_project_item->m_children->Add	( itm_object );
			itm_object->m_parent = owner_project_item;
			
			itm->m_object->owner_project_item->fill_tree_view( owner_project_item->m_tree_node_->nodes );
			itm->m_object->m_object_transform_changed		+= gcnew object_message( this, &object_composite::content_transform_changed_notify);
//.			itm_object->m_project->on_item_added			( itm_object );
			
			setup_object_props								( itm );
		}
		owner_project_item->m_tree_node_->Expand			( );
		owner_project_item->m_tree_node_->EnsureVisible	( );
	}else
	{
		for each (composite_object_item^ itm in m_objects)
		{
			project_item_object^ itm_object = itm->m_object->owner_project_item;
//.			itm_object->m_project->on_item_removed		( itm_object );

			itm_object->m_tree_node_->Remove			( );
			itm_object->m_tree_node_					= nullptr;

			itm->m_object->m_object_transform_changed	-= gcnew object_message( this, &object_composite::content_transform_changed_notify);
			itm->m_props_dictionary						= nullptr;
			itm_object->m_parent						= nullptr;
		}
		R_ASSERT(owner_project_item->m_children);
		R_ASSERT(owner_project_item->m_children->Count == m_objects->Count);

		if ( logic_scene != nullptr )
		{
			close_scene_logic_view(logic_scene);
		}
		
		owner_project_item->m_children->Clear			( );
		owner_project_item->m_children					= nullptr;	
	}

	ce->clear_stack		( );
}

bool object_composite::is_edit_mode( )
{
	return (m_tool_composite->current_edit_object == this);
}

void object_composite::content_transform_changed_notify( object_base^ o )
{
	int idx						= index_of( o );
	R_ASSERT					( idx!=-1);
	composite_object_item^ itm	= m_objects[idx];
	update_object_offset		( itm );
}

} //namespace editor
} //namespace xray


