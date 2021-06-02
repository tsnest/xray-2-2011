////////////////////////////////////////////////////////////////////////////
//	Created		: 22.11.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef BASE_ANIMATION_CONTROLLER_H_INCLUDED
#define BASE_ANIMATION_CONTROLLER_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {
	class expression;
} // namespace mixing
} // namespace animation
} // namespace xray

namespace stalker2 {

struct animation_controller_parameters;

struct XRAY_NOVTABLE base_animation_controller : private boost::noncopyable
{
public:
	virtual	void								initialize			( )	= 0;
// 	virtual	xray::animation::mixing::expression	finalize			( base_animation_controller& next_controller )	= 0;
// 	virtual xray::animation::mixing::expression	selected_animations	( )	= 0;
	virtual	void								set_target			( animation_controller_parameters const& target ) = 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR							( base_animation_controller );
}; // struct base_animation_controller

} // namespace stalker2

#endif // #ifndef BASE_ANIMATION_CONTROLLER_H_INCLUDED