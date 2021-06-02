////////////////////////////////////////////////////////////////////////////
//	Created		: 09.11.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_DEBUG_MODE_STRIP_BUTTON_H_INCLUDED
#define SOUND_DEBUG_MODE_STRIP_BUTTON_H_INCLUDED

#include "editor_world.h"

namespace xray {
namespace editor {

public ref class sound_debug_mode_strip_button : public xray::editor::controls::mode_strip_button
{
public:
	sound_debug_mode_strip_button	( editor_world& world ) :
		m_editor_world		( world )
	{}
	
	virtual void	on_click					( ) override
	{
		m_editor_world.set_soud_stats_mode				( current_mode );
	}
private:
	editor_world&	m_editor_world;
}; // class sound_debug_mode_strip_button

} // namespace editor
} // namespace xray

#endif // #ifndef SOUND_DEBUG_MODE_STRIP_BUTTON_H_INCLUDED