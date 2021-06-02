////////////////////////////////////////////////////////////////////////////
//	Created		: 01.05.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCE_MANAGER_H_INCLUDED
#define RESOURCE_MANAGER_H_INCLUDED

#include <xray/render/core/quasi_singleton.h>
#include <xray/render/core/shader_configuration.h>
#include <xray/render/core/res_common.h>
#include <xray/render/core/res_sampler_state.h>
#include <xray/render/core/texture_slot.h>
#include <xray/render/core/sampler_slot.h>
#include <xray/render/core/shader_macros.h>	// for shader_defines_list only!
#include <xray/render/core/state_cache.h>
#include <xray/render/core/shader_constant_bindings.h>

namespace xray {

namespace vfs {
	struct vfs_notification;
} // namespace fs

namespace configs {
	class lua_config_value;
} // namespace configs

namespace render {

//////////////////////////////////////////////////////////////////////////
// Forward_declarations
class state_descriptor;
class res_state;
class sampler_state_descriptor;
class texture_storage;
class res_texture;
class res_input_layout;
class render_target;
class res_texture;
class res_texture_list;
class res_sampler_list;
class untyped_buffer;
class shader_constant_buffer;
class shader_constant_table;
class res_geometry;
class shader_constant_host;
class res_declaration;
class res_signature;
class res_render_output;
class shader_constant_bindings;
class shader_constant_binding;
struct shader_include_getter;

//////////////////////////////////////////////////////////////////////////
template < typename shader_data>
class xs_descriptor;

struct vs_data;
typedef  xs_descriptor<vs_data>		vs_descriptor;

struct gs_data;
typedef  xs_descriptor<gs_data>		gs_descriptor;

struct ps_data;
typedef  xs_descriptor<ps_data>		ps_descriptor;

//////////////////////////////////////////////////////////////////////////
template <typename shader_data>
class res_xs;

typedef  res_xs<vs_data>			res_vs;
typedef  res_xs<gs_data>			res_gs;
typedef  res_xs<ps_data>			res_ps;

//////////////////////////////////////////////////////////////////////////
template <typename shader_data>
class res_xs_hw;

typedef res_xs_hw<vs_data>	res_vs_hw;
typedef res_xs_hw<gs_data>	res_gs_hw;
typedef res_xs_hw<ps_data>	res_ps_hw;

//////////////////////////////////////////////////////////////////////////
enum enum_buffer_type;
enum enum_shader_type;
enum enum_rt_usage;

//////////////////////////////////////////////////////////////////////////
typedef fastdelegate::FastDelegate< void ( enum_shader_type shader_type, char const* name, char const* error_string) >	shader_compile_error_handler;

struct binary_shader_source;
typedef	xray::resources::resource_ptr < binary_shader_source, xray::resources::unmanaged_intrusive_base > binary_shader_source_ptr;

struct binary_shader_key_type;
typedef associative_vector<binary_shader_key_type, binary_shader_source_ptr, render::vector> binary_shader_sources_type;

class resource_manager : public quasi_singleton<resource_manager>
{
	friend class texture_pool;

public:
	struct str_pred : public std::binary_function<char*, char*, bool>
	{
		inline bool operator()( fs_new::virtual_path_string x, fs_new::virtual_path_string y) const {return x < y;}
	};

	struct shader_source
	{
		shader_source():
#ifdef MASTER_GOLD
		data(memory::g_mt_allocator),
		includes(memory::g_mt_allocator)
#else // #ifdef MASTER_GOLD
		data(::xray::debug::g_mt_allocator),
		includes(::xray::debug::g_mt_allocator)
#endif // #ifdef MASTER_GOLD
		{}
		xray::vectora<u8>		data;
		u32						time;
		xray::vectora<pcstr>	includes;
	};
	
	struct shader_name_config_pair
	{
		shader_name_config_pair ( const char* name, shader_configuration const& config) : name(name), config(config){}
		const char*				name;
		shader_configuration	config;

		inline shader_name_config_pair& operator = ( shader_name_config_pair const & other)
		{
			name = other.name;
			config = other.config;
		}

 		inline bool operator < ( shader_name_config_pair const & other) const
 		{
			int str_res = strcmp	(name, other.name);
 			return (str_res < 0 ) || ( (str_res == 0) && config.configuration < other.config.configuration);
 		}

