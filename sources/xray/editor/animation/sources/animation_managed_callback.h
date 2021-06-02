////////////////////////////////////////////////////////////////////////////
//	Created		: 07.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_MANAGED_CALLBACK_H_INCLUDED
#define ANIMATION_MANAGED_CALLBACK_H_INCLUDED

#include <xray/animation/animation_callback.h>

namespace xray
{
	namespace animation_editor
	{
		struct animation_managed_callback
		{
			animation_managed_callback( ){}
			animation_managed_callback( animation_callback^ callback ):m_delegate( callback ){}

			animation::callback_return_type_enum callback( animation::skeleton_animation_ptr const& animation, pcstr channel_id, u32 callback_time_in_ms, u8 domain_data )
			{
				animation_callback^ d	= m_delegate;
				d( animation, channel_id, callback_time_in_ms, domain_data );

				return animation::callback_return_type_call_me_again;
			}
			gcroot<animation_callback^>	m_delegate;
		}; // class animation_managed_callback

	} // namespace animation_editor
} // namespace xray

#endif // #ifndef ANIMATION_MANAGED_CALLBACK_H_INCLUDED