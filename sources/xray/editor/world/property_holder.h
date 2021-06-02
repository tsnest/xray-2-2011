////////////////////////////////////////////////////////////////////////////
//	Created 	: 04.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef EDITOR_PROPERTY_HOLDER_H_INCLUDED
#define EDITOR_PROPERTY_HOLDER_H_INCLUDED

typedef xray::math::color color;
typedef xray::math::float2 float2;

public delegate bool			boolean_getter_type		();
public delegate void			boolean_setter_type		(bool);

public delegate int				integer_getter_type		();
public delegate void			integer_setter_type		(int);

public delegate float			float_getter_type		();
public delegate void			float_setter_type		(float);

public delegate color			color_getter_type		();
public delegate void			color_setter_type		(color);

public delegate float3			float3_getter_type		();
public delegate void			float3_setter_type		(float3);

public delegate float2			float2_getter_type		();
public delegate void			float2_setter_type		(float2);

public delegate System::String^ string_getter_type		();
public delegate void			string_setter_type		(System::String^);

public delegate System::String^	string_collection_getter_type		(u32 idx);
public delegate u32				string_collection_size_getter_type	();

using xray::editor::wpf_controls::property_grid_editors::external_editor_event_handler;

namespace xray {
class shared_string;

namespace editor {

	class property_holder;	
	
	typedef
		intrusive_ptr<
		property_holder,
		editor_intrusive_base,
		threading::single_threading_policy
		> property_holder_ptr;




class property_holder_holder {
public:
	virtual	property_holder*	object					() = 0;
	virtual						~property_holder_holder	() = 0 {}
}; // class property_holder_holder

class property_holder_collection {
public:
	virtual	void				clear			() = 0;
	virtual	void				insert			(property_holder* holder, u32 const& position) = 0;
	virtual	void				erase			(u32 const& position) = 0;
	virtual	int					index			(property_holder* holder) = 0;
	virtual	property_holder*	item			(u32 const& position) = 0;
	virtual	u32					size			() = 0;
	virtual	void				display_name	(u32 const& item_index, char* const& buffer, u32 const& buffer_size) = 0;
	virtual	property_holder*	create			() = 0;
	virtual	void				destroy			(property_holder* holder) = 0;
}; // class propery_holder_collection

class property_value;

class property_holder : public xray::editor_intrusive_base {
public:
	virtual					~property_holder	( )		{};

	typedef fastdelegate::FastDelegate0<
				property_holder_collection*
			>											collection_getter_type;

public:
	enum enter_text_enum {
		can_enter_text					= int(0),
		cannot_enter_text				= int(1),
	}; // enum can_enter_text_enum

	enum extension_action_enum {
		remove_extension				= int(0),
		keep_extension					= int(1),
	}; // enum remove_extension_enum

	enum value_editor_enum {
		value_editor_combo_box			= int(0),
		value_editor_tree_view			= int(1),
	}; // enum value_editor_enum

	enum readonly_enum {
		property_read_only				= int(0),
		property_read_write				= int(1),
	}; // enum value_editor_enum

	enum notify_parent_on_change_enum {
		notify_parent_on_change			= int(0),
		do_not_notify_parent_on_change	= int(1),
	}; // enum value_editor_enum

	enum password_char_enum {
		password_char					= int(0),
		no_password_char				= int(1),
	}; // enum value_editor_enum

	enum refresh_grid_on_change_enum {
		refresh_grid_on_change			= int(0),
		do_not_refresh_grid_on_change	= int(1),
	}; // enum value_editor_enum
						 
public:
	virtual	property_holder_holder*	holder	() = 0;
	virtual	void	clear				() = 0;
	virtual	property_value*	add_bool		(
						System::String^ identifier,
						System::String^ category,
						System::String^ description,
						bool const &default_value,
						boolean_getter_type^ getter,
						boolean_setter_type^ setter,
						readonly_enum const& read_only = property_read_write,
						notify_parent_on_change_enum const& notify_parent = do_not_notify_parent_on_change,
						password_char_enum const& password = no_password_char,
						refresh_grid_on_change_enum const& refresh_grid = do_not_refresh_grid_on_change
					) = 0;

	virtual	property_value*	add_bool		(
						System::String^ identifier,
						System::String^ category,
						System::String^ description,
						bool const &default_value,
						boolean_getter_type^ getter,
						boolean_setter_type^ setter,
						pcstr values[2],
						readonly_enum const& read_only = property_read_write,
						notify_parent_on_change_enum const& notify_parent = do_not_notify_parent_on_change,
						password_char_enum const& password = no_password_char,
						refresh_grid_on_change_enum const& refresh_grid = do_not_refresh_grid_on_change
					) = 0;