 		inline bool operator == ( shader_name_config_pair const & other) const
 		{
			int str_res = strcmp	(name, other.name);
			return ( (str_res == 0) && config.configuration == other.config.configuration);
 		}
	};

	typedef render::map<shader_name_config_pair, res_vs_hw* >	map_vs_hw;
	typedef render::map<shader_name_config_pair, res_gs_hw* >	map_gs_hw;
	typedef render::map<shader_name_config_pair, res_ps_hw* >	map_ps_hw;
	typedef render::map<fs_new::virtual_path_string, render_target*, str_pred>	map_rt;
	typedef render::map<fs_new::virtual_path_string, res_texture*, str_pred>	map_texture;
		
public:
	resource_manager( );
	~resource_manager( );
	
	res_texture*			create_texture				( char const * name, u32 mip_level_cut = 0, bool use_pool = false, bool load_async = true, bool use_converter = true);
	res_texture*			create_texture2d			( char const * user_name, u32 width, u32 height, D3D_SUBRESOURCE_DATA const* data, DXGI_FORMAT format, D3D_USAGE usage, u32 mip_levels, u32 array_size = 1);
	res_texture*			create_texture3d			( char const * user_name, u32 width, u32 height, u32 depth, D3D_SUBRESOURCE_DATA const * data, DXGI_FORMAT format, u32 usage, u32 mip_levels);
	res_texture*			create_texture_cube			( char const * user_name, u32 width, u32 height, D3D_SUBRESOURCE_DATA const * data, DXGI_FORMAT format, u32 usage, u32 mip_levels, bool use_as_depth_stencil);
	res_texture*			load_texture				( char const * name, u32 mip_level_cut, bool use_pool = false, bool load_async = true, bool use_converter = true);
	res_texture*			find_texture				( char const * name);
	
	untyped_buffer* 			create_buffer				( u32 size, const void * data, enum_buffer_type type, bool dynamic);

	shader_constant_table*		create_const_table			( shader_constant_table const& proto);
	
	shader_constant_host const*	register_constant_binding	( shader_constant_binding const & binding);

	shader_constant_buffer*	create_constant_buffer		( name_string_type const & name, enum_shader_type buffer_dest, D3D_CBUFFER_TYPE type, u32 size);

	res_sampler_state*		create_sampler_state		( sampler_state_descriptor const & sampler_props);

	// Find way to make "res_sampler_state *" const.
	void					register_sampler			( char const * name, res_sampler_state * sampler);
	res_sampler_state *		find_registered_sampler		( char const * name);

	res_vs_hw*				create_vs_hw				( const char * name, shader_configuration shader_config, shader_include_getter* include_getter = 0, binary_shader_sources_type* shader_sources = 0);
	
	res_gs_hw*				create_gs_hw				( const char * name, shader_configuration shader_config, shader_include_getter* include_getter = 0, binary_shader_sources_type* shader_sources = 0);
	
	res_ps_hw*				create_ps_hw				( const char * name, shader_configuration shader_config, shader_include_getter* include_getter = 0, binary_shader_sources_type* shader_sources = 0);
	
	
	
	render_target*			create_render_target		( LPCSTR name, u32 w, u32 h, DXGI_FORMAT fmt, enum_rt_usage usage, ref_texture texture = 0, u32 first_array_slice_index = 0, D3D_USAGE memory_usage = D3D_USAGE_DEFAULT, u32 sample_count = 1);
	
	render_target*			create_volume_render_target	( LPCSTR name, u32 w, u32 h, u32 d, DXGI_FORMAT fmt, enum_rt_usage usage, D3D_USAGE memory_usage);
	
	res_vs*					create_vs					( vs_descriptor const & vs);
	
	res_gs*					create_gs					( gs_descriptor const & gs);
	
	res_ps*					create_ps					( ps_descriptor const & ps);

 	res_state*				create_state				( state_descriptor & descriptor);

	res_declaration*		create_declaration			( D3D_INPUT_ELEMENT_DESC const* dcl, u32 count);

	template< int size >
	inline res_declaration*	create_declaration			( D3D_INPUT_ELEMENT_DESC const (&dcl) [size])				{ return create_declaration( (D3D_INPUT_ELEMENT_DESC const*) &dcl, size);}

	res_signature*			create_signature			( ID3DBlob * signature);

	res_input_layout*		create_input_layout			( res_declaration const * decl, res_signature const * signature);

