////////////////////////////////////////////////////////////////////////////
//	Created		: 08.12.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_DEBUG_POLICIES_H_INCLUDED
#define XRAY_DEBUG_POLICIES_H_INCLUDED

namespace xray {

class debug_policy
{
public:
	enum { debug_enabled = true };

}; // class debug_policy

class no_debug_policy
{
public:
	enum { debug_enabled = false };

}; // class debug_policy

} // namespace xray

#endif // #ifndef XRAY_DEBUG_POLICIES_H_INCLUDED