////////////////////////////////////////////////////////////////////////////
//	Created		: 10.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_item.h"
#include "animation_single_item.h"
#include "animation_collection_item.h"

namespace xray{
namespace animation_editor{

animation_item::animation_item ( animation_collections_editor^ parent_editor )
{
	m_parent_editor		= parent_editor;
	m_is_loaded			= true;
}

animation_item_type animation_item::type::get		( )
{
	if( xray::editor::wpf_controls::dot_net_helpers::is_type<animation_collection_item^>( this ) )
		return animation_item_type::animation_collection_item;

	R_ASSERT( xray::editor::wpf_controls::dot_net_helpers::is_type<animation_single_item^>( this ) );
	return animation_item_type::animation_single_item;
}

} // namespace animation_editor
} // namespace xray