	res_texture_list*		create_texture_list			( texture_slots const & tex_list);

	res_sampler_list*		create_sampler_list			( sampler_slots const & tex_list);

	res_geometry*			create_geometry				( D3D_INPUT_ELEMENT_DESC const* decl, u32 decl_size, u32 vertex_stride, untyped_buffer& vb, untyped_buffer& ib);
	template< int size >
	res_geometry*			create_geometry 			( D3D_INPUT_ELEMENT_DESC const (&decl)[size], u32 vertex_stride, untyped_buffer& vb, untyped_buffer& ib);
	res_geometry*			create_geometry				( res_declaration * dcl, u32 vertex_stride, untyped_buffer& vb, untyped_buffer& ib);

	res_render_output*		create_render_output		( HWND window, bool windowed);
	
	ref_texture				get_color_grading_base_lut	();

	// Release functions
	void		release	( shader_constant_table const*	const_table);
	void		release	( shader_constant_buffer const*	buffer);
	void		release	( res_vs_hw const*			vs);
 	void		release	( res_gs_hw const*			vs);
 	void		release	( res_ps_hw const*			ps);
	void		release	( render_target const*		rt);
	void		release	( res_vs const*				vs);
	void		release	( res_gs const*				gs);
	void		release	( res_ps const*				ps);
	void		release	( res_state const*			state);
	void		release	( res_declaration const*	dcl);
	void		release	( res_signature const*		signature);
	void		release	( res_input_layout const*	dcl);
	void		release	( res_texture_list const*	tex_list);
	void		release	( res_sampler_list const*	tex_list);
	void		release	( res_texture const*		texture);
	void		release	( untyped_buffer const*		buffer);
	void		release	( res_geometry const*		geom);
	void		release	( res_render_output const*	render_output);

public: // Additional functionality
#if 0
	void					initialize_texture_storage	( xray::configs::lua_config_value const & cfg);
#endif

	void					copy		( untyped_buffer* dest, untyped_buffer* source);
	void					copy		( untyped_buffer* dest, u32 dest_pos, untyped_buffer* source, u32 src_pos, u32 size);
	void					copy2D		( res_texture* dest, u32 dest_x, u32 dest_y, res_texture* source, u32 src_x, u32 src_y, u32 size_x, u32 size_y, u32 dest_mip = 0, u32 src_mip = 0);
	void					copy3D		( res_texture* dest, u32 dest_x, u32 dest_y, u32 dest_z, res_texture* source, u32 src_x, u32 src_y, u32 src_z, u32 size_x, u32 size_y, u32 size_z, u32 dest_mip = 0, u32 src_mip = 0);

	void					set_compile_error_handler	( shader_compile_error_handler const & handler);
	bool					add_shader_code				( char const* shader_name, char const* shader_source_code, bool rewrite_exists = false);

	bool					copy_texture_from_file		( res_texture * dest_texture, math::rectangle<math::int2> dest_rect, u32 arr_ind, char const* src_name);


	pcstr					get_shader_path				( ) const;
	pcstr					get_converted_shader_path	( ) const;

public:
	typedef render::map<fs_new::virtual_path_string, shader_source>	map_shader_sources;
	
	void reload_shader_sources		(bool is_recompile_shaders = true);
	void reload_modified_textures	();
	
	void recompile_shaders( render::vector<pcstr> const& changed_global_defines_list);

private:
	
	void on_texture_source_changed  (xray::vfs::vfs_notification const & info);
	
	template<class shader_data> 
	void recompile_xs_shaders(render::vector<fs_new::virtual_path_string> const& shader_files);

	template<class shader_data>
	void recompile_xs_shaders( shader_defines_list const& changed_global_defines_list);

	
	void load_shader_sources		();
	void process_files				( vfs::vfs_iterator const & it);
	
	void on_raw_files_load			( resources::queries_result& data);
	void on_texture_loaded			( resources::queries_result& data, u32 mip_level_cut = 0, bool use_converter = true);
	void on_texture_loaded_staging	( resources::queries_result& data, u32 mip_level_cut, bool use_converter = true);
	void copy_texture_from_file_cb	( resources::queries_result& data);

	res_texture*	create_texture2d_impl	( u32 width, u32 height, D3D_SUBRESOURCE_DATA const* data, DXGI_FORMAT format, D3D_USAGE usage, u32 mip_levels, u32 array_size = 1);
	void			release_impl			( res_texture const* tex);

