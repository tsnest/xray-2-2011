////////////////////////////////////////////////////////////////////////////
//	Created		: 09.06.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef CPU_TIMER_H_INCLUDED
#define CPU_TIMER_H_INCLUDED

namespace xray {
namespace graphics_benchmark {

class benchmark;
class benchmark_test;

class cpu_timer: public boost::noncopyable {
public:
	cpu_timer(benchmark&, benchmark_test*, char const*);
	~cpu_timer();
	void stop();

private:
	bool						m_stoped;
	benchmark&					m_benchmark;
	benchmark_test*	m_bt_interface;
	std::string					m_scope_name;
	float						m_end_time;
	timing::timer				m_timer;

}; // class cpu_timer

} // namespace graphics_benchmark
} // namespace xray

#endif // #ifndef TIME_SCOPE_H_INCLUDED