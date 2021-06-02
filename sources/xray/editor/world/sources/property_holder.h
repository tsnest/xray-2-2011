////////////////////////////////////////////////////////////////////////////
//	Created 	: 06.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_HOLDER_HPP_INCLUDED
#define PROPERTY_HOLDER_HPP_INCLUDED

#include "property_holder_include.h"

ref class property_container;
ref class property_holder_converter;

//using xray::editor::wpf_controls::property_grid_editors::external_editor_event_handler;
class property_holder;

typedef xray::intrusive_ptr<
	property_holder, 
	xray::editor_intrusive_base,	
	xray::threading::single_threading_policy
> property_holder_ptr;

class property_holder : public xray::editor::property_holder {
public:
							property_holder		(
								LPCSTR display_name,
								xray::editor::property_holder_collection* collection,
								xray::editor::property_holder_holder* holder
							);
	virtual					~property_holder	( );
			void			on_dispose			( );
	property_container^		container			( );

public:
	virtual	xray::editor::property_holder_holder* holder( );
	virtual	void			clear				( );

			xray::editor::property_value* add_property	( System::Object^ object, System::Reflection::PropertyInfo^ property_info );

			xray::editor::property_value* add_property( 
								System::Object^ prop_object,
								System::String^ display_value,
								System::String^ category,
								System::String^ description,
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid);

	//bool
	virtual	xray::editor::property_value* add_bool	(
								System::String^ identifier,
								System::String^ category,
								System::String^ description,
								bool const &default_value,
								xray::configs::lua_config_value const& config,
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid
							);
	virtual	xray::editor::property_value* add_bool	(
								System::String^ identifier,
								System::String^ category,
								System::String^ description,
								bool const &default_value,
								boolean_getter_type^ getter,
								boolean_setter_type^ setter,
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid
								);
	virtual	xray::editor::property_value*	add_bool(
								System::String^ identifier,
								System::String^ category,
								System::String^ description,
								bool const &default_value,
								boolean_getter_type^ getter,
								boolean_setter_type^ setter,
								LPCSTR values[2],
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid
							);
	//integer
	virtual	xray::editor::property_value*	add_integer(
								System::String^ identifier,
								System::String^ category,
								System::String^ description,
								int const &default_value,
								xray::configs::lua_config_value const& config,
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid
								);
	virtual	xray::editor::property_value*	add_integer(
								System::String^ identifier,
								System::String^ category,
								System::String^ description,
								int const &default_value,
								integer_getter_type^ getter,
								integer_setter_type^ setter,
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid
							);
	virtual	xray::editor::property_value*	add_integer(
								System::String^ identifier,
								System::String^ category,
								System::String^ description,
								int const &default_value,
								integer_getter_type^ getter,
								integer_setter_type^ setter,
								int const &min_value,
								int const &max_value,
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid
							);
	virtual xray::editor::property_value*	add_integer(
								System::String^ identifier,
								System::String^ category,
								System::String^ description,
								int const &default_value,
								integer_getter_type^ getter,
								integer_setter_type^ setter,
								System::Func<System::Double>^ min_value_func,
								System::Func<System::Double>^ max_value_func,
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid
							);
	virtual	xray::editor::property_value*	add_integer(
								System::String^ identifier,
								System::String^ category,
								System::String^ description,
								int const &default_value,
								integer_getter_type^ getter,
								integer_setter_type^ setter,
								string_collection_getter_type^ values,
								string_collection_size_getter_type^ value_count,
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid
							);
	//float
	virtual	xray::editor::property_value*	add_float(
								System::String^ identifier,
								System::String^ category,
								System::String^ description,
								float const &default_value,
								xray::configs::lua_config_value const& config,
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid
								);
	virtual	xray::editor::property_value*	add_float(
								System::String^ identifier,
								System::String^ category,
								System::String^ description,
								float const &default_value,
								xray::configs::lua_config_value const& config,
								float const &min_value,
								float const &max_value,
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid
								);
	virtual	xray::editor::property_value*	add_float(
								System::String^ identifier,
								System::String^ category,
								System::String^ description,
								float const &default_value,
								float_getter_type^ getter,
								float_setter_type^ setter,
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid
							);
	virtual	xray::editor::property_value*	add_float(
								System::String^ identifier,
								System::String^ category,
								System::String^ description,
								float const &default_value,
								float_getter_type^ getter,
								float_setter_type^ setter,
								float const &min_value,
								float const &max_value,
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid
							);
	virtual xray::editor::property_value*	add_float(
								System::String^ identifier,
								System::String^ category,
								System::String^ description,
								float const &default_value,
								float_getter_type^ getter,
								float_setter_type^ setter,
								System::Func<System::Double>^ min_value_func,
								System::Func<System::Double>^ max_value_func,
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid
							);
	//string
	virtual	xray::editor::property_value*	add_string				(
								System::String^ identifier,
								System::String^ category,
								System::String^ description,
								System::String^ default_value,
								xray::configs::lua_config_value const& config,
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid
								);
	virtual xray::editor::property_value*	add_string				(
								System::String^ identifier,
								System::String^ category,
								System::String^ description,
								System::String^ default_value,
								xray::configs::lua_config_value const& config,
								System::Collections::ArrayList^ values,
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid
								);

