////////////////////////////////////////////////////////////////////////////
//	Created		: 02.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef CONSTANT_BINDING_H_INCLUDED
#define CONSTANT_BINDING_H_INCLUDED

#include "utils.h"
#include "constant.h"

namespace xray {
namespace render_dx10 {

//////////////////////////////////////////////////////////////////////////
// constant_bindings

struct constant_binding //: public boost::noncopyable
{
	//constant_binding( shared_string name, constant_source source) : m_name (name), m_source (source) {}
	template <typename T>
	constant_binding( shared_string name, T const * source) : 
		m_name		(name), 
		m_source	( (void*)source, constant_type_traits<T>::size) ,
		m_type		( (enum_constant_type)constant_type_traits<T>::type),
		m_cls		( (enum_constant_class)constant_type_traits<T>::cls)
		{
			COMPILE_ASSERT(	constant_type_traits<T>::value == true, Only_types_described_above_are_supported_for_constant_binding );
			ASSERT(source, "The source ptr cannot be NULL!");
			ASSERT(name.size() > 0, "The name cannot be NULL!");
		}

// 		template <>
// 		constant_binding( shared_string name, float4x4b const * source) : 
// 		m_name		(name), 
// 			m_source	( (void*)&source->transpose(), sizeof(float4x4)) ,
// 			m_type		( (enum_constant_type)constant_type_traits<float4x4b>::type),
// 			m_cls		( (enum_constant_class)constant_type_traits<float4x4b>::cls)
// 		{
// 			ASSERT(m_source.ptr, "The source ptr cannot be NULL!");
// 			ASSERT(name.size() > 0, "The name cannot be NULL!");
// 		}

		shared_string const &	name()		const	{ return m_name;}
		constant_source			source()	const	{ return m_source;}
		enum_constant_class		cls()		const	{ return m_cls;}
		enum_constant_type		type()		const	{ return m_type;}

private:
	shared_string		m_name;
	constant_source		m_source;	
	enum_constant_type	m_type;
	enum_constant_class	m_cls;
};

//////////////////////////////////////////////////////////////////////////
// constant_bindings
class constant_bindings
{
public:
	inline void	add		( shared_string name, constant_source cb);
	inline void	add		( constant_binding binding);

	inline void	clear	() 		{ m_bindings.clear(); }

	render::vector<constant_binding> const& bindings() const { return m_bindings; }

private:
	class binding_compare_predicate;

private:
	render::vector<constant_binding>	m_bindings;
};


class constant_bindings::binding_compare_predicate
{
public:
	binding_compare_predicate( shared_string name): m_name( name) {}

	bool operator()( const constant_binding& item)
	{return item.name() == m_name;}

private:
	shared_string	m_name;
};


// void constant_bindings::add( shared_string name, constant_source source)
// {
// 	vector<constant_binding>::iterator	it  = std::find_if( m_bindings.begin(), m_bindings.end(), constant_bindings::binding_compare_predicate( name));
// 	vector<constant_binding>::iterator	end = m_bindings.end();
// 
// 	if ( it != end)
// 	{
// 		ASSERT( 0, "the constant with the specified name already was added to bindings list.");
// // 		( *it).source = source;
// // 		return;
// 	}
// 
// 	m_bindings.push_back( constant_binding( name, source));
// }

void constant_bindings::add( constant_binding binding)
{
	render::vector<constant_binding>::iterator	it  = std::find_if( m_bindings.begin(), m_bindings.end(), constant_bindings::binding_compare_predicate( binding.name()));
	render::vector<constant_binding>::iterator	end = m_bindings.end();

	if ( it != end)
	{
		ASSERT( 0, "the constant with the specified name already was added to bindings list.");
	}

	m_bindings.push_back( binding);
}


} // namespace render
} // namespace xray

#endif // #ifndef CONSTANT_BINDING_H_INCLUDED