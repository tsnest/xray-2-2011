////////////////////////////////////////////////////////////////////////////
//	Created		: 03.03.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "evaluate_type.h"

namespace xray {
namespace particle {

enum_evaluate_type string_to_evaluate_type(pcstr name)
{
		 if	(xray::strings::compare(name, "Age")==0)	return age_evaluate_type;
	else if (xray::strings::compare(name, "Random")==0)	return random_evaluate_type;
	
	return age_evaluate_type;
}

} // namespace particle
} // namespace xray