////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BASE_INTERPOLATOR_H_INCLUDED
#define BASE_INTERPOLATOR_H_INCLUDED

#include "interpolator_visitor.h"
#include "interpolator_comparer.h"

namespace xray {
namespace animation {

struct base_interpolator;
class instant_interpolator;
class linear_interpolator;
class fermi_dirac_interpolator;

struct XRAY_NOVTABLE base_interpolator {
public:
	virtual	float	interpolated_value	( float transition_time ) const = 0;
	virtual	base_interpolator*	clone	( mutable_buffer& buffer ) const = 0;
	virtual	float	transition_time		( ) const = 0;

public:
	virtual void	accept				( interpolator_visitor& visitor ) const = 0;

	virtual void	accept				( interpolator_comparer& dispatcher, base_interpolator const& interpolator ) const = 0;
	virtual void	visit				( interpolator_comparer& dispatcher, instant_interpolator const& interpolator ) const = 0;
	virtual void	visit				( interpolator_comparer& dispatcher, linear_interpolator const& interpolator ) const = 0;
	virtual void	visit				( interpolator_comparer& dispatcher, fermi_dirac_interpolator const& interpolator ) const = 0;

protected:
	template < typename T>
	inline	T*		clone_impl			( xray::mutable_buffer& buffer ) const;

public:
	inline	bool	operator ==			( base_interpolator const& other ) const;
	inline	bool	operator !=			( base_interpolator const& other ) const;

	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( base_interpolator )
}; // struct base_interpolator

	inline	interpolator_comparer::enum_result compare	( base_interpolator const& left, base_interpolator const& right );
	inline	bool	operator <			( base_interpolator const& left, base_interpolator const& right );

struct merge_interpolators_predicate {
	inline	bool	operator ( )		( base_interpolator const* const left, base_interpolator const* const right ) const;
}; // struct merge_interpolators_predicate

struct unique_interpolators_predicate {
	inline	bool	operator ( )		( base_interpolator const* const left, base_interpolator const* const right ) const;
}; // struct unique_interpolators_predicate

} // namespace animation
} // namespace xray

#include "base_interpolator_inline.h"

#endif // #ifndef BASE_INTERPOLATOR_H_INCLUDED