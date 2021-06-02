////////////////////////////////////////////////////////////////////////////
//	Created		: 11.06.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef GPU_TIMER_H_INCLUDED
#define GPU_TIMER_H_INCLUDED



namespace xray {
namespace graphics_benchmark {

class benchmark;

class gpu_timer: public boost::noncopyable
{
public:
	gpu_timer();
	~gpu_timer();

	void start();
	void stop();

	// Return elapsed seconds and ticks.
	void get_elapsed_info(double& out_sec, u64& out_ticks, u64& out_frequency) const;

private:
	ID3DQuery *m_start_time,
				*m_stop_time,
				*m_freq_info;
}; // class gpu_timer

} // namespace graphics_benchmark
} // namespace xray

#endif // #ifndef GPU_TIMER_H_INCLUDED