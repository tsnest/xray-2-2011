////////////////////////////////////////////////////////////////////////////
//	Created		: 15.05.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef INPUT_ACTION_BASE_H_INCLUDED
#define INPUT_ACTION_BASE_H_INCLUDED

#include "input_action.h"

namespace xray {
namespace editor {

public ref class input_action_single_base abstract :public input_action
{
public:
	virtual bool		capture		( ) sealed override { execute(); return true; }
	virtual void		execute		( ) new { }

private:
	virtual void		release		( ) sealed new { }
}; // input_action_base

} // namespace editor
} // namespace xray



#endif // #ifndef INPUT_ACTION_BASE_H_INCLUDED