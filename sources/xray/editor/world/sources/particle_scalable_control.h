////////////////////////////////////////////////////////////////////////////
//	Created		: 06.04.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_SCALABLE_CONTROL_H_INCLUDED
#define PARTICLE_SCALABLE_CONTROL_H_INCLUDED

#include "particle_graph_node.h"
#include "float_curve.h"



using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray {
namespace editor {

	ref class particle_scalable_control: public Windows::Forms::UserControl {
public:
	particle_scalable_control(particle_graph_node^ node):m_parent_node(node)
	{
		in_constructor();
	}	

private:

#pragma region | InnerTypes |

#pragma endregion

#pragma region | Initialize |
	private:
		void							in_constructor();
#pragma endregion

#pragma region | Fields |
	private:
		particle_graph_node^			m_parent_node;
		int								m_start_time;
		int								m_length_time;
		int								m_total_time;
		Drawing::Rectangle				m_drawing_rectangle;
		Drawing::Point					m_last_mouse_down_position;
		bool							m_b_left_scale_mode;	
		bool							m_b_right_scale_mode;
		bool							m_b_move_mode;		
#pragma endregion

#pragma region | Methods |
	private:
		void							on_mouse_move			(Object^ , MouseEventArgs^ e);
		void							on_mouse_down			(Object^ , MouseEventArgs^ e);
		void							on_mouse_up				(Object^ , MouseEventArgs^);
		void							on_mouse_enter			(Object^ , EventArgs^);
		void							on_mouse_leave			(Object^ , EventArgs^);
		void							on_paint				(Object^ , PaintEventArgs^ e);
		void							sync_drawing_rectangle	(particle_graph_node^ node);
		void							curve_changed			(System::Object^ , float_curve_event_args^ );

		
	
#pragma endregion

#pragma region | Properties |
	

#pragma endregion

#pragma region | Events |

#pragma endregion

}; // class particle_scalable_control

} // namespace editor
} // namespace xray

#endif // #ifndef PARTICLE_SCALABLE_CONTROL_H_INCLUDED