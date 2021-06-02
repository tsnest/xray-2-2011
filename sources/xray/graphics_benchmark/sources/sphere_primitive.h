////////////////////////////////////////////////////////////////////////////
//	Created		: 18.06.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SPHERE_PRIMITIVE_H_INCLUDED
#define SPHERE_PRIMITIVE_H_INCLUDED

#include "benchmark_test.h"

namespace xray {
namespace graphics_benchmark {

#define SPHERE_NUMVERTEX 92
#define SPHERE_NUMFACES 180
#define SPHERE_NUMVERTEXL 60
#define SPHERE_NUMLINES 60

extern  float3	sphere_vertices[];
extern  WORD	sphere_faces[];
extern  float3	sphere_verticesl[];
extern  WORD	sphere_lines[];


} // namespace graphics_benchmark
} // namespace xray

#endif // #ifndef SPHERE_PRIMITIVE_H_INCLUDED
