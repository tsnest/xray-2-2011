#include "pch.h"

namespace xray {
namespace debug {

command_line::key	g_dont_check_multithreaded_safety("dont_check_multithreaded_safety", 
													  "",
													  "threading",
													  "turn off checks of parallel use of code that is not multithreaded");


} // namespace debug
} // namespace xray
