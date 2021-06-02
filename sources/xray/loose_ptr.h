////////////////////////////////////////////////////////////////////////////
//	Created 	: 15.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_LOOSE_PTR_H_INCLUDED
#define XRAY_LOOSE_PTR_H_INCLUDED

#if XRAY_USE_DEBUG_POINTERS

#include <xray/threading_policies.h>
#include <xray/loose_ptr_base.h>

namespace xray {

template <
	typename object_type,
	typename base_type = loose_ptr_data,
	typename threading_policy = threading::multi_threading_policy
>
class loose_ptr {
public:
	typedef loose_ptr<
		object_type,
		base_type,
		threading_policy
	>						self_type;
	typedef object_type* ( self_type::*unspecified_bool_type ) ( ) const;

public:
	inline					loose_ptr		( object_type* object );
	inline					loose_ptr		( self_type const& object );
	inline					~loose_ptr		( );
	inline	object_type		&operator*		( ) const;
	inline	object_type		*operator->		( ) const;
	inline	bool			operator!		( ) const;
	inline	operator unspecified_bool_type	( ) const;
	inline	self_type		&operator=		( object_type* object );
	inline	self_type		&operator=		( self_type const& object );
	inline	bool			operator==		( self_type const& object ) const;
	inline	bool			operator!=		( self_type const& object ) const;
	inline	bool			operator<		( self_type const& object ) const;
	inline	bool			operator>		( self_type const& object ) const;
	inline	bool			operator==		( object_type const* const object ) const;
	inline	bool			operator!=		( object_type const* const object ) const;
	inline	bool			operator<		( object_type const* const object ) const;
	inline	bool			operator<=		( object_type const* const object ) const;
	inline	bool			operator>		( object_type const* const object ) const;
	inline	bool			operator>=		( object_type const* const object ) const;
	inline	void			swap			( self_type& object );
	inline	bool			equal			( self_type const& object )	const;
	inline	object_type*	c_ptr			( ) const;

private:
	inline	void			dec				( );
	inline	void			set				( object_type* object );
	inline	void			set				( self_type const& object );

private:
	loose_ptr_data*			m_object;
}; // class loose_ptr

template < typename object_type, typename base_type, typename threading_policy >
inline		void			swap			( xray::loose_ptr< object_type, base_type, threading_policy >& left, xray::loose_ptr< object_type, base_type, threading_policy >& right );

} // namespace xray

#include <xray/loose_ptr_inline.h>

	namespace xray {
		template < typename T >
		struct loose {
			typedef loose_ptr_base					base;
			typedef loose_ptr< T >					ptr;
		}; // struct loose
	} // namespace xray

	namespace xray {
		template < typename T >
		inline T* c_ptr	( loose_ptr< T > pointer )
		{
			return	pointer.c_ptr();
		}
	} // namespace xray
#else // #if XRAY_USE_DEBUG_POINTERS
	namespace xray {
		template < typename T >
		struct loose {
			struct dummy {};
			typedef dummy							base;
			typedef T*								ptr;
		}; // struct loose
	} // namespace xray

	namespace xray {
		template < typename T >
		inline T* c_ptr	( T* const pointer )
		{
			return	pointer;
		}
	} // namespace xray
#endif // #if XRAY_USE_DEBUG_POINTERS

#endif // #ifndef XRAY_LOOSE_PTR_H_INCLUDED