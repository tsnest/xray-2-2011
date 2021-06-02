////////////////////////////////////////////////////////////////////////////
//	Module 		: callback_impl.h
//	Created 	: 30.06.2005
//  Modified 	: 23.04.2008
//	Author		: Dmitriy Iassenev
//	Description : callback implementation class
////////////////////////////////////////////////////////////////////////////

#ifndef CS_SCRIPT_CALLBACK_IMPL_H_INCLUDED
#define CS_SCRIPT_CALLBACK_IMPL_H_INCLUDED

#pragma warning(push)
#pragma warning(disable:4995)
#include <luabind/luabind.hpp>
#pragma warning(pop)

namespace cs {
namespace script {

template <typename return_type>
class callback_impl {
public:
	typedef return_type									_return_type;

private:
	typedef callback_impl<_return_type>					self_type;
	typedef luabind::object								functor_type;
	typedef luabind::object								object_type;
	typedef bool (callback_impl::*unspecified_bool_type)() const;

protected:
	functor_type		m_functor;
	object_type			m_object;

private:
	inline	bool		empty							() const;

private:
	template <typename T>
	inline	bool		valid							(T const& object) const;

public:
	inline				callback_impl					();
	inline				callback_impl					(self_type const& callback);
	inline	self_type&	operator=						(self_type const& callback);
	inline	bool		operator==						(self_type const& callback) const;
	inline	bool		operator<						(self_type const& callback) const;
	inline	bool		same_object						(object_type const& object) const;
	inline	void		bind							(functor_type const& functor);
	inline	void		bind							(functor_type const& functor, object_type const& object);
	inline	void		clear							();
	inline				operator unspecified_bool_type	() const;
};

} // namespace script
} // namespace cs

#include <cs/script/callback_impl_inline.h>

#endif // #ifndef CS_SCRIPT_CALLBACK_IMPL_H_INCLUDED