////////////////////////////////////////////////////////////////////////////
//	Created		: 07.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_TYPE_PREDICATES_INLINE_H_INCLUDED
#define XRAY_TYPE_PREDICATES_INLINE_H_INCLUDED

namespace xray {

template <class C, class M>
class member_equal_to_impl
{
public:
	member_equal_to_impl				(M C::*member, M value) : member(member), value(value) {}
	bool	operator () 				(C const & c) const	{	return	c.*member == value;		}
	bool	operator () 				(C const * c) const	{	return	c->*member == value;	}
private:
	M									C::*member;
	M									value;
};

template <class C, class M>
member_equal_to_impl<C, M>  inline		member_equal_to	(M C::*member_, M value)
{
	return								member_equal_to_impl<C, M>(member_, value);
}

namespace compare {

struct equal
{
	template <class T>
	bool   operator ()					(T const & left, T const & right) const { return left == right; }
};

struct not_equal
{
	template <class T>
	bool   operator ()					(T const & left, T const & right) const { return left != right; }
};

struct less
{
	template <class T>
	bool   operator ()					(T const & left, T const & right) const { return left < right; }
};

struct greater
{
	template <class T>
	bool   operator ()					(T const & left, T const & right) const { return left > right; }
};

struct greater_equal
{
	template <class T>
	bool   operator ()					(T const & left, T const & right) const { return left >= right; }
};

struct less_equal
{
	template <class T>
	bool   operator () 					(T const & left, T const & right) const { return left <= right; }
};

} // namespace compare


template <class Compare, class C, class M>
class compare_member_to_impl
{
public:
	compare_member_to_impl				(M C::*member, Compare compare, M value) 
										: member(member), compare(compare), value(value) {}

	bool	operator () 				(C const & c) const	{	return	compare(c.*member, value);	}
	bool	operator () 				(C const * c) const	{	return	compare(c->*member, value);	}
private:
	M									C::*member;
	M									value;
	Compare								compare;
};

template <class Compare, class C, class M>
compare_member_to_impl<Compare, C, M>  inline	compare_member_to (M C::*member_, Compare compare, M value)
{
	return								compare_member_to_impl<Compare, C, M>(member_, compare, value);
}

} // namespace xray

#endif // #ifndef XRAY_TYPE_PREDICATES_INLINE_H_INCLUDED