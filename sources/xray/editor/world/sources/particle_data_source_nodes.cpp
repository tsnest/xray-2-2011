////////////////////////////////////////////////////////////////////////////
//	Created		: 30.04.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "particle_data_source_nodes.h"
#include <xray/editor/base/images/images92x25.h>
#include "particle_graph_node_collection.h"


namespace xray {
namespace editor {

void particle_data_source_fc_node::OnPaint			(PaintEventArgs^ e)
{
	Drawing::Rectangle border_rect = Drawing::Rectangle(0,0, Width - 1, Height - 1);
	e->Graphics->DrawRectangle(System::Drawing::Pens::Black, border_rect);

	Drawing::Rectangle value_rect = Drawing::Rectangle(0,20, Width - 1, Height - 21);
	e->Graphics->DrawRectangle(System::Drawing::Pens::Black, value_rect);
	System::Drawing::Drawing2D::LinearGradientBrush^ brush	= gcnew System::Drawing::Drawing2D::LinearGradientBrush(value_rect, System::Drawing::Color::LimeGreen, System::Drawing::Color::DarkGreen, System::Drawing::Drawing2D::LinearGradientMode::Vertical);
	e->Graphics->FillRectangle( brush, value_rect);
	e->Graphics->DrawString		(Text, gcnew Drawing::Font("Arial", 10,FontStyle::Bold ), gcnew SolidBrush(Drawing::Color::Black),  border_rect);
	Drawing2D::Matrix^ M =  e->Graphics->Transform;

	float sx = Width / (fc->max_time - fc->min_time);
	float sy = (Height - 20) / (fc->max_time - fc->min_time);

	M->Translate(0, 20);
	M->Scale(sx,sy);
	fc->draw(e->Graphics, gcnew System::Drawing::Pen(Color::Black,3), M);

}
void particle_data_source_fc_node::curve_changed( System::Object^, xray::editor::float_curve_event_args^ ){
	this->Refresh();
}






void particle_data_source_cc_node::OnPaint			(PaintEventArgs^ e)
{
	Drawing::Rectangle border_rect = Drawing::Rectangle(0,0, Width - 1, Height - 1);
	e->Graphics->DrawRectangle(System::Drawing::Pens::Black, border_rect);
	e->Graphics->DrawString		(Text, gcnew Drawing::Font("Arial", 10,FontStyle::Bold ), gcnew SolidBrush(Drawing::Color::Black),  border_rect);
	Drawing::Rectangle value_rect = Drawing::Rectangle(0,20 , Width - 1,Height - 21);
	e->Graphics->DrawRectangle(System::Drawing::Pens::Black, value_rect);
	cc->draw(e->Graphics, value_rect);

}

void particle_data_source_cc_node::curve_changed(){
	this->Refresh();
}


} // namespace editor
} // namespace xray


