////////////////////////////////////////////////////////////////////////////
//	Created		: 09.06.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BENCHMARK_H_INCLUDED
#define BENCHMARK_H_INCLUDED

#include "benchmark_types.h"
#include "benchmark_renderer.h"
#include "counters_result.h"

namespace xray {
namespace graphics_benchmark {

class gpu_timer;
class benchmark_test;


class benchmark: public boost::noncopyable
{
public:

	benchmark						( char const* benchmark_file);
	~benchmark						( );

	void register_test_class		( creator_func_type creator_func, char const* class_name);
	bool execute_benchmark_file		( char const* file_name);
	bool execute_test				( group_to_execute& group, test_to_execute& test, gpu_timer& gtimer, bool preview = false);
	u32  get_current_test_index		( )	const { return m_current_test_index;}
	void get_window_resolution		( u32& out_size_x, u32& out_size_y) const { out_size_x = m_window_size_x; out_size_y = m_window_size_y;}
	void start_execution			( );

	void save_results				( );

	benchmark_renderer& renderer	( )	{ return m_benchmark_renderer;}

	static std::string target_to_string ( benchmark_target_enum t);
	static benchmark_target_enum string_to_target ( std::string const& t );

private:

	benchmark_class* get_benchmark_class( char const* class_name) const;

	void on_load_benchmark_file			( resources::queries_result & in_result );

	bool initialize						( );
	bool post_initialize				( );

	void finalize						( );

	bool initialize_d3d					( );
	void finalize_d3d					( );

	bool initialize_ati					( );
	bool initialize_nvidia				( );
	bool initialize_xbox				( ) { return false; }
	bool initialize_ps3					( ) { return false; }

	void pre_finalize					( );

	void finalize_ati					( );
	void finalize_nvidia				( );
	void finalize_xbox					( ) { }
	void finalize_ps3					( ) { }

	ID3DDevice* d3d_device				( ) const;
	
	HWND get_new_window					( u32 window_size_x, u32 window_size_y);

	void initialize_group_rts			( group_to_execute& group);

	// Don't change order of members!
private:
	std::string							m_benchmark_file_name;
	bool								m_show_realtime_counter_results;
	bool								m_show_preview_window;
	bool								m_is_valid_benchmark_file_name;

	std::vector<group_to_execute>		m_groups;
	std::string							m_log_file_name;
	u32									m_current_test_index;
	u32									m_start_gpu_time;

	HWND								m_hwnd;
	u32									m_window_size_x;
	u32									m_window_size_y;
	bool								m_is_full_screen;

	benchmark_target_enum				m_benchmark_target;
	//render::shader_macros				m_shader_macros;

	//typedef std::multimap<std::string, counters_result> results_type;
	//results_type m_results;

	//render::resource_manager			m_res_mgr;

	bool								m_benchmark_loaded;
	bool								m_counters_valid;

	benchmark_renderer					m_benchmark_renderer;

	benchmark_class*					m_first_class;
}; // class benchmark

} // namespace graphics_benchmark
} // namespace xray

#endif // #ifndef BENCHMARK_H_INCLUDED