////////////////////////////////////////////////////////////////////////////
//	Created		: 03.02.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_DATA_SOURCE_COLOR_H_INCLUDED
#define PARTICLE_DATA_SOURCE_COLOR_H_INCLUDED

#include "particle_data_source_base.h"

namespace xray {
	namespace particle_editor {

	public ref class particle_data_source_color : public particle_data_source_base {
		public:
			particle_data_source_color( )
			{
				name = "float_curve";

			}
			particle_data_source_color( System::String^ source_name, configs::lua_config_value source_config );

			[System::ComponentModel::DisplayNameAttribute("Data")]
			virtual property System::Object^ data
			{
				System::Object^ get( ) { return m_curve_container; }
			}
			virtual property System::String^ name;

			virtual property on_data_source_data_changed^	data_source_data_changed;


		private:
			xray::editor::wpf_controls::color_curve^						m_curve_container;
			configs::lua_config_value*										m_source_config;


		private:
			void on_curve_changed	( System::Object^ sender, System::EventArgs^ e );
	}; // class particle_data_source_color



} // namespace particle_editor
} // namespace xray

#endif // #ifndef PARTICLE_DATA_SOURCE_COLOR_H_INCLUDED