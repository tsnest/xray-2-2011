////////////////////////////////////////////////////////////////////////////
//	Created 	: 26.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_INCREMENTABLE_H_INCLUDED
#define PROPERTY_INCREMENTABLE_H_INCLUDED

namespace xray {
namespace editor {
namespace controls {

public interface class property_incrementable {
public:
	virtual void	increment	(float const% increment) = 0;
}; // interface class property_value

} // namespace controls
} // namespace editor
} // namespace xray

#endif // ifndef PROPERTY_INCREMENTABLE_H_INCLUDED