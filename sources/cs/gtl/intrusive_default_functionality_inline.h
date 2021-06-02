////////////////////////////////////////////////////////////////////////////
//	Module 		: intrusive_default_functionality_inline.h
//	Created 	: 30.06.2005
//  Modified 	: 28.05.2007
//	Author		: Dmitriy Iassenev
//	Description : intrusive pointer default functionality template class inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_INTRUSIVE_DEFAULT_FUNCTIONALITY_INLINE_H_INCLUDED
#define CS_GTL_INTRUSIVE_DEFAULT_FUNCTIONALITY_INLINE_H_INCLUDED

#define TEMPLATE_SPECIALIZATION	template <typename ChildType>
#define functionality			gtl::intrusive_default_functionality<ChildType>

TEMPLATE_SPECIALIZATION
inline ChildType& functionality::self		()
{
	return	(ChildType&)*this;
}

TEMPLATE_SPECIALIZATION
inline ChildType const& functionality::self	() const
{
	return	(ChildType const&)*this;
}

#undef TEMPLATE_SPECIALIZATION
#undef functionality

#endif // #ifndef CS_GTL_INTRUSIVE_DEFAULT_FUNCTIONALITY_INLINE_H_INCLUDED