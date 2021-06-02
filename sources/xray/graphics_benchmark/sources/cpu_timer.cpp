////////////////////////////////////////////////////////////////////////////
//	Created		: 09.06.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "cpu_timer.h"
#include "benchmark.h"

namespace xray {
namespace graphics_benchmark {

cpu_timer::cpu_timer(benchmark& benchmark, benchmark_test* bt_interface, char const* scope_name):
	m_benchmark(benchmark),
	m_bt_interface(bt_interface),
	m_scope_name(scope_name),
	m_stoped(false)
{
	//m_benchmark.begin_time_scope(m_bt_interface,m_scope_name.c_str());
	m_timer.start();
}

cpu_timer::~cpu_timer()
{
	//if (!m_stoped)
	//	m_benchmark.end_time_scope(m_bt_interface,m_scope_name.c_str(),m_timer.get_elapsed_sec());
}

void cpu_timer::stop()
{
	m_stoped = true;
	//m_benchmark.end_time_scope(m_bt_interface,m_scope_name.c_str(),m_timer.get_elapsed_sec());
}

} // namespace graphics_benchmark
} // namespace xray