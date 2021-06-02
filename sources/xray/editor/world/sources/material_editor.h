////////////////////////////////////////////////////////////////////////////
//	Created		: 23.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MATERIAL_EDITOR_H_INCLUDED
#define MATERIAL_EDITOR_H_INCLUDED

#include "resource_editor.h"
#include <xray/render/facade/model.h>
#include <xray/render/engine/base_classes.h>

using xray::resources::queries_result;
using System::Collections::ArrayList;

namespace xray {

namespace render{
	class world; 

	namespace editor {
		class renderer;
	} // namespace editor
} // namespace render

namespace editor {

	ref class	material_editor_source;
	ref class	resource_editor_resource;
	ref class	material;
	
	enum class material_editor_mode{ edit_material, edit_instances, unknown };
	ref class	material_editor: public resource_editor
	{
		typedef resource_editor super;
	#pragma region | Initialize |


	public:
		material_editor( editor_world& world, material_editor_mode mode, bool selector_mode )
		:resource_editor	( world, selector_mode ),
		m_mode				( mode ),
		m_load_preview_model_query( NULL ),
		m_load_preview_material_query( NULL ),
		m_auto_refresh		( true ),
		m_global_auto_refresh ( false )
		{
			in_constructor	( );
		}
		virtual ~material_editor	( );
	private:
		void in_constructor();


	#pragma endregion

	#pragma region |   Fields   |

	public:
		editor_base::scene_view_panel^		m_view_window;

	private:
		
		material_editor_source^				m_materials_source;
		controls::file_view_panel_base^		m_material_view_panel;
		xray::editor_base::input_engine^	m_input_engine;
		editor_base::gui_binder^			m_gui_binder;
		material_editor_mode				m_mode;
		bool								m_auto_refresh;
		bool								m_global_auto_refresh;
		render::static_model_ptr*			m_preview_model;
		render::static_model_ptr*			m_model_sphere;
		render::static_model_ptr*			m_model_cube;

		ToolStripMenuItem^					m_add_material_menu_item;
		ToolStripMenuItem^					m_remove_material_menu_item;
		ToolStripMenuItem^					m_rename_material_menu_item;
		ToolStripMenuItem^					m_clone_material_menu_item;
		ToolStripMenuItem^					m_copy_material_menu_item;
		ToolStripMenuItem^					m_paste_material_menu_item;
		ToolStripMenuItem^					m_create_folder_menu_item;


	#pragma endregion

	#pragma region | Properties |


		render::editor::renderer*			m_editor_renderer;
		render::scene_ptr*					m_scene;
		render::scene_view_ptr*				m_scene_view;
		render::render_output_window_ptr*	m_output_window;
		query_result_delegate*				m_load_preview_model_query;
		query_result_delegate*				m_load_preview_material_query;

	protected:
		virtual property controls::tree_view^ resources_tree_view 
		{
			controls::tree_view^			get( )	override
			{
				return m_material_view_panel->tree_view;
			}
		}

		virtual property int		resource_image_index
		{
			int get		( )		override
			{
				return xray::editor_base::node_material;
			}
		}
		virtual property int		modified_resource_image_index
		{
			int get		( ) override
			{
				return xray::editor_base::node_material_modified;
			}
		}
		
	public:
		virtual void				on_resource_changed	( ) override;
		virtual void				tick			( ) override;
		virtual void				clear_resources	( ) override;
		virtual void				load_settings			( RegistryKey^ product_key ) override;
		virtual void				save_settings			( RegistryKey^ product_key ) override;

		virtual property System::String^ selected_name
		{
			System::String^ get	( ) override;
			void			set	( System::String^ value ) override;
		}
	#pragma endregion

	#pragma region |   Methods  |


	private:
		void						query_preview_models			( );
		void						preview_models_ready			( resources::queries_result& data );
		void						set_preview_model				( render::static_model_ptr v );

		void						action_refresh_preview_material	( );
		void						action_preview_model_cube		( );
		bool						is_preview_model_cube			( );
		void						action_preview_model_sphere		( );
		bool						is_preview_model_sphere			( );
		void						action_setup_current_diffuse_texture( );
		bool						is_setup_current_diffuse_texture( );

		void						action_auto_refresh				( );
		void						global_action_auto_refresh		( );
		bool						is_auto_refresh					( );
		bool						is_global_auto_refresh			( );

		void						preview_material_ready			( resources::queries_result& data );
		
		void						on_render_resources_ready		( resources::queries_result& data );
		void						query_create_render_resources	( );
		
		void						on_resource_selected			( Object^ sender,controls::tree_view_selection_event_args^ e );
		void						add_tool_stip_item_click		( Object^ sender,System::EventArgs^ e );
		void						item_label_renamed				( Object^ sender, NodeLabelEditEventArgs^ e );
		void						remove_tool_stip_item_click		( Object^ sender,System::EventArgs^ e );
		void						rename_tool_stip_item_click		( Object^ sender,System::EventArgs^ e );
		void						clone_tool_stip_item_click		( Object^ sender,System::EventArgs^ e );
		void						copy_tool_stip_item_click		( Object^ sender,System::EventArgs^ e );
		void						paste_tool_stip_item_click		( Object^ sender,System::EventArgs^ e );
		void						create_folder_tool_stip_item_click( Object^ sender,System::EventArgs^ e );
		void						load_all_resources				( Object^ sender,System::EventArgs^ e );

	protected:
		System::String^				validate_resource_name			( System::String^ name_to_check );
		virtual bool				load_panels_layout				( ) override;
		virtual IDockContent^		layout_for_panel				( System::String^ panel_name ) override;
		virtual document_base^		create_new_document				( ) override;
		virtual void				on_menu_opening					( System::Object^ sender, System::ComponentModel::CancelEventArgs^ e ) override;
		resource_editor_resource^	create_resource					( );
		resource_editor_resource^	select_resource_by_name			( System::String^ fullpath );

	public:
		void						on_resource_loaded				( resource_editor_resource^ data, bool is_correct );
		void						on_resource_changed				( resource_editor_resource^ data );
	#pragma endregion

	}; // class material_editor

	ref class material_chooser: public editor_base::chooser
	{
	public:
								material_chooser( editor_world& w, material_editor_mode mode );
		virtual System::String^	id				( );
		virtual bool			choose			( System::String^ current, System::String^ filter, System::String^% result, bool read_only );

	private:
		material_editor_mode	m_mode;
		editor_world&			m_editor_world;
	};

} // namespace editor
} // namespace xray

#endif // #ifndef MATERIAL_EDITOR_H_INCLUDED
