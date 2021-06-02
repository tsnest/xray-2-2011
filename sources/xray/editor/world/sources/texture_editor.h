////////////////////////////////////////////////////////////////////////////
//	Created		: 15.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TEXTURE_EDITOR_H_INCLUDED
#define TEXTURE_EDITOR_H_INCLUDED

#include "resource_editor.h"
#include "texture_editor_source.h"

namespace xray
{
	namespace editor
	{
		ref class texture_conversion_options_panel;
		ref class texture_editor_resource;

		typedef xray::resources::unmanaged_resource_ptr unmanaged_resource_ptr;

		public ref class texture_editor: public resource_editor
		{
			typedef resource_editor super;

		#pragma region | Initialize |


		public:
			texture_editor(editor_world& world)
			:resource_editor(world, false), 
			m_textures_source(gcnew texture_editor_source())
			{
				Name			= "texture_editor";
				Text			= "Texture Editor";
				in_constructor	( );
			}

			texture_editor(editor_world& world,  Object^ filter, bool is_run_as_selector)
			:resource_editor	( world, is_run_as_selector ), 
			m_textures_source	( gcnew texture_editor_source(filter) )
			{
				Name			= "texture_editor";
				Text			= "Texture Editor";
				in_constructor	( );
			}

		private:
			static texture_editor()
			{
				s_chessboard			= nullptr;
				s_alive_editors			= 0;
			}
			void in_constructor();


		#pragma endregion

		#pragma region |   Fields   |


		private:
			texture_editor_source^						m_textures_source;
			controls::file_view_panel_base^				m_texture_view_panel;
			void on_resource_changed					( resource_editor_resource^ data );
			void on_resource_loaded						( resource_editor_resource^ resource, bool is_correct );

		public:
			static Bitmap^								s_chessboard;
			static int									s_alive_editors;


		#pragma endregion

		#pragma region | Properties |


		protected:
			virtual property controls::tree_view^	resources_tree_view 
			{
				controls::tree_view^		get( )	override
				{
					return m_texture_view_panel->tree_view;
				}
			}
			virtual property int					resource_image_index
			{
				int							get( ) override
				{
					return xray::editor_base::node_texture;
				}
			}
			virtual property int					modified_resource_image_index
			{
				int							get( ) override
				{
					return xray::editor_base::node_texture_modified;
				}
			}

		public:
			property controls::file_view_panel_base^	textures_panel
			{
				controls::file_view_panel_base^		get( )	{return m_texture_view_panel;}
			}

			virtual property System::String^		selected_name
			{
				System::String^			get( ) override;
				void					set( System::String^ value	) override;
			}

		#pragma endregion

		#pragma region |   Methods  |

		public:
		virtual void					tick							( ) override;

		private:
			void						on_resource_selected			( Object^ sender,controls::tree_view_selection_event_args^ e );
			static void					create_chessboard				( );
			static void					distroy_chessboard				( );
			void						form_closed						( Object^ sender, FormClosedEventArgs^ e );

		protected:
			virtual bool				load_panels_layout				( ) override;
			virtual IDockContent^		layout_for_panel				( System::String^ panel_name ) override;
			virtual document_base^		create_new_document				( ) override;
			virtual void				on_menu_opening					( System::Object^ sender, System::ComponentModel::CancelEventArgs^ e ) override;


		#pragma endregion

		}; // class texture_editor

		struct texture_editor_ptr
		{
			texture_editor_ptr(editor_world& w){
					m_editor = gcnew texture_editor(w, nullptr, true);
			}
			texture_editor_ptr(editor_world& w, gcroot<System::Object^> filter){
					m_editor = gcnew texture_editor(w, filter, true);
			}

			gcroot<texture_editor^> m_editor;

			~texture_editor_ptr()
			{
				delete m_editor;
			}

			texture_editor^ operator ->()
			{
				return m_editor;
			}
		};//struct texture_editor_ptr

	} // namespace editor
} // namespace xray

#endif // #ifndef TEXTURE_EDITOR_H_INCLUDED