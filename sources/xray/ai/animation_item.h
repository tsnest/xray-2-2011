////////////////////////////////////////////////////////////////////////////
//	Created		: 21.09.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_ANIMATION_ITEM_H_INCLUDED
#define XRAY_AI_ANIMATION_ITEM_H_INCLUDED

namespace xray {
namespace ai {

struct animation_item
{
	animation_item						( resources::unmanaged_resource_ptr const& new_item, pcstr filename );

	resources::unmanaged_resource_ptr	animation;
	fs_new::virtual_path_string			name;
}; // struct animation_item

struct animation_item_wrapper
{
	inline animation_item_wrapper		( animation_item const* const item ) :
		animation						( item ),
		next							( 0 )
	{
	}
	
	animation_item const*				animation;
	animation_item_wrapper*				next;
}; // struct animation_item_wrapper

typedef intrusive_list< animation_item_wrapper,
						animation_item_wrapper*,
						&animation_item_wrapper::next >	animations_type;

typedef fixed_vector< animation_item const*, 32 >		animation_items_type;

} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_ANIMATION_ITEM_H_INCLUDED