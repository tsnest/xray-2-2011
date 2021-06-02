////////////////////////////////////////////////////////////////////////////
//	Created 	: 23.01.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_MOUSE_EVENTS_H_INCLUDED
#define PROPERTY_MOUSE_EVENTS_H_INCLUDED

namespace xray {
namespace editor {
namespace controls {

ref class property_grid;

public interface class property_mouse_events {
public:
	virtual void	on_double_click	(property_grid^ property_grid) = 0;
}; // interface class property_mouse_events

} // namespace controls
} // namespace editor
} // namespace xray

#endif // ifndef PROPERTY_MOUSE_EVENTS_H_INCLUDED