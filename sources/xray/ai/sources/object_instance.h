////////////////////////////////////////////////////////////////////////////
//	Created		: 22.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_INSTANCE_H_INCLUDED
#define OBJECT_INSTANCE_H_INCLUDED

#include <xray/ai/parameter_types.h>

namespace xray {
namespace ai {
namespace planning {

class object_instance
{
public:
			inline							object_instance	(
												parameter_type object_type,
												object_instance_type const& instance,
												pcstr object_caption
											);

			inline	parameter_type			get_type	( ) const	{ return m_type; }
			inline	object_instance_type	get_instance( ) const	{ return m_instance; }
			inline	pcstr					get_caption	( ) const	{ return m_caption.c_str(); }

	template < typename T >
	static	inline	void					set_instance( object_instance_type& instance, T value );

private:	
	parameter_type							m_type;
	object_instance_type					m_instance;
	fixed_string< 256 >						m_caption;
}; // class object_instance

} // namespace planning
} // namespace ai
} // namespace xray

#include "object_instance_inline.h"

#endif // #ifndef OBJECT_INSTANCE_H_INCLUDED