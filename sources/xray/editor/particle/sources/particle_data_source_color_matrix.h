////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_DATA_SOURCE_COLOR_MATRIX_H_INCLUDED
#define PARTICLE_DATA_SOURCE_COLOR_MATRIX_H_INCLUDED

#include "particle_data_source_base.h"

namespace xray {
	namespace particle_editor {

		public ref class particle_data_source_color_matrix : public particle_data_source_base {
		public:
			particle_data_source_color_matrix( )
			{
				name = "color_matrix";

			}
			particle_data_source_color_matrix ( System::String^ source_name, configs::lua_config_value source_config );

			[System::ComponentModel::DisplayNameAttribute("Data")]
			virtual property System::Object^ data
			{
				System::Object^ get( ) { return m_color_matrix_container; }
			}
			virtual property System::String^ name;

			virtual property on_data_source_data_changed^	data_source_data_changed;


		private:
			xray::editor::wpf_controls::color_matrix^						m_color_matrix_container;
			configs::lua_config_value*										m_source_config;


		private:
			void on_matrix_changed	( System::Object^ sender, System::EventArgs^ e );
		}; // class particle_data_source_float


	} // namespace particle_editor
} // namespace xray

#endif // #ifndef PARTICLE_DATA_SOURCE_COLOR_MATRIX_H_INCLUDED