////////////////////////////////////////////////////////////////////////////
//	Created		: 07.05.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs_path.h>

namespace xray {
namespace fs {

path_string   convert_to_native (pcstr const path)
{
	return path_string(path);
}

} // namespace fs
} // namespace xray