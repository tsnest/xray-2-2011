////////////////////////////////////////////////////////////////////////////
//	Created		: 09.04.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_COMPOSITE_H_INCLUDED
#define OBJECT_COMPOSITE_H_INCLUDED

#include "object_base.h"

namespace xray {
namespace editor {

ref class tool_composite;
ref class object_scene;


typedef System::Collections::Generic::Dictionary<System::String^, bool>		composite_object_item_public_props;
typedef System::Collections::Generic::KeyValuePair<System::String^, bool>	composite_object_item_public_props_it;

public ref class composite_object_item
{
public:
						composite_object_item	( );
						~composite_object_item	( );

	void				set_edit_transform		( math::float4x4 const& m );
	void				do_edit_transform		( );

	object_base^						m_object;
	math::float4x4*						m_relative_transform;
	composite_object_item_public_props^	m_props_dictionary;
};

typedef System::Collections::Generic::List<composite_object_item^>		composite_object_item_list;

public ref class object_composite :public object_base
{
	typedef object_base		super;
public:
					object_composite			( tool_composite^ t );
	virtual			~object_composite		( );


	virtual void	load_props				( configs::lua_config_value const& t )	override;
	virtual void	save					( configs::lua_config_value t )			override;
	virtual void	render					( )										override;
	virtual void	on_selected				( bool b )								override;
	virtual	void	set_visible				( bool bvisible )						override;
	virtual void	set_transform			( math::float4x4 const& transform )		override;
	virtual void	destroy_collision		( )										override;
			void	save_as_library			( configs::lua_config_value& t );
			void	load_from_library		( configs::lua_config_value const& t );
			void	re_load_lib				( );

	virtual void	load_contents			( )	override;
	virtual void	unload_contents			( bool bdestroy ) override;
	virtual bool	get_persistent			( )	override;
	virtual enum_terrain_interaction get_terrain_interaction( )						override	{return enum_terrain_interaction::tr_vertical_snap;}

	virtual wpf_controls::property_container^	get_property_container			( )			override;
			wpf_controls::property_container^	get_object_property_container	( object_base^ o );
			void								get_public_properties_container	( wpf_controls::property_container^ to_container );

			void	switch_to_edit_mode		( bool value );
			bool	is_edit_mode			( );

			void	edit_add_object			( object_base^ o );
			void	edit_remove_object		( object_base^ o, bool destroy );

			void	destroy_contents		( );

			bool	touch_object			( object_base^ o );

			void	update_object_offset	( composite_object_item^ itm );// transform -> offset
			void	update_object_transform	( composite_object_item^ itm );// offset -> transform

			object_base^ get_child_by_name	( System::String^ name );

	composite_object_item_list^	m_objects;
private:
			void	add_object				( object_base^ o, math::float4x4* offset );
			void	setup_object_props		( composite_object_item^ itm );
			void	update_objects_transform( );
			void	initialize_collision	( );
			int		index_of				( object_base^ o );
			void	content_transform_changed_notify( object_base^ o );
			

	tool_composite^				m_tool_composite;

public:
	property object_scene^		logic_scene;
}; // class object_composite

} //namespace editor
} // namespace xray

#endif // #ifndef OBJECT_COMPOSITE_H_INCLUDED