	virtual	xray::editor::property_value*	add_string(
								System::String^ identifier,
								System::String^ category,
								System::String^ description,
								System::String^ default_value,
								string_getter_type^ getter,
								string_setter_type^ setter,
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid
							);
	virtual xray::editor::property_value*	add_string(
								System::String^ identifier,
								System::String^ category,
								System::String^ description,
								System::String^ default_value,
								string_getter_type^ getter,
								string_setter_type^ setter,
								external_editor_event_handler^	type_of_external_editor,
								System::Object^ filter,
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid
							);
	virtual	xray::editor::property_value*	add_string(
								System::String^ identifier,
								System::String^ category,
								System::String^ description,
								System::String^ default_value,
								string_getter_type^ getter,
								string_setter_type^ setter,
								System::String^ default_extension,	// ".dds",
								System::String^ file_mask,			// "Texture files (*.dds)|*.dds",
								System::String^ default_folder,		// "R:\\development\\priquel\\resources\\gamedata\\textures\\sky",
								System::String^ caption,				// "Select texture..."
								enter_text_enum const& can_enter_text,
								extension_action_enum const& remove_extension,
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid
							);
	virtual xray::editor::property_value*	add_string(
								System::String^ identifier,
								System::String^ category,
								System::String^ description,
								System::String^ default_value,
								xray::configs::lua_config_value const& config,
								System::String^ default_extension,
								System::String^ file_mask,
								System::String^ default_folder,
								System::String^ caption,
								enter_text_enum const& can_enter_text,
								extension_action_enum const& extension_action,
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid
								);

	virtual	xray::editor::property_value*	add_string(
								System::String^ identifier,
								System::String^ category,
								System::String^ description,
								System::String^ default_value,
								string_getter_type^ getter,
								string_setter_type^ setter,
								System::Collections::ArrayList^ values,
								value_editor_enum const& value_editor,
								enter_text_enum const& can_enter_text,
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid
							);
	virtual	xray::editor::property_value*	add_string(
								System::String^ identifier,
								System::String^ category,
								System::String^ description,
								System::String^ default_value,
								string_getter_type^ getter,
								string_setter_type^ setter,
								System::Func<System::Int32, System::String^>^ values,
								System::Func<System::Int32>^ value_count,
								value_editor_enum const& value_editor,
								enter_text_enum const& can_enter_text,
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid
							);
	//color
	virtual xray::editor::property_value*	add_color(
								System::String^ identifier,
								System::String^ category,
								System::String^ description,
								color const& default_value,
								xray::configs::lua_config_value const& config,
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid
							);

	virtual	xray::editor::property_value*	add_color(
								System::String^ identifier,
								System::String^ category,
								System::String^ description,
								color const& default_value,
								color_getter_type^ getter,
								color_setter_type^ setter,
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid
							);
	//vec3f
	virtual	xray::editor::property_value*	add_vec3f(
								System::String^ identifier,
								System::String^ category,
								System::String^ description,
								float3 const& default_value,
								float3_getter_type^ getter,
								float3_setter_type^ setter,
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid
							);
	virtual	xray::editor::property_value*	add_vec3f(
								System::String^ identifier,
								System::String^ category,
								System::String^ description,
								float3 const& default_value,
								xray::configs::lua_config_value const& config,
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid
							);

	//vec2f
	virtual	xray::editor::property_value*	add_vec2f(
								System::String^ identifier,
								System::String^ category,
								System::String^ description,
								float2 const& default_value,
								float2_getter_type^ getter,
								float2_setter_type^ setter,
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid
							);
	//property_holder
	virtual	xray::editor::property_value*	add_property_holder(
								System::String^ identifier,
								System::String^ category,
								System::String^ description,
								xray::editor::property_holder_ptr value,
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid
							);
	xray::editor::property_value* add_special_property_holder	(
								System::String^ identifier,
								System::String^ category,
								System::String^ description,
								xray::editor::property_holder_ptr value,
								System::String^	holder_type,
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid
							);
	//collection
	virtual	xray::editor::property_value*	add_collection(
								System::String^ identifier,
								System::String^ category,
								System::String^ description,
								xray::editor::property_holder_collection* collection,
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid
							);
	virtual	xray::editor::property_value*	add_collection(
								System::String^ identifier,
								System::String^ category,
								System::String^ description,
								collection_getter_type const& collection_getter,
								readonly_enum const& read_only,
								notify_parent_on_change_enum const& notify_parent,
								password_char_enum const& password,
								refresh_grid_on_change_enum const& refresh_grid
							);

public:
	typedef xray::editor::property_holder_collection	collection_type;

	System::String^			display_name		( );
	collection_type*		collection			( );
	System::Object^			get_value			( System::String^ property_id );
	void					remove_property		( System::String^ property_id );

private:
	gcroot<property_container^>					m_container;
	gcroot<System::String^>						m_display_name;
	collection_type*							m_collection;
	xray::editor::engine*						m_engine;
	xray::editor::property_holder_holder*		m_holder;
	bool										m_disposing;
}; // class property_holder

#endif // ifndef PROPERTY_HOLDER_HPP_INCLUDED