////////////////////////////////////////////////////////////////////////////
//	Module 		: object_factory_inline.h
//	Created 	: 27.05.2004
//  Modified 	: 21.04.2005
//	Author		: Dmitriy Iassenev
//	Description : object factory class inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_OBJECT_FACTORY_INLINE_H_INCLUDED
#define CS_GTL_OBJECT_FACTORY_INLINE_H_INCLUDED

#define TEMPLATE_SPECIALIZATION template <\
	typename _base_class_type,\
	typename _clsid_type\
>
#define _object_factory gtl::object_factory<_base_class_type,_clsid_type>

TEMPLATE_SPECIALIZATION
inline	bool _object_factory::constructor_predicate::operator()	(constructor const* constructor0, constructor const* constructor1) const
{
	return					constructor0->clsid() < constructor1->clsid();
}

TEMPLATE_SPECIALIZATION
inline	bool _object_factory::constructor_predicate::operator()	(constructor const* _constructor, _clsid_type const& clsid) const
{
	return					_constructor->clsid() < clsid;
}

TEMPLATE_SPECIALIZATION
inline	_object_factory::clsid_predicate::clsid_predicate		(_clsid_type const& clsid)
{
	m_clsid					= clsid;
}

TEMPLATE_SPECIALIZATION
inline	bool _object_factory::clsid_predicate::operator()		(constructor const* _constructor) const
{
	return					m_clsid == _constructor->clsid();
}

TEMPLATE_SPECIALIZATION
template <typename _class_type>
inline	_object_factory::object_constructor_default<_class_type>::object_constructor_default	(_clsid_type const& clsid) :
	inherited				(clsid)
{
}

TEMPLATE_SPECIALIZATION
template <typename _class_type>
inline	typename _object_factory::_base_class_type* _object_factory::object_constructor_default<_class_type>::object_instance() const
{
	return					cs_new<_class_type>();
}

TEMPLATE_SPECIALIZATION
inline	void _object_factory::add			(constructor* _constructor)
{
#ifndef NDEBUG
	const_iterator			I = 
#endif // #ifndef NDEBUG
		std::find_if(constructors().begin(),constructors().end(),clsid_predicate(_constructor->clsid()));
	ASSERT					(I == constructors().end(), "object constructor is added to the object factory twice");
	m_constructors.push_back(_constructor);
	m_actual				= false;
}

TEMPLATE_SPECIALIZATION
template <typename _class_type>
inline	void _object_factory::add			(_clsid_type const& clsid)
{
	add						(cs_new<object_constructor_default<_class_type> >(clsid));
}

TEMPLATE_SPECIALIZATION
inline	typename _object_factory::CONSTRUCTORS const& _object_factory::constructors	() const
{
	return					m_constructors;
}

TEMPLATE_SPECIALIZATION
inline	const typename _object_factory::constructor*	_object_factory::get_constructor	(_clsid_type const& clsid, bool no_assert) const
{
	const_iterator			I = std::lower_bound(constructors().begin(),constructors().end(),clsid,constructor_predicate());
	if ((I == constructors().end()) || ((*I)->clsid() != clsid)) {
		R_ASSERT			(no_assert,"Class is not registered in the object factory!");
		return				0;
	}
	return					*I;
}

TEMPLATE_SPECIALIZATION
inline	_object_factory::object_factory		()
{
	m_actual				= true;
}

TEMPLATE_SPECIALIZATION
inline	_object_factory::~object_factory	()
{
	gtl::destroy			(m_constructors);
}

TEMPLATE_SPECIALIZATION
inline void _object_factory::actualize		() const
{
	if (actual())
		return;
	
	std::sort				(m_constructors.begin(),m_constructors.end(),constructor_predicate());
	m_actual				= true;
}

TEMPLATE_SPECIALIZATION
inline void _object_factory::construct		()
{
}

TEMPLATE_SPECIALIZATION
inline typename _object_factory::_base_class_type* _object_factory::object_instance(_clsid_type const& clsid, bool no_assert) const
{
	actualize				();
	constructor const*		_constructor = get_constructor(clsid,no_assert);
	return					_constructor ? _constructor->object_instance() : 0;
}

TEMPLATE_SPECIALIZATION
inline bool _object_factory::actual				() const
{
	return					m_actual;
}

#undef TEMPLATE_SPECIALIZATION
#undef _object_factory

#endif // #ifndef CS_GTL_OBJECT_FACTORY_INLINE_H_INCLUDED