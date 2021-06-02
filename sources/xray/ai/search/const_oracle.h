////////////////////////////////////////////////////////////////////////////
//	Created		: 18.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_CONST_ORACLE_H_INCLUDED
#define XRAY_AI_SEARCH_CONST_ORACLE_H_INCLUDED

#include <xray/ai/search/oracle.h>

namespace xray {
namespace ai {
namespace planning {

class const_oracle : public oracle
{
public:
	inline	const_oracle				( property_value_type const& value, pcstr id );
	
	virtual	property_value_type	value	( );

private:
	typedef oracle						super;

private:
	property_value_type					m_value;
};

} // namespace planning
} // namespace ai
} // namespace xray

#include <xray/ai/search/const_oracle_inline.h>

#endif // #ifndef XRAY_AI_SEARCH_CONST_ORACLE_H_INCLUDED