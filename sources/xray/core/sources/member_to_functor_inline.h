////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MEMBER_TO_FUNCTOR_INLINE_H_INCLUDED
#define MEMBER_TO_FUNCTOR_INLINE_H_INCLUDED

namespace xray {

//-----------------------------------------------------------
// implementation
//-----------------------------------------------------------

template <class C, class M, class Functor>
class member_to_functor
{
public:
	member_to_functor(M C::*member, Functor functor) : 
				member_(member), functor_(functor) {}

	bool	operator () (C const& c) const
	{
		return	functor_(c.*member_);
	}

	bool	operator () (C const* c) const
	{
		return	functor_(c->*member_);
	}

private:
	M			C::*member_;
	Functor		functor_;
};

template <class C, class M, class Functor>
member_to_functor<C, M, Functor>  inline
pass_member_to_functor	(M C::*member_, Functor const& func)
{
	return	member_to_functor<C, M, Functor>(member_, func);
}

} // namespace xray

#endif // MEMBER_TO_FUNCTOR_INLINE_H_INCLUDED