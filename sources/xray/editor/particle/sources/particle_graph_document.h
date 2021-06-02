////////////////////////////////////////////////////////////////////////////
//	Created		: 19.01.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_GRAPH_H_INCLUDED
#define PARTICLE_GRAPH_H_INCLUDED

#include "particle_hypergraph.h"
#include "particle_data_sources_holder.h"
#include <xray/render/facade/particles.h>
#include <xray/render/engine/base_classes.h>

using namespace System;
using namespace System::ComponentModel;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

using namespace xray::editor::controls;

namespace xray
{
	namespace particle_editor
	{
		ref class particle_editor;
		ref class particle_graph_node;
		ref class particle_links_manager;
		ref class particle_hypergraph;
		ref class particle_data_sources_panel;
		ref class xray::editor::controls::hypergraph::node;
		ref class xray::editor::controls::hypergraph::link;
		/// <summary>
		/// Summary for particle_graph_document
		/// </summary>
		public ref class particle_graph_document : public xray::editor::controls::document_base
		{
			typedef xray::editor::controls::document_base super;
			typedef Generic::List<xray::editor::controls::hypergraph::node^ >	nodes;
			typedef Generic::List<xray::editor::controls::hypergraph::link^ >	links;
			
		#pragma  region | initialize |

		public:
			particle_graph_document(document_editor_base^ d, particle_editor^ editor):
			  document_base(d), m_particle_editor(editor)
			{
				InitializeComponent();
				//
				//TODO: Add the constructor code here
				//
				in_constructor();
			}

		protected:
			/// <summary>
			/// Clean up any resources being used.
			/// </summary>
			~particle_graph_document()
			{
				//CRT_DELETE(m_particle_system_ptr);
				if (components)
				{
					delete components;
				}
			}

		private:
			void in_constructor();
			/// <summary>
			/// Required designer variable.
			/// </summary>
			System::ComponentModel::Container ^components;

		#pragma region Windows Form Designer generated code
			/// <summary>
			/// Required method for Designer support - do not modify
			/// the contents of this method with the code editor.
			/// </summary>
			void InitializeComponent(void)
			{
				this->SuspendLayout();
				// 
				// particle_graph_document
				// 
				this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
				this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				this->ClientSize = System::Drawing::Size(284, 264);
				this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(204)));
				this->Name = L"particle_graph_document";
				this->Text = L"Paricle Graph";
				this->ResumeLayout(false);
			}

		#pragma endregion

		#pragma  endregion

		#pragma  region |   Fields   |

		private:
			static String^								c_extension = ".particle";

			particle_editor^							m_particle_editor;
			particle_hypergraph^						m_hypergraph;
			particle_links_manager^						m_links_manager;
			particle_data_sources_holder^				m_data_sources;
			bool										m_is_lod_document;
			String^										m_lod_name;
			xray::particle::particle_system**			m_particle_system_ptr;
			math::float4x4*								m_particle_system_root_matrix;
						
			Collections::Generic::List<xray::editor::controls::hypergraph::node^>^		m_cuted_nodes;
			
			Boolean									m_has_name;

			particle_graph_node^											m_focused_node;
			Collections::Generic::List<editor_base::property_visualizer^>	m_prop_visualizers;
		#pragma  endregion

		#pragma  region | Properties |
		public:
			property math::float4x4					particle_system_root_matrix
			{
				math::float4x4						get(){return *m_particle_system_root_matrix;}
			}
			property String^						lod_name{
				String^								get(){return m_lod_name;}
				void								set(String^ value){m_lod_name = value;}
			}
			property particle_hypergraph^			hypergraph{
				particle_hypergraph^				get(){return m_hypergraph;}
			}
			property particle_editor^				parent_editor{
				particle_editor^					get(){return m_particle_editor;}
			}
			property particle_data_sources_holder^	data_sources{
				particle_data_sources_holder^		get(){return m_data_sources;}
			}
			property bool							is_lod_document{
				bool								get(){return m_is_lod_document;}
			}
			property String^						document_extension{
				String^								get(){return c_extension;}
			}
			property xray::particle::particle_system**	particle_system_ptr {
				xray::particle::particle_system**		get(){return m_particle_system_ptr;}
			}
		
		#pragma  endregion

		#pragma  region |   Methods  |

		public:
			void						create_root_node		(Object^ sender, EventArgs^ e);

			xray::editor::controls::hypergraph::link^	create_connection		(xray::editor::controls::hypergraph::node^ source, xray::editor::controls::hypergraph::node^ destination);
			void						add_connection			(xray::editor::controls::hypergraph::node^ source, xray::editor::controls::hypergraph::node^ destination);
			void						add_connection			(links^ lnks);
			particle_graph_node^		make_node				();
			
			
			virtual void				save() override;
			void						load_from_config(configs::lua_config_ptr config);
			virtual void				load_partialy(String^ part_path) override;
			virtual	void				undo() override;
			virtual	void				redo() override;
			
			void						on_document_loaded		(xray::resources::queries_result& data);
			void						on_selection_changed	();
			void						reload_document			();
			virtual void				on_form_activated		(System::Object^ sender, System::EventArgs^ e) override;
			virtual void				on_form_closing			(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e) override;

			virtual void				copy					(Boolean is_cut) override;
			virtual void				paste					() override;
			virtual void				del						() override;
			virtual String^				get_document_name		(String^ full_name) override;

			void						on_restart_particle_system();
			void						set_looping_preview_particle_system( bool looping );
			u32							get_num_info_fields();
			void						gather_statistics(xray::particle::preview_particle_emitter_info* out_buffer);

			void						particle_system_property_changed		(particle_graph_node^ node, xray::configs::lua_config_value& value);
			void						particle_system_node_added				(particle_graph_node^ node, xray::configs::lua_config_value& value);
			void						particle_system_node_removed			(particle_graph_node^ node );
			void						particle_system_property_order_changed	(String^ node_id0, String^ node_id1);
			void						draw									( );
			void						root_matrix_modified					( cli::array<System::String^>^ );
			void						node_property_modified					( cli::array<System::String^>^ );
			void						setup_editors_and_visualizers			( particle_graph_node^ n );
		#pragma  endregion

		};
	}//namespace particle_editor
}//namespace xray
#endif // #ifndef PARTICLE_GRAPH_H_INCLUDED