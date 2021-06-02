////////////////////////////////////////////////////////////////////////////
//	Created		: 08.02.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef MATERIAL_INSTANCE_H_INCLUDED
#define MATERIAL_INSTANCE_H_INCLUDED

#include "material.h"

namespace xray {
namespace editor {

class editor_world;

ref class material_instance : public material
{
	typedef material super;
	typedef xray::editor::wpf_controls::property_container		wpf_property_container;
	typedef xray::editor::wpf_controls::property_descriptor		property_descriptor;

public:
					material_instance		( editor_world& w );
	virtual			~material_instance		( );

public:		
	virtual wpf_property_container^	get_property_container	( bool chooser_mode ) override;

	virtual void				init_new					( ) override;
	virtual void				copy_from					( resource_editor_resource^ other ) override;
	virtual void				create_from					( configs::lua_config_value const& config ) override;
	virtual System::String^		resource_name				( System::String^ short_name ) override;
	virtual System::String^		source_path					( System::String^ short_name, System::String^ resources_path ) override;

	property System::String^ base_material
	{
		System::String^			get						( );
		void					set						( System::String^ value );
	}

protected:
	virtual void				load_from_config			( configs::lua_config_ptr config ) override;
	bool						check_material_inheritance	( );
	void						base_material_selector		( wpf_controls::property_editors::property^ prop, Object^ filter );
};// class material_instance

} // namespace editor
} // namespace xray

#endif // #ifndef MATERIAL_INSTANCE_H_INCLUDED