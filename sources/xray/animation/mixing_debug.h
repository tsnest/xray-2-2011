////////////////////////////////////////////////////////////////////////////
//	Created		: 30.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#if !defined(MASTER_GOLD) && !defined(XRAY_ANIMATION_CHECK_CONSTRUCTORS)
#	define	XRAY_ANIMATION_CHECK_CONSTRUCTORS
#endif // #ifndef MASTER_GOLD

#ifdef XRAY_ANIMATION_CHECK_CONSTRUCTORS
namespace xray {
namespace animation {
namespace mixing {

struct tree_stats {
	threading::atomic32_type	constructors_count;
	threading::atomic32_type	destructors_count;
	threading::atomic32_type	difference;

	inline tree_stats				( ) :
		constructors_count	( 0 ),
		destructors_count	( 0 ),
		difference			( 0 )
	{
	}

	inline tree_stats				(
			u32 constructors_count,
			u32 destructors_count,
			u32 difference
		) :
		constructors_count	( constructors_count ),
		destructors_count	( destructors_count ),
		difference			( difference )
	{
	}

	inline tree_stats				( tree_stats const volatile & other ) :
		constructors_count	( other.constructors_count ),
		destructors_count	( other.destructors_count ),
		difference			( other.difference )
	{
	}

	inline tree_stats& operator =	( tree_stats const& other )
	{
		constructors_count	= other.constructors_count;
		destructors_count	= other.destructors_count;
		difference			= other.difference;
		return				*this;
	}

	inline tree_stats& operator -=	( tree_stats const& other )
	{
		constructors_count	-= other.constructors_count;
		destructors_count	-= other.destructors_count;
		difference			-= other.difference;
		return				*this;
	}
}; // struct tree_stats

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifdef XRAY_ANIMATION_CHECK_CONSTRUCTORS

#endif // #ifndef DEBUG_H_INCLUDED