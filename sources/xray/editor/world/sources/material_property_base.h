////////////////////////////////////////////////////////////////////////////
//	Created		: 25.10.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MATERIAL_PROPERTY_BASE_H_INCLUDED
#define MATERIAL_PROPERTY_BASE_H_INCLUDED

using namespace System::Collections::Generic;
using xray::editor::wpf_controls::i_property_value;

namespace xray 
{
	namespace editor 
	{
		ref class material_effect;

		ref class material_property_base abstract : i_property_value
		{
		public:
			event System::Action<material_property_base^>^ value_changed;

		public:
			material_property_base ( )
			{
				dependant_properties	= gcnew List<material_property_base^>();
				dependency_end_points	= gcnew Dictionary<material_property_base^, Object^>();
				property_attributes		= gcnew List<System::Attribute^>( );
				visible					= true;
			}
			
			virtual ~material_property_base(){}

		protected:
			 material_effect^	m_parent;

		public:
			Dictionary<material_property_base^, Object^>^		dependency_end_points;

		public:
			property			material_effect^				parent
			{
				material_effect^	get(){ return m_parent; }
			}
			property			System::String^					name;
			property			System::String^					show_id;
			property			bool							visible;
			property			List<material_property_base^>^	dependant_properties;
			property			List<System::Attribute^>^		property_attributes;
			property			Object^							inner_value;
			property			System::String^					subtype;
			property			int								group_id;
			virtual property	System::Type^					value_type 			
			{
				System::Type^ get() abstract; 
			}

		protected:
			void				on_value_changed					( )
			{
				value_changed( this );
			}

		public:
			virtual				Object^			get_value			( ) abstract;
			virtual				void			set_value			( Object^ value ) abstract;

			virtual				bool			match_to			( configs::lua_config_value config ) = 0;
			virtual				bool			match_to			( Object^ value ) = 0;
			virtual				Object^			get_value			( configs::lua_config_value config ) = 0;
			virtual				bool			check_dependencies	( )						= 0;
			virtual				void			set_is_visible		( bool new_visible )	= 0;
			virtual configs::lua_config_value*	get_config			( )						= 0;

		};// ref class material_property_base
	} // namespace editor
} // namespace xray

#endif // #ifndef MATERIAL_PROPERTY_BASE_H_INCLUDED