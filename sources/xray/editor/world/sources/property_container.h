////////////////////////////////////////////////////////////////////////////
//	Created 	: 07.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_CONTAINER_HPP_INCLUDED
#define PROPERTY_CONTAINER_HPP_INCLUDED

#include "property_holder.h"

namespace xray {
namespace editor {
namespace controls {
	interface class property_value;
} // namespace controls
} // namespace editor
} // namespace xray

ref class property_container_converter;
interface class property_container_holder;

[System::ComponentModel::TypeConverterAttribute(property_container_converter::typeid)]
public ref class property_container :
	public Flobbster::Windows::Forms::PropertyBag,
	public xray::editor::controls::property_container_interface
{
public:
	typedef Flobbster::Windows::Forms::PropertySpec				PropertySpec;
	typedef Flobbster::Windows::Forms::PropertySpecEventArgs	PropertySpecEventArgs;
	typedef System::Collections::IDictionary					IDictionary;
	typedef System::Collections::ArrayList						ArrayList;
	typedef System::Object										Object;
	typedef System::String										String;
	typedef xray::editor::controls::property_value				property_value;

	typedef property_holder::readonly_enum						readonly_enum;
	typedef property_holder::notify_parent_on_change_enum		notify_parent_on_change_enum;
	typedef property_holder::password_char_enum					password_char_enum;
	typedef property_holder::refresh_grid_on_change_enum		refresh_grid_on_change_enum	;

public:
								property_container	(property_holder* holder, property_container_holder^ container_holder);
	virtual						~property_container	();
								!property_container	();
	property_holder&			holder				();
	property_container_holder%	container_holder	();
			void				add_property		(PropertySpec^ description, property_value^ value);
			void				remove_property(PropertySpec^ description);
			void				add_property		(
									PropertySpec^ description,
									property_value^ value,
									readonly_enum read_only,
									notify_parent_on_change_enum notify_parent,
									password_char_enum password,
									refresh_grid_on_change_enum refresh_grid
								);
	virtual	property_value^		value				(PropertySpec^ description);
	IDictionary%				properties			();
	ArrayList%					ordered_properties	();
			void				clear				();
	virtual System::String^		ToString			() override		{return "";}

private:
			void				get_value_handler	(Object^ sender, PropertySpecEventArgs^ e);
			void				set_value_handler	(Object^ sender, PropertySpecEventArgs^ e);
			bool				equal_category		(String^ new_category, String^ old_category);
			String^				update_categories	(String^ new_category);
			void				try_update_name		(PropertySpec^ description, String^ name);
			void				update_names		(String^ name);

private:
	IDictionary^				m_properties;
	ArrayList^					m_ordered_properties;
	property_holder*			m_holder;
	property_container_holder^	m_container_holder;
}; // class property_container

ref class ro_property_wrapper
{
public:
	ro_property_wrapper(System::String^ name, bool initial):m_property_name(name),m_is_readwrite(initial){};

	System::String^			m_property_name;
	bool					get_readwrite() {return m_is_readwrite;}
	void					set_readwrite(bool b)	{m_is_readwrite=b;}
private:
	bool					m_is_readwrite;
};

public ref class property_restrictor
{
	typedef System::ComponentModel::ReadOnlyAttribute	ReadOnlyAttribute;
public:
				property_restrictor	();
		void	reset				();
		void	add_property		(System::String^ name, bool b_readonly);
		bool	is_readonly			(System::String^ property_name);
		void	apply_to			(property_holder* holder);
		void	create_from			(property_holder* holder);
		void	load				(xray::configs::lua_config_value const& lv);
		void	save				(xray::configs::lua_config_value lv);
		void	in_create			();
		void	in_destroy			();
protected:
	System::Collections::ArrayList^	m_restricted_items;
public:
	xray::editor::property_holder*		m_property_holder; //tmp for external initialization-destruction
};//class property_restrictor

#endif // ifndef PROPERTY_CONTAINER_HPP_INCLUDED