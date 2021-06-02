////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2009
//	Author		: Igor Lobanchikov
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef RENDER_DEVICE_H_INCLUDED
#define RENDER_DEVICE_H_INCLUDED

namespace xray {
namespace render {


class render_device: public quasi_singleton<render_device>
{
public:
	render_device();
	~render_device();
	
	bool	begin_frame();
	void	end_frame();
	void	test_corrporate_level();
	
	u32	get_frame() {return m_frame;}

private:
	enum device_state
	{
		ds_ok = 0,
		ds_lost,
		ds_need_reset
	};

private:
	void	reset( bool precache = true ) const;
	void	setup_states() const;

	//	Platform specific
	device_state	get_device_state() const;
	void			begin_scene_platform();
	void			end_scene_platform();
	void			setup_states_platform() const;

private:
	bool	m_frame_started;
	u32		m_frame;
	device_state m_device_state;
}; // class render_device

} // namespace render 
} // namespace xray 


#endif // #ifndef RENDER_DEVICE_H_INCLUDED