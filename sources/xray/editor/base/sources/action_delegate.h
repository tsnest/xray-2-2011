////////////////////////////////////////////////////////////////////////////
//	Created		: 25.10.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ACTION_DELEGATE_H_INCLUDED
#define ACTION_DELEGATE_H_INCLUDED

#include "action_single.h"

namespace xray {
namespace editor_base {

ref class action_single;

public delegate void execute_delegate_managed();
public delegate bool enabled_delegate_managed();
public delegate bool checked_delegate_managed();

public ref class action_delegate : editor_base::action_single
{
public:
	action_delegate ( System::String^ name, execute_delegate_managed^ execute_del );
	virtual			~action_delegate ();

	void			set_enabled(enabled_delegate_managed^ d );
	void			set_checked(checked_delegate_managed^ execute_del );

	virtual bool	do_it	() override 
	{
		execute_delegate_man	();
		return true;
	}

	virtual bool	enabled	() override
	{
		if( enabled_delegate_man )
			return enabled_delegate_man ();

		return true;
	}
	
	virtual xray::editor_base::checked_state checked_state () override
	{
		if(checked_delegate_man)
			return checked_delegate_man() ? editor_base::checked_state::checked : editor_base::checked_state::unchecked;
		else
			return editor_base::checked_state::uncheckable;
	}

private:
	execute_delegate_managed^	execute_delegate_man;
	enabled_delegate_managed^	enabled_delegate_man;

	checked_delegate_managed^	checked_delegate_man;

}; // class action_delegate

} // namespace editor_base
} // namespace xray


#endif // #ifndef ACTION_DELEGATE_H_INCLUDED