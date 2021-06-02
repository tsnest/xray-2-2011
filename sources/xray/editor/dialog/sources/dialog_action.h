////////////////////////////////////////////////////////////////////////////
//	Created		: 16.03.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef DIALOG_ACTION_H_INCLUDED
#define DIALOG_ACTION_H_INCLUDED

#pragma managed( push, off )

#include <xray/fixed_string.h>

namespace xray {
namespace dialog_editor {

enum e_action_functions {
	no_act_func	= 0,
	give_info	= 1,
	give_item	= 2,
};

class dialog_action {
	friend ref class dialog_link_action;
public:
						dialog_action		(u8 func=0, pstr par=NULL);
						~dialog_action		();
			void		do_action			();	
			void		load				(xray::configs::lua_config_value const& cfg);
			void		save				(xray::configs::lua_config_value cfg);

private:
	static	void		action_give_info	(pcstr info);
	static	void		action_give_item	(pcstr item_name);

private:
	e_action_functions	m_function;
	pstr				m_param;
	static u32			m_created_counter;
}; // class dialog_action

} // namespace dialog_editor 
} // namespace xray 

#pragma managed( pop )

#endif // #ifndef DIALOG_ACTION_H_INCLUDED