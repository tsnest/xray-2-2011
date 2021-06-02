////////////////////////////////////////////////////////////////////////////
//	Created		: 08.04.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_GROUP_OBJECT_H_INCLUDED
#define ANIMATION_GROUP_OBJECT_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections::Generic;
using namespace System::Collections::ObjectModel;
using namespace System::Collections::Specialized;
using namespace xray::editor::wpf_controls::hypergraph;

namespace xray {
namespace animation_editor {
	
	ref class animation_node_interval;
	ref class animation_node_interval_instance;
	ref class animation_node_clip_instance;
	ref class animation_groups_document;
	value class intervals_request;

	public enum class animation_groups_gait
	{	
		stand		= 0, 
		walk,
		walk_fast, 
		run, 
		walk_jump,
		take
	};

	public enum class animation_groups_flag
	{	
		idle		= 1 << 0,
		take		= 1 << 1,
		strafe		= 1 << 2,
		step_left	= 1 << 3,
		step_right	= 1 << 4
	};

	public ref class animation_group_object
	{
		typedef ObservableCollection<animation_node_interval_instance^> intervals_list;
		typedef Dictionary<String^, List<intervals_request>^ > intervals_request_type;
	public:
						animation_group_object		(animation_groups_document^ d);
						~animation_group_object		();
		void			save						(xray::configs::lua_config_value& cfg);
		void			save_group_settings			(xray::configs::lua_config_value& cfg);
		void			load						(xray::configs::lua_config_value const& cfg);
		void			load_group_settings			(xray::configs::lua_config_value const& cfg);
		void			refresh_view				();
		void			interval_property_changed	(animation_node_interval_instance^ interval, System::String^ property_name, System::Object^ new_val, System::Object^ old_val);
animation_node_interval^	get_interval			(u32 index);
		
		[CategoryAttribute("Group properties"), DisplayNameAttribute("id"), node_property_attribute(false, false), BrowsableAttribute(false)]
		property Guid id
		{
			Guid		get() {return m_id;};
			void		set(Guid new_val) {m_id = new_val;};
		};
		[CategoryAttribute("Group properties"), DisplayNameAttribute("name"), node_property_attribute(false, false)]
		property String^ name
		{
			String^		get() {return m_name;};
			void		set(String^ new_val);
		};
		[CategoryAttribute("Group properties"), DisplayNameAttribute("path"), node_property_attribute(false, false), ReadOnlyAttribute(true)]
		property String^ path
		{
			String^		get() {return m_path;};
			void		set(String^ new_val) {m_path = source_path+new_val+extension;};
		};
		[CategoryAttribute("Group properties"), DisplayNameAttribute("position"), node_property_attribute(false, false), node_sub_properties_attribute(false, false), ReadOnlyAttribute(true), BrowsableAttribute(false)]
		property System::Windows::Point position
		{
			System::Windows::Point	get();
			void					set(System::Windows::Point new_val);
		};
		[CategoryAttribute("Group properties"), DefaultValueAttribute(animation_groups_gait::stand), DisplayNameAttribute("gait"), node_property_attribute(false, false)]
		property animation_groups_gait gait
		{
			animation_groups_gait	get() {return m_gait;};
			void					set(animation_groups_gait new_val);
		};
		[CategoryAttribute("Group properties"), DefaultValueAttribute(animation_groups_flag::idle), DisplayNameAttribute("flag"), node_property_attribute(false, false)]
		property animation_groups_flag flag
		{
			animation_groups_flag	get() {return m_flag;};
			void					set(animation_groups_flag new_val);
		};
		[CategoryAttribute("Group properties"), DefaultValueAttribute(0.0f), DisplayNameAttribute("mix_time"), node_property_attribute(false, false)]
		property float mix_time
		{
			float					get() {return m_mix_time;};
			void					set(float new_val);
		};
		[CategoryAttribute("Group properties"), DisplayNameAttribute("intervals"), node_property_attribute(false, false), node_sub_properties_attribute(false, false)]
		property intervals_list^ intervals
		{
			intervals_list^	get() {return m_intervals;};
			void set(intervals_list^) {};
		};
		[CategoryAttribute("Group properties"), DisplayNameAttribute("view"), BrowsableAttribute(false)]
		property properties_node^ view
		{
			properties_node^		get() {return m_view;};
			void					set(properties_node^ n);
		};

		static property String^ source_path
		{
			String^	get( );
		};
		static property String^ extension
		{
			String^	get() {return ".lua";};
		};

	private:
		void			on_animation_loaded			(xray::animation_editor::animation_node_clip_instance^ new_clip);
		void			view_position_changed		(System::Object^, PropertyChangedEventArgs^ e);
		void			intervals_collection_changed(System::Object^, NotifyCollectionChangedEventArgs^ e);
		
	private:
		animation_groups_document^	m_parent;
		Guid						m_id;
		String^						m_name;
		String^						m_path;
		animation_groups_gait		m_gait;
		animation_groups_flag		m_flag;
		float						m_mix_time;
		System::Windows::Point		m_position;
		intervals_list^				m_intervals;
		properties_node^			m_view;
		intervals_request_type^		m_request;
		bool						m_is_expanded;
	}; // class animation_group_object
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_GROUP_OBJECT_H_INCLUDED