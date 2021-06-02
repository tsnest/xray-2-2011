////////////////////////////////////////////////////////////////////////////
//	Created		: 06.10.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TEXTURE_EDITOR_RESOURCE_H_INCLUDED
#define TEXTURE_EDITOR_RESOURCE_H_INCLUDED

#include "resource_editor_resource.h"
#include "attribute.h"


using namespace System::IO;
using namespace System::Collections::Generic;

namespace xray {
namespace editor {

ref class texture_editor_resource: public resource_editor_resource
{
	typedef resource_editor_resource super;
	typedef xray::editor::wpf_controls::property_container wpf_property_container;
	typedef xray::editor::wpf_controls::property_descriptor property_descriptor;
	typedef xray::editor::wpf_controls::property_editors::property prop;
public:
									texture_editor_resource			( System::String^ resource_name, resource_changed_delegate^ ,resource_load_delegate^ );
	virtual							~texture_editor_resource		( );
	void							options_loaded					( resources::queries_result& data );

	[ReadOnlyAttribute(true)]
	[CategoryAttribute("Common")]
	property int					width;

	[ReadOnlyAttribute(true)]
	[CategoryAttribute("Common")]
	property int					height;


public:
	configs::lua_config_ptr*		m_options_config;
	configs::lua_config_ptr*		m_options_config_backup;
	query_result_delegate*			m_load_query;


	virtual wpf_property_container^		get_property_container	( bool )					override;

	virtual System::String^			resource_name			( System::String^ short_name )	override { UNREACHABLE_CODE(); return "resources/textures/" + short_name; }
	virtual System::String^			source_path				( System::String^ short_name, System::String^ resources_path )	override { UNREACHABLE_CODE(); return resources_path + "/sources/textures/" + short_name; }
	virtual void					load					( System::String^ )				override { UNREACHABLE_CODE();}
	virtual void					save					( )								override;
	virtual void					apply_changes			( ) override;
	virtual void					preview_changes			( ) override									{ UNREACHABLE_CODE(); };
	virtual void					reset_to_default		( ) override;
	virtual void					init_new				( ) override									{ UNREACHABLE_CODE(); };
	virtual void					copy_from				( resource_editor_resource^ )	override	{ UNREACHABLE_CODE(); };

private:
			void						init				( );
			configs::lua_config_value	opts()				{ return (*m_options_config)->get_root()["options"]; }

			static System::Collections::ArrayList			m_type_list;
			static System::Collections::ArrayList			m_dimension_list;
			static System::Collections::ArrayList			m_format_list;
			static System::Collections::ArrayList			m_format_back_list;

public:
		[ReadOnlyAttribute(true)]
		[CategoryAttribute("Common")]
		property System::String^	name
		{
			System::String^ get	( );
		}

		[ReadOnlyAttribute(true)]
		[CategoryAttribute("Common")]
		property System::String^	type
		{
			System::String^ get	( );
		}

		[CategoryAttribute("Common")]
		property System::String^	dimension
		{
			System::String^ get	( );
			void			set	( System::String^ value );
		}

		[CategoryAttribute("Common")]
		property System::String^	format
		{
			System::String^	get	( );
			void			set	( System::String^ value );
		}

		[CategoryAttribute("Common")]
		property bool	has_mip_levels
		{
			bool			get	( );
			void			set	( bool value );
		}

		[CategoryAttribute("Common")]
		property bool	srgb
		{
			bool			get	( );
			void			set	( bool value );
		}

		[CategoryAttribute("Common")]
		property bool	high_quality
		{
			bool			get	( );
			void			set	( bool value );
		}
}; // class texture_editor_resource

} // namespace editor
} // namespace xray

#endif // #ifndef TEXTURE_EDITOR_RESOURCE_H_INCLUDED