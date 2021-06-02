////////////////////////////////////////////////////////////////////////////
//	Created		: 30.09.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_H_INCLUDED
#define MIXING_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

enum playing_type_enum {
	// play cyclic animation infinitely
	// on animation end just rewind to the start
	play_cyclically,		
	
	// play single animation;
	// on animation end set time scale for animation to 0
	// as a consequence animation will stay at the last frame infinitely
	play_once_and_freeze_at_end,
	
	// play single animation;
	// on animation end instantly remove animation
	// as a consequence animation will be removed immediately after its end
	play_once_and_remove_at_end,
}; // enum playing_type_enum

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef MIXING_H_INCLUDED