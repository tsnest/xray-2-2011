////////////////////////////////////////////////////////////////////////////
//	Created		: 30.04.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_DATA_SOURCE_NODE_H_INCLUDED
#define PARTICLE_DATA_SOURCE_NODE_H_INCLUDED

#include "particle_graph_node.h"
#include "particle_editor.h"

namespace xray {
	namespace editor {

		public ref class particle_data_source_fc_node : public particle_graph_node{
			typedef particle_graph_node super;
		public:
			particle_data_source_fc_node(particle_editor^ editor):particle_graph_node(editor){
			}
			 
		
		public:
			property float_curve^			fc;

		private:
		protected:
			virtual void					OnMouseMove		(MouseEventArgs^		) override{};
			virtual void					OnMouseDown		(MouseEventArgs^		) override{};
			virtual void					OnMouseUp		(MouseEventArgs^		) override{};
			virtual void					OnDragOver		(DragEventArgs^			) override{};
			virtual void					OnDragEnter		(DragEventArgs^			) override{};
			virtual void					OnDragLeave		(EventArgs^				) override{};
			virtual void					OnDragDrop		(DragEventArgs^			) override{};
			virtual void					OnGiveFeedback	(GiveFeedbackEventArgs^ ) override{};
			virtual void					OnPaint			(PaintEventArgs^		) override;
			virtual void					OnKeyDown		(KeyEventArgs^			) override{};
			virtual void					OnKeyUp			(KeyEventArgs^			) override{};
	

		public:
			virtual	Boolean					can_accept_node(particle_graph_node^ ) override {return false;};
			void							curve_changed( System::Object^, xray::editor::float_curve_event_args^ );

		}; // class particle_data_source_fc_node


		public ref class particle_data_source_cc_node : public particle_graph_node{
			typedef particle_graph_node super;
		public:
			particle_data_source_cc_node(particle_editor^ editor):particle_graph_node(editor){
			}


		public:
			property color_curve^			cc;

		private:
		protected:
			virtual void					OnMouseMove		(MouseEventArgs^		) override{};
			virtual void					OnMouseDown		(MouseEventArgs^		) override{};
			virtual void					OnMouseUp		(MouseEventArgs^		) override{};
			virtual void					OnDragOver		(DragEventArgs^			) override{};
			virtual void					OnDragEnter		(DragEventArgs^			) override{};
			virtual void					OnDragLeave		(EventArgs^				) override{};
			virtual void					OnDragDrop		(DragEventArgs^			) override{};
			virtual void					OnGiveFeedback	(GiveFeedbackEventArgs^ ) override{};
			virtual void					OnPaint			(PaintEventArgs^		) override;
			virtual void					OnKeyDown		(KeyEventArgs^			) override{};
			virtual void					OnKeyUp			(KeyEventArgs^			) override{};


		public:
			virtual	Boolean					can_accept_node(particle_graph_node^ ) override {return false;};
			void							curve_changed();

		}; // class particle_data_source_cc_node
	} // namespace editor
} // namespace xray

#endif // #ifndef PARTICLE_DATA_SOURCE_NODE_H_INCLUDED