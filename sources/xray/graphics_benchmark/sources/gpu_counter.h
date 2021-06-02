////////////////////////////////////////////////////////////////////////////
//	Created		: 24.06.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef GPU_COUNTER_H_INCLUDED
#define GPU_COUNTER_H_INCLUDED


namespace xray {
namespace graphics_benchmark {

class gpu_counter: public boost::noncopyable
{
public:
	gpu_counter();
	~gpu_counter();

	void start();
	void stop();

	// Return time in seconds.
	u64 get_pixel_processing_time() const;

private:
	ID3DCounter *m_pixel_processing_counter;
}; // class gpu_counter

} // namespace graphics_benchmark
} // namespace xray

#endif // #ifndef GPU_COUNTER_H_INCLUDED