////////////////////////////////////////////////////////////////////////////
//	Created		: 25.03.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "single_sound_wrapper.h"
#include "sound_editor.h"
#include "wav_file_options.h"
#include "sound_object_instance.h"
#include <xray/sound/sound_emitter.h>
#include <xray/sound/wav_utils.h>
#include <xray/sound/spl_utils.h>
#include <xray/fs_utils.h>

using xray::sound_editor::single_sound_wrapper;
using xray::editor::wpf_controls::float_curve_key;
using xray::editor::wpf_controls::float_curve_key_type;
using xray::editor::wpf_controls::float_curve_monotone_type;
using xray::sound_editor::sound_object_instance;

single_sound_wrapper::single_sound_wrapper(String^ name)
:sound_object_wrapper(name)
{
	m_spl_curve = gcnew float_curve();
	m_spl_curve->name = m_name;
	m_spl_curve->monotone_type = float_curve_monotone_type::down;
}

single_sound_wrapper::~single_sound_wrapper()
{
	delete m_spl_curve;
	delete m_wav_options;
}

sound_rms_ptr single_sound_wrapper::get_sound_rms()
{
	return (static_cast_resource_ptr<sound::single_sound_ptr>(*m_sound))->get_sound_rms();
}

sound_spl_ptr single_sound_wrapper::get_sound_spl()
{
	return (static_cast_resource_ptr<sound::single_sound_ptr>(*m_sound))->get_sound_spl();
}

void single_sound_wrapper::save(xray::configs::lua_config_value& root)
{
	typedef List<float_curve_key^> keys_list;
	keys_list^ curve_keys = m_spl_curve->keys;
	xray::math::float_curve c;
	c.reserve(curve_keys->Count);
	xray::fixed_string<8> key_name;
	for(int i=0; i<curve_keys->Count; ++i)
	{
		float_curve_key^ k = curve_keys[i];
		key_name.assignf("key%d", i);
		pcstr str = key_name.c_str();
		root[str]["key_type"] = static_cast<u8>(k->key_type);
		root[str]["position"] = xray::math::float2((float)k->position_x, (float)k->position_y);
		root[str]["left_tangent"] = xray::math::float2((float)k->left_tangent.X,(float)k->left_tangent.Y);
		root[str]["right_tangent"] = xray::math::float2((float)k->right_tangent.X,(float)k->right_tangent.Y);
	}
}

void single_sound_wrapper::save()
{
	m_wav_options->save(m_name);
	String^ file_path = gcnew String(sound_editor::sound_resources_path + "single/" + m_name + ".single_sound_options");
	unmanaged_string str(file_path);
	configs::lua_config_ptr const& config = configs::create_lua_config(str.c_str());
	configs::lua_config_value root = config->get_root()["spl"];
	save(root);
	config->save(configs::target_sources);
	if(m_sound->c_ptr())
		get_sound_spl()->get_curve()->load(root);
}

void single_sound_wrapper::load(xray::configs::lua_config_value const& cfg)
{
	XRAY_UNREFERENCED_PARAMETER(cfg);
}

void single_sound_wrapper::load(Action<sound_object_wrapper^>^ options_callback, Action<sound_object_wrapper^>^ sound_callback)
{
	m_sound_loaded_callback = sound_callback;
	m_options_loaded_callback = options_callback;
	String^ raw_path = gcnew String(sound_editor::sound_resources_path + "single/" + m_name + ".raw_options");
	String^ ss_path = gcnew String(sound_editor::sound_resources_path + "single/" + m_name + ".single_sound_options");
	fs::path_string source_wav_path;
	source_wav_path.assignf			(	"%ssingle/%s.wav",
										unmanaged_string( sound_editor::sound_resources_path ).c_str(),
										unmanaged_string( m_name ).c_str()
									);

	fs_new::physical_path_info source_wav_info = 
		resources::get_physical_path_info( source_wav_path.c_str(), resources::sources_mount );

	if (source_wav_info.is_file())
	{
		m_is_exist				= false;
		return;
	}
	else
		m_is_exist				= true;

	unmanaged_string raw_options_path(raw_path);
	unmanaged_string ss_options_path(ss_path);
	resources::request request[] = { 
		raw_options_path.c_str(), resources::lua_config_class,
		ss_options_path.c_str(), resources::lua_config_class
	};
	query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &single_sound_wrapper::on_wav_options_loaded), g_allocator);
	query_resources(request, 2, boost::bind(&query_result_delegate::callback, q, _1), g_allocator);
}

