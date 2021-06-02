////////////////////////////////////////////////////////////////////////////
//	Created		: 06.06.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef UNIQUE_NAME_HELPER_H_INCLUDED
#define UNIQUE_NAME_HELPER_H_INCLUDED

namespace xray {
namespace model_editor {

ref class unique_name_helper {
public:
	void				add				( System::String^ name );
	void				remove			( System::String^ name );
	System::String^		get_unique_name	( System::String^ name_template );
	bool				contains		( System::String^ name );
	void				clear();
private:
	typedef System::Collections::Generic::Dictionary<System::String^, System::Byte> string_dictionary;
	string_dictionary m_dictionary;
}; // class name_unique_helper

} // namespace model_editor
} // namespace xray

#endif // #ifndef UNIQUE_NAME_HELPER_H_INCLUDED