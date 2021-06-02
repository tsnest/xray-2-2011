////////////////////////////////////////////////////////////////////////////
//	Created		: 19.10.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef BEHAVIOUR_EDITOR_H_INCLUDED
#define BEHAVIOUR_EDITOR_H_INCLUDED

namespace xray {
namespace editor {


ref class behaviour_editor : public xray::editor_base::tool_window
{
public:
	behaviour_editor		( );
	virtual					~behaviour_editor	( ){};
	virtual void							clear_resources			( ){};
	virtual bool							close_query				( ){return true;}
	virtual	void							tick					( ){};
	virtual void							load_settings			( RegistryKey^ /*product_key*/ ){};
	virtual void							save_settings			( RegistryKey^ /*product_key*/ ){};
	virtual System::String^					name					( )					{ return m_name; };
	virtual	void							Show					( System::String^ , System::String^ )	{ m_form->Show(); m_form->BringToFront(); }
	virtual System::Windows::Forms::Form^	main_form				( ) { return m_form; }

private:
	void									load_behaviours_library	( );
	void									on_behaviours_library_loaded( xray::resources::queries_result& data );

private:
	System::String^							m_name;
	System::Windows::Forms::Form^			m_form;

}; // class behaviour_editor


} // namespace editor
} // namespace xray

#endif // #ifndef BEHAVIOUR_EDITOR_H_INCLUDED