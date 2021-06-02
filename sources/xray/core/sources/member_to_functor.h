////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MEMBER_TO_FUNCTOR_H_INCLUDED
#define MEMBER_TO_FUNCTOR_H_INCLUDED

namespace xray {

template <class C, class M, class Functor>
class member_to_functor;

template <class C, class M, class Functor>
member_to_functor<C, M, Functor>
pass_member_to_functor	(M C::*member_, Functor const& func);

} // namespace xray

#include "member_to_functor_inline.h"

#endif // MEMBER_TO_FUNCTOR_H_INCLUDED