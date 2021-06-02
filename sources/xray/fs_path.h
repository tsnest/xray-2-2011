#ifndef FS_PATH_H_INCLUDED
#define FS_PATH_H_INCLUDED

#include <xray/fs_path_string.h>

namespace xray	{
namespace fs	{

XRAY_CORE_API	path_string		convert_to_native	(pcstr path);
XRAY_CORE_API	path_string		convert_to_portable (pcstr path);
XRAY_CORE_API	pstr 			convert_to_portable_in_place	(pstr const path);
XRAY_CORE_API	pstr 			convert_to_native_in_place		(pstr const path);
} // namespace fs
} // namespace xray

#endif // FS_PATH_H_INCLUDED