////////////////////////////////////////////////////////////////////////////
//	Created		: 10.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_collection_item.h"
#include "animation_editor.h"
#include "animation_single_item.h"
#include "animation_collections_editor.h"

#pragma managed ( push, off )
#	include <xray/animation/api.h>
#	include <xray/animation/mixing_expression.h>
#	include <xray/animation/animation_expression_emitter.h>
#pragma managed ( push, on )

namespace xray{
namespace animation_editor{

using xray::animation::animation_expression_emitter_ptr;
using xray::editor::wpf_controls::dot_net_helpers;

static animation_collection_item::animation_collection_item( )
{
	extension = ".anim_collection";
}
animation_collection_item::animation_collection_item( animation_collections_editor^ parent_editor, String^ collection_name ): animation_item( parent_editor )
{
	m_name = collection_name;
	m_type = animation_collection_type::random;
	m_is_cyclic_repeat			= false;
	m_is_dont_repeat_previous	= false;
	m_loading_sub_items			= 0;
	m_is_child_last_animation	= true;

	items = gcnew List<animation_item^>( );
}

static Boolean is_correct_collection( animation_collection_item^ item )
{
	for each( animation_item^ sub_item in item->items )
	{
		animation_collection_item^ collection = dot_net_helpers::as_type<animation_collection_item^>( sub_item );

		if( collection == nullptr )
			continue;

		if( collection->items->Count == 0 )
			return false;

		return is_correct_collection( collection );
	}
	return false;
}

Boolean animation_collection_item::is_correct::get( )
{
	if( items->Count == 0 )
		return false;

	return is_correct_collection( this );
}

Boolean animation_collection_item::can_save ( )
{
	if( items->Count == 0 )
		return false;

	for each( animation_item^ item in items )
	{
		animation_collection_item^ collection_item = dot_net_helpers::as_type<animation_collection_item^>( item );
		if( collection_item != nullptr && !collection_item->can_save( ) )
			return false;
	}

	return true;
}

void animation_collection_item::save ( )
{
	configs::lua_config_ptr cfg_ptr = xray::configs::create_lua_config(
		unmanaged_string( animation_editor::animation_collections_absolute_path + "/" + m_name + extension ).c_str( )
	);

	configs::lua_config_value value = cfg_ptr->get_root( )["collection"];
	save			( value );
	cfg_ptr->save	( configs::target_default );
}

void animation_collection_item::save ( configs::lua_config_value& config_value )
{
	save_anim_item( config_value, this );
}

void animation_collection_item::load ( )
{
	m_is_loaded					= false;
	String^ path				= gcnew String( animation_editor::animation_collections_path + "/" + m_name + extension );
	query_result_delegate* q	= NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &animation_collection_item::item_loaded_callback ), g_allocator);

	unmanaged_string lua_path ( path );

	resources::request requests[2] = {
		{ lua_path.c_str( ), resources::lua_config_class },
	};

	xray::resources::query_resource_params params		(
		requests, 
		NULL, 
		1, 
		boost::bind( &query_result_delegate::callback, q, _1 ),  
		g_allocator
	);

	query_resources										( params );
}

void animation_collection_item::load ( configs::lua_config_value const& config_value )
{
	load_anim_item ( config_value, this );

	if( m_loading_sub_items != 0 )
		return;

	m_is_loaded = true;
	loaded( this, EventArgs::Empty );
}

void animation_collection_item::add_item ( animation_item^ item )
{
	insert_item( items->Count, item );
}

void animation_collection_item::insert_item	( Int32 index, animation_item^ item )
{
	items->Insert	( index, item );

	if( !item->is_loaded )
	{
		m_is_loaded		= false;
		++m_loading_sub_items;
		item->loaded	+= gcnew EventHandler( this, &animation_collection_item::sub_item_loaded_callback );
	}
}

void animation_collection_item::remove_item ( animation_item^ item )
{
	items->Remove	( item );
}

void animation_collection_item::remove_item_at ( Int32 index )
{
	items->RemoveAt	( index );
}

xray::animation::mixing::expression animation_collection_item::emit ( mutable_buffer& buffer, bool& is_last_animation )
{
	R_ASSERT( items->Count > 0 );
	
	bool is_child_last_animation;

	switch ( m_type )
	{
	case animation_collection_type::random:
	{
		is_last_animation				= !is_cyclic_repeat;
		u32 const animations_count		= items->Count;

		if( !is_dont_repeat_previous )
		{
			m_current_animation_index	= m_random.Next( animations_count );
			break;
		}

		s32 previous_animation_index	= m_current_animation_index;
		do
		{
			m_current_animation_index	= m_random.Next( animations_count );
		} while ( previous_animation_index == m_current_animation_index );
	
		break;
	}

	case animation_collection_type::sequential:
	{
		if( !m_is_child_last_animation )
			break;

		s32 const animations_count			= items->Count;
		m_current_animation_index			= ( m_current_animation_index + 1 ) % animations_count;
		is_last_animation					= !is_cyclic_repeat && ( m_current_animation_index == animations_count - 1 );
		break;
	}

	default:
		NODEFAULT							( m_current_animation_index = u32(-1) );
	}	

	expression ret_expression	= items[ m_current_animation_index ]->emit( buffer, is_child_last_animation );
	m_is_child_last_animation	= is_child_last_animation;
	is_last_animation			= is_last_animation && m_is_child_last_animation;

	return ret_expression;
}

