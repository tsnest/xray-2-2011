////////////////////////////////////////////////////////////////////////////
//	Created		: 23.05.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_STATISTICS_H_INCLUDED
#define XRAY_RENDER_STATISTICS_H_INCLUDED

#include <xray/render/core/quasi_singleton.h>
#include <xray/render/core/gpu_timer.h>


//------------------------------------------------
// TODO: place in separate files!
//------------------------------------------------

#define USE_GPU_TIMERS			0
#define USE_RENDER_STATISTICS	1

namespace xray {
	namespace ui{
		struct world;
	}
namespace render {

struct statistics_base;

struct statistics_group: public boost::noncopyable
{
					statistics_group	(pcstr group_name);
					~statistics_group	();
	u32				render				(xray::ui::world& ui_world, u32 x, u32 y);
	void			start				();
	u32				get_num_text_lines	() const;
	statistics_base*					first_statistics;
	
private:
	friend class						statistics;
	xray::fixed_string<128>				m_name;
	statistics_group*					m_next;
}; // struct statistics_group


struct statistics_base: public boost::noncopyable
{
					statistics_base		(statistics_group* group, pcstr name);
					~statistics_base	();
	virtual void	print				(fs_new::virtual_path_string& out_result) = 0;
	virtual void	start				() = 0;
	
protected:
	friend struct						statistics_group;
	xray::fixed_string<128>				m_name;
	statistics_base*					m_next;
}; // struct statistics_base


template< class T > struct statistics_value: public statistics_base
{
					statistics_value	(statistics_group* group, pcstr name);
	virtual void	start				();
	virtual void	print				(fs_new::virtual_path_string& out_result) = 0;
			T		average				();
	
	T									value;
protected:
	static int const					num_history_values = 16;
	T									history[num_history_values];
	u32									history_index;
}; // struct statistics_base


struct statistics_float: public statistics_value<double>
{
					statistics_float	(statistics_group* group, pcstr name): statistics_value<double>(group, name) {}
	virtual void	print				(fs_new::virtual_path_string& out_result);
}; // struct statistics_float


struct statistics_int: public statistics_value<int>
{
					statistics_int		(statistics_group* group, pcstr name): statistics_value<int>(group, name) {}
	virtual void	print				(fs_new::virtual_path_string& out_result);
}; // struct statistics_int

struct statistics_cpu_gpu: public statistics_base
{
	statistics_cpu_gpu	(statistics_group* group, pcstr name): statistics_base(group, name), cpu_time(0, 0), gpu_time(0, 0) {}
	
	virtual void		start();
	virtual void		print (fs_new::virtual_path_string& out_result);
	
