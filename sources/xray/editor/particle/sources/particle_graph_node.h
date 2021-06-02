////////////////////////////////////////////////////////////////////////////
//	Created		: 21.01.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_GRAPH_NODE_H_INCLUDED
#define PARTICLE_GRAPH_NODE_H_INCLUDED

#include "particle_editor.h"
#include "particle_node_properties.h"
#include "particle_graph_node_style.h"
#include "particle_hypergraph.h"
#include "images/images92x25.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace xray {
	namespace particle_editor {

		ref class particle_graph_node_event;
		ref class particle_graph_node_action;
		ref class particle_graph_node_property;
		ref class particle_graph_node_emitter;
		
		ref class	particle_graph_node_collection;

		public ref class particle_graph_node : public xray::editor::controls::hypergraph::node, public i_cloneable<particle_graph_node>
		{

		#pragma region | Initialize |

		public:
			particle_graph_node(particle_editor^ editor):
			m_parent_editor(editor)
			{
				InitializeComponent();
				in_constructor();
			}

			!particle_graph_node()
			{
				CRT_DELETE(m_node_config);
				delete properties;
			}
			~particle_graph_node()
			{
				if(components)
					delete components;
				this->!particle_graph_node();
			}

		private:
			static particle_graph_node()
			{
				m_cursors		= gcnew xray::editor::controls::drag_cursor_collection();
			}

			void in_constructor() new;

		#pragma region Windows Form Designer generated code

			void InitializeComponent(void)
			{
				this->DoubleBuffered = true;
				this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				this->Size = System::Drawing::Size(120,65);		
				this->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &particle_graph_node::hypergraph_node_MouseClick);
			}

		#pragma endregion

		private:
			System::ComponentModel::Container^	components;

		#pragma endregion

		#pragma region |   Fields   |

		private:
			Drawing::Point						m_last_mouse_down_position;
			Boolean								m_is_mouse_down;
			Boolean								m_is_on_graph;
			Single								m_drag_start_distance;
			particle_graph_node^				m_active_node;
			particle_graph_node_collection^		m_children;
			Point								m_parent_position_offset;
		
			
				
		protected:
			particle_editor^					m_parent_editor;
			particle_graph_node^				m_parent_node;	
			configs::lua_config_value*			m_node_config;
					
		public:
		
			bool								m_highlighted;

		
		private:
			static xray::editor::controls::drag_cursor_collection^	m_cursors;
			
		#pragma endregion

		#pragma region | Properties |

		internal:
			property particle_editor^						parent_editor{
				particle_editor^							get(){return m_parent_editor;}
			}
			
// 			property xray::editor_base::property_holder_wrapper^	prop_holder{
// 				xray::editor_base::property_holder_wrapper^			get(){return m_property_holder;};
// 			}
			
			property particle_node_properties^				properties;
			property particle_graph_node^					active_node 
			{
				particle_graph_node^		get(){return m_active_node;}
				void						set(particle_graph_node^ node){m_active_node = node;}
			}
			property particle_graph_node_collection^		children
			{
				particle_graph_node_collection^ get(){return m_children;}
			}

			property particle_hypergraph^					parent_graph
			{		
				particle_hypergraph^			get(){return safe_cast<particle_hypergraph^>(m_parent);}
			}

			property particle_graph_node^					parent_node {
				particle_graph_node^		get(){return m_parent_node;}
				void						set(particle_graph_node^ node){set_parent_node_impl(node);}
			}

			property particle_graph_document^				parent_document{
				particle_graph_document^					get();
			}

			property Boolean								is_on_graph
			{
				Boolean		get(){return m_is_on_graph;}
				void		set(Boolean value){m_is_on_graph = value;}
			}
			property particle_graph_node_style^ style{
				particle_graph_node_style^	get(){return safe_cast<particle_graph_node_style^>(m_style);}
				void						set(particle_graph_node_style^ value){m_style = value;}
			}		

			property String^				particle_entity_type{
				String^ get(){
					return gcnew String((*m_node_config)["particle_entity_type"]);
				}
				void set(String^ value){
					(*m_node_config)["particle_entity_type"] = unmanaged_string(value).c_str();
				}
			}
			property String^				parent_lod_name{
				String^ get(){
					if (!(*m_node_config).value_exists("parent_lod_name"))
						return nullptr;
					return gcnew String((*m_node_config)["parent_lod_name"]);
				}
				void set(String^ value){
					(*m_node_config)["parent_lod_name"] = unmanaged_string(value).c_str();
				}
			}

			property String^								node_config_field{
				String^ get(){
					return gcnew String((*m_node_config)["node_config_field"]);
				}
				void set(String^ value){
					(*m_node_config)["node_config_field"] = unmanaged_string(value).c_str();
				}
			}
			
			[DisplayNameAttribute("Name")]
			[CategoryAttribute("General")]
			
			property String^								node_text{
				String^ get(){
				//	m_node_config->save_as("d:/node_name");
					return gcnew String((*properties->config)["Name"]);
				}
				void set(String^ value){
					(*properties->config)["Name"] = unmanaged_string(value).c_str();
					if (parent_hypergraph != nullptr)
						parent_hypergraph->Invalidate(true);
				}
			}
			
			[DisplayNameAttribute("Enabled")]
			[CategoryAttribute("General")]
			
			property bool								enabled{
				bool get(){
					return bool((*properties->config)["Enabled"]);
				}
				void set(bool value);
			}

			property images92x25						image_type
			{
				images92x25 get(){
					return (images92x25)(int)((*m_node_config)["image_type"]);
				}
				void set(images92x25 value){
					(*m_node_config)["image_type"] = int(value);
				}
			}

			property configs::lua_config_value* node_config 
			{
				configs::lua_config_value*	get(){return m_node_config;}
			}

		public:
			[ReadOnlyAttribute(true)]
			[DisplayNameAttribute("GUID")]
			[CategoryAttribute("General")]
			property String^	id {
				String^ get(){
					return gcnew String((*m_node_config)["id"]);
				}
				void set(String^ value){
					if (value == nullptr)
						return;
					(*m_node_config)["id"] = unmanaged_string(value).c_str();
				}
			}
		
		#pragma endregion

		#pragma region |  Methods   |

		protected:
			virtual void					OnMouseMove		(MouseEventArgs^		e) override;
			virtual void					OnMouseDown		(MouseEventArgs^		e) override;
			virtual void					OnMouseUp		(MouseEventArgs^		e) override;
			virtual void					OnDragOver		(DragEventArgs^			e) override;
			virtual void					OnDragEnter		(DragEventArgs^			e) override;
			virtual void					OnDragLeave		(EventArgs^				e) override;
			virtual void					OnDragDrop		(DragEventArgs^			e) override;
			virtual void					OnGiveFeedback	(GiveFeedbackEventArgs^ e) override;
			virtual void					OnPaint			(PaintEventArgs^		e) override;
		
		public:
			virtual particle_graph_node^	clone									( );
			void							show_children							( );
			void							hide_children							( );
			void							process_node_selection					( );
			void							recalc_children_position_offsets		( );
			void							recalc_parent_position_offset			( );
			void							get_initialize_config					( configs::lua_config_value config );
			void							load									( configs::lua_config_value config );
			void							clone_from_config						( configs::lua_config_value config );
			void							save									( configs::lua_config_value config );
			void							save_with_only_default_inheritance		( configs::lua_config_value config );
			
								
			void							add_children							( particle_graph_node^ node );
			void							remove_children							( particle_graph_node^ node );
			virtual Boolean					can_accept_node							( particle_graph_node^ node );
			void							show_properties							( );
			void							check_for_reload_lower_lodes			( );
			void							on_new_node_added						( particle_graph_node^ node , int index );
			void							on_node_removed							( particle_graph_node^ node );
			void							on_change_node_index					( String^ moved_node_id, String^ moved_to_node_id );
			void							notify_render_on_property_changed		(cli::array<System::String^>^ names);

			void							recalculate_position					( node^ node );
		
			inline bool		is_emitter_node							(){return this->GetType() == particle_graph_node_emitter::typeid;} 
			inline bool		is_action_node							(){return this->GetType() == particle_graph_node_action::typeid;} 
			inline bool		is_property_node						(){return this->GetType() == particle_graph_node_property::typeid;} 
			inline bool		is_event_node							(){return this->GetType() == particle_graph_node_event::typeid;}

			inline bool		is_root_node							(){return particle_entity_type == "particle_entity_root_type";}

			inline bool		is_particle_group_node					(){return particle_entity_type == "particle_entity_particle_group_type";}

		private:
			System::Void					hypergraph_node_MouseClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
			void							set_parent_node_impl(particle_graph_node^ node);
		


		#pragma endregion

		}; // ref class particle_graph_node
	}; //namespace particle_editor
}; //namespace xray
#endif // #ifndef PARTICLE_GRAPH_NODE_H_INCLUDED
