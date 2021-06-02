////////////////////////////////////////////////////////////////////////////
//	Created		: 29.10.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_VIEWER_CHANGE_PROPERTY_H_INCLUDED
#define ANIMATION_VIEWER_CHANGE_PROPERTY_H_INCLUDED

namespace xray {
namespace animation_editor {
	ref class animation_viewer_hypergraph;
	ref class animation_viewer_graph_node_base;
	ref class animation_viewer_graph_link;
	ref class animation_viewer_graph_link_weighted;

	public ref class animation_viewer_change_property : public xray::editor_base::command
	{
		typedef xray::editor::wpf_controls::property_container wpf_property_container;
		typedef System::Collections::Generic::Dictionary<u32, float> source_id_to_koef_type;
		typedef System::Collections::Generic::List<animation_viewer_graph_link^> links;
	public:
						animation_viewer_change_property	(animation_viewer_hypergraph^ h, wpf_property_container^ pc, System::String^ property_name, System::Object^ new_v, System::Object^ old_v);
		virtual			~animation_viewer_change_property	() {};

		virtual bool	commit								() override;
		virtual void	rollback							() override;

	private:
				bool	set_value							(System::Object^ value);
				void	init_weight_values					(animation_viewer_graph_link_weighted^ lnk);

	private:
		animation_viewer_hypergraph^ 	m_hypergraph;
		u32 							m_src_id;
		u32 							m_dst_id;
		System::Object^ 				m_old_value;
		System::Object^ 				m_new_value;
		System::String^ 				m_changed_property_name;
		bool							m_parent_is_anim_clip;
		source_id_to_koef_type^			m_new_values;
		source_id_to_koef_type^			m_old_values;
	}; // class animation_viewer_change_property
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_VIEWER_CHANGE_PROPERTY_H_INCLUDED