	virtual	property_value*	add_integer(
						System::String^ identifier,
						System::String^ category,
						System::String^ description,
						int const &default_value,
						integer_getter_type^ getter,
						integer_setter_type^ setter,
						readonly_enum const& read_only = property_read_write,
						notify_parent_on_change_enum const& notify_parent = do_not_notify_parent_on_change,
						password_char_enum const& password = no_password_char,
						refresh_grid_on_change_enum const& refresh_grid = do_not_refresh_grid_on_change
					) = 0;

	virtual	property_value*	add_integer(
						System::String^ identifier,
						System::String^ category,
						System::String^ description,
						int const &default_value,
						integer_getter_type^ getter,
						integer_setter_type^ setter,
						int const &min_value,
						int const &max_value,
						readonly_enum const& read_only = property_read_write,
						notify_parent_on_change_enum const& notify_parent = do_not_notify_parent_on_change,
						password_char_enum const& password = no_password_char,
						refresh_grid_on_change_enum const& refresh_grid = do_not_refresh_grid_on_change
					) = 0;
	virtual property_value*	add_integer(
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
					) = 0;
	virtual	property_value*	add_integer(
						System::String^ identifier,
						System::String^ category,
						System::String^ description,
						int const &default_value,
						integer_getter_type^ getter,
						integer_setter_type^ setter,
						string_collection_getter_type^ values,
						string_collection_size_getter_type^ value_count,
						readonly_enum const& read_only = property_read_write,
						notify_parent_on_change_enum const& notify_parent = do_not_notify_parent_on_change,
						password_char_enum const& password = no_password_char,
						refresh_grid_on_change_enum const& refresh_grid = do_not_refresh_grid_on_change
					) = 0;
	virtual	property_value*	add_float(
						System::String^ identifier,
						System::String^ category,
						System::String^ description,
						float const &default_value,
						float_getter_type^ getter,
						float_setter_type^ setter,
						readonly_enum const& read_only = property_read_write,
						notify_parent_on_change_enum const& notify_parent = do_not_notify_parent_on_change,
						password_char_enum const& password = no_password_char,
						refresh_grid_on_change_enum const& refresh_grid = do_not_refresh_grid_on_change
					) = 0;
	virtual	property_value*	add_float(
						System::String^ identifier,
						System::String^ category,
						System::String^ description,
						float const &default_value,
						float_getter_type^ getter,
						float_setter_type^ setter,
						float const &min_value,
						float const &max_value,
						readonly_enum const& read_only = property_read_write,
						notify_parent_on_change_enum const& notify_parent = do_not_notify_parent_on_change,
						password_char_enum const& password = no_password_char,
						refresh_grid_on_change_enum const& refresh_grid = do_not_refresh_grid_on_change
					) = 0;
	virtual property_value* add_float(
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
					) = 0;
	virtual	property_value*	add_string(
						System::String^ identifier,
						System::String^ category,
						System::String^ description,
						System::String^ default_value,
						string_getter_type^ getter,
						string_setter_type^ setter,
						readonly_enum const& read_only = property_read_write,
						notify_parent_on_change_enum const& notify_parent = do_not_notify_parent_on_change,
						password_char_enum const& password = no_password_char,
						refresh_grid_on_change_enum const& refresh_grid = do_not_refresh_grid_on_change
					) = 0;
	virtual xray::editor::property_value*	add_string(
						System::String^ identifier,
						System::String^ category,
						System::String^ description,
						System::String^ default_value,
						string_getter_type^ getter,
						string_setter_type^ setter,
						external_editor_event_handler^	type_of_external_editor,
						System::Object^ filter,
						readonly_enum const& read_only = property_read_write,
						notify_parent_on_change_enum const& notify_parent = do_not_notify_parent_on_change,
						password_char_enum const& password = no_password_char,
						refresh_grid_on_change_enum const& refresh_grid = do_not_refresh_grid_on_change
					) = 0;
	virtual	property_value*	add_string(
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
						readonly_enum const& read_only = property_read_write,
						notify_parent_on_change_enum const& notify_parent = do_not_notify_parent_on_change,
						password_char_enum const& password = no_password_char,
						refresh_grid_on_change_enum const& refresh_grid = do_not_refresh_grid_on_change
					) = 0;
	virtual	property_value*	add_string(
						System::String^ identifier,
						System::String^ category,
						System::String^ description,
						System::String^ default_value,
						string_getter_type^ getter,
						string_setter_type^ setter,
						System::Collections::ArrayList^ values,
						value_editor_enum const& value_editor,
						enter_text_enum const& can_enter_text,
						readonly_enum const& read_only = property_read_write,
						notify_parent_on_change_enum const& notify_parent = do_not_notify_parent_on_change,
						password_char_enum const& password = no_password_char,
						refresh_grid_on_change_enum const& refresh_grid = do_not_refresh_grid_on_change
					) = 0;
	virtual	property_value*	add_string(
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
						readonly_enum const& read_only = property_read_write,
						notify_parent_on_change_enum const& notify_parent = do_not_notify_parent_on_change,
						password_char_enum const& password = no_password_char,
						refresh_grid_on_change_enum const& refresh_grid = do_not_refresh_grid_on_change
					) = 0;
	virtual	property_value*	add_color(
						System::String^ identifier,
						System::String^ category,
						System::String^ description,
						color const& default_value,
						color_getter_type^ getter,
						color_setter_type^ setter,
						readonly_enum const& read_only = property_read_write,
						notify_parent_on_change_enum const& notify_parent = do_not_notify_parent_on_change,
						password_char_enum const& password = no_password_char,
						refresh_grid_on_change_enum const& refresh_grid = do_not_refresh_grid_on_change
					) = 0;
	virtual	property_value*	add_vec3f(
						System::String^ identifier,
						System::String^ category,
						System::String^ description,
						float3 const& default_value,
						float3_getter_type^ getter,
						float3_setter_type^ setter,
						readonly_enum const& read_only = property_read_write,
						notify_parent_on_change_enum const& notify_parent = do_not_notify_parent_on_change,
						password_char_enum const& password = no_password_char,
						refresh_grid_on_change_enum const& refresh_grid = do_not_refresh_grid_on_change
					) = 0;
	virtual	property_value*	add_vec2f(
						System::String^ identifier,
						System::String^ category,
						System::String^ description,
						float2 const& default_value,
						float2_getter_type^ getter,
						float2_setter_type^ setter,
						readonly_enum const& read_only = property_read_write,
						notify_parent_on_change_enum const& notify_parent = do_not_notify_parent_on_change,
						password_char_enum const& password = no_password_char,
						refresh_grid_on_change_enum const& refresh_grid = do_not_refresh_grid_on_change
					) = 0;
	virtual	property_value*	add_property_holder(
						System::String^ identifier,
						System::String^ category,
						System::String^ description,
						property_holder_ptr value,
						readonly_enum const& read_only = property_read_write,
						notify_parent_on_change_enum const& notify_parent = do_not_notify_parent_on_change,
						password_char_enum const& password = no_password_char,
						refresh_grid_on_change_enum const& refresh_grid = do_not_refresh_grid_on_change
					) = 0;
	virtual	property_value*	add_collection(
						System::String^ identifier,
						System::String^ category,
						System::String^ description,
						property_holder_collection* collection,
						readonly_enum const& read_only = property_read_write,
						notify_parent_on_change_enum const& notify_parent = do_not_notify_parent_on_change,
						password_char_enum const& password = no_password_char,
						refresh_grid_on_change_enum const& refresh_grid = do_not_refresh_grid_on_change
					) = 0;
	// probably, dummy method, should be removed sometimes
	virtual	property_value*	add_collection(
						System::String^ identifier,
						System::String^ category,
						System::String^ description,
						collection_getter_type const& collection_getter,
						readonly_enum const& read_only = property_read_write,
						notify_parent_on_change_enum const& notify_parent = do_not_notify_parent_on_change,
						password_char_enum const& password = no_password_char,
						refresh_grid_on_change_enum const& refresh_grid = do_not_refresh_grid_on_change
					) = 0;
}; // class property_holder

class property_value {
public:
	virtual	void		attribute	(property_holder::readonly_enum const& read_only) = 0;
	virtual	void		attribute	(property_holder::notify_parent_on_change_enum const& notify_parent) = 0;
	virtual	void		attribute	(property_holder::password_char_enum const& password_char) = 0;
	virtual	void		attribute	(property_holder::refresh_grid_on_change_enum const& refresh_grid) = 0;
}; // class property_value




} // namespace editor
} // namespace xray

#endif // ifndef EDITOR_PROPERTY_HOLDER_H_INCLUDED