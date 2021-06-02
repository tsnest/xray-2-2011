////////////////////////////////////////////////////////////////////////////
//	Created		: 08.09.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_VIEWER_GRAPH_NODE_ANIMATION_H_INCLUDED
#define ANIMATION_VIEWER_GRAPH_NODE_ANIMATION_H_INCLUDED

#include "animation_viewer_graph_node_base.h"
#include <xray/animation/skeleton_animation.h>

using namespace System::Collections::Generic;
using namespace xray::editor::wpf_controls::property_editors;

namespace xray {
namespace animation {
	namespace mixing { class animation_clip;}
	struct base_interpolator;
}
namespace animation_editor {

	ref class animation_viewer_graph_node_base;
	ref class animation_node_clip_instance;
	ref class animation_node_playing_instance;
	ref class animation_node_interval;
	ref class animation_ui_type_lexeme_editor;
	ref class animation_viewer_hypergraph;
	value class intervals_request;

	public ref class animation_viewer_graph_node_animation: public animation_viewer_graph_node_base
	{
		typedef animation_viewer_graph_node_base						super;
		typedef xray::editor::wpf_controls::property_container			wpf_property_container;
		typedef List<animation_node_interval^>							intervals_list;
		typedef Dictionary<String^, List<intervals_request>^ >			intervals_request_type;
		typedef List<animation_node_clip_instance^>						instances_list;
	public:
									animation_viewer_graph_node_animation	() {initialize();};
		virtual						~animation_viewer_graph_node_animation	();
		virtual String^				get_text								() override;
		virtual void				save									(xray::configs::lua_config_value cfg) override;
		virtual void				load									(xray::configs::lua_config_value const& cfg, animation_viewer_hypergraph^ h) override;
		virtual void				on_added								(xray::editor::controls::hypergraph::hypergraph_control^ parent) override;
		virtual void				draw_caption							(System::Windows::Forms::PaintEventArgs^ e) override;
				String^				name									() { return m_name; }

				void				init_properties							(animation_viewer_graph_node_animation^ new_sync_node);
				bool				synchronized_with_node					(animation_viewer_graph_node_animation^ start_node);
				bool				additive								();
				u32					synchronization_group					();
				void				clear_intervals							();
				void				add_interval							(animation_node_clip_instance^ inst, u32 interval_index);
				u32					lexeme_length_in_frames					();

				wpf_property_container^	container							() {return m_container;};
				xray::animation::skeleton_animation_ptr clip				();

		[CategoryAttribute("Node properties"), DisplayNameAttribute("animation_clip")/*, ReadOnlyAttribute(true)*/]
		property wpf_property_container^	animation_clip
		{
			wpf_property_container^	get										();
			void					set										(wpf_property_container^) {};
		};

		[CategoryAttribute("Node properties"), DisplayNameAttribute("playing_instance")]
		property wpf_property_container^	playing_instance
		{
			wpf_property_container^	get										();
			void					set										(wpf_property_container^) {};
		};

		[CategoryAttribute("Node properties"), DisplayNameAttribute("intervals"), attributes::external_editor_attribute(animation_ui_type_lexeme_editor::typeid)]
		property intervals_list^	intervals
		{
			intervals_list^			get										() {return m_intervals;};
			void					set										(intervals_list^) {};
		};

		property animation_viewer_graph_node_animation^	sync_node;
		property u32									cycle_from_interval_id;

	protected:
		virtual void				initialize								() override;
		virtual void				on_double_click							(System::Object^, System::EventArgs^) override;

	private:
				void				request_animation						();
				void				on_animation_loaded						(animation_node_clip_instance^ new_clip);

	private:
		wpf_property_container^				m_container;
		intervals_list^						m_intervals;
		System::String^						m_name;
		intervals_request_type^				m_request;
		animation_node_playing_instance^	m_playing_instance;
		instances_list^						m_clips;
	}; // class animation_viewer_graph_node_animation

	public ref class animation_ui_type_lexeme_editor : public i_external_property_editor
	{
	public:
		virtual	void				run_editor	( property^ prop );
	}; // ref class animation_lexeme_editor
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_VIEWER_GRAPH_NODE_ANIMATION_H_INCLUDED