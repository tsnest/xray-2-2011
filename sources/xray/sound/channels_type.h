////////////////////////////////////////////////////////////////////////////
//	Created		: 26.05.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef CHANNELS_TYPE_H_INCLUDED
#define CHANNELS_TYPE_H_INCLUDED

namespace xray {
namespace sound {

enum channels_type 
{
	mono,
	stereo,
	channels_type_count,
}; // enum channels_type

enum channels 
{
	front_left = 0,
    front_right,
    front_center,
    lfe,
    back_left,
    back_right,
    back_center,
    side_left,
    side_right,

	channels_count
}; // enum channels_type

} // namespace sound
} // namespace xray

#endif // #ifndef CHANNELS_TYPE_H_INCLUDED