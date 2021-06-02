////////////////////////////////////////////////////////////////////////////
//	Created		: 30.06.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_PROPERTIES_H_INCLUDED
#define OBJECT_PROPERTIES_H_INCLUDED

namespace xray {
namespace editor_base {

public ref class object_properties 
{
public:
	static 	void initialize_property_container( System::Object^ o, xray::editor::wpf_controls::property_container^ container );

private:

}; // class object_properties

} // namespace editor_base
} // namespace xray

#endif // #ifndef OBJECT_PROPERTIES_H_INCLUDED