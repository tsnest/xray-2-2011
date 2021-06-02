////////////////////////////////////////////////////////////////////////////
//	Created		: 12.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_single_item.h"
#include "animation_editor.h"

#pragma managed ( push, off )
#	include <xray/animation/api.h>
#	include <xray/animation/mixing_expression.h>
#	include <xray/animation/animation_expression_emitter.h>
#pragma managed ( push, on )

namespace xray{
namespace animation_editor{

using xray::animation::animation_expression_emitter_ptr;
using xray::animation::animation_expression_emitter;

static animation_single_item::animation_single_item	( )
{
	extension = ".clip";
}

animation_single_item::animation_single_item ( animation_collections_editor^ parent_editor, String^ animation_name ): animation_item( parent_editor )
{
	m_name			= animation_name;
	m_is_loaded		= false;
}

animation_single_item::~animation_single_item ( )
{
	if( m_animation_ptr != NULL )
		DELETE( m_animation_ptr );
}

void animation_single_item::load ( )
{
	m_is_loaded					= false;
	query_result_delegate* q	= NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &animation_single_item::item_loaded_callback ), g_allocator);
	query_resource				( unmanaged_string ( m_name ).c_str( ), resources::single_animation_class, boost::bind(&query_result_delegate::callback, q, _1), g_allocator);
}

xray::animation::mixing::expression animation_single_item::emit	( mutable_buffer& buffer, bool& is_last_animation )
{
	animation_expression_emitter_ptr ptr	= *m_animation_ptr;
	animation_expression_emitter* c_ptr		= ptr.c_ptr( );
	return c_ptr->emit						( buffer, is_last_animation );
}

void animation_single_item::item_loaded_callback(xray::resources::queries_result& data)
{
	R_ASSERT( data.is_successful( ) );

	m_animation_ptr = NEW(animation_expression_emitter_ptr)( static_cast_resource_ptr<animation_expression_emitter_ptr>( data[0].get_unmanaged_resource( ) ) );
	m_is_loaded		= true;

	loaded( this, EventArgs::Empty );
}

} // namespace animation_editor
} // namespace xray