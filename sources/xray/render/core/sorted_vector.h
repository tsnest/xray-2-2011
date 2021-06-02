////////////////////////////////////////////////////////////////////////////
//	Created		: 19.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SORTED_VECTOR_H_INCLUDED
#define SORTED_VECTOR_H_INCLUDED

namespace xray {
namespace render_dx10 {

template <typename T, typename M>
class sorted_vector : protected render::vector<T*>
{
public:
	using render::vector::const_iterator;
	using render::vector::iterator;
	using render::vector::begin;
	using render::vector::end;
	using render::vector::operator [];
	using render::vector::size;

	sorted_vector() : render::vector () {}
	sorted_vector( sorted_vector const & other ) : render::vector( other) {}
	~sorted_vector();

	T *		try_add		( T const& element);
	T &		add			( M const& element);
	T *		find		( M const& value);
	//bool		find	( T const& element) {}

private:

}; // class sorted_vector

} // namespace render
} // namespace xray

#include "sorted_vector_inline.h"

#endif // #ifndef SORTED_VECTOR_H_INCLUDED