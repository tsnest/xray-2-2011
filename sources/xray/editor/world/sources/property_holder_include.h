////////////////////////////////////////////////////////////////////////////
//	Created 	: 04.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_HOLDER_INCLUDE_HPP_INCLUDED
#define PROPERTY_HOLDER_INCLUDE_HPP_INCLUDED

#pragma unmanaged
#include <boost/noncopyable.hpp>
#include <fastdelegate/fastdelegate.h>
#include <utility>
#pragma managed
#include <xray/editor/world/property_holder.h>

generic <typename type0, typename type1>
private ref struct Pair {
	type0	first;
	type1	second;
};

template <typename T>
class value_holder : private boost::noncopyable {
public:
	inline							value_holder	( T& value ) :
		m_value	(value)
	{
	}

	inline T const&	DELEGATE_CALL	get				( )
	{
		return	(m_value);
	}

	inline void	DELEGATE_CALL		set				( T const &value )
	{
		m_value	= value;
	}

private:
	T&	m_value;
};

#endif // ifndef PROPERTY_HOLDER_INCLUDE_HPP_INCLUDED