////////////////////////////////////////////////////////////////////////////
//	Created 	: 12.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef EDITOR_FLUSH_COMMAND_H_INCLUDED
#define EDITOR_FLUSH_COMMAND_H_INCLUDED

#include <xray/render/base/engine_wrapper.h>
#include <xray/render/base/editor_renderer.h>
#include <fastdelegate/fastdelegate.h>

namespace xray {
namespace render {
namespace editor {

class editor_renderer;

class flush_command :
	public engine::command,
	private boost::noncopyable
{
public:
	inline				flush_command	( editor_renderer& renderer ): m_renderer			( renderer ){ }

	virtual				~flush_command	( );
	virtual	void		execute			( );

private:
	editor_renderer&	m_renderer;
}; // struct command_delegate

} // namespace editor
} // namespace render
} // namespace xray

#endif // #ifndef EDITOR_FLUSH_COMMAND_H_INCLUDED