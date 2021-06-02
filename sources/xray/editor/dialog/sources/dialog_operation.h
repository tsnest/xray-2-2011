////////////////////////////////////////////////////////////////////////////
//	Created		: 16.03.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef DIALOG_OPERATION_H_INCLUDED
#define DIALOG_OPERATION_H_INCLUDED


#include "dialog_expression.h" 

#pragma managed( push, off )
namespace xray {
namespace dialog_editor {
	class dialog_operation : public dialog_expression 
	{
	public:
											dialog_operation	();
		virtual								~dialog_operation	();
		virtual bool						check				()=0;
		virtual	void						load				(xray::configs::lua_config_value const& cfg)=0;
		virtual	void						save				(xray::configs::lua_config_value cfg)=0;
		virtual	void						destroy				();
				vector<dialog_expression*>*	childs				()							{return &m_childs;};
				void						add_child			(dialog_expression* chld)	{m_childs.push_back(chld);};

	protected:
		vector<dialog_expression*> m_childs;
	}; // class dialog_operation
} // namespace dialog_editor 
} // namespace xray 
#pragma managed( pop )
#endif // #ifndef DIALOG_OPERATION_H_INCLUDED
