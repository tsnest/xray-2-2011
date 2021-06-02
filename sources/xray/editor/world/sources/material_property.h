////////////////////////////////////////////////////////////////////////////
//	Created		: 20.10.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MATERIAL_PROPERTY_H_INCLUDED
#define MATERIAL_PROPERTY_H_INCLUDED

#include "material_property_base.h"
#include "material_property_read_writers.h"
#include "material_editor.h"

namespace xray 
{
	namespace editor 
	{
		template<typename TType>
		ref class material_property_converted : material_property_base
		{
		protected:
			

		public:
			material_property_converted (	System::String^ n, 
											System::String^ value_path,  
											const configs::lua_config_value& config_value, 
											material_property_read_writer<TType>^ read_writer )
			{
				m_path_steps		= value_path->Split('/');
				name				= n;
				m_config			= NEW(configs::lua_config_value)(config_value);
				m_read_writer		= read_writer;
				inner_value			= m_read_writer->read( this->config_value( ) );
			}

			material_property_converted (	material_effect^ effect, 
											System::String^ n, 
											System::String^ value_path,  
											const configs::lua_config_value& config_value, 
											material_property_read_writer<TType>^ read_writer )
			{
				m_parent			= effect;
				m_path_steps		= value_path->Split('/');
				name				= n;
				m_config			= NEW(configs::lua_config_value)(config_value);
				m_read_writer		= read_writer;
				inner_value			= m_read_writer->read( this->config_value( ) );
			}

			virtual ~material_property_converted ()
			{
				DELETE( m_config );
			}

		protected:
			configs::lua_config_value*					m_config;
			array<System::String^>^						m_path_steps;
			material_property_read_writer<TType>^		m_read_writer;

		protected:
			configs::lua_config_value config_value( )
			{
				configs::lua_config_value return_value = *m_config;

				if( m_path_steps == nullptr || ( m_path_steps->Length == 1 && m_path_steps[0] == "" ))
					return return_value;

				for each( System::String^ path_step in m_path_steps )
					return_value = return_value[unmanaged_string( path_step ).c_str( )];

				return return_value;
			}

		private:
			bool			check_dependant_properties_depedecies( )
			{
				bool ret_val = false;
				
				for each( material_property_base^ prop in dependant_properties )
					ret_val		= prop->check_dependencies( ) || ret_val;

				return			ret_val;
			}

		public:
			virtual configs::lua_config_value*	get_config			( ) override { return m_config; }
			virtual				Object^			get_value			( ) override
			{
				return inner_value;
			}
			virtual				void			set_value			( Object^ value ) override
			{
				inner_value = value;
				m_read_writer->write( config_value( ), (TType)value );
				
				if( check_dependant_properties_depedecies( ) )
				{
					m_parent->m_parent_stage->m_parent->properties_changed( );
				}

				on_value_changed ( );
			}
			virtual property	System::Type^			value_type
			{
				System::Type^ get( ) override {return TType::typeid;}
			}
			virtual				Object^			get_value			( configs::lua_config_value config ) override
			{
				return m_read_writer->read( config );
			}
			virtual				bool			match_to			( configs::lua_config_value config ) override
			{
				return safe_cast<TType>( inner_value ) == m_read_writer->read( config );
			}
			virtual				bool			match_to			( Object^ value ) override
			{
				bool is_match = safe_cast<TType>( inner_value ) ==  safe_cast<TType>( value );
				return is_match;
			}
			virtual				bool			check_dependencies	( ) override
			{
				bool new_visible	= true;
				for each( KeyValuePair<material_property_base^, Object^>^ entry in dependency_end_points )
					new_visible = new_visible && entry->Key->visible && entry->Key->match_to( entry->Value );

				bool ret_val		= (visible != new_visible);
				set_is_visible		( new_visible );
				check_dependant_properties_depedecies( );
				return				ret_val;
			}
			virtual					void			set_is_visible	( bool new_visible ) override
			{
				bool is_different = visible != new_visible;
				visible = new_visible;
				if(is_different)
				{
					/*if( new_visible )
						parent->effect_value[ unmanaged_string( property_name ).c_str() ].assign_lua_value( *m_config_value );
					else
						parent->effect_value.erase( unmanaged_string( property_name ).c_str() );*/
				}
			}

		}; // ref class material_property_converted

		template<typename TType>
		ref class material_property : material_property_converted<TType>
		{
		public:
			material_property ( System::String^ property_name, System::String^ value_path,  const configs::lua_config_value& config_value )
				:material_property_converted( property_name, value_path, config_value, gcnew material_property_value_read_writer<TType>() )
			{	
			}
			material_property ( material_effect^ effect, System::String^ property_name, System::String^ value_path,  const configs::lua_config_value& config_value )
				:material_property_converted( effect, property_name, value_path, config_value, gcnew material_property_value_read_writer<TType>() )
			{	
			}
		};// ref class material_property

	} // namespace editor
} // namespace xray

#endif // #ifndef MATERIAL_PROPERTY_H_INCLUDED