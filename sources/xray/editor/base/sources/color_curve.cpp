////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "color_curve.h"
#include <xray/configs.h>
//#include <xray/editor/base/converting.h>

using xray::editor_base::color_curve;
using xray::editor_base::color_curve_pair;
using System::Drawing::Color;


Color lin_interp_color(float t, float ta, Color va, float tb, Color vb)
{
	float _a  = va.A + (t-ta) * (vb.A-va.A) / (tb-ta);
	float _r  = va.R + (t-ta) * (vb.R-va.R) / (tb-ta);
	float _g  = va.G + (t-ta) * (vb.G-va.G) / (tb-ta);
	float _b  = va.B + (t-ta) * (vb.B-va.B) / (tb-ta);
	return Color::FromArgb(	System::Convert::ToInt32(_a), 
		System::Convert::ToInt32(_r), 
		System::Convert::ToInt32(_g), 
		System::Convert::ToInt32(_b));
}

Color clamp_color(Color v, Color min, Color max)
{
	int _a, _r, _g, _b;
	if(v.A<min.A) _a=min.A;	
	else
		if(v.A>max.A) _a=max.A;
		else _a = v.A;

		if(v.R<min.R) _r=min.R;	
		else
			if(v.R>max.R) _r=max.R;
			else _r = v.R;

			if(v.G<min.G) _g=min.G;	
			else
				if(v.G>max.G) _g=max.G;
				else _g = v.G;

				if(v.B<min.B) _b=min.B;	
				else
					if(v.B>max.B) _b=max.B;
					else _b = v.B;

					return Color::FromArgb(_a, _r, _g, _b);
}

color_curve::color_curve()
:m_min_time(0.0f),
m_max_time(100.0f),
m_curve_config(nullptr),
m_initialized_by_config(false)
{
	m_min_value	= Color::FromArgb(255,0,0,0);
	m_max_value	= Color::FromArgb(255,255,255,255);

	m_container = gcnew System::Collections::Generic::List<color_curve_pair^>;
	add_key(m_min_time, Color::Red);
	add_key(20.f, Color::White);
	add_key(70.f, Color::Green);
	//	add_key(75.f, Color::Blue);
	add_key(m_max_time, Color::Red);
}


color_curve::color_curve(xray::configs::lua_config_value curve_config)
:m_min_time(0.0f),
m_max_time(100.0f),
m_initialized_by_config(true)
{
	m_container = gcnew System::Collections::Generic::List<color_curve_pair^>;
	//	m_curve_config = NEW(xray::configs::lua_config_value)(curve_config);
	m_curve_config = new xray::configs::lua_config_value(curve_config);
	m_min_value	= Color::FromArgb(255,0,0,0);
	m_max_value	= Color::FromArgb(255,255,255,255);
	add_key(m_min_time, Color::Black);
	add_key(m_max_time, Color::White);
	load_keys_from_config(curve_config);
}

void color_curve::alter_key(color_curve_pair^ i_key, float time, Color value)
{
	color_curve_pair^ key = safe_cast<color_curve_pair^>(i_key);
	//if(key!=m_container[0] && key!=m_container[m_container->Count-1])
	key->time		= xray::math::clamp_r(time, min_time, max_time);

	key->value			= value; //clamp_color(value, min_value, max_value);
	m_container->Sort	();
	on_curve_changed();
}

color_curve_pair^ color_curve::add_key(float time, Color value)
{
	color_curve_pair^ key = gcnew color_curve_pair(time, value);
	m_container->Add	(key);
	m_container->Sort	();
	on_curve_changed();
	return key;
}

void color_curve::remove_key(int idx)
{
	m_container->RemoveAt(idx);
	on_curve_changed();
}

void color_curve::remove_key(color_curve_pair^ pair)
{
	color_curve_pair^ key = safe_cast<color_curve_pair^>(pair);
	m_container->Remove(key);
	on_curve_changed();
}

