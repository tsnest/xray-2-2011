////////////////////////////////////////////////////////////////////////////
//	Created		: 22.05.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef ACTION_SINGLE_H_INCLUDED
#define ACTION_SINGLE_H_INCLUDED

#include "action_continuous.h"
#include "input_action_single.h"

namespace xray {
namespace editor {

public ref class action_single abstract : action, input_action_single_base
{
 public:
							action_single	( System::String^ name ):m_name( name ){}
	virtual					~action_single	( )	{ this->!action_single(); }
							!action_single	( ) { }

	virtual System::String^	name			( ) override	{ return m_name; }
	virtual	System::String^	text			( )				{ return name();}
	virtual bool			enabled			( )	override	{ return true; }
	virtual checked_state	checked_state	( )				{ return checked_state::uncheckable;}

private:
	System::String^		m_name;
}; // class action_single

} // namespace editor
} // namespace xray


#endif // #ifndef ACTION_SINGLE_H_INCLUDED