////////////////////////////////////////////////////////////////////////////
//	Created		: 14.05.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef GUI_ACTION_H_INCLUDED
#define GUI_ACTION_H_INCLUDED

#include "action.h"

namespace xray {
namespace editor_base {

public ref class action_continuous abstract: action
{
protected:
						action_continuous		( String^ name ): m_name(name) { ASSERT( m_name != nullptr ); }

	virtual				~action_continuous		( )	{}
public:
	
	virtual		bool			enabled			( )	override		{ return true; }

	virtual		String^			name			( ) sealed override	{ return m_name; }
	virtual		editor_base::checked_state	checked_state	( ) override		{ return checked_state::uncheckable;}
	virtual		bool			do_it			( ) override sealed			{ return true; }

private:
				String^			m_name;
}; // class action_continuous

} // namespace editor_base
} // namespace xray


#endif // #ifndef GUI_ACTION_H_INCLUDED