////////////////////////////////////////////////////////////////////////////
//	Module 		: object_constructor_base.h
//	Created 	: 27.05.2004
//  Modified 	: 21.04.2005
//	Author		: Dmitriy Iassenev
//	Description : object constructor base class
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_OBJECT_CONSTRUCTOR_BASE_H_INCLUDED
#define CS_GTL_OBJECT_CONSTRUCTOR_BASE_H_INCLUDED

namespace gtl {

template <typename _base_class_type, typename _clsid_type>
class object_constructor_base {
protected:
	typedef _base_class_type	_base_class_type;
	typedef _clsid_type			_clsid_type;

protected:
	_clsid_type					m_clsid;

public:
	inline						object_constructor_base	(_clsid_type const& clsid);
	inline	_clsid_type const&	clsid					() const;
	virtual _base_class_type*	object_instance			() const = 0;
};

};

#include <cs/gtl/object_constructor_base_inline.h>

#endif // #ifndef CS_GTL_OBJECT_CONSTRUCTOR_BASE_H_INCLUDED