////////////////////////////////////////////////////////////////////////////
//	Created		: 03.02.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "float_curve.h"
#include <xray/configs.h>
//#include <xray/editor/base/converting.h>

using xray::editor_base::float_curve;
using xray::editor_base::float_curve_pair;


float lin_interp_float(float t, float ta, float va, float tb, float vb)
{
	return va + (t-ta) * (vb-va) / (tb-ta);
}

float_curve::float_curve(xray::configs::lua_config_value const& curve_config)
:m_min_time(0.0f),
m_max_time(100.0f),
m_min_value(0.0f),
m_max_value(100.0f),
m_initialized_by_config(true)
{
	m_container = gcnew System::Collections::Generic::List<float_curve_pair^>;
	m_curve_config = NEW(xray::configs::lua_config_value)(curve_config);
	load_keys_from_config(curve_config);
	add_key(m_min_time, m_max_value/2);	
	add_key(m_max_time, m_max_value/2);
}

float_curve::float_curve()
:m_min_time(0.0f),
m_max_time(100.0f),
m_min_value(0.0f),
m_max_value(100.0f),
m_initialized_by_config(false),
m_curve_config(nullptr)
{
	m_container = gcnew System::Collections::Generic::List<float_curve_pair^>;
 	add_key(m_min_time, m_min_value);
 	add_key(20.f, 80.f);
 	add_key(60.f, 60.f);
 	add_key(80.f, 20.f);
 	add_key(m_max_time, m_max_value);
}

float_curve::float_curve(float set_min_time, float set_max_time, float set_min_value, float set_max_value):
	m_min_time(set_min_time),
	m_max_time(set_max_time),
	m_min_value(set_min_value),
	m_max_value(set_max_value),
	m_initialized_by_config(false),
	m_curve_config(nullptr)
{
	m_container = gcnew System::Collections::Generic::List<float_curve_pair^>;	
}

void float_curve::alter_key(float_curve_pair^ key, float time, float value)
{
	if(key!=m_container[0] && key!=m_container[m_container->Count-1])
		key->time		= xray::math::clamp_r(time, min_time, max_time);
		key->value			= xray::math::clamp_r(value, min_value, max_value);
	
	
		
	m_container->Sort	();
	
	key_altered(this, gcnew float_curve_event_args(key));
}

void float_curve::add_key(float time, float value)
{
	float_curve_pair^ key	= gcnew float_curve_pair(time, value);
	
	m_container->Add		(key);
	m_container->Sort		();

	key_added(this, gcnew float_curve_event_args(key));
}

void float_curve::load_keys_from_config(xray::configs::lua_config_value const& keys_config){
	configs::lua_config_value::iterator begin	= keys_config.begin();
	configs::lua_config_value::iterator end		= keys_config.end();

 	for(;begin != end;++begin){
  		float_curve_pair^ key	= gcnew float_curve_pair(float((*begin)["key"]), float((*begin)["value"]));
  
  		m_container->Add		(key);
  		m_container->Sort		();
 

 	}
}

void float_curve::save_keys_to_config		(xray::configs::lua_config_value& keys_config){
	Int32 i = 0;
	for each(float_curve_pair^ p in keys){
		xray::configs::lua_config_value key = keys_config[i];
		key["key"] = p->time;
		key["value"] = p->value;
		i++;
	}
}

void float_curve::remove_key(int idx)
{
	float_curve_pair^ key	= m_container[idx];
	m_container->RemoveAt	(idx);
	key_removed(this,  gcnew float_curve_event_args(key));
}

void float_curve::clear_keys()
{
	m_container->Clear();
	keys_cleared(this, EventArgs::Empty);
}

float float_curve::calc_value(float time)
{
	if (time <= min_time) 
		return m_container[0]->value;

	int last_key_idx = count - 1;
	
	if (time >= max_time) return m_container[last_key_idx]->value;

	for(int i=0; i<last_key_idx; ++i)
	{
		if (m_container[i]->time == time) return m_container[i]->value;
		if (m_container[i]->time <= time && m_container[i+1]->time > time) 
		{
			float		val;
			float ka	= m_container[i]->time;
			float kb	= m_container[i+1]->time;
			float va	= m_container[i]->value;
			float vb	= m_container[i+1]->value;

			val			= lin_interp_float(time, ka, va, kb, vb);
			return val;
		}
	}
	return m_container[last_key_idx]->value;
}

void float_curve::synchronize_config(){
	if (!m_initialized_by_config)
		return;
	m_curve_config->clear	( );
	save_keys_to_config(*m_curve_config);
}

void		float_curve::save(xray::configs::lua_config_value& config){
 	config["min_time"]		= m_min_time;
 	config["max_time"]		= m_max_time;
 	config["min_value"]		= m_min_value;
 	config["max_value"]		= m_max_value;
	
	xray::configs::lua_config_value keys_config = config["keys"];
	
	save_keys_to_config		(keys_config);
	
}

void		float_curve::load(xray::configs::lua_config_value& config){
 	m_min_time		= config["min_time"];		
 	m_max_time		= config["max_time"];		
 	m_min_value		= config["min_value"];
 	m_max_value		= config["max_value"];		
	xray::configs::lua_config_value keys = config["keys"];
	load_keys_from_config(keys);

}

void		float_curve::draw(System::Drawing::Graphics^ g, System::Drawing::Pen^ pen, System::Drawing::Drawing2D::Matrix^ matrix)
{
 	//Draw lines
 	cli::array<System::Drawing::PointF>^	lines = gcnew cli::array<System::Drawing::PointF>(keys->Count );
 	for(int idx=0; idx<count; ++idx)
 	{
 		float_curve_pair^ pair	= key[idx];
 		PointF	pt				= PointF(pair->time, pair->value);
 		lines[idx]				= pt; 		
 	}
 	matrix->TransformPoints(lines);
  
 	g->DrawLines		(pen, lines);

}

