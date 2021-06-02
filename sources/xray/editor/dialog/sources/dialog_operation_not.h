////////////////////////////////////////////////////////////////////////////
//	Created		: 16.03.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef DIALOG_OPERATION_NOT_H_INCLUDED
#define DIALOG_OPERATION_NOT_H_INCLUDED

#include "dialog_operation.h" 

#pragma managed( push, off )
namespace xray {
namespace dialog_editor {
	class dialog_operation_not : public dialog_operation 
	{
	public:
						dialog_operation_not();
		virtual			~dialog_operation_not();
		virtual	bool	check				();
		virtual	void	load				(xray::configs::lua_config_value const& cfg);
		virtual	void	save				(xray::configs::lua_config_value cfg);

	private:
		static u32	m_created_counter;
	}; // class dialog_operation_not
} // namespace dialog_editor 
} // namespace xray 
#pragma managed( pop )
#endif // #ifndef DIALOG_OPERATION_NOT_H_INCLUDED