Color color_curve::calc_value(float time)
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
			Color		val;
			float ka	= m_container[i]->time;
			float kb	= m_container[i+1]->time;
			Color va	= m_container[i]->value;
			Color vb	= m_container[i+1]->value;

			val			= lin_interp_color(time, ka, va, kb, vb);
			return		val;
		}
	}
	return m_container[last_key_idx]->value;
}

void		color_curve::save(xray::configs::lua_config_value config){
	config["min_time"]		= m_min_time;
	config["max_time"]		= m_max_time;
	config["min_value"]		= m_min_value.ToArgb();
	config["max_value"]		= m_max_value.ToArgb();

	xray::configs::lua_config_value keys_config = config["keys"];

	save_keys_to_config		(keys_config);

}

void		color_curve::load(xray::configs::lua_config_value config){
	m_min_time		= config["min_time"];		
	m_max_time		= config["max_time"];		
	m_min_value		= System::Drawing::Color::FromArgb((int)config["min_value"]);
	m_max_value		= System::Drawing::Color::FromArgb((int)config["max_value"]);		

	load_keys_from_config(config["keys"]);

}

void color_curve::load_keys_from_config(xray::configs::lua_config_value keys_config){
	configs::lua_config_value::iterator begin	= keys_config.begin();
	configs::lua_config_value::iterator end		= keys_config.end();

	for(;begin != end;++begin){
		color_curve_pair^ key	= gcnew color_curve_pair(float((*begin)["key"]), System::Drawing::Color::FromArgb((int)((*begin)["value"])));

		m_container->Add		(key);
		m_container->Sort		();


	}
}

void color_curve::save_keys_to_config		(xray::configs::lua_config_value keys_config){
	Int32 i = 0;
	for each(color_curve_pair^ p in m_container){
		xray::configs::lua_config_value key = keys_config[i];
		key["key"] = p->time;
		key["value"] = p->value.ToArgb();
		i++;
	}
}

void color_curve::synchronize_config(){
	if (!m_initialized_by_config)
		return;

	m_curve_config->clear	( );
	save_keys_to_config(*m_curve_config);
	on_curve_changed();
}


void		color_curve::draw(System::Drawing::Graphics^ g, System::Drawing::Rectangle rect)
{
	//Draw lines
	float sx	= rect.Width / (max_time-min_time);

	//Draw pre gradient
	float w				=	(key[0]->time-min_time)*sx;
	RectangleF gr_rect		(min_time*sx, (float)rect.Location.Y, w, System::Convert::ToSingle(rect.Height));
	if(!(gr_rect.Width == 0 || gr_rect.Height == 0))
	{
		Drawing::SolidBrush^ brush	= gcnew Drawing::SolidBrush(key[0]->value);
		g->FillRectangle	(brush, gr_rect);
	}

	//Draw Gradient
	for(int idx = 1; idx<count; ++idx)
	{
		color_curve_pair^ pair		= key[idx];
		color_curve_pair^ prev_pair	= key[idx-1];
		float w						= (pair->time-prev_pair->time)*sx;
		RectangleF gr_rect			(prev_pair->time*sx, (float)rect.Location.Y, w, System::Convert::ToSingle(rect.Height));
		if(gr_rect.Width == 0 || gr_rect.Height == 0)
			continue;
		Drawing2D::LinearGradientBrush^ brush	= gcnew Drawing2D::LinearGradientBrush(gr_rect, prev_pair->value, pair->value, System::Drawing::Drawing2D::LinearGradientMode::Horizontal);
		g->FillRectangle	(brush, gr_rect);
	}

	//Draw post gradient
	w				=	(max_time - key[count-1]->time)*sx;
	gr_rect			=	RectangleF(key[count-1]->time*sx, (float)rect.Location.Y, w, System::Convert::ToSingle(rect.Height));
	if(!(gr_rect.Width == 0 || gr_rect.Height == 0))
	{
		Drawing::SolidBrush^ brush	= gcnew Drawing::SolidBrush(key[count-1]->value);
		g->FillRectangle	(brush, gr_rect);
	}
}

