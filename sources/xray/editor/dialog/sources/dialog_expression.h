////////////////////////////////////////////////////////////////////////////
//	Created		: 16.03.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef DIALOG_EXPRESSION_H_INCLUDED
#define DIALOG_EXPRESSION_H_INCLUDED

#pragma managed( push, off )
namespace xray {
namespace dialog_editor {
	class dialog_expression
	{
	public:
		virtual	~dialog_expression(){};
		virtual bool	check	()=0;
		virtual	void	load	(xray::configs::lua_config_value const& cfg)=0;
		virtual	void	save	(xray::configs::lua_config_value cfg)=0;
		virtual	void	destroy	()=0;
	}; // class dialog_expression
} // namespace dialog_editor 
} // namespace xray 
#pragma managed( pop )
#endif // #ifndef DIALOG_EXPRESSION_H_INCLUDED
