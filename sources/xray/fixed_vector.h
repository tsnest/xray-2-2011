////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.08.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_FIXED_VECTOR_H_INCLUDED
#define XRAY_FIXED_VECTOR_H_INCLUDED

namespace xray {

template < typename T, int max_count >
class fixed_vector : public buffer_vector< T > {
public:
	typedef fixed_vector<T, max_count>	self_type;
	typedef buffer_vector<T>			super;
	typedef typename super::size_type	size_type;
	typedef typename super::value_type	value_type;

public:
	inline				fixed_vector	();
	inline				fixed_vector	(size_type count, value_type const &value);
	inline	IMPLICIT	fixed_vector	(self_type const &other);
	template <typename input_iterator>
	inline				fixed_vector	(input_iterator const& first, input_iterator const& last);

	inline	self_type &	operator=		(self_type const &other);

	inline	size_type	capacity		( ) const;
	inline	size_type	max_size		( ) const;

public:
	struct allign_helper {
		char	m_store[sizeof(T)];
	}; // struct allign_helper

private:
	allign_helper		m_buffer[max_count];
};

} // namespace xray

	template < typename T, int max_count >
	inline	void		swap			( xray::fixed_vector< T, max_count >& left, xray::fixed_vector< T, max_count >& right);

#include <xray/fixed_vector_inline.h>

#endif // #ifndef XRAY_FIXED_VECTOR_H_INCLUDED
