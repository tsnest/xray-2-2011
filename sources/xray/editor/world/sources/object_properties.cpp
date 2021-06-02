#include "pch.h"
#include "object_base.h"
#include "property_holder.h"
#include "float_curve.h"

namespace xray {
namespace editor {

void initialize_property_holder(object_base^ o)
{
	property_holder* holder			= o->get_property_holder();
	System::Type^ t					= o->GetType();
	array<System::Reflection::PropertyInfo^>^	props;
	props							= t->GetProperties();
	
	System::String^					display_name;
	System::String^					description;
	System::String^					category;
	property_holder::readonly_enum	readonly;

	for(int p=0; p<props->Length; ++p)
	{
		System::Reflection::PropertyInfo^ pi	= props[p];

		array<System::Object^>^ attribs			= pi->GetCustomAttributes(DisplayNameAttribute::typeid, true);
		if(attribs->Length==0)	continue; // not a browsable attribute

		DisplayNameAttribute^ att_name			= safe_cast<DisplayNameAttribute^>(attribs[0]);
		display_name							= att_name->DisplayName;
		
		attribs									= pi->GetCustomAttributes(DescriptionAttribute::typeid, true);
		DescriptionAttribute^ att_desc			= safe_cast<DescriptionAttribute^>(attribs[0]);
		description								= att_desc->Description;
		
		attribs									= pi->GetCustomAttributes(CategoryAttribute::typeid, true);
		CategoryAttribute^ att_category			= safe_cast<CategoryAttribute^>(attribs[0]);
		category								= att_category->Category;

		attribs									= pi->GetCustomAttributes(ReadOnlyAttribute::typeid, true);
		
		ReadOnlyAttribute^ att_readonly			= safe_cast<ReadOnlyAttribute^>(attribs[0]);
		readonly								= att_readonly->IsReadOnly ? property_holder::property_read_only : property_holder::property_read_write;

		System::Type^ t							= pi->PropertyType;
		//if(t==System::String::typeid)
		//{
		//	attribs								= pi->GetCustomAttributes(ValueAttribute::typeid, true);
		//	ValueAttribute^ att_def_value		= safe_cast<ValueAttribute^>(attribs[0]);
		//	System::String^ def_value			= safe_cast<System::String^>(att_def_value->m_params[0]);

		//	holder->add_string(
		//		display_name, 
		//		category, 
		//		description, 
		//		def_value,
		//		safe_cast<string_getter_type^>(System::Delegate::CreateDelegate(string_getter_type::typeid, o, pi->GetGetMethod())),
		//		safe_cast<string_setter_type^>(System::Delegate::CreateDelegate(string_setter_type::typeid, o, pi->GetSetMethod())),
		//		readonly,
		//		property_holder::do_not_notify_parent_on_change,
		//		property_holder::no_password_char,
		//		property_holder::do_not_refresh_grid_on_change
		//	);
		//}else
		if(t==float3::typeid)
		{
			float3 def_value					(0,0,0);
			attribs								= pi->GetCustomAttributes(ValueAttribute::typeid, true);
			for each (ValueAttribute^ att in attribs)
			{
				if(att->m_type==e_def_val)
				{
					def_value.x					= (float)att->m_params[0];
					def_value.y					= (float)att->m_params[1];
					def_value.z					= (float)att->m_params[2];
				}
			}
			holder->add_vec3f(
				display_name, 
				category, 
				description, 
				def_value,
				safe_cast<float3_getter_type^>(System::Delegate::CreateDelegate(float3_getter_type::typeid, o, pi->GetGetMethod())),
				safe_cast<float3_setter_type^>(System::Delegate::CreateDelegate(float3_setter_type::typeid, o, pi->GetSetMethod())),
				readonly,
				property_holder::do_not_notify_parent_on_change,
				property_holder::no_password_char,
				property_holder::do_not_refresh_grid_on_change
			);
		}else
		if(t==float2::typeid)
		{
			float2 def_value					(0,0);
			attribs								= pi->GetCustomAttributes(ValueAttribute::typeid, true);
			for each (ValueAttribute^ att in attribs)
			{
				if(att->m_type==e_def_val)
				{
					def_value.x					= (float)att->m_params[0];
					def_value.y					= (float)att->m_params[1];
				}
			}
			holder->add_vec2f(
				display_name, 
				category, 
				description, 
				def_value,
				safe_cast<float2_getter_type^>(System::Delegate::CreateDelegate(float2_getter_type::typeid, o, pi->GetGetMethod())),
				safe_cast<float2_setter_type^>(System::Delegate::CreateDelegate(float2_setter_type::typeid, o, pi->GetSetMethod())),
				readonly,
				property_holder::do_not_notify_parent_on_change,
				property_holder::no_password_char,
				property_holder::do_not_refresh_grid_on_change
			);
		}else
		if(t==color::typeid)
		{
			color def_value						(1.0f, 1.0f, 1.0f, 1.0f);
			attribs								= pi->GetCustomAttributes(ValueAttribute::typeid, true);
			for each (ValueAttribute^ att in attribs)
			{
				if(att->m_type==e_def_val)
				{
					def_value.x 				= (float)att->m_params[0];
					def_value.y 				= (float)att->m_params[1];
					def_value.z 				= (float)att->m_params[2];
					def_value.w 				= (float)att->m_params[3];
				}
			}
			holder->add_color(
				display_name, 
				category, 
				description, 
				def_value,
				safe_cast<color_getter_type^>(System::Delegate::CreateDelegate(color_getter_type::typeid, o, pi->GetGetMethod())),
				safe_cast<color_setter_type^>(System::Delegate::CreateDelegate(color_setter_type::typeid, o, pi->GetSetMethod())),
				readonly,
				property_holder::do_not_notify_parent_on_change,
				property_holder::no_password_char,
				property_holder::do_not_refresh_grid_on_change
			);
		}else
		if(t==int::typeid)
		{
			int min_value=0;
			int max_value=0;
			int def_value=0;
			attribs			= pi->GetCustomAttributes(ValueAttribute::typeid, true);
			for each (ValueAttribute^ att in attribs)
			{
				if(att->m_type==e_def_val)
					def_value = (int)att->m_params[0];
				else
				if(att->m_type==e_min_val)
					min_value = (int)att->m_params[0];
				else
				if(att->m_type==e_max_val)
					max_value = (int)att->m_params[0];
			}

			holder->add_integer(
				display_name, 
				category, 
				description, 
				def_value,
				safe_cast<integer_getter_type^>(System::Delegate::CreateDelegate(integer_getter_type::typeid, o, pi->GetGetMethod())),
				safe_cast<integer_setter_type^>(System::Delegate::CreateDelegate(integer_setter_type::typeid, o, pi->GetSetMethod())),
				min_value,
				max_value,
				readonly,
				property_holder::do_not_notify_parent_on_change,
				property_holder::no_password_char,
				property_holder::do_not_refresh_grid_on_change
			);
		}else
		if(t==float::typeid)
		{
			float min_value=0;
			float max_value=0;
			float def_value=0;
			attribs			= pi->GetCustomAttributes(ValueAttribute::typeid, true);
			for each (ValueAttribute^ att in attribs)
			{
				if(att->m_type==e_def_val)
					def_value = (float)att->m_params[0];
				else
				if(att->m_type==e_min_val)
					min_value = (float)att->m_params[0];
				else
				if(att->m_type==e_max_val)
					max_value = (float)att->m_params[0];
			}

			holder->add_float(
				display_name, 
				category, 
				description, 
				def_value,
				safe_cast<float_getter_type^>(System::Delegate::CreateDelegate(float_getter_type::typeid, o, pi->GetGetMethod())),
				safe_cast<float_setter_type^>(System::Delegate::CreateDelegate(float_setter_type::typeid, o, pi->GetSetMethod())),
				min_value,
				max_value,
				readonly,
				property_holder::do_not_notify_parent_on_change,
				property_holder::no_password_char,
				property_holder::do_not_refresh_grid_on_change
			);
		}else
		if(t==bool::typeid)
		{
			bool def_value=false;
			attribs			= pi->GetCustomAttributes(ValueAttribute::typeid, true);
			for each (ValueAttribute^ att in attribs)
			{
				if(att->m_type==e_def_val)
					def_value = (bool)att->m_params[0];
			}

			holder->add_bool(
				display_name, 
				category, 
				description, 
				def_value,
				safe_cast<boolean_getter_type^>(System::Delegate::CreateDelegate(boolean_getter_type::typeid, o, pi->GetGetMethod())),
				safe_cast<boolean_setter_type^>(System::Delegate::CreateDelegate(boolean_setter_type::typeid, o, pi->GetSetMethod())),
				readonly,
				property_holder::do_not_notify_parent_on_change,
				property_holder::no_password_char,
				property_holder::do_not_refresh_grid_on_change
			);
		}else {
			dynamic_cast<::property_holder*>(holder)->add_property( o, pi );
		}
		//			UNREACHABLE_CODE();
	} // cycle 

// 	float_curve^ fc			= gcnew float_curve(/*-1.0f, 10.0f, 0.0f, 100.0f*/);
// 	::property_holder* ph	= dynamic_cast<::property_holder*>(holder);
// 
// 	ph->add_property		( fc );
} // void initialize_property_holder

} // namespace editor
} // namespace xray
