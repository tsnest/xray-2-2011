////////////////////////////////////////////////////////////////////////////
//	Created		: 18.10.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_POLYMORPH_VECTOR_H_INCLUDED
#define XRAY_RENDER_ENGINE_POLYMORPH_VECTOR_H_INCLUDED

#error "do not use this header"

#include <iterator>
#include <boost/type_traits/is_pointer.hpp>

namespace xray {
namespace render {

template < typename T >
class polymorph_vector_base : public boost::noncopyable
{
public :
	class iterator
	{
		friend class polymorph_vector_base;
	public:
		inline  iterator &	operator ++ ( )
		{
			m_ptr = (T*) (((char*)m_ptr) + m_pitch);
			return *this;
		}

		inline	bool	operator == ( iterator const & other)
		{
			return other.m_ptr == m_ptr && other.m_pitch == m_pitch;
		}

		inline  bool	operator != ( iterator const & other)
		{
			return other.m_ptr != m_ptr || other.m_pitch != m_pitch;
		}


		T* operator * ( )
		{
			return m_is_ptr ?  (*m_ptr_ptr) : (m_ptr);
		}


	private:

		union{
			T*		m_ptr;
			T**		m_ptr_ptr;
		};

		u16		m_pitch;
		bool	m_is_ptr;
	};

public :
	typedef u32					size_type;

								polymorph_vector_base	( u16 pitch, bool is_ptr):
									m_pitch		(pitch),
									m_is_ptr	(is_ptr)
								{
								}

	inline	iterator			begin			( );
	inline	iterator			begin			( ) const;

	inline	iterator			end				( );
	inline	iterator			end				( ) const;

	inline	T*					operator [ ]	( size_type index );
	inline	T const *			operator [ ]	( size_type index ) const;

	inline size_type			size			( );

protected:

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable:4201)
#endif // #if defined(_MSC_VER)

	union{
		struct{
			T*							m_begin;
			T*							m_end;
#ifdef DEBUG
			T*							m_max_end;
#endif // #ifdef DEBUG
		};
		struct{
			T**							m_begin_ptr;
			T**							m_end_ptr;
#ifdef DEBUG
			T**							m_max_end_ptr;
#endif // #ifdef DEBUG
		};
	};
#if defined(_MSC_VER)
#	pragma warning(pop)
#endif // #if defined(_MSC_VER)

const	u16							m_pitch;
const	bool						m_is_ptr;

};

template < typename T>
class polymorph_vector_traits
{
	enum { ptr_type = false};
};

template < typename T>
class polymorph_vector_traits<T*>
{
	enum { ptr_type = true};
};


template < typename T, typename base >
class polymorph_vector : public polymorph_vector_base<base>
{
public:
	typedef T const*								const_iterator;
	typedef T const*								const_pointer;
	typedef T const&								const_reference;
	typedef typename boost::remove_cv<T>::type const & non_volatile_const_reference;
	typedef std::reverse_iterator< const_iterator >	const_reverse_iterator;

	typedef T*										iterator;
	typedef T*										pointer;
	typedef typename boost::remove_cv<T>::type *	non_volatile_pointer;
	typedef T&										reference;
	typedef std::reverse_iterator< iterator >		reverse_iterator;

	typedef ptrdiff_t								difference_type;
	typedef u32										size_type;
	typedef T										value_type;

private:
	typedef polymorph_vector< T, base >				self_type;

public:
 	inline							polymorph_vector	( );
	inline							polymorph_vector	( u32 capacity);
// 	inline							polymorph_vector	( pvoid buffer, size_type max_count, size_type live_count = 0 );
// 	inline							polymorph_vector	( pvoid buffer, size_type max_count, size_type count, value_type const& value );
// 	inline							polymorph_vector	( pvoid buffer, size_type max_count, self_type const& other );
// 	template < typename input_iterator >
// 	inline							polymorph_vector	( pvoid buffer, size_type max_count, input_iterator const& begin, input_iterator const& end );
	inline							~polymorph_vector	( );
	inline	void					create_buffer		( u32 capacity);

	inline	self_type				&operator=		( self_type const& other );

	template < typename input_iterator >
	inline	void					assign			( input_iterator begin, input_iterator const& end );
	inline	void					assign			( size_type count, non_volatile_const_reference value );

//	inline	void					swap			( self_type& other );
	inline	void					clear			( );
	inline	void					resize			( size_type size );
	inline	void					resize			( size_type size, non_volatile_const_reference value );
	inline	void					reserve			( size_type size );

	template < typename input_iterator >
	inline	void					insert			( iterator const& where, input_iterator begin, input_iterator const& last );
	inline	void					insert			( iterator const& where, size_type count, non_volatile_const_reference value );
	inline	void					insert			( iterator const& where, non_volatile_const_reference value );

