////////////////////////////////////////////////////////////////////////////
//	Created		: 19.01.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_GRAPH_H_INCLUDED
#define PARTICLE_GRAPH_H_INCLUDED

#include "particle_hypergraph.h"
#include "particle_data_source_nodes.h"
#include "particle_data_sources_holder.h"


using namespace System;
using namespace System::ComponentModel;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

using namespace xray::editor::controls;

namespace xray
{
	namespace editor
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
		public ref class particle_graph_document : public controls::document_base
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
			static String^							c_extension = ".particle";

			particle_editor^						m_particle_editor;
			particle_hypergraph^					m_hypergraph;
			particle_links_manager^					m_links_manager;
			particle_data_sources_holder^			m_data_sources;
		
			
			Collections::Generic::List<controls::hypergraph::node^>^		m_cuted_nodes;
			
			Boolean									m_has_name;

		#pragma  endregion

		#pragma  region | Properties |
		public:
			property particle_hypergraph^			hypergraph{
				particle_hypergraph^				get(){return m_hypergraph;}
			}
			property particle_editor^				parent_editor{
				particle_editor^					get(){return m_particle_editor;}
			}
			property particle_data_sources_holder^	data_sources{
				particle_data_sources_holder^		get(){return m_data_sources;}
			}

		
		#pragma  endregion

		#pragma  region |   Methods  |

		public:
			void						create_root_node		(Object^ sender, EventArgs^ e);

			controls::hypergraph::link^	create_connection		(controls::hypergraph::node^ source, controls::hypergraph::node^ destination);
			void						add_connection			(controls::hypergraph::node^ source, controls::hypergraph::node^ destination);
			void						add_connection			(links^ lnks);
			particle_graph_node^		make_node				();
			
			virtual void				save() override;
			virtual void				load() override;
			virtual	void				undo() override;
			virtual	void				redo() override;

			void						on_document_loaded		(xray::resources::queries_result& data);
			void						on_selection_changed	();
			virtual void				on_form_activated		(System::Object^ sender, System::EventArgs^ e) override;
			virtual void				on_form_closing			(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e) override;

			virtual void				copy					(Boolean is_cut) override;
			virtual void				paste					() override;
			virtual void				del						() override;
			virtual String^				get_document_name		(String^ full_name) override;

		#pragma  endregion

		};
	}//namespace editor
}//namespace xray
#endif // #ifndef PARTICLE_GRAPH_H_INCLUDED