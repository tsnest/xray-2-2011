////////////////////////////////////////////////////////////////////////////
//	Created		: 05.02.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef DIALOG_LINK_ACTION_H_INCLUDED
#define DIALOG_LINK_ACTION_H_INCLUDED

using namespace System;
using namespace System::Collections;
using namespace System::ComponentModel;

#include "dialog_action.h"

namespace xray {
namespace dialog_editor {
	class dialog_action;
	enum e_action_functions;

	public ref class dialog_link_action : public Windows::Forms::TreeNode
	{
		typedef Windows::Forms::TreeNode super;
	public:
		enum class e_func_names : int
		{
			no_func		= 0,
			give_info	= 1,
			give_item	= 2,
		};

		dialog_link_action					();
		dialog_link_action					(dialog_action* act);
		~dialog_link_action					();

		dialog_action*			action_copy	();
		[CategoryAttribute("Action properties"), DisplayNameAttribute("Name"), 
			DescriptionAttribute("Name of action to be called"), ReadOnlyAttribute(true)]
		property String^ name
		{
			String^				get			()					{return Name;}
		};

		[CategoryAttribute("Action properties"), DisplayNameAttribute("Function"), 
			DescriptionAttribute("Name of action to be called")]
		property e_func_names func_names
		{
			e_func_names		get			()					{return e_func_names(m_action->m_function);};
			void				set			(e_func_names nv)	{m_action->m_function = e_action_functions(nv);};
		};

		[CategoryAttribute("Action properties"), DisplayNameAttribute("Parameter"), 
			DescriptionAttribute("Parameter of action to be called")]
		property String^ param1
		{
			String^				get			()					{return gcnew String(m_action->m_param);};
			void				set			(String^ nv);
		};

	private:
		dialog_action*	m_action;
	}; // ref class dialog_link_action
} // namespace dialog_editor
} // namespace xray

#endif // #ifndef DIALOG_LINK_ACTION_H_INCLUDED