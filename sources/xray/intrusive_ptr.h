////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_INTRUSIVE_PTR_H_INCLUDED
#define XRAY_INTRUSIVE_PTR_H_INCLUDED

#include <xray/intrusive_base.h>
#include <xray/threading_policies.h>
#include <algorithm>							// for std::swap

namespace xray {

template <
	typename ObjectType,
	typename BaseType,
	typename ThreadingPolicyType
>
class intrusive_ptr {
public:
	typedef ObjectType		object_type;
	typedef intrusive_ptr<
		ObjectType,
		BaseType,
		ThreadingPolicyType
	>						self_type;
	typedef typename threading::policy_resolver< ThreadingPolicyType >::policy_type	policy_type;
	typedef object_type* ( self_type::*unspecified_bool_type )	( ) const;

public:
	inline					intrusive_ptr	( );
	inline					intrusive_ptr	( object_type* object );
	inline					intrusive_ptr	( self_type const& object );
	inline					~intrusive_ptr	( );
	inline	object_type		&operator*		( ) const;
	inline	object_type		*operator->		( ) const;
	inline	bool			operator!		( ) const;
	inline	operator unspecified_bool_type	( ) const;
	inline	self_type		&operator=		( object_type* object );
	inline	self_type		&operator=		( self_type const& object );
	inline	bool			operator==		( self_type const& object ) const;
	inline	bool			operator!=		( self_type const& object ) const;
	inline	bool			operator<		( self_type const& object ) const;
	inline	bool			operator<=		( self_type const& object ) const;
	inline	bool			operator>		( self_type const& object ) const;
	inline	bool			operator>=		( self_type const& object ) const;
	inline	bool			operator==		( object_type const* const object ) const;
	inline	bool			operator!=		( object_type const* const object ) const;
	inline	bool			operator<		( object_type const* const object ) const;
	inline	bool			operator<=		( object_type const* const object ) const;
	inline	bool			operator>		( object_type const* const object ) const;
	inline	bool			operator>=		( object_type const* const object ) const;
	inline	void			swap			( self_type& object );
	inline	bool			equal			( self_type const& object )	const;
	inline	bool			equal			( object_type* const object ) const;
	inline	object_type*	c_ptr			( ) const;

private:
	inline	void			dec				( );
	inline	void			set				( object_type* object );
	inline	void			set				( self_type const& object );

private:
	object_type*			m_object;
}; // class intrusive_ptr

template <
	typename DestinationType,
	typename ObjectType,
	typename BaseType,
	typename ThreadingPolicyType
>
inline  DestinationType	static_cast_resource_ptr	( intrusive_ptr<ObjectType, BaseType, ThreadingPolicyType> const& source );

} // namespace xray

template <
	typename ObjectType,
	typename BaseType,
	typename ThreadingPolicyType
>
inline	void				swap			( xray::intrusive_ptr<ObjectType, BaseType, ThreadingPolicyType>& left, xray::intrusive_ptr<ObjectType, BaseType, ThreadingPolicyType>& right );

#include <xray/intrusive_ptr_inline.h>

#endif // #ifndef XRAY_INTRUSIVE_PTR_H_INCLUDED