///////////////////////////////////////////// 		P R I V A T E   M E T H O D S		//////////////////////////////////////////

void animation_collection_item::load_anim_item ( configs::lua_config_value const& config_value, animation_collection_item^ item )
{
	item->m_is_loaded						= false;
	item->m_is_cyclic_repeat				= config_value["is_cyclic_repeat"];
	item->m_is_dont_repeat_previous			= config_value["is_dont_repeat_previous"];
	item->m_type							= (animation_collection_type)( gcnew String( (pcstr)config_value["collection_type"] ) == "sequential" );

	item->items->Clear	( );

	if( !config_value.value_exists( "animation_items" ) )
		return;

	configs::lua_config_value items_value = config_value["animation_items"];
	int count = items_value.size( );
	for ( int i = 0; i < count; ++i )
	{
		animation_item_type type		= (animation_item_type)(int)items_value[i]["type"];
	
		switch( type )
		{
		case animation_item_type::animation_single_item:
		{
			String^ name					= gcnew String( (pcstr)items_value[i]["path"] );
			animation_single_item^ sub_item	= m_parent_editor->get_single( name );
			items->Add						( sub_item );

			if( sub_item->is_loaded )
				continue;

			++m_loading_sub_items;
			sub_item->loaded += gcnew EventHandler( this, &animation_collection_item::sub_item_loaded_callback );
			break;
		}

		case animation_item_type::animation_composite_item:
		case animation_item_type::animation_collection_item:
		{
			animation_collection_item^ sub_item;

			if( items_value[i].value_exists( "name" ) )
			{
				String^ name						= gcnew String( (pcstr)items_value[i]["name"] );
				sub_item							= gcnew animation_collection_item( parent_editor, name );
				sub_item->is_internal_collection	= true;
				sub_item->load						( items_value[i] );
				items->Add							( sub_item );
			}
			else
			{
				String^ name						= gcnew String( (pcstr)items_value[i]["path"] );
				sub_item							= m_parent_editor->get_collection( name );
				items->Add							( sub_item );
			}

			if( sub_item->is_loaded )
				continue;

			++m_loading_sub_items;
			sub_item->loaded += gcnew EventHandler( this, &animation_collection_item::sub_item_loaded_callback );
			break;
		}

		default:
			NODEFAULT( );
		}
	}
}

void animation_collection_item::save_anim_item ( configs::lua_config_value& config_value, animation_collection_item^ item )
{
	config_value["is_cyclic_repeat"]			= item->m_is_cyclic_repeat;
	config_value["is_dont_repeat_previous"]		= item->m_is_dont_repeat_previous;
	config_value["collection_type"]				= unmanaged_string( item->m_type.ToString( ) ).c_str( );

	int count = item->items->Count;
	if( count == 0 )
		return;

	configs::lua_config_value items_value = config_value["animation_items"];
	for ( int i = 0; i < count; ++i )
	{
		items_value[i]["type"] = (int)items[i]->type;

		animation_item^ item = items[i];
		if( dot_net_helpers::is_type<animation_collection_item^>( item ) )
		{
			animation_collection_item^ collection_item = safe_cast<animation_collection_item^>( item );
			if( collection_item->is_internal_collection )
			{	
				items_value[i]["name"] = unmanaged_string( collection_item->name ).c_str( );
				configs::lua_config_value item_value = items_value[i];
				collection_item->save( item_value );
				continue;
			}
		}

		items_value[i]["path"] = unmanaged_string( items[i]->name ).c_str( );
	}
}

void animation_collection_item::item_loaded_callback(xray::resources::queries_result& data)
{
	R_ASSERT( data.is_successful( ) );
	
	configs::lua_config_ptr cfg = static_cast_resource_ptr<configs::lua_config_ptr>( data[0].get_unmanaged_resource( ) );
	load( cfg->get_root( )["collection"] );

	if( m_is_loaded )
		loaded( this, EventArgs::Empty );
}

void animation_collection_item::sub_item_loaded_callback ( Object^, EventArgs^  )
{
	--m_loading_sub_items;
	if( m_loading_sub_items == 0 )
	{
		m_is_loaded		= true;
		loaded			( this, EventArgs::Empty );
	}
}

} // namespace animation_editor
} // namespace xray
