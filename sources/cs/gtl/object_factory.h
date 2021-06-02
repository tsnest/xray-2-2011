////////////////////////////////////////////////////////////////////////////
//	Module 		: object_factory.h
//	Created 	: 27.05.2004
//  Modified 	: 21.04.2005
//	Author		: Dmitriy Iassenev
//	Description : object factory class
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_OBJECT_FACTORY_H_INCLUDED
#define CS_GTL_OBJECT_FACTORY_H_INCLUDED

#include <boost/noncopyable.hpp>
#include <cs/gtl/object_constructor_base.h>
#include <cs/gtl/destruction/destroy.h>
#include <cs/gtl/destruction/vector.h>

namespace gtl {

template <typename _base_class_type, typename _clsid_type>
class object_factory : public ::boost::noncopyable {
public:
	typedef _base_class_type										_base_class_type;
	typedef _clsid_type												_clsid_type;
	typedef object_constructor_base<_base_class_type,_clsid_type>	constructor;

protected:
	struct constructor_predicate {
		inline	bool				operator()					(constructor const* constructor0, constructor const* constructor1) const;
		inline	bool				operator()					(constructor const* _constructor, _clsid_type const& clsid) const;
	};

	struct clsid_predicate {
		_clsid_type					m_clsid;

		inline						clsid_predicate				(_clsid_type const& clsid);
		inline	bool				operator()					(constructor const* _constructor) const;
	};

	template <typename _class_type>
	class object_constructor_default : public constructor {
	protected:
		typedef constructor			inherited;

	public:
		inline						object_constructor_default	(_clsid_type const& clsid);
		virtual _base_class_type*	object_instance			() const;
	};


public:
	typedef cs_vector<constructor*>					CONSTRUCTORS;
	typedef typename CONSTRUCTORS::iterator			iterator;
	typedef typename CONSTRUCTORS::const_iterator	const_iterator;

protected:
	mutable CONSTRUCTORS		m_constructors;
	mutable bool				m_actual;

private:
	inline	bool				actual				() const;
	inline	CONSTRUCTORS const&	constructors		() const;

protected:
	inline	void				actualize			() const;
	inline	constructor const*	get_constructor	(_clsid_type const& clsid, bool no_assert = false) const;
	inline	void				add					(constructor* _constructor);
	template <typename _class_type>
	inline	void				add					(_clsid_type const& clsid);

public:
	inline						object_factory		();
	virtual						~object_factory		();
	virtual void				construct			();
	inline	_base_class_type*	object_instance	(_clsid_type const& clsid, bool no_assert = false) const;
};

};

#include <cs/gtl/object_factory_inline.h>

#endif // #ifndef CS_GTL_OBJECT_FACTORY_H_INCLUDED