////////////////////////////////////////////////////////////////////////////
//	Created		: 02.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef RENDER_OPTIONS_EDITOR_RESOURCE_H_INCLUDED
#define RENDER_OPTIONS_EDITOR_RESOURCE_H_INCLUDED

#include "resource_editor_resource.h"
#include "attribute.h"


using namespace System;
using namespace System::IO;
using namespace System::Collections::Generic;

namespace xray {

namespace render{
	class world; 

	namespace editor {
		class renderer;
	} // namespace editor
} // namespace render

namespace editor {

	class				editor_world;

ref class render_options_editor_resource: public resource_editor_resource
{
	typedef resource_editor_resource super;
	typedef xray::editor::wpf_controls::property_container wpf_property_container;
	typedef xray::editor::wpf_controls::property_descriptor property_descriptor;
	typedef xray::editor::wpf_controls::property_editors::property prop;
public:
									render_options_editor_resource	( editor_world& in_editor_world, System::String^ resource_name, resource_changed_delegate^ ,resource_load_delegate^ );
	virtual							~render_options_editor_resource	( );
	void							options_loaded					( resources::queries_result& data );
	void							load_internal					( );
	
	virtual void					copy_from						( resource_editor_resource^ other ) override;
public:
	configs::lua_config_ptr*		m_options_template_config;
	configs::lua_config_ptr*		m_options_config;
	configs::lua_config_ptr*		m_options_config_backup;
	query_result_delegate*			m_load_query;

	Dictionary<System::String^, xray::editor::wpf_controls::i_property_value^>	m_properties;

	virtual wpf_property_container^		get_property_container	( bool )					override;

	virtual System::String^			resource_name			( System::String^ short_name )	override { return "resources/render/" + short_name + ".cfg"; }
	virtual System::String^			source_path				( System::String^ short_name, System::String^ resources_path )	override { return resources_path + "/sources/render/" + short_name + ".cfg"; }
	virtual void					load					( System::String^ )				override { UNREACHABLE_CODE();}
	virtual void					save					( )								override;
	virtual void					apply_changes			( ) override;
	virtual void					preview_changes			( ) override									{ UNREACHABLE_CODE(); };
	virtual void					reset_to_default		( ) override;
	virtual void					init_new				( ) override									{ UNREACHABLE_CODE(); };
	//virtual void					copy_from				( resource_editor_resource^ )	override	{ UNREACHABLE_CODE(); };

private:
			void						store_backup_config	( );
			
			void						init				( );
			configs::lua_config_value	opts()				{ return (*m_options_config)->get_root()["options"]; }

			static System::Collections::ArrayList			m_type_list;
			static System::Collections::ArrayList			m_dimension_list;
			static System::Collections::ArrayList			m_format_list;
			static System::Collections::ArrayList			m_format_back_list;

			editor_world&									m_editor_world;
public:
		[ReadOnlyAttribute(true)]
		[CategoryAttribute("Common")]
		property System::String^	name
		{
			System::String^ get	( );
		}
}; // class render_options_editor_resource

} // namespace editor
} // namespace xray

#endif // #ifndef RENDER_OPTIONS_EDITOR_RESOURCE_H_INCLUDED