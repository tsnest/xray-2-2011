////////////////////////////////////////////////////////////////////////////
//	Module 		: intrusive_ptr.h
//	Created 	: 30.06.2005
//  Modified 	: 28.05.2007
//	Author		: Dmitriy Iassenev
//	Description : intrusive pointer template
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_INTRUSIVE_PTR_H_INCLUDED
#define CS_GTL_INTRUSIVE_PTR_H_INCLUDED

#include <cs/gtl/intrusive_base.h>
#include <cs/gtl/intrusive_default_functionality.h>

#pragma pack(push,4)

namespace gtl {

template <
	typename object_type,
	typename base_type = intrusive_base,
	template <typename> class functionaly_type = intrusive_default_functionality
>
class intrusive_ptr : 
	public
		functionaly_type<
			intrusive_ptr<
				object_type,
				base_type,
				functionaly_type
			>
		>
{
private:
	typedef intrusive_ptr<
		object_type,
		base_type,
		functionaly_type
	>															self_type;
//	typedef base_type											base_type;
//	typedef functionaly_type<self_type>							functionaly_type;
	typedef object_type* (self_type::*unspecified_bool_type) () const;

private:
	object_type*					m_object;

private:
	inline	void				dec								();
	inline	void				set								(object_type*	 object);
	inline	void				set								(self_type const& object);

public:
	inline						intrusive_ptr					();
	inline						intrusive_ptr					(object_type*	 object);
	inline						intrusive_ptr					(self_type const& object);
	inline						~intrusive_ptr					();
	inline	object_type&			operator*						() const;
	inline	object_type*			operator->						() const;
	inline	bool				operator!						() const;
	inline						operator unspecified_bool_type	() const;
	inline	self_type&			operator=						(object_type*	 object);
	inline	self_type&			operator=						(self_type const& object);
	inline	bool				operator==						(self_type const& object) const;
	inline	bool				operator!=						(self_type const& object) const;
	inline	bool				operator<						(self_type const& object) const;
	inline	bool				operator>						(self_type const& object) const;
	inline	void				swap							(self_type&		 object);
	inline	bool				equal							(self_type const& object)	const;
	inline	object_type*			get							() const;
};

template <typename T>
struct is_intrusive_ptr {
	template <typename T1, typename T2, template <typename> class T3>
	static boost::type_traits::yes_type	is_ptr_tester(intrusive_ptr<T1,T2,T3>*);
	static boost::type_traits::no_type	is_ptr_tester(...);
	enum { value = (sizeof(boost::type_traits::yes_type)==sizeof(is_ptr_tester((T*)0))) };
};

}

#pragma pack(pop)

template <typename object_type, typename base_type, typename _functionaly_type>
inline		void				swap							(gtl::intrusive_ptr<object_type,base_type> & lhs, gtl::intrusive_ptr<object_type,base_type> & rhs);

#include <cs/gtl/intrusive_ptr_inline.h>

#endif // #ifndef CS_GTL_INTRUSIVE_PTR_H_INCLUDED