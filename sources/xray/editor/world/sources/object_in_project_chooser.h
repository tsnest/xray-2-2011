////////////////////////////////////////////////////////////////////////////
//	Created		: 21.04.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_IN_PROJECT_CHOOSER_H_INCLUDED
#define OBJECT_IN_PROJECT_CHOOSER_H_INCLUDED

namespace xray {
namespace editor {

ref class project;

ref class object_in_project_chooser: public editor_base::chooser
{
	typedef editor_base::chooser super;
public:
						object_in_project_chooser( project^ p );
	virtual System::String^	id				( );
	virtual bool			choose			( System::String^ initial, System::String^ filter, System::String^% result, bool read_only );

private:
	project^			m_project;
};

} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_IN_PROJECT_CHOOSER_H_INCLUDED