////////////////////////////////////////////////////////////////////////////
//	Created		: 26.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_TYPE_VARIANT_INLINE_H_INCLUDED
#define XRAY_TYPE_VARIANT_INLINE_H_INCLUDED

namespace xray {
namespace detail {

template <class T>
class global_type_id_holder
{
	static threading::atomic32_type		s_next_type_id;
	template <class>
	friend class type_to_int;
};

template <class T>
threading::atomic32_type	global_type_id_holder<T>::s_next_type_id	=	1;

template <class T>
class type_to_int
{
public:
	static u32		get ()
	{
		if ( !s_id )
		{
			while ( threading::interlocked_exchange(s_lock, 1) != 0 ) { ; }
			if ( !s_id )
				s_id				=	threading::interlocked_increment(global_type_id_holder<int>::s_next_type_id) - 1;
			threading::interlocked_exchange	(s_lock, 0);
		}
		return							s_id;
	}

private:
	static threading::atomic32_type		s_id;
	static threading::atomic32_type		s_lock;
};

template <class T>
threading::atomic32_type				type_to_int<T>::s_id		=	0;
template <class T>
threading::atomic32_type				type_to_int<T>::s_lock		=	0;

class abstract_type_helper
{
public:
	virtual	~abstract_type_helper				() {}
	virtual void destroy						(pvoid buffer) = 0;
	virtual void copy							(mutable_buffer dest_buffer, const_buffer src_buffer) = 0;
	virtual abstract_type_helper * copy_helper	(mutable_buffer dest_buffer) = 0;
};

template <class T>
class concrete_type_helper : public abstract_type_helper
{
public:	
	virtual void destroy				(pvoid buffer)
	{
		XRAY_UNREFERENCED_PARAMETER		(buffer);
		((T*)buffer)->~T();
	}

#ifndef __SNC__
	template < bool is_pod_type >
	struct helper {
		static inline void placement_new		( pvoid const buffer )
		{
			new ( buffer )	T ( );
		}
	}; // struct helper

	template < >
	struct helper< true > {
		static inline void placement_new		( pvoid const buffer )
		{
			XRAY_UNREFERENCED_PARAMETER		(buffer);
		}
	};
#endif // #ifndef __SNC__

	virtual void copy					(mutable_buffer dest_buffer, const_buffer src_buffer)
	{
		R_ASSERT						(dest_buffer.size() >= sizeof(T) && src_buffer.size() >= sizeof(T));
#ifdef __SNC__
		new ( dest_buffer.c_ptr() )	T	( );
#else // #ifdef __SNC__
		helper< boost::is_pod<T>::value >::placement_new( dest_buffer.c_ptr() );
#endif // #ifdef __SNC__
		* (T *)dest_buffer.c_ptr()	=	* (T *)src_buffer.c_ptr();
	}

	virtual abstract_type_helper * copy_helper	(mutable_buffer dest_buffer)
	{
		R_ASSERT						(dest_buffer.size() >= sizeof(* this));
		return							new	 (dest_buffer.c_ptr())	concrete_type_helper<T>(* this);
	}
};

} // namespace detail

template <int MaxSize>
variant<MaxSize>::variant() : m_type_id(0), m_helper(NULL) 
{
}

template <int MaxSize>
variant<MaxSize>::variant(variant const & other) : m_type_id(other.m_type_id), m_helper(NULL)
{
	*this		=	other;
}

template <int MaxSize>
void   variant<MaxSize>::operator = (variant const & other)
{
	if ( this == & other )
		return;

	destroy_previous_variable_if_needed	();
	m_type_id		=	other.m_type_id;
	if ( other.m_helper )
	{
		other.m_helper->copy		(mutable_buffer(m_storage), const_buffer(other.m_storage));
		m_helper				=	other.m_helper->copy_helper	(mutable_buffer(m_helper_storage));
	}
}

template <int MaxSize>
template <class T>
void   variant<MaxSize>::set (T const & value)
{
	COMPILE_ASSERT		(sizeof(m_storage) >= sizeof(T), this_variant_MaxSize_is_insufficient_to_store_such_type);
	destroy_previous_variable_if_needed	();

	m_type_id		=	::xray::detail::type_to_int<T>::get();

	new (m_storage)		T(value);
	COMPILE_ASSERT		(sizeof(m_helper_storage) >= sizeof(::xray::detail::concrete_type_helper<T>), increase_sizeof_m_destroyer_storage);
	m_helper		=	new (m_helper_storage) ::xray::detail::concrete_type_helper<T>();
}

template <int MaxSize>
template <class T>
bool   variant<MaxSize>::try_get (T & out_value)
{
#ifdef XRAY_STATIC_LIBRARIES
	CURE_ASSERT (m_type_id == detail::type_to_int<T>::get(), return false);
#endif // XRAY_STATIC_LIBRARIES

	out_value			=	*(T*)m_storage;
	return					true;
}

template <int MaxSize>
variant<MaxSize>::~variant ()
{
	destroy_previous_variable_if_needed	();
}

template <int MaxSize>
void   variant<MaxSize>::destroy_previous_variable_if_needed	()
{
	if ( !m_helper )
		return;
	m_helper->destroy		(m_storage);
	m_helper			=	NULL;
}

} // namespace xray

#endif // #ifndef XRAY_TYPE_VARIANT_INLINE_H_INCLUDED