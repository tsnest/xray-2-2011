////////////////////////////////////////////////////////////////////////////
//	Created		: 25.03.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SINGLE_SOUND_WRAPPER_H_INCLUDED
#define SINGLE_SOUND_WRAPPER_H_INCLUDED

#include "sound_object_wrapper.h"
#include <xray/sound/single_sound.h>

using namespace System;
using namespace System::ComponentModel;
using namespace xray::sound;
using xray::editor::wpf_controls::float_curve;

namespace xray {
namespace sound_editor {

	ref class sound_object_wrapper;
	ref class wav_file_options;
	ref class sound_object_instance;

	public ref class single_sound_wrapper: public sound_object_wrapper
	{
		typedef System::Collections::Generic::List<sound_collection_item^> items_list;
	public:
							single_sound_wrapper	(String^ name);
							~single_sound_wrapper	();
		virtual void		apply_changes			(bool load_resources) override;
		virtual void		revert_changes			() override;
		virtual void		save					() override;
		virtual void		save					(xray::configs::lua_config_value& cfg) override;
		virtual void		load					(Action<sound_object_wrapper^>^ options_callback, Action<sound_object_wrapper^>^ sound_callback) override;
		virtual void		load					(xray::configs::lua_config_value const& cfg) override;
		virtual items_list^	items					() override {return nullptr;};
		virtual void		add_item				(sound_collection_item^ itm) override {XRAY_UNREFERENCED_PARAMETER(itm);};
		virtual void		remove_item				(sound_collection_item^ itm) override {XRAY_UNREFERENCED_PARAMETER(itm);};
		sound_rms_ptr		get_sound_rms			();
		sound_spl_ptr		get_sound_spl			();

		[CategoryAttribute("Single sound"), DisplayNameAttribute("wav_options"), DescriptionAttribute("wav options")]
		property wav_file_options^ wav_options
		{
			wav_file_options^ get() {return m_wav_options;};
			void set(wav_file_options^) {};
		}

		[CategoryAttribute("Single sound"), DisplayNameAttribute("spl_curve"), DescriptionAttribute("spl curve")]
		property float_curve^ spl_curve
		{
			float_curve^ get() {return m_spl_curve;};
			void set(float_curve^) {};
		}

	private:
		void				on_single_sound_loaded	(xray::resources::queries_result& data);
		void				on_wav_options_loaded	(xray::resources::queries_result& data);
		void				read_spl_curve			(xray::configs::lua_config_value const& cfg);

	private:
		float_curve^					m_spl_curve;
		wav_file_options^				m_wav_options;
	}; // class single_sound_wrapper
} // namespace sound_editor 
} // namespace xray
#endif // #ifndef SINGLE_SOUND_WRAPPER_H_INCLUDED