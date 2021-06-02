////////////////////////////////////////////////////////////////////////////
//	Created		: 22.05.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef EDITOR_ACTION_H_INCLUDED
#define EDITOR_ACTION_H_INCLUDED

namespace xray {
namespace editor {

public enum class checked_state{uncheckable,checked,unchecked};

public interface class action 
{
public:
	virtual void			execute			( ) = 0;
	virtual bool			enabled			( ) = 0;
	virtual	System::String^	name			( );
	virtual	System::String^	text			( ) = 0;
	virtual checked_state	checked_state	( ) = 0;
}; // class action

} // namespace editor
} // namespace xray


#endif // #ifndef EDITOR_ACTION_H_INCLUDED