#ifndef XRAY_FS_PATH_STRING_H_INCLUDED
#define XRAY_FS_PATH_STRING_H_INCLUDED

#include <xray/fixed_string.h>

namespace xray	{
namespace fs	{

static	u32	const max_path_length			= sizeof(string_path) / sizeof(char);
typedef	fixed_string< max_path_length >		path_string;

//template class XRAY_CORE_API fixed_string< max_path_length >;

} // namespace fs
} // namespace xray

#endif // XRAY_FS_PATH_STRING_H_INCLUDED