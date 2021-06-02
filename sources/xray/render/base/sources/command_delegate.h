////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_DELEGATE_H_INCLUDED
#define COMMAND_DELEGATE_H_INCLUDED

#include <xray/render/base/engine_wrapper.h>
#include <fastdelegate/fastdelegate.h>

namespace xray {
namespace render {

class command_delegate :
	public engine::command,
	private boost::noncopyable
{
public:
	typedef boost::function< void ( ) >	Delegate;

public:
					command_delegate	( Delegate const& delegate );
	virtual	void	execute				( );

private:
	Delegate		m_delegate;
}; // struct command_delegate

} // namespace render
} // namespace xray

#endif // #ifndef COMMAND_DELEGATE_H_INCLUDED