////////////////////////////////////////////////////////////////////////////
//	Created		: 18.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <xray/ai/search/const_oracle.h>

namespace xray {
namespace ai {
namespace planning {

const_oracle::property_value_type const_oracle::value	( )
{
	return m_value;
}

} // namespace planning
} // namespace ai
} // namespace xray