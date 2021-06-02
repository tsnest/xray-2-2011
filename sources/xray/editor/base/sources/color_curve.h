////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef COLOR_CURVE_H_INCLUDED
#define COLOR_CURVE_H_INCLUDED

namespace xray {
namespace editor_base {

	ref class color_curve_type_editor;
	using namespace System;
	using namespace System::Drawing;
	

	public ref struct color_curve_pair: public System::IComparable
	{
		color_curve_pair(float t, Color v):time(t), value(v){}
		float time;
		Color value;

		virtual property Double offset
		{
			Double	get() 					{return time;}
			void	set(Double value)		{time = (Single)value;}
		}
		virtual property Color color
		{
			Color	get()					{return value;}
			void	set(Color color_value)	{value = color_value;}
		}

		virtual int CompareTo( Object^ obj ) 
		{
			color_curve_pair^ temp = safe_cast<color_curve_pair^>(obj);

			return time.CompareTo(temp->time);
		}
	};

	public delegate void color_curve_changed();

	[System::ComponentModel::EditorAttribute(color_curve_type_editor::typeid, System::Drawing::Design::UITypeEditor::typeid)]
	public ref class color_curve
	{
		Color						m_min_value;
		Color						m_max_value;
		float						m_min_time;
		float						m_max_time;
		bool						m_initialized_by_config;
		configs::lua_config_value*	m_curve_config;

		System::Collections::Generic::List<color_curve_pair^>^	m_container;
	public:
		color_curve		();
		color_curve		(xray::configs::lua_config_value curve_config);
		virtual property Color min_value{
			Color	get()			{return m_min_value;}
			void	set(Color v)	{m_min_value=v;}
		}
		virtual property Color max_value{
			Color	get()			{return m_max_value;}
			void	set(Color v)	{m_max_value=v;}
		}
		virtual property float min_time{
			float	get()			{return m_min_time;}
			void	set(float v)	{m_min_time=v; if(m_min_time>m_max_time) m_min_time=m_max_time;}
		}
		virtual property float max_time{
			float	get()			{return m_max_time;}
			void	set(float v)	{m_max_time=v; if(m_min_time>m_max_time) m_max_time=m_min_time;}
		}
		property configs::lua_config_value*	curve_config{
			configs::lua_config_value*		get(){return m_curve_config;}
		}

		color_curve_pair^			add_key					(float time, Color value);
		virtual void				alter_key				(color_curve_pair^ key, float time, Color value);
		void						remove_key				(int idx);
		virtual void				remove_key				(color_curve_pair^ pair);
		Color						calc_value				(float time);
		void						save					(configs::lua_config_value config);
		void						load					(configs::lua_config_value config);
		void						load_keys_from_config	(configs::lua_config_value keys_config);
		void						save_keys_to_config		(configs::lua_config_value keys_config);
		virtual void				synchronize_config		( );

		void						draw					(Drawing::Graphics^ g, Drawing::Rectangle rect);

		virtual property int count{
			int			get()	{return m_container->Count;}
		}

		virtual	color_curve_pair^ pair(int index)
		{
			return m_container[index];
		}

		property color_curve_pair^ key [int]{
			color_curve_pair^	get (int idx)	{return m_container[idx];}
		}

	public:
		event color_curve_changed^ on_curve_changed;
	};
} // namespace editor_base
} // namespace xray
#endif // #ifndef COLOR_CURVE_H_INCLUDED