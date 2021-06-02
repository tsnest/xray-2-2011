////////////////////////////////////////////////////////////////////////////
//	Created		: 16.02.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCE_CHOOSER_H_INCLUDED
#define RESOURCE_CHOOSER_H_INCLUDED

namespace xray {
namespace editor_base {

public interface class chooser
{
public:
	virtual System::String^	id			( )	= 0;
	virtual bool			choose		( System::String^ current, System::String^ filter, System::String^% result, bool read_only ) = 0;
}; // class resource_chooser

public ref class resource_chooser
{
public:
	static void				register_chooser	( chooser^ c );
	static void				unregister_chooser	( chooser^ c );
	static bool				choose				( System::String^ id, System::String^ current, System::String^ filter, System::String^% result, bool read_only );
	static void				set_shared_context	( System::String^ key, System::Object^ value );
	static System::Object^	get_shared_context	( System::String^ key );

private:
	static System::Collections::Generic::Dictionary<System::String^, chooser^>	m_choosers;
	static System::Collections::Generic::Dictionary<System::String^, System::Object^>	m_shared_context;
};

} // namespace editor_base
} // namespace xray

#endif // #ifndef RESOURCE_CHOOSER_H_INCLUDED