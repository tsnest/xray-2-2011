////////////////////////////////////////////////////////////////////////////
//	Created		: 20.05.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef OPTIONS_PAGE_H_INCLUDED
#define OPTIONS_PAGE_H_INCLUDED

namespace xray {
namespace editor {

public interface class options_page 
{
public:
	
	virtual void	activate_page	( );
	virtual void	deactivate_page	( );
	virtual bool	accept_changes	( );
	virtual void	cancel_changes	( );
	virtual bool	changed			( );

	virtual System::Windows::Forms::Control^	get_control	( );
	
}; // class options_page

public interface class options_manager
{
public:
	virtual	void	register_page	( System::String^ full_name, options_page^ page );
	virtual	void	unregister_page	( System::String^ full_name );
	
}; // class options_manager

} // namespace editor
} // namespace xray



#endif // #ifndef OPTIONS_PAGE_H_INCLUDED