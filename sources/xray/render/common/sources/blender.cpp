////////////////////////////////////////////////////////////////////////////
//	Created		: 13.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "blender.h"

namespace xray {
namespace render {


u32	xrPREAD(memory::reader& mem_reader)
{
	u32 T = mem_reader.r_u32();
	mem_reader.skip_string();
	return T;
}

void xrPREAD_MARKER(memory::reader& mem_reader)
{
	XRAY_UNREFERENCED_PARAMETER	( mem_reader );
	R_ASSERT(xrPID_MARKER==xrPREAD(mem_reader));
}

void read_marker(memory::reader& mem_reader)
{
	xrPREAD_MARKER(mem_reader);
}

void read_int(memory::reader& mem_reader, int& int_prop)
{
	XRAY_UNREFERENCED_PARAMETERS	( mem_reader, int_prop );
}

void read_bool(memory::reader& mem_reader, bool& bool_prop)
{
	XRAY_UNREFERENCED_PARAMETERS	( mem_reader, bool_prop );
}

void read_string(memory::reader& mem_reader, char* str_prop)
{
	XRAY_UNREFERENCED_PARAMETERS	( mem_reader, str_prop );
}

void blender::load(memory::reader& mem_reader)
{
	mem_reader.r	(&m_desc, sizeof(m_desc), sizeof(m_desc));

	// Properties
	xrPREAD_MARKER(mem_reader);
	xrP_Integer	priority;
	xrPREAD_PROP(mem_reader,xrPID_INTEGER, priority);
	m_priority = priority.value;
	xrP_BOOL strict_sort;
	xrPREAD_PROP(mem_reader,xrPID_BOOL, strict_sort);
	m_strict_sort = strict_sort.value != 0;
	xrPREAD_MARKER(mem_reader);
	xrPREAD_PROP(mem_reader,xrPID_TEXTURE, m_ot_name);
	xrPREAD_PROP(mem_reader,xrPID_MATRIX, m_ot_xform);
	
	//read_marker(mem_reader);
	//read_int(mem_reader, m_priority);
	//read_bool(mem_reader, m_strict_sort);
	//
	//read_marker(mem_reader);
	//read_string(mem_reader, m_ot_name);
	//read_string(mem_reader, m_ot_xform);
}

void blender::compile(blender_compiler& compiler, const blender_compilation_options& options)
{
	XRAY_UNREFERENCED_PARAMETERS	( compiler, options );

	//if (C.bEditor)	C.SetParams	(oPriority.value,oStrictSorting.value?true:false);
	//else			C.SetParams	(oPriority.value,oStrictSorting.value?true:false);
}

} // namespace render 
} // namespace xray 
