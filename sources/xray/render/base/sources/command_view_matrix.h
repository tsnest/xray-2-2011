////////////////////////////////////////////////////////////////////////////
//	Created 	: 11.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_VIEW_MATRIX_H_INCLUDED
#define COMMAND_VIEW_MATRIX_H_INCLUDED

#include <xray/render/base/engine_wrapper.h>

namespace xray {
namespace render {

struct platform;

class command_view_matrix :
	public engine::command,
	private boost::noncopyable
{
public:
						command_view_matrix	( platform& platform, float4x4 const& view );
	virtual	void		execute				( );

private:
	float4x4			m_view;
	platform&		m_platform;
}; // struct command_delegate

} // namespace render
} // namespace xray

#endif // #ifndef COMMAND_VIEW_MATRIX_H_INCLUDED