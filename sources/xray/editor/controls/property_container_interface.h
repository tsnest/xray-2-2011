////////////////////////////////////////////////////////////////////////////
//	Created 	: 23.01.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_CONTAINER_INTERFACE_H_INCLUDED
#define PROPERTY_CONTAINER_INTERFACE_H_INCLUDED

namespace xray {
namespace editor {
namespace controls {

interface class property_value;

public interface class property_container_interface {
public:
	typedef Flobbster::Windows::Forms::PropertySpec	PropertySpec;

public:
	virtual	property_value^	value	(PropertySpec^ description) = 0;
}; // interface class property_container_interface

} // namespace controls
} // namespace editor
} // namespace xray

#endif // ifndef PROPERTY_CONTAINER_INTERFACE_H_INCLUDED