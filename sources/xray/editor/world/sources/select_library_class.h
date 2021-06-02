////////////////////////////////////////////////////////////////////////////
//	Created		: 18.02.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SELECT_LIBRARY_CLASS_H_INCLUDED
#define SELECT_LIBRARY_CLASS_H_INCLUDED

namespace xray {
namespace editor {

ref class tool_base;

ref class library_class_chooser: public editor_base::chooser
{
public:
						library_class_chooser( tool_base^ tool );
	virtual System::String^	id				( );
	virtual bool			choose			( System::String^ current, System::String^ filter, System::String^% result, bool read_only );

private:
	tool_base^			m_tool;
};

} // namespace editor
} // namespace xray

#endif // #ifndef SELECT_LIBRARY_CLASS_H_INCLUDED