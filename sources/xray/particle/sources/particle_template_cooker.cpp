////////////////////////////////////////////////////////////////////////////
//	Created		: 26.07.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_template_cooker.h"
#include "particle_system.h"
#include "particle_emitter.h"
#include "particle_actions.h"
#include "stream.h"
#include <xray/resources_unmanaged_allocator.h>
#include <xray/type_variant.h>

#include <xray/particle/world.h>

namespace xray {
	
	using namespace resources;

namespace particle {

particle_template_cooker::particle_template_cooker(xray::particle::world& world) 
	: super(resources::particle_system_class, reuse_true, use_resource_manager_thread_id),
	m_world(world)
{
}

void particle_template_cooker::translate_query( resources::query_result& parent )
{
	//fixed_string<260> new_name	= "resources/particles/";
	//new_name					+= parent.get_requested_path();
	//new_name+=".particle";
	//query_resource( new_name.c_str(), 
	//	resources::config_lua_class, 
	//	boost::bind( &particle_template_cooker::on_sub_resources_loaded, this, _1), 
	//	xray::resources::unmanaged_allocator(),
	//	NULL, 
	//	0, 
	//	&parent 
	//);
	
	fs::path_string		old_path			(parent.get_requested_path());
	fs::path_string		root_path			=  "resources/particles";
	fs::path_string		root_path_conv		=  "resources.converted/particles";
	particle_template_cooker_data * data	=  UNMANAGED_NEW(data, particle_template_cooker_data);
	
	data->resource_path.append			(old_path.c_str());
	data->query_result					= &parent;
	
	resources::query_fs_iterator(
		root_path.c_str(), 
		boost::bind(&particle_template_cooker::on_fs_iterator_ready_source, this, data, _1),
		xray::resources::unmanaged_allocator()
		);
	
	resources::query_fs_iterator(
		root_path_conv.c_str(), 
		boost::bind(&particle_template_cooker::on_fs_iterator_ready_converted, this, data, _1),
		xray::resources::unmanaged_allocator()
		);
}

void particle_template_cooker::on_fs_iterator_ready_source( particle_template_cooker_data* data, xray::resources::fs_iterator fs_it )
{
	fs::path_string source_path;
	source_path.append						(data->resource_path.begin(), data->resource_path.end());
	source_path.append						(".particle");
	fs_iterator fs_it_source			=	fs_it.find_child(source_path.c_str());
	
	fs::path_info		source_info;
	fs::path_string		source_absolute_path;
	
	if (fs_it_source.is_end())
		data->source_info_time = 0;
	else
	{
		fs_it_source.get_disk_path		(source_absolute_path);
		fs::get_path_info				(&source_info, source_absolute_path.c_str());
		data->source_info_time			= source_info.file_last_modify_time;
	}
	
	data->source_ready				= true;
	if( data->converted_ready)
		on_fs_iterators_ready( data);
}

void particle_template_cooker::on_fs_iterator_ready_converted( particle_template_cooker_data* data, xray::resources::fs_iterator fs_it )
{
	fs::path_string converted_path;
	converted_path.append			(data->resource_path.c_str());
	converted_path.append			(".binary");
	
	xray::resources::fs_iterator fs_it_converted		= fs_it.find_child(converted_path.c_str());
	
	if(fs_it_converted.is_end())
	{
		request_convertion				(&data->resource_path, data->query_result);
		UNMANAGED_DELETE(data);
		return;
	}
	
	fs::path_info		converted_info;
	fs::path_string		converted_absolute_path;
	
	fs_it_converted.get_disk_path	(converted_absolute_path);
	fs::get_path_info				(&converted_info, converted_absolute_path.c_str());
	
	data->converted_info_time = converted_info.file_last_modify_time;
	data->converted_ready = true;
	if( data->source_ready)
		on_fs_iterators_ready( data);
}

void particle_template_cooker::on_fs_iterators_ready( particle_template_cooker_data* data )
{
	if(data->source_info_time > data->converted_info_time)
	{
		request_convertion	(&data->resource_path, data->query_result);
		UNMANAGED_DELETE	(data);
		return;
	}
	
	fs::path_string converted_res_path;
	converted_res_path.append("resources.converted/particles/");
	converted_res_path.append(data->query_result->get_requested_path());
	converted_res_path.append(".binary");
	
	fs::path_string						destination_path;

	if (!data->query_result->convert_logical_to_disk_path(&destination_path, converted_res_path.c_str()))
	{
		data->query_result->finish_query			(result_error);
		return;
	}

	FILE* file;
	if(!fs::open_file(&file, fs::open_file_read, destination_path.c_str()))
		return;
	
	fseek(file,0,SEEK_END);
	u32 binary_size = ftell(file);
	fseek(file,0,SEEK_SET);
	
	u8* in_buffer				= UNMANAGED_ALLOC(u8, binary_size);
	fread						( in_buffer, 1, binary_size, file );
	fclose						( file );
	
	particle_system* ps			= new(in_buffer) particle_system;
	in_buffer+=sizeof(particle_system);

	xray::particle::stream load_st(in_buffer, true);
	ps->load_binary(load_st);
	
	resources::query_result_for_cook* parent	= data->query_result;
	parent->set_unmanaged_resource				( ps, resources::memory_type_non_cacheable_resource, sizeof(particle_system) );
	parent->finish_query						( result_success );
}

void particle_template_cooker::request_convertion(fs::path_string* resource_path, query_result_for_cook* parent)
{
	fixed_string<260> new_name	= "resources/particles/";
	new_name					+= *resource_path;
	new_name					+= ".particle";

	query_resource( new_name.c_str(), 
		resources::config_lua_class, 
		boost::bind( &particle_template_cooker::on_lua_config_loaded, this, _1), 
		xray::resources::unmanaged_allocator(),
		NULL, 
		parent 
		);
}


struct stream_calc_size: public stream
{
	stream_calc_size():
		stream(0, false),
		m_size(0)
	{}
private:
	virtual stream& sync_data(void*, u32 const data_size)
	{
		m_size+=data_size;
		return *this;
	}
	virtual void operator += (u32 size) 
	{ 
		m_size+=size;
	}
public:
	u32 get_size() const {return m_size;}
private:
	u32 m_size;
};


void particle_template_cooker::on_sub_resources_loaded( resources::queries_result& /*data*/ )
{
//	R_ASSERT									( data.is_successful() );
//	
//	configs::lua_config_ptr config_ptr			= static_cast_checked<configs::lua_config *>( data[0].get_unmanaged_resource().c_ptr() );
//	configs::lua_config_value const & config	= config_ptr->get_root();
//
//#if 1
//	FILE* file;
//	if(!fs::open_file(&file, fs::open_file_read, "F:/1.binary"))
//		return;
//	fseek(file,0,SEEK_END);
//	u32 binary_size = ftell(file);
//	fseek(file,0,SEEK_SET);
//	
//	u8* in_buffer				= UNMANAGED_ALLOC(u8, binary_size);
//	fread							( in_buffer, 1, binary_size, file );
//	fclose							( file );
//	
//	particle_system* ps			= UNMANAGED_NEW(ps, particle_system);
//	
//	xray::particle::stream load_st(in_buffer, true);
//	ps->load_binary(load_st);
//	
//	resources::query_result_for_cook* parent	= data.get_parent_query();
//	parent->set_unmanaged_resource				( ps, resources::memory_type_non_cacheable_resource, sizeof(particle_system) );
//	parent->finish_query						( result_success );
//	
//#else
//
//	particle_system* created_resource			= UNMANAGED_NEW(created_resource, particle_system);
//	parse_root(created_resource, config["root"]);
//	resources::query_result_for_cook* parent	= data.get_parent_query();
//	parent->set_unmanaged_resource				( created_resource, resources::memory_type_non_cacheable_resource, sizeof(particle_system) );
//	parent->finish_query						( result_success );
//	
//	stream_calc_size calc_st;
//	created_resource->save_binary(calc_st);
//	
//	u32 binary_size = calc_st.get_size();
//	u8* out_buffer	= UNMANAGED_ALLOC(u8, binary_size);
//	
//	xray::particle::stream save_st(out_buffer, false);
//	created_resource->save_binary(save_st);
//	
//	fs::path_string					destination_path = "F:/1.binary";
//	
//	FILE* file;
//	if(!fs::open_file(&file, fs::open_file_create | fs::open_file_truncate | fs::open_file_write, destination_path.c_str()))
//		return;
//	
//	fwrite							( out_buffer, 1, binary_size, file );
//	fclose							( file );
//#endif
}

void particle_template_cooker::on_lua_config_loaded(resources::queries_result& result)
{
	resources::query_result_for_cook * const	parent	=	result.get_parent_query();
	
	if (!result.is_successful())
	{
		parent->finish_query			(result_error);
		return;
	}
	
	configs::lua_config_ptr config_ptr			= static_cast_checked<configs::lua_config *>( result[0].get_unmanaged_resource().c_ptr() );
	configs::lua_config_value const & config	= config_ptr->get_root();
	particle_system temp_resource;
	parse_root(&temp_resource, config);
	
	
	stream_calc_size calc_st;
	temp_resource.save_binary(calc_st);
	
	
	u32 binary_size = sizeof(particle_system) + calc_st.get_size();
	
	u8* out_buffer	= UNMANAGED_ALLOC(u8, binary_size);
	u8* start_out_buffer = out_buffer;
	
	xray::memory::copy(out_buffer,sizeof(particle_system),&temp_resource,sizeof(particle_system));
	out_buffer += sizeof(particle_system);
	
	u8* right_out_buffer = out_buffer;
	
	xray::particle::stream save_st(out_buffer, false);
	temp_resource.save_binary(save_st);
	
	fs::path_string converted_res_path;
	converted_res_path.append("resources.converted/particles/");
	converted_res_path.append(parent->get_requested_path());
	converted_res_path.append(".binary");
	
	fs::path_string						destination_path;
	
	if (!result.get_parent_query()->convert_logical_to_disk_path(&destination_path, converted_res_path.c_str()))
	{
		parent->finish_query			(result_error);
		return;
	}
	
	fs::path_string dir				= destination_path;
	dir.set_length					(dir.rfind('/'));
	
	if(fs::get_path_info(NULL, dir.c_str()) == fs::path_info::type_nothing)
	{
		fs::make_dir_r(dir.c_str());
	}
	
	FILE* file;
	if(!fs::open_file(&file, fs::open_file_create | fs::open_file_truncate | fs::open_file_write, destination_path.c_str()))
		return;
	
	fwrite							( start_out_buffer, 1, binary_size, file );
	fclose							( file );
	
	particle_system* new_resource	= new (start_out_buffer) particle_system;
	xray::particle::stream load_st(right_out_buffer, true);
	new_resource->load_binary(load_st);
	
	parent->set_unmanaged_resource				( new_resource, resources::memory_type_non_cacheable_resource, sizeof(particle_system) );
	parent->finish_query						( result_success );
}

void particle_template_cooker::delete_resource(resources::unmanaged_resource* res)
{
	particle_system* ps = static_cast_checked<particle_system*>(res);

	res->~unmanaged_resource();
	
	UNMANAGED_FREE(ps);
}
/*
static bool is_table_named(configs::lua_config_value const& table)
{
	return table.get_type()==configs::t_table_named;
}

static particle_emitter::enum_screen_alignment screen_alignment_name_to_type(fixed_string<260> const& name)
{
	if (name=="TO CAMERA")		return particle_emitter::to_camera;
	else if (name=="TO PATH")	return particle_emitter::to_path;
	else if (name=="TO WORLD")	return particle_emitter::to_world;

	return particle_emitter::to_camera;
}
static particle_emitter::enum_locked_axis locked_axis_name_to_type(fixed_string<260> const& name)
{
	if (name=="X")		return particle_emitter::x_axis;
	else if (name=="Y") return particle_emitter::y_axis;
	else if (name=="Z") return particle_emitter::z_axis;

	return particle_emitter::none_axis;
}
static particle_emitter::enum_sub_uv_method sub_uv_method_name_to_type(fixed_string<260> const& name)
{
	// SMOTH ?
	if (name=="LINEAR")				return particle_emitter::linear;
	else if (name=="LINEAR_SMOTH")	return particle_emitter::linear_smooth;
	else if (name=="RANDOM")		return particle_emitter::random;
	else if (name=="RANDOM_SMOTH")	return particle_emitter::random_smooth;

	return particle_emitter::linear;
}
*/
static particle_action* construct_action(particle_emitter* emitter, configs::lua_config_value action_config)
{
	fixed_string<260> type_name =  (pcstr)action_config["name"];
	
	particle_action* particle_act = 0;
	
	if (type_name=="InitDomain")
	{
		configs::lua_config_value prop_config = action_config["properties"];
		particle_act = UNMANAGED_NEW(particle_act, particle_action_source);
		particle_act->set_defaults();
		particle_act->load(prop_config);
		emitter->set_source_action( (particle_action_source*)particle_act );
	}
	else if (type_name=="InitColor")
	{
		configs::lua_config_value prop_config = action_config["properties"];
		particle_act = UNMANAGED_NEW(particle_act, particle_action_initial_color);
		particle_act->set_defaults();
		particle_act->load(prop_config);
	}
	else if (type_name=="InitSize")
	{
		configs::lua_config_value prop_config = action_config["properties"];
		particle_act = UNMANAGED_NEW(particle_act, particle_action_initial_size);
		particle_act->set_defaults();
		particle_act->load(prop_config);
	}
	else if (type_name=="InitVelocity")
	{
		configs::lua_config_value prop_config = action_config["properties"];
		particle_act = UNMANAGED_NEW(particle_act, particle_action_initial_velocity);
		particle_act->set_defaults();
		particle_act->load(prop_config);
	}
	else if (type_name=="InitRotation")
	{
		configs::lua_config_value prop_config = action_config["properties"];
		particle_act = UNMANAGED_NEW(particle_act, particle_action_initial_rotation);
		particle_act->set_defaults();
		particle_act->load(prop_config);
	}
	else if (type_name=="VelocityLife")
	{
		configs::lua_config_value prop_config = action_config["properties"];
		particle_act = UNMANAGED_NEW(particle_act, particle_action_velocity_over_lifetime);
		particle_act->set_defaults();
		particle_act->load(prop_config);
	}
	else if (type_name=="Size/Velocity")
	{
		configs::lua_config_value prop_config = action_config["properties"];
		particle_act = UNMANAGED_NEW(particle_act, particle_action_size_over_velocity);
		particle_act->set_defaults();
		particle_act->load(prop_config);
	}
	else if (type_name=="Size/Life")
	{
		configs::lua_config_value prop_config = action_config["properties"];
		particle_act = UNMANAGED_NEW(particle_act, particle_action_size_over_lifetime);
		particle_act->set_defaults();
		particle_act->load(prop_config);
	}
	else if (type_name=="Billboard") 
	{
		emitter->m_particle_type = particle_emitter::billboard;
		configs::lua_config_value prop_config = action_config["properties"];
		
		// Problems with default values reading.
		/*
		emitter->m_use_sub_uv			= (bool)prop_config["UseSubUV"];
		emitter->m_sub_image_horizontal = (bool)prop_config["SubImgHorizontal"];
		emitter->m_sub_image_vertical	= (bool)prop_config["SubImgVertical"];
		emitter->m_sub_image_change		= (bool)prop_config["SubImgChange"];
		emitter->m_screen_alignment		= screen_alignment_name_to_type((pcstr)prop_config["ScreenAlignment"]);
		emitter->m_locked_axis			= locked_axis_name_to_type((pcstr)prop_config["LockAxisFlags"])
		emitter->m_sub_uv_method		= sub_uv_method_name_to_type((pcstr)prop_config["Method"])
		*/
	}
	else if (type_name=="Mesh")		 emitter->m_particle_type = particle_emitter::mesh;
	else if (type_name=="Trail")	 emitter->m_particle_type = particle_emitter::trail;
	else if (type_name=="Beam")		 emitter->m_particle_type = particle_emitter::beam;
	else if (type_name=="Decal")	 emitter->m_particle_type = particle_emitter::decal;
	else if (type_name=="onDeath")	 emitter->m_event_flags |= particle_emitter::on_death;
	else if (type_name=="onBirth")	 emitter->m_event_flags |= particle_emitter::on_birth;
	else if (type_name=="onPlay")	 emitter->m_event_flags |= particle_emitter::on_play;
	else if (type_name=="onCollide") emitter->m_event_flags |= particle_emitter::on_collide;
	
	return particle_act;
}

particle_action* particle_template_cooker::parse_action( particle_emitter*, configs::lua_config_value const& )
{
	return 0;
}

void particle_template_cooker::parse_emitter_type( particle_emitter* emitter, configs::lua_config_value const& emitter_config )
{
	fixed_string<260> type_name =  (pcstr)(emitter_config)["name"];
	if (type_name=="Billboard")
	{
		emitter->m_particle_type = particle_emitter::billboard;
	}
	if (type_name=="Mesh")
	{
		emitter->m_particle_type = particle_emitter::mesh;
	}
	if (type_name=="Trail")
	{
		emitter->m_particle_type = particle_emitter::trail;
	}
}

void particle_template_cooker::parse_emitter( particle_emitter* emitter, configs::lua_config_value const& emitter_config )
{
	configs::lua_config_value const& props_config = emitter_config["properties"];
	emitter->load(props_config);
	
	configs::lua_config_value const& nodes_config = emitter_config["children"];
	
	emitter->m_actions		= 0;
	emitter->m_num_actions	= 0;
	emitter->m_burst_list	= 0;
	
	for ( configs::lua_config_value::const_iterator it  = nodes_config.begin();	it != nodes_config.end(); ++it)
	{
		particle_action* new_action = construct_action( emitter, *it );
		
		if (!new_action) continue;
		emitter->add_action(new_action);
		emitter->m_num_actions++;
	}
	
	particle_action* test_action = UNMANAGED_NEW(test_action, particle_action_lifetime);
	
	test_action->set_defaults();
	emitter->add_action( test_action );
	emitter->m_num_actions++;
}

void particle_template_cooker::parse_group( particle_system_lod& ps_lod, configs::lua_config_value const& group_config )
{
	//configs::lua_config_value const& props_config = group_config["properties"];
	//float group_delay	  = (float)props_config["Delay"];
	//float group_lifefime  = (float)props_config["LifeTime"];
	
	ps_lod.m_num_emitters = 0;
	
	for ( configs::lua_config_value::const_iterator it = group_config.begin(); it != group_config.end(); ++it )
		ps_lod.m_num_emitters++;
	
	ps_lod.m_emitters_array = UNMANAGED_ALLOC(particle_emitter, ps_lod.m_num_emitters);
	
	u32 emitter_index = 0;
	for ( configs::lua_config_value::const_iterator it = group_config.begin(); it != group_config.end(); ++it )
	{
		ps_lod.m_emitters_array[emitter_index].set_defaults();
		parse_emitter( &(ps_lod.m_emitters_array[emitter_index]), *it );
		emitter_index++;
	}
	
	//configs::lua_config_value const& emitters_config = group_config["children"];
	//
	//for ( configs::lua_config_value::const_iterator it  = emitters_config.begin(); 
	//	it != emitters_config.end(); 
	//	++it)
	//{
	//	particle_emitter* emitter			= UNMANAGED_NEW(emitter, particle_emitter);
	//	ps->add_emitter(emitter);
	//	parse_emitter( emitter, *it );
	//	
	//	emitter->m_delay = group_delay;
	//	emitter->m_lifefime = group_lifefime;
	//}
}

void particle_template_cooker::parse_root( particle_system* ps, configs::lua_config_value const& config )
{
	u32 num_lods = 1;
	u32 lod_index = 0;
	
	ps->m_num_lods = num_lods;
	
	ps->m_lods = UNMANAGED_ALLOC(particle_system_lod, num_lods);
	
	// TODO: Iterate all lods.
	
	configs::lua_config_value const& lod_config = config["LOD 0"];
	
	ps->m_lods[lod_index].m_parent = ps;
	
	// Fix default values reading.
	// ps->m_lods[lod_index].m_total_length = (float)lod_config["TotalLength"];
	
	configs::lua_config_value const& groups_and_emitters_config = lod_config["children"];
	
	ps->m_lods[lod_index].m_num_emitters = 0;
	
	for ( configs::lua_config_value::const_iterator it = groups_and_emitters_config.begin(); it != groups_and_emitters_config.end(); ++it )
	{
		fixed_string<260> type_name = (pcstr)(*it)["name"];
		if (type_name=="PEmitter") 
			ps->m_lods[lod_index].m_num_emitters++;
	}
	ps->m_lods[lod_index].m_emitters_array = UNMANAGED_ALLOC(particle_emitter, ps->m_lods[lod_index].m_num_emitters);
	
	u32 emitter_index = 0;
	for ( configs::lua_config_value::const_iterator it = groups_and_emitters_config.begin(); it != groups_and_emitters_config.end(); ++it )
	{
		fixed_string<260> type_name = (pcstr)(*it)["name"];
		
		if (type_name=="PEmitter") 
		{
			ps->m_lods[lod_index].m_emitters_array[emitter_index].set_defaults();
			
			parse_emitter( &(ps->m_lods[lod_index].m_emitters_array[emitter_index]), *it );
			emitter_index++;
		}
		else if (type_name=="PGroup")   
			parse_group( ps->m_lods[lod_index], *it );
	}
}

static xray::uninitialized_reference<particle_template_cooker>	s_particle_template_cook_object;

void register_particles_template_cooker( xray::particle::world& world )
{
	XRAY_CONSTRUCT_REFERENCE	( s_particle_template_cook_object, particle_template_cooker ) ( world );
	resources::register_cook	( s_particle_template_cook_object.c_ptr() );
}

void unregister_particles_template_cooker()
{
	resources::unregister_cook	( resources::particle_system_class );
	XRAY_DESTROY_REFERENCE		( s_particle_template_cook_object );
}

} // namespace particle
} // namespace xray