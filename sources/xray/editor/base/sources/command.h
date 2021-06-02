////////////////////////////////////////////////////////////////////////////
//	Created		: 30.03.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_H_INCLUDED
#define COMMAND_H_INCLUDED

namespace xray {
namespace editor_base {

public ref class command abstract
{

public:
	virtual			~command		()	{}
	virtual bool	commit			() = 0;
	virtual void	rollback		() = 0;

	virtual void	start_preview	()	{ };
	virtual bool	end_preview		()	{ return true; };

}; // class command

}// namespace editor_base
}// namespace xray

#endif // #ifndef COMMAND_H_INCLUDED