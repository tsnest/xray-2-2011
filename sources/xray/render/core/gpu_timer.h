////////////////////////////////////////////////////////////////////////////
//	Created		: 23.05.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_GPU_TIMER_H_INCLUDED
#define XRAY_RENDER_GPU_TIMER_H_INCLUDED

namespace xray {
namespace render {

class gpu_timer: public boost::noncopyable
{
public:
			gpu_timer			();
			~gpu_timer			();
	void	start				();
	void	stop				();
	void	get_elapsed_info	(double& out_sec, u64& out_ticks, u64& out_frequency) const;
	double	get_elapsed_sec		() const;

private:
	ID3DQuery*					m_start_time;
	ID3DQuery*					m_stop_time;
	ID3DQuery*					m_freq_info;
	ID3DQuery*					out_empty_query_ptr;
	D3D_QUERY_DESC				m_query_desc;
}; // class gpu_timer

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_GPU_TIMER_H_INCLUDED