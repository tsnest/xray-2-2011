////////////////////////////////////////////////////////////////////////////
//	Created		: 09.06.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef GRAPHICS_BENCHMARK_MEMORY_H_INCLUDED
#define GRAPHICS_BENCHMARK_MEMORY_H_INCLUDED

#include <xray/buffer_vector.h>
#include <xray/fixed_vector.h>
#include <xray/associative_vector.h>
#include <xray/hash_multiset.h>

#include <map>
#include <vector>
#include <set>

namespace xray {
namespace graphics_benchmark {

extern	xray::memory::doug_lea_allocator_type	g_allocator;

} // namespace graphics_benchmark
} // namespace xray

#include <xray/render/facade/render_allocator.h>

#endif // #ifndef GRAPHICS_BENCHMARK_MEMORY_H_INCLUDED