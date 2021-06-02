////////////////////////////////////////////////////////////////////////////
//	Created		: 12.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_collection_document_commands.h"
#include "animation_collection_document.h"
#include "animation_single_item.h"
#include "animation_collection_item.h"
#include "animation_collections_editor.h"

namespace xray{
namespace animation_editor{

using xray::editor::controls::tree_node;
using xray::editor::wpf_controls::dot_net_helpers;


ref class item_updater
{
public:
	item_updater( animation_collection_document^ document, animation_collection_item^ item )
	{
		m_item		= item;
		m_document	= document;
	}

private:
	animation_collection_item^		m_item;
	animation_collection_document^	m_document;

public:
	void	run_update	( Object^ , EventArgs^ )
	{
		m_item->loaded -= gcnew EventHandler( this, &item_updater::run_update );
		m_document->update_tree( m_document->collection_node( ), m_item );
	}
};

////////////////////////////////////////////////////////////////////////////
// public ref class sound_collection_add_items_command /////////////////////
////////////////////////////////////////////////////////////////////////////
animation_collection_document_add_items_command::animation_collection_document_add_items_command ( animation_collection_document^ d, hierarchy_item_id^ destination_index, List<String^>^ items_paths, animation_item_type item_type ):
	m_document( d ),
	m_destination_index( destination_index )
{
	m_new_items_paths			= gcnew List<String^>( items_paths->Count );
	m_new_items_paths->AddRange	( items_paths );
	m_items_type				= item_type;
}

animation_collection_document_add_items_command::~animation_collection_document_add_items_command ( )
{
	m_new_items_paths->Clear	( );
	m_new_items_paths			= nullptr;
	m_document					= nullptr;
}

bool animation_collection_document_add_items_command::commit( )
{
	animation_item^ item;

	for( Int32 i = m_new_items_paths->Count - 1; i >= 0; --i )
	{
		String^ path = m_new_items_paths[i];
		item = ( m_items_type == animation_item_type::animation_single_item ) ?
			(animation_item^)m_document->get_editor( )->get_single		( path ) :
			(animation_item^)m_document->get_editor( )->get_collection	( path );

		if( !item->is_loaded && m_items_type == animation_item_type::animation_collection_item  )
		{
			item_updater^ updater	= gcnew item_updater( m_document, safe_cast<animation_collection_item^>( item ) );
			item->loaded			+= gcnew EventHandler( updater, &item_updater::run_update );
		}

		m_document->insert_item_into_collection		( m_destination_index, item );
	}

	return true;
}

void animation_collection_document_add_items_command::rollback( )
{
	Int32 count = m_new_items_paths->Count;

	for( Int32 i = 0; i < count; ++i )
		m_document->remove_item_from_collection( m_destination_index );
}
////////////////////////////////////////////////////////////////////////////
// public ref class sound_collection_remove_items_command //////////////////
////////////////////////////////////////////////////////////////////////////
animation_collection_document_remove_items_command::animation_collection_document_remove_items_command( animation_collection_document^ document, List<tree_node^>^ nodes_list ):
	m_document( document )
{
	m_saved_data = gcnew List<stored_item_data>( nodes_list->Count );

	nodes_list->Remove( m_document->collection_node( ) );

	int count = nodes_list->Count;
	for( int i = 0; i < count; ++i )
	{
		tree_node^ node			= nodes_list[i];
		tree_node^ tmp_node		= node;
		while( tmp_node != m_document->collection_node( ) )
		{
			if( nodes_list->Contains( tmp_node->Parent ) )
				goto next_list_item;

			tmp_node = safe_cast<tree_node^>( tmp_node->Parent );
		}

		animation_item^				item				= safe_cast<animation_item^>( nodes_list[i]->Tag );
		animation_collection_item^	collection_item		= dot_net_helpers::as_type<animation_collection_item^>( item );

		if( collection_item != nullptr && collection_item->is_internal_collection )
			m_saved_data->Add( stored_item_data( m_document->get_id_from_node( nodes_list[i] ), item->name, item->type, item ) );
		else
			m_saved_data->Add( stored_item_data( m_document->get_id_from_node( nodes_list[i] ), item->name, item->type ) );

next_list_item:;
	}
}

animation_collection_document_remove_items_command::~animation_collection_document_remove_items_command( )
{	
	m_saved_data->Clear		( );
	m_saved_data			= nullptr;
	m_document				= nullptr;
}

bool animation_collection_document_remove_items_command::commit( )
{
	for ( Int32 i = m_saved_data->Count - 1; i >= 0; --i )
		m_document->remove_item_from_collection( m_saved_data[i].m_index );

	return true;
}

void animation_collection_document_remove_items_command::rollback( )
{
	for each( stored_item_data data in m_saved_data )
	{
		animation_item^ item = ( data.m_type == animation_item_type::animation_single_item ) ?
			(animation_item^)m_document->get_editor( )->get_single		( data.m_name ) :
			( data.m_item != nullptr ) ?
				data.m_item :
				(animation_item^)m_document->get_editor( )->get_collection	( data.m_name );

		m_document->insert_item_into_collection( data.m_index, item );
	}
}

////////////////////////////////////////////////////////////////////////////
// public ref class animation_collection_document_reorganize_command /////////////////////
////////////////////////////////////////////////////////////////////////////
animation_collection_document_reorganize_command::animation_collection_document_reorganize_command( animation_collection_document^ document, hierarchy_item_id^ destination_id, List<tree_node^>^ nodes_list ):
	m_document			( document ),
	m_destination_id	( destination_id )
{
	m_stored_ids	= gcnew array<hierarchy_item_id^>	( nodes_list->Count );
	
	TreeNodeCollection^ nodes = m_document->get_parent_node_by_id( destination_id )->nodes;

	for( Int32 i = destination_id->last_index; i >= 0; --i )
	{
		if( i >= nodes->Count )
			continue;

		if( nodes_list->Contains( safe_cast<tree_node^>( nodes[i] ) ) )
			--m_destination_id->last_index;
	}
	m_destination_id->last_index = math::max( m_destination_id->last_index, 0 );

	for( Int32 i = 0; i < nodes_list->Count; ++i )
		m_stored_ids[i] = m_document->get_id_from_node( nodes_list[i] );
}

animation_collection_document_reorganize_command::~animation_collection_document_reorganize_command ( )
{
	m_document					= nullptr;
}

bool animation_collection_document_reorganize_command::commit( )
{
	List<animation_item^> items = gcnew List<animation_item^>( m_stored_ids->Length );

	for ( Int32 i = m_stored_ids->Length - 1; i >= 0; --i )
	{
		hierarchy_item_id^ id						= m_stored_ids[i];
		animation_item^ item						= safe_cast<animation_item^>( m_document->get_node_by_id( id )->Tag );
		items.Insert								( 0, item );
		m_document->remove_item_from_collection		( id );
	}

	for( Int32 i = items.Count - 1; i >= 0; --i )
		m_document->insert_item_into_collection		( m_destination_id, items[i] );

	return true;
}

void animation_collection_document_reorganize_command::rollback( )
{
	List<animation_item^> items = gcnew List<animation_item^>( m_stored_ids->Length );

	Int32 count = m_stored_ids->Length;
	for( Int32 i = 0; i < count; ++i )
	{
		animation_item^ item						= safe_cast<animation_item^>( m_document->get_node_by_id( m_destination_id )->Tag );
		items.Add									( item );
		m_document->remove_item_from_collection		( m_destination_id );
	}

	count = items.Count;
	for( Int32 i = 0; i < count; ++i )
		m_document->insert_item_into_collection		( m_stored_ids[i], items[i] );
}

////////////////////////////////////////////////////////////////////////////
// public ref class sound_collection_change_objects_property_command ///////
////////////////////////////////////////////////////////////////////////////

animation_collection_document_change_objects_property_command::animation_collection_document_change_objects_property_command( animation_collection_document^ document, hierarchy_item_id^ destination_id, String^ prop_name, Object^ old_val, Object^ new_val ):
	m_document		( document ),
	m_destination_id( destination_id ),
	m_property_name	( prop_name ),
	m_old_val		( old_val ),
	m_new_val		( new_val )
{
}

animation_collection_document_change_objects_property_command::~animation_collection_document_change_objects_property_command( )
{
	m_document			= nullptr;
	m_destination_id	= nullptr;
}

bool animation_collection_document_change_objects_property_command::commit( )
{
	animation_collection_item^ item = safe_cast<animation_collection_item^>( m_document->get_node_by_id( m_destination_id )->Tag );
	System::Reflection::PropertyInfo^ propertyInfo = item->GetType( )->GetProperty( m_property_name );
	R_ASSERT( propertyInfo );

	propertyInfo->SetValue( item, m_new_val, nullptr );

	if( m_property_name == "name" )
	{
		m_document->update_tree_names( );

		if( item == m_document->collection( ) )
			m_document->rename_to( m_new_val->ToString( ), m_old_val->ToString( ) );
	}

	m_document->update_properties( );

	return true;
}

void animation_collection_document_change_objects_property_command::rollback( )
{
	animation_collection_item^ item = safe_cast<animation_collection_item^>( m_document->get_node_by_id( m_destination_id )->Tag );
	System::Reflection::PropertyInfo^ propertyInfo = item->GetType( )->GetProperty( m_property_name );
	R_ASSERT( propertyInfo );

	propertyInfo->SetValue( item, m_old_val, nullptr );

	if( m_property_name == "name" )
	{
		m_document->update_tree_names( );

		if( item == m_document->collection( ) )
			m_document->rename_to( m_old_val->ToString( ), m_new_val->ToString( ) );
	}

	m_document->update_properties( );
}

////////////////////////////////////////////////////////////////////////////
// public ref class sound_collection_create_collection_command /////////////
////////////////////////////////////////////////////////////////////////////

animation_collection_document_create_collection_command::animation_collection_document_create_collection_command( animation_collection_document^ d, hierarchy_item_id^ destination_id ):
	m_document			( d ),
	m_destination_id	( destination_id )
{
}

animation_collection_document_create_collection_command::~animation_collection_document_create_collection_command( )
{
}

Boolean animation_collection_document_create_collection_command::commit( )
{
	animation_collection_item^ item			= gcnew animation_collection_item( m_document->get_editor( ), "unnamed" );
	item->is_internal_collection			= true;
	m_document->insert_item_into_collection	( m_destination_id, item );

	return true;
}

void animation_collection_document_create_collection_command::rollback( )
{
	m_document->remove_item_from_collection	( m_destination_id );
}

} // namespace animation_editor
} // namespace xray
