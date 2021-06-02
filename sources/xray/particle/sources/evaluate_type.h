////////////////////////////////////////////////////////////////////////////
//	Created		: 03.03.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef EVALUATE_TYPE_H_INCLUDED
#define EVALUATE_TYPE_H_INCLUDED

#include "help_functions.h"

namespace xray {
namespace particle {

enum enum_evaluate_type
{
	age_evaluate_type,
	random_evaluate_type,
};

enum_evaluate_type string_to_evaluate_type(pcstr name);

} // namespace particle
} // namespace xray

#endif // #ifndef EVALUATE_TYPE_H_INCLUDED