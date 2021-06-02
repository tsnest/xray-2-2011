////////////////////////////////////////////////////////////////////////////
//	Created		: 01.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef RENDER_OPTIONS_EDITOR_H_INCLUDED
#define RENDER_OPTIONS_EDITOR_H_INCLUDED

#include "resource_editor.h"
#include <xray/render/engine/base_classes.h>
#include "render_options_editor_source.h"

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

	//ref class	material_editor_source;
	ref class	resource_editor_resource;
	ref class render_options_editor_source;

	ref class	render_options_editor: public resource_editor
	{
		typedef resource_editor super;
	#pragma region | Initialize |


	public:
		render_options_editor( editor_world& world );
	
		virtual ~render_options_editor	( );
	private:
		void in_constructor();

	#pragma endregion

	#pragma region |   Fields   |

	private:
		render_options_editor_source^		m_render_options_source;
		controls::file_view_panel_base^		m_render_options_view_panel;
		ToolStripMenuItem^					m_clone_material_menu_item;
	#pragma endregion

	#pragma region | Properties |

		render::editor::renderer*			m_editor_renderer;
	public:
		property controls::file_view_panel_base^	render_options_panel
		{
			controls::file_view_panel_base^		get( )	{return m_render_options_view_panel;}
		}
	protected:

		virtual property controls::tree_view^ resources_tree_view 
		{
			controls::tree_view^			get( )	override
			{
				return m_render_options_view_panel->tree_view;
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
	#pragma endregion

	#pragma region |   Methods  |
	public:
		virtual void					tick						( ) override;
		virtual void					on_resource_changed			( ) override;
	private:
		void						form_closed						( Object^ sender, FormClosedEventArgs^ e );
		void						on_resource_selected			( Object^ sender,controls::tree_view_selection_event_args^ e );
	protected:
		virtual document_base^		create_new_document				( ) override;
		virtual bool				load_panels_layout				( ) override;
		System::String^				validate_resource_name			( System::String^ name_to_check );
		resource_editor_resource^	create_resource					( System::String^ in_name );
	public:
		virtual property System::String^ selected_name
		{
			System::String^ get	( ) override;
			void			set	( System::String^ value ) override;
		}
		void						on_resource_changed				( resource_editor_resource^ data );
		void						on_resource_loaded				( resource_editor_resource^ resource, bool is_correct );
		void						clone_tool_stip_item_click		( Object^ , System::EventArgs^ );
	#pragma endregion

	}; // class material_editor

} // namespace editor
} // namespace xray

#endif // #ifndef RENDER_OPTIONS_EDITOR_H_INCLUDED