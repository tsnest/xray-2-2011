////////////////////////////////////////////////////////////////////////////
//	Created		: 16.03.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef DIALOG_PRECONDITION_H_INCLUDED
#define DIALOG_PRECONDITION_H_INCLUDED

#include "dialog_expression.h" 

#pragma managed( push, off )
namespace xray {
namespace dialog_editor {
	enum e_precondition_functions
	{
		no_prec_func	= 0,
		has_info		= 1,
		has_item		= 2,
	};

	class dialog_precondition : public dialog_expression
	{
		friend ref class dialog_link_precondition;
	public:
						dialog_precondition	(int func=0, pstr par=NULL);
		virtual			~dialog_precondition();
		virtual bool	check				();
		virtual	void	load				(xray::configs::lua_config_value const& cfg);
		virtual	void	save				(xray::configs::lua_config_value cfg);
		virtual	void	destroy				();

	private:
		static	bool	predicate_has_info	(pcstr info);
		static	bool	predicate_has_item	(pcstr item_name);

	private:
		e_precondition_functions	m_function;
		pstr						m_param;
		static u32					m_created_counter;
	}; // class dialog_precondition
} // namespace dialog_editor 
} // namespace xray 
#pragma managed( pop )
#endif // #ifndef DIALOG_PRECONDITION_H_INCLUDED
