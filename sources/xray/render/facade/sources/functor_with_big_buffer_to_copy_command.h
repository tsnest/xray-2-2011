////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef FUNCTOR_WITH_BIG_BUFFER_TO_COPY_COMMAND_H_INCLUDED
#define FUNCTOR_WITH_BIG_BUFFER_TO_COPY_COMMAND_H_INCLUDED

#include <xray/render/facade/base_command.h>
#include <fastdelegate/fastdelegate.h>

namespace xray {
namespace render {


template <typename T>
class functor_with_big_buffer_to_copy_command :
	public base_command,
	private boost::noncopyable
{
public:
	typedef boost::function< void ( T const& ) >					on_execute_type;
	typedef boost::function< void ( base_command& ) >				on_defer_execution_type;

public:
							functor_with_big_buffer_to_copy_command	( on_execute_type const& on_execute, T const & data, on_defer_execution_type const& on_defer_execution = on_defer_execution_type() );
	virtual	void			execute									( );
	virtual	void			defer_execution							( );

private:
	on_execute_type			m_on_execute;
	on_defer_execution_type	m_on_defer_execution;
	T const					m_data;
}; // struct functor_with_big_buffer_to_copy_command

} // namespace render
} // namespace xray

#include "functor_with_big_buffer_to_copy_command_inline.h"

#endif // #ifndef FUNCTOR_WITH_BIG_BUFFER_TO_COPY_COMMAND_H_INCLUDED