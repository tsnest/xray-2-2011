////////////////////////////////////////////////////////////////////////////
//	Created		: 22.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MATERIAL_H_INCLUDED
#define MATERIAL_H_INCLUDED

#include "resource_editor_resource.h"

using namespace System::Collections::Generic;
using namespace xray::editor::wpf_controls::property_editors::attributes;

namespace xray {
namespace editor {

class editor_world;
ref class	material;
ref class	material_stage;

ref class material : public resource_editor_resource
{
	typedef xray::editor::wpf_controls::property_container		wpf_property_container;
	typedef xray::editor::wpf_controls::property_descriptor		property_descriptor;

public:
								material					( editor_world& w );
	virtual						~material					( );

	virtual wpf_property_container^	get_property_container	( bool chooser_mode ) override;

	virtual void				init_new					( ) override;
	virtual void				copy_from					( resource_editor_resource^ other ) override;
	virtual void				create_from					( configs::lua_config_value const& config );
	virtual void				apply_changes				( ) override;
	virtual void				preview_changes				( ) override;
	virtual void				reset_to_default			( ) override;
	virtual void				load						( System::String^ name ) override;
	virtual void				save						( ) override;
	virtual System::String^		resource_name				( System::String^ short_name ) override;
	virtual System::String^		source_path					( System::String^ short_name, System::String^ resources_path ) override;
	void						stage_loaded				( );

protected:
	int							m_loading_stages;

	configs::lua_config_ptr*	m_material_config;
	configs::lua_config_ptr*	m_material_config_backup;
	query_result_delegate*		m_load_query;
public:		
	editor_world&				m_editor_world;
	configs::lua_config_ptr*	material_config				( )		{ return m_material_config; }
	Dictionary<System::String^, material_stage^>	m_stages;

public:
	property System::String^	description
	{
		System::String^			get							( );
		void					set							( System::String^ value );
	}

protected:
	void						config_loaded				( resources::queries_result& data );
	configs::lua_config_value	config_root					( );
	void						load_internal				( );
	void						store_backup_config			( );
	virtual void				load_from_config			( configs::lua_config_ptr config );
	void						destroy_contents			( );
private:
	bool						add_stage					( );
	bool						remove_stage				( Object^ stage_name );
	void						check_integrity				( );
}; // class material

} // namespace editor
} // namespace xray

#endif // #ifndef MATERIAL_H_INCLUDED