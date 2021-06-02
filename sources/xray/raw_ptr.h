////////////////////////////////////////////////////////////////////////////
//	Created 	: 16.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RAW_PTR_H_INCLUDED
#define XRAY_RAW_PTR_H_INCLUDED

#if XRAY_USE_DEBUG_POINTERS

namespace xray {

template < typename type, typename pointer_type >
class non_null_ptr;

template <
	typename object_type,
	typename base_type,
	typename threading_policy
>
class loose_ptr;

template < typename type, typename pointer_type = type* >
class raw_ptr {
public:
	typedef raw_ptr<
				type,
				pointer_type
			>			self_type;
	typedef type* ( self_type::*unspecified_bool_type ) ( ) const;

public:
	inline	IMPLICIT	raw_ptr		( pointer_type value );

	template < typename type2, typename pointer_type2 >
	inline	IMPLICIT	raw_ptr		( raw_ptr< type2, pointer_type2 > const& value );

	template < typename type2, typename pointer_type2 >
	inline	IMPLICIT	raw_ptr		( non_null_ptr< type2, pointer_type2 > const& value );

	template < typename type2, typename base_type, typename threading_policy >
	inline	IMPLICIT	raw_ptr		( loose_ptr< type2, base_type, threading_policy > const& value );

	inline	type*		operator ->	( ) const;
	inline	type&		operator *	( ) const;
	inline	bool		operator!	( ) const;
	inline	operator unspecified_bool_type	( ) const;
	inline	self_type&	operator =	( type* object );
	inline	self_type&	operator =	( self_type const& object );
	inline	bool		operator ==	( self_type const& object ) const;
	inline	bool		operator !=	( self_type const& object ) const;
	inline	bool		operator <	( self_type const& object ) const;
	inline	bool		operator <=	( self_type const& object ) const;
	inline	bool		operator >	( self_type const& object ) const;
	inline	bool		operator >=	( self_type const& object ) const;
	inline	void		swap		( self_type& object );
	inline	bool		equal		( self_type const& object )	const;

protected:
	pointer_type		m_value;
};

template < typename type, typename pointer_type >
inline		void		swap		( xray::raw_ptr< type, pointer_type >& left, xray::raw_ptr< type, pointer_type >& right );

template < typename inherited_type, typename type, typename pointer_type >
inline		bool		operator==	( inherited_type* left, xray::raw_ptr< type, pointer_type > const& right );

template < typename inherited_type, typename type, typename pointer_type >
inline		bool		operator!=	( inherited_type* left, xray::raw_ptr< type, pointer_type > const& right );

template < typename inherited_type, typename type, typename pointer_type >
inline		bool		operator<	( inherited_type* left, xray::raw_ptr< type, pointer_type > const& right );

template < typename inherited_type, typename type, typename pointer_type >
inline		bool		operator<=	( inherited_type* left, xray::raw_ptr< type, pointer_type > const& right );

template < typename inherited_type, typename type, typename pointer_type >
inline		bool		operator>	( inherited_type* left, xray::raw_ptr< type, pointer_type > const& right );

template < typename inherited_type, typename type, typename pointer_type >
inline		bool		operator>=	( inherited_type* left, xray::raw_ptr< type, pointer_type > const& right );

} // namespace xray

#include <xray/raw_ptr_inline.h>

	namespace xray {
		template < typename T, typename T_pointer = T* >
		struct raw {
			typedef raw_ptr< T, T_pointer >			ptr;
		}; // struct raw
	}
#else // #if XRAY_USE_DEBUG_POINTERS
	namespace xray {
		template < typename T, typename T_pointer = T* >
		struct raw {
			typedef T*								ptr;
		}; // struct raw
	} // namespace xray
#endif // #if XRAY_USE_DEBUG_POINTERS

#endif // #ifndef XRAY_RAW_PTR_H_INCLUDED