////////////////////////////////////////////////////////////////////////////
//	Created 	: 06.10.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_DEBUG_CHECK_INVARIANTS
#define XRAY_DEBUG_CHECK_INVARIANTS

// instructions:
// add check_invariants () const to your class
// put XRAY_CHECK_INVARIANTS(your_class) in class functions

#ifdef MASTER_GOLD
#	define XRAY_CHECK_INVARIANTS 
#else // #ifdef MASTER_GOLD
#	define XRAY_CHECK_INVARIANTS(Type)	xray::debug::detail::check_invariants_raii<Type> \
																check_invariants_helper$	(this)
#endif // #ifdef MASTER_GOLD

namespace xray {
namespace debug {
namespace detail {

template <class T>
class check_invariants_raii
{
public:
	template <class T1>
	check_invariants_raii (T1 const * const object)
	{
		m_object					=	object;
		m_object->check_invariants		();
	}

	~check_invariants_raii ()
	{
		m_object->check_invariants		();
	}

private:
	T const *							m_object;
};

} // namespace detail
} // namespace debug
} // namespace xray

#endif // #ifndef XRAY_DEBUG_CHECK_INVARIANTS