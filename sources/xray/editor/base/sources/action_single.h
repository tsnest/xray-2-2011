////////////////////////////////////////////////////////////////////////////
//	Created		: 22.05.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef ACTION_SINGLE_H_INCLUDED
#define ACTION_SINGLE_H_INCLUDED

#include "action.h"

namespace xray {
namespace editor_base {
ref class scene_view_panel;

public ref class action_single abstract : action
{
 public:
							action_single	( String^ name ):m_name( name ){}
	virtual					~action_single	( ){};

	virtual String^			name			( ) override	{ return m_name; }
	virtual bool			enabled			( )	override	{ return true; }
	virtual editor_base::checked_state checked_state	( )	override	{ return checked_state::uncheckable;}

	virtual bool			capture			( ) sealed override { return do_it(); }
	virtual void			release			( ) sealed override	{ }
	virtual bool			execute			( ) sealed override	{ /*UNREACHABLE_CODE();*/ return true;}

private:
	String^					m_name;

}; // class action_single


public ref class mouse_action_touch_object : public action_single
{
	typedef action_single	super;
public:

	mouse_action_touch_object		( System::String^ name, scene_view_panel^ v );

	virtual bool		do_it		( ) override;

private:
	scene_view_panel^	m_window_view;
}; // class mouse_action_touch_object

} // namespace editor_base
} // namespace xray


#endif // #ifndef ACTION_SINGLE_H_INCLUDED