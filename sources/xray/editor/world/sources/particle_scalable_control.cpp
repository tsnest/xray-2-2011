////////////////////////////////////////////////////////////////////////////
//	Created		: 06.04.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_scalable_control.h"
#include "particle_graph_document.h"

namespace xray{
namespace editor{

void particle_scalable_control::in_constructor(){
	this->Name					= m_parent_node->Name;
	this->Text					= m_parent_node->Text;
	m_total_time				= 600;
	
	m_start_time				= (int)(Single)(m_parent_node->prop_holder->get_value("Delay"));
	m_length_time				= (int)(Single)(m_parent_node->prop_holder->get_value("LifeTime"));
	
	this->Height				= 25;
	this->Width					= m_total_time;
	this->BackColor				= SystemColors::ControlDark;
	this->Location				= Drawing::Point(0,5);
	
	m_b_left_scale_mode			= false;
	m_b_right_scale_mode		= false;
	m_b_move_mode				= false;
	m_last_mouse_down_position	= this->Location;
	m_drawing_rectangle			= System::Drawing::Rectangle(this->Location.X + (int)m_start_time, 0, (int)m_length_time, this->Height - 1);

	m_parent_node->m_time_parameters_holder->on_property_changed += gcnew particle_property_changed(this, &particle_scalable_control::sync_drawing_rectangle);

	
	
	this->MouseMove 	+= gcnew System::Windows::Forms::MouseEventHandler	(this, &particle_scalable_control::on_mouse_move);
	this->MouseDown 	+= gcnew System::Windows::Forms::MouseEventHandler	(this, &particle_scalable_control::on_mouse_down);
	this->MouseUp		+= gcnew System::Windows::Forms::MouseEventHandler	(this, &particle_scalable_control::on_mouse_up);
	this->MouseEnter	+= gcnew System::EventHandler						(this, &particle_scalable_control::on_mouse_enter);
	this->MouseLeave	+= gcnew System::EventHandler						(this, &particle_scalable_control::on_mouse_leave);
	this->Paint			+= gcnew System::Windows::Forms::PaintEventHandler	(this, &particle_scalable_control::on_paint);
}

void particle_scalable_control::curve_changed(System::Object^ , float_curve_event_args^ ){
	Refresh();
}

void particle_scalable_control::on_mouse_move	(Object^ , MouseEventArgs^ e){ 	
	if (!(m_b_left_scale_mode || m_b_right_scale_mode || m_b_move_mode)){
		if (math::abs(e->Location.X - m_drawing_rectangle.Left) < 5)
			Windows::Forms::Cursor::Current		= Cursors::SizeWE;
		else if (math::abs(e->Location.X - m_drawing_rectangle.Right) < 5)
			Windows::Forms::Cursor::Current		= Cursors::SizeWE;		
		else if (e->Location.X < m_drawing_rectangle.Right && e->Location.X > m_drawing_rectangle.Left)
			Windows::Forms::Cursor::Current		= Cursors::SizeAll;
		return;
	}


	int x_axis_offset = e->Location.X - m_last_mouse_down_position.X;

	if (m_b_left_scale_mode){
		Windows::Forms::Cursor::Current		= Cursors::SizeWE;
		m_parent_node->m_time_parameters_holder->set_start_length_offsets((float)x_axis_offset);						
	}
	else if (m_b_right_scale_mode){
		Windows::Forms::Cursor::Current		= Cursors::SizeWE;
		m_parent_node->m_time_parameters_holder->set_length_time_impl((float)(m_parent_node->prop_holder->get_value("LifeTime")) + x_axis_offset) ;
	}
	else if (m_b_move_mode){
		Windows::Forms::Cursor::Current		= Cursors::SizeAll;
		m_parent_node->m_time_parameters_holder->set_start_time_impl((float)(m_parent_node->prop_holder->get_value("Delay")) + x_axis_offset);
	}
	m_last_mouse_down_position = e->Location;
	//Refresh();
	safe_cast<particle_time_based_layout_panel^>(this->Parent)->parent_editor->multidocument_base->properties_panel->property_grid_control->Refresh();
	
	
}
void particle_scalable_control::on_mouse_down	(Object^ , MouseEventArgs^ e){ 
	if (math::abs(e->Location.X - m_drawing_rectangle.Left) < 5)
		m_b_left_scale_mode = true;
	else if (math::abs(e->Location.X - m_drawing_rectangle.Right) < 5)
		m_b_right_scale_mode = true;
	else if (e->Location.X < m_drawing_rectangle.Right && e->Location.X > m_drawing_rectangle.Left)
		m_b_move_mode = true;
	m_last_mouse_down_position = e->Location;
	safe_cast<particle_time_based_layout_panel^>(this->Parent)->parent_editor->multidocument_base->show_properties(m_parent_node->prop_holder->container());
}

void particle_scalable_control::on_mouse_up		(Object^ , MouseEventArgs^){ 
	m_b_left_scale_mode			= false;
	m_b_right_scale_mode		= false;
	m_b_move_mode				= false;
}
void particle_scalable_control::on_mouse_enter	(Object^ , EventArgs^){ 

}
void particle_scalable_control::on_mouse_leave	(Object^ , EventArgs^){ 

}
void particle_scalable_control::on_paint		(Object^ , PaintEventArgs^ e){ 
	StringFormat^ fmt = gcnew StringFormat();
	fmt->Alignment = StringAlignment::Center;
	e->Graphics->DrawRectangle	(System::Drawing::Pens::Black, m_drawing_rectangle);


	System::Drawing::Drawing2D::LinearGradientBrush^ brush	= gcnew System::Drawing::Drawing2D::LinearGradientBrush(m_drawing_rectangle, System::Drawing::Color::LimeGreen, System::Drawing::Color::DarkGreen, System::Drawing::Drawing2D::LinearGradientMode::Vertical);


	e->Graphics->FillRectangle( brush, m_drawing_rectangle);
	e->Graphics->DrawString		(Text, gcnew Drawing::Font("Arial", 10,FontStyle::Bold ), gcnew SolidBrush(Drawing::Color::Black),  m_drawing_rectangle, fmt);
}

void particle_scalable_control::sync_drawing_rectangle(particle_graph_node^ node){
	m_drawing_rectangle			= System::Drawing::Rectangle((int)(Single)(node->prop_holder->get_value("Delay")), 0, (int)(Single)(node->prop_holder->get_value("LifeTime")), this->Height - 1);
	Refresh();
}


}//namespace editor
}//namespace xray