	template <typename shader_data>
	render::map<shader_name_config_pair, res_xs_hw<shader_data>* > & get_xs_container() {  }

	template <> map_vs_hw &	get_xs_container<vs_data>() { return m_vs_hw_registry;}
	template <> map_gs_hw &	get_xs_container<gs_data>() { return m_gs_hw_registry;}
	template <> map_ps_hw &	get_xs_container<ps_data>() { return m_ps_hw_registry;}
	


	template <typename shader_data>
	res_xs_hw<shader_data> *	create_xs_hw_impl ( const char * name, shader_configuration shader_config, shader_include_getter* include_getter = 0, binary_shader_sources_type* shader_sources = 0);

	template <typename shader_data>
	void						release_impl ( res_xs_hw<shader_data> const*	xs_hw);

public:

	template <typename T>
	static void call_resource_destructor( T* resource) { resource->~T();};
	
	fs_new::virtual_path_string get_full_shader_path(pcstr short_shader_path, enum_shader_type shader_type) const;
	resource_manager::shader_source& get_shader_source_by_short_name(pcstr shader_name, enum_shader_type shader_type, bool& out_is_found);
	resource_manager::shader_source& get_shader_source_by_full_name(pcstr shader_name, bool& out_is_found);
	map_shader_sources const& get_shader_sources() const { return m_sources; }

private:
	map_vs_hw									m_vs_hw_registry;
	map_gs_hw									m_gs_hw_registry;
	map_ps_hw									m_ps_hw_registry;
	map_rt										m_rt_registry;
	map_texture									m_texture_registry;

	map_shader_sources							m_sources;

	render::vector<shader_constant_table*>			m_const_tables;
	render::vector<shader_constant_buffer*>		m_const_buffers;
	render::vector<untyped_buffer*>					m_buffers;

	typedef render::vector<res_vs*> vs_cache;
	typedef render::vector<res_gs*> gs_cache;
	typedef render::vector<res_ps*> ps_cache;
	
	vs_cache									m_v_shaders;
	gs_cache									m_g_shaders;
	ps_cache									m_p_shaders;
	
	render::vector<res_state*>									m_states;

	state_cache<ID3DRasterizerState, D3D_RASTERIZER_DESC>		m_rs_cache;
	state_cache<ID3DDepthStencilState, D3D_DEPTH_STENCIL_DESC>	m_dss_cache;
	state_cache<ID3DBlendState, D3D_BLEND_DESC>					m_bs_cache;
	state_cache<ID3DSamplerState, D3D_SAMPLER_DESC>				m_sampler_cache;

	render::vector<res_declaration*>			m_declarations;
	render::vector<res_signature*>				m_signatures;
	render::vector<res_input_layout*>			m_input_layouts;
	render::vector<res_texture_list*>			m_texture_lists;
	render::vector<res_sampler_list*>			m_sampler_lists;
	render::vector<res_render_output*>			m_render_outputs;

	typedef std::pair< name_string_type, res_sampler_state *>	samplers_registry_record;
	typedef render::vector<samplers_registry_record>			samplers_registry;
	samplers_registry											m_samplers_registry;

	render::vector<u32>							m_vs_ids;
	render::vector<u32>							m_ps_ids;
	render::vector<u32>							m_gs_ids;

	typedef render::vector<res_geometry*>		vec_geoms;
	vec_geoms									m_geometries;

	
	map_shader_sources							m_previous_sources;
	
	render::vector<fs_new::virtual_path_string>	m_files_list;
	render::vector<u32>							m_file_times_list;

	bool										m_loading_incomplete;

	shader_constant_bindings					m_const_bindings;

	texture_storage*							m_texture_storage;
	texture_storage*							m_texture_storage_staging;
	
	shader_compile_error_handler				m_compile_error_handler;

	bool										m_is_shader_reloading;
	bool										m_need_recompile_shader_if_source_reloaded;
	
	typedef render::vector<fs_new::virtual_path_string>		textures_to_reload_vector;
	textures_to_reload_vector					m_textures_to_reload;

	ref_texture									m_color_grading_base_lut;
	u32											m_watcher_subscribe_id;
}; // class resource_manager

} // namespace render 
} // namespace xray 


#include "resource_manager_inline.h"

#endif // #ifndef RESOURCE_MANAGER_H_INCLUDED
