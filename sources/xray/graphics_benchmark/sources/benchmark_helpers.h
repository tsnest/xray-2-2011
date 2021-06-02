////////////////////////////////////////////////////////////////////////////
//	Created		: 21.06.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BENCHMARK_HELPERS_H_INCLUDED
#define BENCHMARK_HELPERS_H_INCLUDED

#include "benchmark_test.h"
#include "counters_result.h"

namespace xray {
namespace graphics_benchmark {


std::string file_to_string(char const* file_name, bool& out_valid_file);
DXGI_FORMAT get_dxgi_format(std::string const& name);

} // namespace graphics_benchmark
} // namespace xray

#endif // #ifndef BENCHMARK_HELPERS_H_INCLUDED