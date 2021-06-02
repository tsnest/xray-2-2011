////////////////////////////////////////////////////////////////////////////
//	Created		: 19.08.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef CHANNELS_DOCUMENT_H_INCLUDED
#define CHANNELS_DOCUMENT_H_INCLUDED

using namespace System;
using namespace System::Windows::Forms;

namespace xray {
namespace animation {
	class skeleton_animation;
	struct world;
}
namespace animation_editor {
	ref class channels_editor;

	public ref class channels_document : public xray::editor::controls::document_base
	{
		enum class enum_channel_id
		{
			channel_translation_x = 0,
			channel_translation_y,
			channel_translation_z,
			channel_rotation_x,
			channel_rotation_y,
			channel_rotation_z,
			channel_scale_x,
			channel_scale_y,
			channel_scale_z,
		};

		typedef xray::editor::controls::document_base super;
		typedef System::Collections::Generic::KeyValuePair<float, float> time_value_pair;
		typedef System::Collections::Generic::List<time_value_pair> channel_data;
		typedef System::Collections::Generic::Dictionary<enum_channel_id, channel_data^> bone_data;
		typedef System::Collections::Generic::Dictionary<System::String^, bone_data^> animation_data;

	public:
							channels_document			(channels_editor^ ed);
							~channels_document			();

		virtual	void		save						() override;
		virtual	void		load						() override;
		virtual	void		undo						() override;
		virtual	void		redo						() override;
		virtual	void		copy						(bool del) override;
		virtual	void		paste						() override;
		virtual	void		select_all					() override;
		virtual	void		del							() override;

	private:
				void		on_resources_loaded			(xray::resources::queries_result& result);
				void		bones_selection_changed		(System::Object^ obj, System::EventArgs^ e);
				void		channels_selection_changed	(System::Object^ obj, System::EventArgs^ e);

	private:
		System::ComponentModel::Container^	components;
		channels_editor^					m_editor;
		animation_data^						m_data;
		animation_data^						m_check_data;
		System::Windows::Forms::Label^		m_channels_label;
		System::Windows::Forms::ListBox^	m_bones_list_box;
		System::Windows::Forms::ListBox^	m_channels_list_box;
		static bool							m_allocator_inited = false;
	}; // ref class channels_document
} // namespace animation_editor
} // namespace xray
#endif // #ifndef CHANNELS_DOCUMENT_H_INCLUDED