	statistics_float	cpu_time;
	statistics_float	gpu_time;
}; // struct statistics_gpu_cpu

struct cpu_gpu_timer_scope: public boost::noncopyable
{
	cpu_gpu_timer_scope(statistics_cpu_gpu& counter):
		m_counter(counter)
	{
#if USE_GPU_TIMERS
		m_gpu_timer.start();
		m_gpu_timer.stop();
		
		double flush_time = m_gpu_timer.get_elapsed_sec();
		XRAY_UNREFERENCED_PARAMETER(flush_time);
		
		m_gpu_timer.start();
#endif // #if USE_GPU_TIMERS
		m_cpu_tumer.start();
	}
	~cpu_gpu_timer_scope()
	{
		m_counter.cpu_time.value += 1000.0 * double(m_cpu_tumer.get_elapsed_ticks()) / double(xray::timing::g_qpc_per_second);
		
#if USE_GPU_TIMERS
		m_gpu_timer.stop();
		m_counter.gpu_time.value += float(m_gpu_timer.get_elapsed_sec() * 1000.0f);
#endif // #if USE_GPU_TIMERS
	}
	statistics_cpu_gpu& m_counter;
#if USE_GPU_TIMERS
	gpu_timer			m_gpu_timer;
#endif // #if USE_GPU_TIMERS
	timing::timer		m_cpu_tumer;
};

struct timer_scope: public boost::noncopyable
{
	timer_scope(statistics_float& counter):
		m_counter(counter)
	{
		m_cpu_tumer.start();
	}
	~timer_scope()
	{
		m_counter.value += 1000.0 * double(m_cpu_tumer.get_elapsed_ticks() / xray::timing::g_qpc_per_second);
	}
	statistics_float&	m_counter;
	timing::timer		m_cpu_tumer;
};

struct gbuffer_statistics_group: public statistics_group
{
	gbuffer_statistics_group	(pcstr group_name);
	statistics_cpu_gpu			pre_pass_execute_time;
	statistics_cpu_gpu			material_pass_execute_time;
};

struct visibility_statistics_group: public statistics_group
{
	visibility_statistics_group	(pcstr group_name);
	statistics_float			culling_time;
	statistics_float			models_updating_time;
	statistics_int				num_draw_calls;
	statistics_int				num_triangles;
	statistics_int				num_total_rendered_triangles;
	statistics_int				num_total_rendered_points;
	statistics_int				num_models;
	statistics_int				num_lights;
	statistics_int				num_particles;
	statistics_int				num_speedtree_instances;
};

struct lights_statistics_group: public statistics_group
{
	lights_statistics_group		(pcstr group_name);
	statistics_cpu_gpu			accumulate_lighting_time;
	statistics_cpu_gpu			forward_lighting_time;
	statistics_cpu_gpu			shadow_map_time;
};

struct particles_statistics_group: public statistics_group
{
	particles_statistics_group	(pcstr group_name);
	statistics_cpu_gpu			execute_time;
	statistics_cpu_gpu			sprites_execute_time;
	statistics_cpu_gpu			beamtrails_execute_time;
	statistics_cpu_gpu			meshes_execute_time;
	// TODO:
	statistics_int				num_total_instances;
};

struct cascaded_sun_shadow_statistics_group: public statistics_group
{
	cascaded_sun_shadow_statistics_group(pcstr group_name);
	statistics_cpu_gpu			execute_time_cascade_1;
	statistics_cpu_gpu			execute_time_cascade_2;
	statistics_cpu_gpu			execute_time_cascade_3;
	statistics_cpu_gpu			execute_time_cascade_4;
	statistics_int				num_models_cascade_1;
	statistics_int				num_models_cascade_2;
	statistics_int				num_models_cascade_3;
	statistics_int				num_models_cascade_4;
};

struct postprocess_statistics_group: public statistics_group
{
	postprocess_statistics_group(pcstr group_name);
	statistics_cpu_gpu			execute_time;
};

struct forward_stage_statistics_group: public statistics_group
{
	forward_stage_statistics_group(pcstr group_name);
	statistics_cpu_gpu			execute_time;
};

struct general_statistics_group: public statistics_group
{
	general_statistics_group	(pcstr group_name);
	statistics_cpu_gpu			render_frame_time;
	statistics_float			cpu_render_frame_time;
	statistics_int				num_setted_shader_constants;
	statistics_int				fps;
	statistics_int				cpu_fps;
};

struct speedtree_statistics_group: public statistics_group
{
	speedtree_statistics_group	(pcstr group_name);
	statistics_cpu_gpu			render_time;
	statistics_float			culling_time;
	//statistics_int				num_last_lod_billboards;
	statistics_int				num_instances;
	//statistics_int				num_unique_trees;
};

struct forward_decals_statistics_group: public statistics_group
{
	forward_decals_statistics_group	(pcstr group_name);
	statistics_float			execute_time;
	statistics_int				num_decals;
	statistics_int				num_decal_draw_calls;
};

struct deferred_decals_statistics_group: public statistics_group
{
	deferred_decals_statistics_group	(pcstr group_name);
	statistics_float			execute_time;
	statistics_int				num_decals;
	statistics_int				num_decal_draw_calls;
};

struct distortion_pass_statistics_group: public statistics_group
{
	distortion_pass_statistics_group	(pcstr group_name);
	statistics_cpu_gpu					accumulate_time;
	statistics_cpu_gpu					apply_time;
};

struct ssao_statistics_group: public statistics_group
{
	ssao_statistics_group				(pcstr group_name);
	statistics_cpu_gpu					ssao_accumulate_time;
	statistics_cpu_gpu					ssao_blurring_time;
};

struct sky_statistics_group: public statistics_group
{
	sky_statistics_group				(pcstr group_name);
	statistics_cpu_gpu					execute_time;
};

struct lpv_statistics_group: public statistics_group
{
	lpv_statistics_group				(pcstr group_name);
	statistics_cpu_gpu					lpv_lookup_time;
	statistics_cpu_gpu					propagation_time;
	statistics_cpu_gpu					gv_injection_time;
	statistics_cpu_gpu					vpl_injection_time;
	statistics_cpu_gpu					rsm_downsample_time;
	statistics_cpu_gpu					rsm_rendering_time;
};

class statistics: public quasi_singleton<statistics>
{
public:
	statistics								();
	~statistics								();
	void start								();
	void render								(xray::ui::world& ui_world, u32 x, u32 y);
	u32 get_num_text_lines					() const;
	statistics_group*						first_group;
	
	postprocess_statistics_group			postprocess_stat_group;
	ssao_statistics_group					ssao_stat_group;
	distortion_pass_statistics_group		distortion_pass_stat_group;
	particles_statistics_group				particles_stat_group;
	sky_statistics_group					sky_stat_group;
	speedtree_statistics_group				speedtree_stat_group;
	forward_stage_statistics_group			forward_stage_stat_group;
	cascaded_sun_shadow_statistics_group	cascaded_sun_shadow_stat_group;
	gbuffer_statistics_group				gbuffer_stat_group;
	forward_decals_statistics_group			forward_decals_stat_group;
	lpv_statistics_group					lpv_stat_group;
	deferred_decals_statistics_group		deferred_decals_stat_group;
	lights_statistics_group					lights_stat_group;
	visibility_statistics_group				visibility_stat_group;
	general_statistics_group				general_stat_group;
	
private:
	friend struct							statistics_base;
	friend struct							statistics_group;
}; // class statistics

/*
#if USE_RENDER_STATISTICS
#	define BEGIN_CPUGPU_TIMER(c) { cpu_gpu_timer_scope local_cpu_gpu_timer_scope(c);
#	define END_CPUGPU_TIMER }
#	define BEGIN_TIMER(c) { timer_scope local_timer_scope(c);
#	define END_TIMER }
#else // #if USE_RENDER_STATISTICS
#	define BEGIN_CPUGPU_TIMER(c)
#	define END_CPUGPU_TIMER
#	define BEGIN_TIMER(c)
#	define END_TIMER
#endif // #if USE_RENDER_STATISTICS
*/

#if USE_RENDER_STATISTICS
#	define BEGIN_CPUGPU_TIMER(c) { cpu_gpu_timer_scope local_cpu_gpu_timer_scope(c);
#	define END_CPUGPU_TIMER }
#else // #if USE_RENDER_STATISTICS
#	define BEGIN_CPUGPU_TIMER(c)
#	define END_CPUGPU_TIMER
#endif // #if USE_RENDER_STATISTICS

#	define BEGIN_TIMER(c) { timer_scope local_timer_scope(c);
#	define END_TIMER }

} // namespace render
} // namespace xray

#include "statistics_inline.h"

#endif // #ifndef XRAY_RENDER_STATISTICS_H_INCLUDED