void single_sound_wrapper::on_single_sound_loaded(xray::resources::queries_result& data)
{
	ASSERT(data.is_successful());
	(*m_sound) = static_cast_resource_ptr<sound::sound_emitter_ptr>(data[0].get_unmanaged_resource());
	m_is_loaded = true;
	if(m_sound_loaded_callback)
		m_sound_loaded_callback(this);
}

void single_sound_wrapper::on_wav_options_loaded(xray::resources::queries_result& data)
{
	configs::lua_config_ptr wav_cfg;
	configs::lua_config_ptr spl_cfg;

	if(data[0].is_successful())
		wav_cfg = static_cast_resource_ptr<configs::lua_config_ptr>(data[0].get_unmanaged_resource());
	else
	{
		pcstr requested_path = data[0].get_primary_requested_path();
		xray::fs_new::native_path_string wav_path;
		bool const convertion_result	=	xray::resources::convert_virtual_to_physical_path(&wav_path, requested_path, resources::sources_mount);
		R_ASSERT							(convertion_result);
		xray::fs_new::set_extension_for_path(&wav_path, "wav");
		xray::fs_new::synchronous_device_interface const & device	=	xray::resources::get_synchronous_device();
		wav_cfg = xray::sound::wav_utils::create_default_config(wav_path, device);
	}

	if(data[1].is_successful())
		spl_cfg = static_cast_resource_ptr<configs::lua_config_ptr>(data[1].get_unmanaged_resource());
	else
		spl_cfg = xray::sound::create_default_spl_config(data[1].get_primary_requested_path(), g_allocator);

	m_wav_options = gcnew wav_file_options(wav_cfg);
	read_spl_curve(spl_cfg->get_root()["spl"]);
	if(m_options_loaded_callback)
		m_options_loaded_callback(this);

	math::float4x4 matrix = math::float4x4().identity();
	math::float4x4 const* matrix_pointers[] = {&matrix};
	unmanaged_string wav_file_name(m_name);
	resources::request request = { 
		wav_file_name.c_str(), resources::single_sound_class 
	};

	query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &single_sound_wrapper::on_single_sound_loaded), g_allocator);
	resources::query_resource_params params(&request, NULL, 1, 
		boost::bind(&query_result_delegate::callback, q, _1),
		g_allocator, NULL, matrix_pointers
	);

	query_resources(params);
}

void single_sound_wrapper::read_spl_curve(xray::configs::lua_config_value const& cfg)
{
	m_spl_curve->keys->Clear();
	xray::math::float_curve curve = xray::math::float_curve();
	curve.load(cfg);
	for(u32 i=0; i<curve.get_num_points(); ++i)
	{
		float_curve_key^ key = gcnew float_curve_key();
		xray::math::curve_point<float> pt;
		curve.get_point(i, pt);
		if(i)
			key->key_type = static_cast<float_curve_key_type>(pt.interp_type);
		else
			key->key_type = float_curve_key_type::step;

		key->position_x = pt.time;
		key->position_y = pt.upper_value;
		if(math::is_zero(pt.tangent_in))
		{
			key->left_tangent.X = 0.0f;
			key->left_tangent.Y = 0.0f;
		}
		else
		{
			math::float2 tg = float2(1.0f, pt.tangent_in);
			tg = float2(pt.time - tg[0], pt.upper_value - tg[1]);
			key->left_tangent = System::Windows::Point(tg[0], tg[1]);
		}

		if(math::is_zero(pt.tangent_out))
		{
			key->right_tangent.X = 0.0f;
			key->right_tangent.Y = 0.0f;
		}
		else
		{
			math::float2 tg = float2(1.0f, pt.tangent_out);
			tg = float2(pt.time, pt.upper_value) + tg;
			key->right_tangent = System::Windows::Point(tg[0], tg[1]);
		}

		if(i==0 || i==(curve.get_num_points()-1))
			key->key_type = float_curve_key_type::breaked;
		else
			key->key_type = float_curve_key_type::locked_auto;

		m_spl_curve->keys->Add(key);
	}
}

void single_sound_wrapper::apply_changes(bool load_resources)
{
	save();
	if (load_resources)
		load(nullptr, nullptr);
}

void single_sound_wrapper::revert_changes()
{
	load(nullptr, nullptr);
}