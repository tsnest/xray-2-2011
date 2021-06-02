////////////////////////////////////////////////////////////////////////////
//	Created		: 27.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_ANIMATION_H_INCLUDED
#define XRAY_ANIMATION_ANIMATION_H_INCLUDED

namespace xray {
namespace animation {

class XRAY_ANIMATION_API noncopyable 
{
protected:
	inline						noncopyable	( ) { }
	inline						~noncopyable( ) { }

private:  // emphasize the following members are private
								noncopyable	( const noncopyable& );
	inline	noncopyable const&	operator=	( const noncopyable& );
}; // class noncopyable 

} // namespace animation
} // namespace xray

#endif // #ifndef XRAY_ANIMATION_ANIMATION_H_INCLUDED