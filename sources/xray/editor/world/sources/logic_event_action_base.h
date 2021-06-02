////////////////////////////////////////////////////////////////////////////
//	Created		: 12.05.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef LOGIC_EVENT_ACTION_BASE_H_INCLUDED
#define LOGIC_EVENT_ACTION_BASE_H_INCLUDED

namespace xray {
namespace editor {

public interface class logic_event_action_base {
public:
	virtual property System::String^	action_type;	
	virtual void						save( configs::lua_config_value config );
	virtual void						load( configs::lua_config_value const& config );

}; // class logic_event_action_base

} // namespace editor
} // namespace xray

#endif // #ifndef LOGIC_EVENT_ACTION_BASE_H_INCLUDED