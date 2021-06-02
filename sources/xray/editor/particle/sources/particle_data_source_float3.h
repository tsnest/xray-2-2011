////////////////////////////////////////////////////////////////////////////
//	Created		: 03.02.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_DATA_SOURCE_FLOAT3_H_INCLUDED
#define PARTICLE_DATA_SOURCE_FLOAT3_H_INCLUDED

#include "particle_data_source_base.h"

namespace xray {
	namespace particle_editor {

		public ref class particle_data_source_float3 : public particle_data_source_base {
		public:
			particle_data_source_float3( )
			{
				name = "float_curve";
			}
			particle_data_source_float3( System::String^ source_name, configs::lua_config_value source_config );

			[System::ComponentModel::DisplayNameAttribute("Data")]
			virtual property System::Object^ data
			{
				System::Object^ get( ) { return m_curves; }
			}
			virtual property System::String^ name;
			
			virtual property on_data_source_data_changed^	data_source_data_changed;

		private:
			System::Collections::Generic::List<editor::wpf_controls::float_curve^>^ m_curves;
		
			void on_data_changed( configs::lua_config_value data_changes_config ) { data_source_data_changed( data_changes_config );}

		}; // class particle_data_source_float


	} // namespace particle_editor
} // namespace xray

#endif // #ifndef PARTICLE_DATA_SOURCE_FLOAT3_H_INCLUDED