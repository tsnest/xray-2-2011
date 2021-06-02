#include "pch.h"
#include "color_curve_editor.h"
#include "float_curve.h"

using xray::editor::controls::color_curve_editor;
using xray::editor::color_curve;
using xray::editor::color_curve_pair;
using System::Drawing::Drawing2D::LinearGradientBrush;

void color_curve_editor::in_constructor()
{
	m_pen			= gcnew System::Drawing::Pen(Color::Black);
	m_pen->DashStyle= System::Drawing::Drawing2D::DashStyle::DashDot;
	m_fill_brush	= gcnew System::Drawing::Drawing2D::LinearGradientBrush(Point(0,0), Point(50,0), Color::White, Color::Red);
	Enabled = false;
}

float color_curve_editor::client_point_to_curve(Point client_point)
{
	float sx = Width / (m_curve->max_time-m_curve->min_time);
	
	return client_point.X / sx;
}

bool is_near(float p1, float p2, float d);
Void color_curve_editor::color_curve_editor_MouseDown(System::Object^, System::Windows::Forms::MouseEventArgs^ e)
{
	float  pt			= client_point_to_curve(e->Location);
	int count			= m_curve->count;
	for(int idx=0; idx<count; ++idx)
	{
		color_curve_pair^ pair	= m_curve->key[idx];
		if(is_near(pair->time, pt, 2))
		{
			m_captured = pair;
			break;
		}
	}
}

Void  color_curve_editor::color_curve_editor_MouseDoubleClick(System::Object^, System::Windows::Forms::MouseEventArgs^  e)
{
	float  time				= client_point_to_curve(e->Location);
	color_curve_pair^ _pair = m_captured;
	if(_pair)
		colorDialog1->Color = _pair->value;
	else
		colorDialog1->Color = m_curve->calc_value(time);
	
	if(colorDialog1->ShowDialog()==System::Windows::Forms::DialogResult::OK)
	{
		if(_pair)
			_pair->value		= colorDialog1->Color;
		else
			m_curve->add_key	(time, colorDialog1->Color);
		
		m_curve->synchronize_config();

		Invalidate				(false);
	}
}

Void color_curve_editor::color_curve_editor_MouseEnter(System::Object^, System::EventArgs^)
{}

Void color_curve_editor::color_curve_editor_MouseLeave(System::Object^, System::EventArgs^)
{
	m_captured = nullptr;
	m_curve->synchronize_config();
}

Void color_curve_editor::color_curve_editor_MouseUp(System::Object^, System::Windows::Forms::MouseEventArgs^)
{
	m_captured = nullptr;
	m_curve->synchronize_config();
}

Void color_curve_editor::color_curve_editor_MouseMove(System::Object^, System::Windows::Forms::MouseEventArgs^ e)
{
	if(m_captured)
	{
		float  pt			= client_point_to_curve(e->Location);
		Color clr			= m_captured->value;
		m_curve->alter_key	(m_captured, pt, clr);
		Invalidate			();
	}
}

Void color_curve_editor::color_curve_editor_KeyDown(System::Object^, System::Windows::Forms::KeyEventArgs^  e)
{
	if(m_captured && e->KeyCode==Keys::Delete)
	{
		int count			= m_curve->count;
		for(int idx=0; idx<count; ++idx)
		{
			if(m_curve->key[idx]==m_captured)
			{
				m_curve->remove_key(idx);
				m_curve->synchronize_config();
				Invalidate			();
				break;
			}
		}
	}
}

Color select_pen_color(Color c)
{
	if(c.GetBrightness()>0.5) //white=1, black=0
		return Color::Black;
	else	
		return Color::White;
}

Void color_curve_editor::color_curve_editor_Paint(System::Object^, System::Windows::Forms::PaintEventArgs^  e)
{
	if(!m_curve)	return;

	m_curve->draw(e->Graphics, Drawing::Rectangle(0,0,Width - 1, Height - 1));

	int rect_w = 4;
	float sx	= (Width-rect_w) / (m_curve->max_time-m_curve->min_time);
	for(int idx=0; idx<m_curve->count; ++idx)
	{
		color_curve_pair^ pair		= m_curve->key[idx];
		int t						= System::Convert::ToInt32(pair->time*sx);
		m_pen->Color				= select_pen_color(pair->value);
		e->Graphics->DrawRectangle	(m_pen, System::Drawing::Rectangle(t-rect_w/2, 0, rect_w, Height));
	}
}

using xray::editor::color_curve_type_editor;
using System::Drawing::Design::UITypeEditorEditStyle;
using System::ComponentModel::ITypeDescriptorContext;
bool color_curve_type_editor::GetPaintValueSupported(ITypeDescriptorContext^ )
{
	// let the property browser know we'd like
	// to do custom painting.
	return true;
}

void color_curve_type_editor::PaintValue(System::Drawing::Design::PaintValueEventArgs^ pe)
{
	System::Drawing::Pen^ pen = gcnew System::Drawing::Pen(Color::Red);
	pe->Graphics->DrawEllipse(pen, System::Drawing::Rectangle(2,2,5,5));
}

UITypeEditorEditStyle color_curve_type_editor::GetEditStyle(ITypeDescriptorContext^ )
{
	return UITypeEditorEditStyle::Modal;
}

Object^ color_curve_type_editor::EditValue(ITypeDescriptorContext^ , System::IServiceProvider^ provider, Object^ value)
{
// get the editor service.
	IWindowsFormsEditorService^ edSvc = safe_cast<IWindowsFormsEditorService^>(provider->GetService(IWindowsFormsEditorService::typeid));

	// create our UI
	color_curve_editor^ ui = gcnew color_curve_editor();
	ui->SelectedObject = (color_curve^)value;
	     
	// initialize the ui with the settings for this vertex
	edSvc->DropDownControl(ui);

	// return the updated value;
	return ui->SelectedObject;
}
