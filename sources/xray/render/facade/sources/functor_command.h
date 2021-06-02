////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef FUNCTOR_COMMAND_H_INCLUDED
#define FUNCTOR_COMMAND_H_INCLUDED

#include <xray/render/facade/base_command.h>

namespace xray {
namespace render {

class functor_command :
	public base_command,
	private boost::noncopyable
{
public:
	typedef boost::function< void ( ) >					on_execute_type;
	typedef boost::function< void ( base_command& ) >	on_defer_execution_type;

public:
							functor_command	( on_execute_type const& on_execute, on_defer_execution_type const& on_defer_execution = on_defer_execution_type() );
	virtual	void			execute			( );
	virtual	void			defer_execution	( );

private:
	on_execute_type			m_on_execute;
	on_defer_execution_type	m_on_defer_execution;
}; // struct functor_command

} // namespace render
} // namespace xray

#endif // #ifndef FUNCTOR_COMMAND_H_INCLUDED