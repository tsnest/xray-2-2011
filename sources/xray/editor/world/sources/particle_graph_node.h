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
#include <xray/editor/base/images/images92x25.h>
#include "particle_node_property_holders.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace xray {
	namespace editor {
		
		ref class	particle_graph_node_collection;

		public ref class particle_graph_node : public controls::hypergraph::node, public i_cloneable<particle_graph_node>
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
				DELETE						(m_property_holder);
				DELETE						(m_properties_config);					
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
				m_cursors		= gcnew controls::drag_cursor_collection();
			}

			void in_constructor() new;

		#pragma region Windows Form Designer generated code

			void InitializeComponent(void)
			{
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
			bool								m_enabled;
			particle_graph_node_collection^		m_children;
			Point								m_parent_position_offset;
			::property_holder*					m_property_holder;
			String^								m_config_field;
			particle_editor^					m_parent_editor;
				
		protected:
			xray::editor_base::images92x25		m_image_type;
			particle_graph_node^				m_parent_node;

		public:
		
			bool									m_highlighted;
			configs::lua_config_value*				m_properties_config;
			particle_node_time_parameters_holder^	m_time_parameters_holder;
		
		private:
			static controls::drag_cursor_collection^	m_cursors;
			
		#pragma endregion

		#pragma region | Properties |

		internal:
			property particle_editor^						parent_editor{
				particle_editor^							get(){return m_parent_editor;}
			}
			
			property ::property_holder*						prop_holder{
				::property_holder*							get(){return m_property_holder;};
			}
			property String^								node_config_field{
				String^										get(){return m_config_field;}
				void										set(String^ value){m_config_field = value;}
			}
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
			virtual void					OnKeyDown		(KeyEventArgs^			e) override;
			virtual void					OnKeyUp			(KeyEventArgs^			e) override;

		public:
			virtual particle_graph_node^	clone									();
			void							show_children							();
			void							hide_children							();
			void							process_node_selection					();
			void							recalc_children_position_offsets		();
			void							recalc_parent_position_offset			();
			void							load									(configs::lua_config_value config);
			void							save									(configs::lua_config_value config);
			void							load_node_properties					(configs::lua_config_value config);
						
			void							add_children							(particle_graph_node^ node);
			void							remove_children							(particle_graph_node^ node);
			virtual Boolean					can_accept_node							(particle_graph_node^ node);
			void							set_enabled								(bool enabled);

			void							recalculate_position					(node^ node);
		
			inline bool						enabled									(){return m_enabled;};

			inline bool						is_emitter_node							(){return properties->type == "emitter";}
			inline bool						is_action_node							(){return properties->type == "action";}
			inline bool						is_property_node						(){return properties->type == "property";}
			inline bool						is_event_node							(){return properties->type == "event";}
			inline bool						is_root_node							(){return properties->name == "Root";}
			inline bool						is_particle_group_node					(){return properties->name == "PGroup";}

		private:
			System::Void					hypergraph_node_MouseClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
			void							set_parent_node_impl(particle_graph_node^ node);
		


		#pragma endregion

		}; // ref class particle_graph_node
	}; //namespace editor
}; //namespace xray
#endif // #ifndef PARTICLE_GRAPH_NODE_H_INCLUDED
