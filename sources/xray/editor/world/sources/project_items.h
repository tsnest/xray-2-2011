////////////////////////////////////////////////////////////////////////////
//	Created		: 20.03.2009
//	Author		: AndreW Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROJECT_ITEMS_H_INCLUDED
#define PROJECT_ITEMS_H_INCLUDED

#include "project_defines.h"
#include "attribute.h"

namespace xray {
namespace editor {

class editor_world;

tree_node^ make_hierrarchy		( tree_node_collection^ root, System::String^ name );
tree_node^ find_hierrarchy		( tree_node_collection^ root, System::String^ name );
bool is_valid_object_name		( System::String^ s );

public ref class project_item_base abstract
{
public:
	u32						id						( )					{return m_id;}
							project_item_base		( project^ p);
	virtual					~project_item_base		( );

	virtual void			after_name_edit			( System::String^% new_name )	= 0;
	virtual object_base^	get_object				( )								= 0;
	virtual bool			get_persistent			( )								= 0;
	virtual bool			get_selectable			( )								{ return true; }
	virtual bool			visible_for_project_tree( )								{ return true; }

	virtual void			clear					( bool b_destroy_object )		= 0;
	virtual project_item_base^ get					( System::String^ name )		= 0;
	virtual void			remove					( project_item_base^ itm )		= 0;
	virtual void			add						( project_item_base^ itm )		= 0;

			void			load_base				( configs::lua_config_value const& cfg, bool load_ids );
	virtual void			save					( configs::lua_config_value& cfg, bool save_ids, bool save_guid );

	virtual void			on_selected				( bool b )						= 0;
	virtual System::String^	get_name				( )								= 0;
	virtual void			set_name				( System::String^ )				= 0;

	bool					get_selected			( );
	void					set_selected			( bool b );

	System::String^			get_full_name			( );
	System::String^			get_full_path			( );
	bool					is_child_of				( project_item_base^ pg );
	bool					check_correct_existance	( System::String^% name, u32 index );
	virtual void			get_all_items			( project_items_list^% result_list, bool recursive ) =0;

	virtual void			on_before_manual_delete ( ) { };
	
	tree_node^				m_tree_node_;
	project_item_base^		m_parent;
	bool					m_selected;
	System::Guid			m_guid;
	project^				m_project;
	controls::tree_node_type			m_node_type;

			void					assign_id			( u32 id);
	static 	project_item_base^		object_by_id		( u32 id);
	static	project_item_base^		object_by_full_name	( System::String^ full_name );

private:
	u32 							generate_id			();
	static	System::Collections::Generic::SortedDictionary<u32,project_item_base^>	s_id_to_object;
	static	u32						m_last_generated_id = 0;
	u32								m_id;
};

public ref class project_item_object : public project_item_base
{
	typedef project_item_base	super;
public:
							project_item_object	( project^ p );
							project_item_object	( project^ p, object_base^ object );

	void					load				(	configs::lua_config_value const& root, 
													pcstr table_id,
													string_list^ loaded_names, 
													item_id_list^ loaded_ids, 
													bool load_ids,
													bool load_guid );

	void					load_content		(	configs::lua_config_value const& root, 
													pcstr table_id,
													string_list^ loaded_names, 
													item_id_list^ loaded_ids, 
													bool load_ids,
													bool load_guid,
													bool fill_tree );

	virtual void			save				(	configs::lua_config_value& cfg, 
													bool save_ids, 
													bool save_guid )					override;

	virtual void			get_all_items		( project_items_list^% result_list, bool recursive )	override;
	virtual void			clear				( bool b_destroy_object )				override;
	void					fill_tree_view		( tree_node_collection^ nodes );

	virtual project_item_base^ get				( System::String^ name )				override;
	virtual void			remove				( project_item_base^ itm )				override;
	virtual void			add					( project_item_base^ itm )				override;

	virtual System::String^	get_name			( )										override;
	virtual void			set_name			( System::String^ )						override;

	void					refresh_ui			( );

	virtual object_base^	get_object			( )										override { return m_object_base;}
	virtual void			after_name_edit		( System::String^% new_name)			override;
	virtual void			on_selected			( bool b )								override;
	virtual bool			get_persistent		( )										override;
	virtual bool			get_selectable		( )										override;
	virtual bool			visible_for_project_tree( )									override;
	virtual void			on_before_manual_delete	( ) override;

	object_base^			m_object_base;
	project_items_list^		m_children;
}; // class project_item_object


public ref class project_item_folder :	public project_item_base
{
	typedef project_item_base	super;
public:
							project_item_folder	( project^ p );
	virtual					~project_item_folder( );

			void			load				(	configs::lua_config_value const& root,
													pcstr table_id,
													string_list^ loaded_names, 
													item_id_list^ loaded_ids, 
													bool load_ids,
													bool load_guid );

			void			load_content		(	configs::lua_config_value const& root, 
													configs::lua_config_value const& current, 
													pcstr table_id,
													string_list^ loaded_names, 
													item_id_list^ loaded_ids, 
													bool load_ids,
													bool load_guid,
													bool fill_tree );

	virtual void			save				( configs::lua_config_value& cfg, bool save_ids, bool save_guid )	override;

	virtual void			get_all_items		( project_items_list^% result_list, bool recursive )	override;
	virtual void			clear				(  bool b_destroy_object )				override;
	void					fill_tree_view		( tree_node_collection^ nodes );

	virtual project_item_base^ get				( System::String^ name )		override;
	virtual void			remove				( project_item_base^ itm )		override;
	virtual void			add					( project_item_base^ itm )		override;
	
	void					set_focused			( );

	virtual object_base^	get_object			( )								override		{ return nullptr; }
	virtual void			after_name_edit		( System::String^% new_name )	override;
	virtual void			on_selected			( bool b )						override;
	virtual bool			get_persistent		( )								override;
	virtual bool			get_selectable		( )								override		{ return m_parent!=nullptr; }
	virtual void			on_before_manual_delete	( )							override;

	wpf_controls::property_container^ get_property_container( );

protected :
	void					set_name_internal	( System::String^ s );
public:

	virtual System::String^	get_name			( )						override	{ return m_name; }
	virtual void			set_name			( System::String^ s )	override	{ set_name_internal(s->ToLower()); }

	project_items_list		m_folders;
	project_items_list		m_objects;

private:
	System::String^			m_name;
	bool					m_locked;
public:
	math::float3*			m_pivot;

	[DisplayNameAttribute("name"), DescriptionAttribute("name"), CategoryAttribute("general")]
	[DefaultValueAttribute(""), ReadOnlyAttribute(false), MergablePropertyAttribute(false)]
	property System::String^		folder_name{
		System::String^ 			get()					{return get_name();}
		void						set(System::String^ n);
	}

	[DisplayNameAttribute("lock"), DescriptionAttribute("lock"), CategoryAttribute("general")]
	[DefaultValueAttribute(""), ReadOnlyAttribute(false), MergablePropertyAttribute(false)]
	property bool		locked{
		bool 						get()					{return m_locked;}
		void						set(bool v);
	}

}; // class project_item_folder

public ref class project_background_item
{
public:
	System::String^				m_texture_name;
	System::Drawing::Point		m_position_lt;
	System::Drawing::Bitmap^	m_bitmap;
	int							m_scale;	// meter_per_pixel
	void load					( pcstr resources_path );
	void unload					( );
	static System::String^	get_root_path (pcstr resources_path);
};
typedef System::Collections::Generic::List<project_background_item^>	map_backgrounds;

ref class resource_loading_dialog;
public ref class missing_resource_registry
{
public:
	subst_record^			find				( System::String^ key, System::String^ source );
	void					clear				( );
	System::String^			choose_missing_resource( tool_base^ tool, System::String^ resource_name );
private:
	resource_loading_dialog^	m_dialog;
	typedef System::Collections::Generic::List<subst_record^>						subst_list;
	typedef System::Collections::Generic::Dictionary<System::String^, subst_list^>	subst_dictionary;

	void					register_substitute	( System::String^ key, System::String^ source, System::String^ dest, subst_resource_result result );
	subst_dictionary		m_dictionary;
};

} // namespace editor
} // namespace xray

#endif // #ifndef PROJECT_ITEMS_H_INCLUDED