#include "pch.h"
#include "object_properties.h"
#include "value_attribute.h"
#include "property_vec3f.h"
#include "property_color.h"

using System::Windows::Vector;
using System::ComponentModel::DisplayNameAttribute;
using System::ComponentModel::DefaultValueAttribute;
using xray::editor::wpf_controls::property_descriptor;
using xray::editor::wpf_controls::property_editors::attributes::value_range_and_format_attribute;

namespace xray {
namespace editor_base {

void object_properties::initialize_property_container( System::Object^ o, 
													  xray::editor::wpf_controls::property_container^ container )
{
	array<System::Reflection::PropertyInfo^>^	props	= o->GetType()->GetProperties();

//	for each( System::Reflection::PropertyInfo^ pi in props )
	for(int idx = props->Length-1; idx>=0; --idx)
	{
		PropertyInfo^ pi = props[idx];
		array<System::Object^>^ attribs			= pi->GetCustomAttributes(DisplayNameAttribute::typeid, true);
		if(attribs->Length==0)
			continue;			// not a browsable attribute



		System::Type^ t							= pi->PropertyType;
		if( t == System::String::typeid )
		{
			container->properties->add_from_obj( o, pi );
		}
		else if ( t == Float3::typeid )
		{
			float3 def_value					(0,0,0);
			attribs								= pi->GetCustomAttributes(ValueAttribute::typeid, true);
			for each (ValueAttribute^ att in attribs)
			{
				if(att->m_type==ValueAttribute::value_type::e_def_val)
				{
					def_value.x					= (float)att->m_params[0];
					def_value.y					= (float)att->m_params[1];
					def_value.z					= (float)att->m_params[2];
				}
			}

			property_descriptor^ desc	= container->properties->add( pi, gcnew property_vec3f( o, pi ) );
			desc->default_value			= gcnew Float3( def_value );
		}
		else if ( t == float2::typeid )
		{
			float2 def_value					(0,0);
			attribs								= pi->GetCustomAttributes(ValueAttribute::typeid, true);
			for each (ValueAttribute^ att in attribs)
			{
				if(att->m_type==ValueAttribute::value_type::e_def_val)
				{
					def_value.x					= (float)att->m_params[0];
					def_value.y					= (float)att->m_params[1];
				}
			}
			property_descriptor^ desc	= container->properties->add( pi, gcnew property_vec2f( o, pi ) );
			desc->default_value			= gcnew Vector( def_value.x, def_value.y );
		}
		else if( t == xrayColor::typeid )
		{
			System::Windows::Media::Color def_value = System::Windows::Media::Colors::White;
			attribs								= pi->GetCustomAttributes(ValueAttribute::typeid, true);
			for each (ValueAttribute^ att in attribs)
			{
				if(att->m_type==ValueAttribute::value_type::e_def_val)
				{
					def_value.ScR 				= (float)att->m_params[0];
					def_value.ScG 				= (float)att->m_params[1];
					def_value.ScB 				= (float)att->m_params[2];
					def_value.ScA 				= (float)att->m_params[3];
				}
			}

			property_descriptor^ desc	= container->properties->add( pi, gcnew property_color( o, pi ) );
			desc->default_value			= def_value;
		}
		else if( t == int::typeid )
		{
			container->properties->add_from_obj( o, pi );
		}
		else if( t == float::typeid )
		{
			container->properties->add_from_obj( o, pi );
		}
		else if( t == bool::typeid )
		{
			container->properties->add_from_obj( o, pi );
		}
		else
		{
			container->properties->add_from_obj( o, pi );
		}
		//			UNREACHABLE_CODE();
	} // cycle 
	container->owner = o;
} // void initialize_property_holder

} // namespace editor_base
} // namespace xray