	inline	void					erase			( iterator const& begin, iterator const& end );
	inline	void					erase			( iterator const& where );

	inline	void					pop_back		( );
	inline	void					push_back		( non_volatile_const_reference value );

	inline	reference				at				( size_type index );
	inline	const_reference			at				( size_type index ) const;

	inline	reference				operator [ ]	( size_type index );
	inline	const_reference			operator [ ]	( size_type index ) const;

	inline	reference				back			( );
	inline	const_reference			back			( ) const;

	inline	reference				front			( );
	inline	const_reference			front			( ) const;

	inline	iterator				begin			( );
	inline	const_iterator			begin			( ) const;

	inline	iterator				end				( );
	inline	const_iterator			end				( ) const;

	inline	reverse_iterator		rbegin			( );
	inline	const_reverse_iterator	rbegin			( ) const;

	inline	reverse_iterator		rend			( );
	inline	const_reverse_iterator	rend			( ) const;

	inline	bool					empty			( ) const;
	inline	size_type				size			( ) const;

	inline	size_type				capacity		( ) const;
	inline	size_type				max_size		( ) const;

private:
	static inline void				construct		( pointer p );
	static inline void				construct		( pointer p, non_volatile_const_reference value );
	static inline void				construct		( iterator begin, iterator const& end );
	static inline void				construct		( iterator begin, iterator const& end, non_volatile_const_reference value );

private:
	static inline void				destroy			( pointer p );
	static inline void				destroy			( iterator begin, iterator const& end );

private:
	inline							polymorph_vector	( self_type const& other );

	template < typename Ptr_type>
	inline			bool 			detect_ptr_type		(Ptr_type) { return false; }

	template < typename Ptr_type>
	inline			bool 			detect_ptr_type		(Ptr_type*) { return true; asda;}

	template<typename V>	inline		void		set_buffer	( V* begin, V* max_end)		
	{ 
		m_begin		= begin; 
		m_end		= m_begin;
#ifdef DEBUG
		m_max_end	= max_end;
#endif
		UNREFERENCED_PARAMETER(max_end);
	}

	template<typename V>	inline		void		set_buffer	( V** begin, V** max_end)		
	{ 
// 		T const asd = (T)100;
// 		COMPILE_ASSERT( (int)(static_cast<base const *>(asd)) == (int)100, _The_pointer_of_the_derived_type_has_different_offset_);
		//V* ptr = (V*)100; 
		ASSERT( begin == NULL || (int)(static_cast<base const *>(*begin)) == (int)(*begin),"The_pointer_of_the_derived_type_has_different_offset");
		m_begin_ptr		= (base**)begin; 
		m_end_ptr		= m_begin_ptr;	
#ifdef DEBUG
		m_max_end_ptr	= (base**)max_end;
#endif
		UNREFERENCED_PARAMETER(max_end);
	}

	template<typename V>	inline		void		set_end	( V* end)		{ m_end		= end;}
	template<typename V>	inline		void		set_end	( V** end)		{ m_end_ptr	= (base**)end;}


	template< bool is_ptr>	inline		pointer		__begin_impl() const;
	template<>				inline		pointer		__begin_impl<false>() const	{ return (pointer)m_begin; }
	template<>				inline		pointer		__begin_impl<true>() const	{ return (pointer)m_begin_ptr; }

	template< bool is_ptr>	inline		pointer		__end_impl() const;
	template<>				inline		pointer		__end_impl<false>() const	{ return (pointer)m_end; }
	template<>				inline		pointer		__end_impl<true>() const	{ return (pointer)m_end_ptr; }

#ifdef DEBUG
	template<bool is_ptr>	inline		pointer		__max_end_impl() const;
	template<>				inline		pointer		__max_end_impl<false>() const	{ return (pointer)m_max_end; }
	template<>				inline		pointer		__max_end_impl<true>() const	{ return (pointer)m_max_end_ptr; }
#endif // #ifdef DEBUG

	pointer							_begin				() const { return __begin_impl<boost::is_pointer<T>::value>(); }//{ 	}
	pointer							_end				() const { return __end_impl<boost::is_pointer<T>::value>(); }
#ifdef DEBUG
	pointer							_max_end			() const { return __max_end_impl<boost::is_pointer<T>::value>();}
#endif // #ifdef DEBUG

};

} // namespace render
} // namespace xray

// 	template < typename T >
// 	inline	void					swap			( ::xray::polymorph_vector< T >& left, ::xray::polymorph_vector< T >& right );

#include "polymorph_vector_inline.h"

#endif // #ifndef XRAY_RENDER_ENGINE_POLYMORPH_VECTOR_H_INCLUDED