#include "pch.h"
#include "float_curve_editor.h"
#include "float_curve.h"

using xray::editor::controls::float_curve_editor;
using xray::editor::float_curve;

PointF  float_curve_editor::client_point_to_curve(Point client_point)
{
	float sx	= (Width-m_point_rect_sz.Width) / (m_curve->max_time-m_curve->min_time);
	float sy	= (Height-m_point_rect_sz.Height) / (m_curve->max_value-m_curve->min_value);
	
	return PointF(client_point.X/sx, client_point.Y/sy);
}

void float_curve_editor::in_constructor()
{
	m_pen			= gcnew System::Drawing::Pen(Color::Black);
	m_mesh_pen		= gcnew System::Drawing::Pen(Color::Gray);
	m_fill_brush	= gcnew System::Drawing::SolidBrush(SystemColors::Info);
	m_point_rect_sz	= SizeF(5.0f, 5.0f);
	Enabled			= false;
	ResizeRedraw	= true;

	m_grid_h_step				= 20;
	m_grid_v_step				= 20;
	m_grid_min_h_line_spacing	= 15;
	m_grid_min_v_line_spacing	= 15;
	m_grid_font					= gcnew Drawing::Font("Arial", 6);
	m_grid_font_brush			= gcnew Drawing::SolidBrush(Drawing::Color::Gray);

	max_grid_width_track_bar->Minimum	= 20;
	max_grid_width_track_bar->Maximum	= 100; 
	max_grid_width_track_bar->Value		= 100;

	grid_track_bar_visible				= false;
}

bool is_near(float p1, float p2, float d)
{
	return(Math::Abs(p1-p2)<d || Math::Abs(p2-p1)<d);
}

Void float_curve_editor::float_curve_editor_MouseDown(System::Object^, System::Windows::Forms::MouseEventArgs^  e)
{
	PointF  pt			= client_point_to_curve(e->Location);
	int count			= m_curve->count;
	for(int idx=0; idx<count; ++idx)
	{
		float_curve_pair^ pair	= m_curve->key[idx];
		if(is_near(pair->time, pt.X, m_point_rect_sz.Width/2.0f) && near(pair->value, pt.Y, m_point_rect_sz.Height/2.0f))
		{
			m_captured = pair;
			break;
		}
	}
}

Void  float_curve_editor::float_curve_editor_KeyDown(System::Object^, System::Windows::Forms::KeyEventArgs^  e)
{
	if(m_captured && e->KeyCode==Keys::Delete)
	{
		int count			= m_curve->count;
		for(int idx=0; idx<count; ++idx)
		{
			if(m_curve->key[idx]==m_captured)
			{
				m_curve->remove_key(idx);
				Invalidate			();
				m_curve->synchronize_config();
				break;
			}
		}
	}
}

Void float_curve_editor::float_curve_editor_MouseMove(System::Object^, System::Windows::Forms::MouseEventArgs^  e)
{
	if(m_captured)
	{
		PointF  pt			= client_point_to_curve(e->Location);
		m_curve->alter_key	(m_captured, pt.X, pt.Y);
		Invalidate			(false);
	}
}

Void float_curve_editor::float_curve_editor_MouseDoubleClick(System::Object^, System::Windows::Forms::MouseEventArgs^  e)
{
	if(m_captured)
		return;

	PointF  pt				= client_point_to_curve(e->Location);
	m_curve->add_key		(pt.X, pt.Y);
	Invalidate				(false);
	m_curve->synchronize_config();
}

Void float_curve_editor::float_curve_editor_MouseUp(System::Object^, System::Windows::Forms::MouseEventArgs^)
{
	m_captured = nullptr;
	m_curve->synchronize_config();
}

Void float_curve_editor::float_curve_editor_MouseLeave(System::Object^, System::EventArgs^)
{
	m_captured = nullptr;
	m_curve->synchronize_config();
}

Void float_curve_editor::max_grid_width_track_bar_Scroll(System::Object^, System::EventArgs^)
{
	Single multiplier = max_grid_width_track_bar->Value/m_curve->max_time;

	m_curve->max_time = float(max_grid_width_track_bar->Value);
	for each(float_curve_pair^ key in m_curve->keys)
	{
		key->time *= multiplier;
	}
	this->Invalidate();
}

using xray::editor::float_curve_type_editor;
using System::Drawing::Design::UITypeEditorEditStyle;
using System::ComponentModel::ITypeDescriptorContext;
bool float_curve_type_editor::GetPaintValueSupported(ITypeDescriptorContext^ )
{
	// let the property browser know we'd like
	// to do custom painting.
	return true;
}

void float_curve_type_editor::PaintValue(System::Drawing::Design::PaintValueEventArgs^ pe)
{
	System::Drawing::Pen^ pen = gcnew System::Drawing::Pen(Color::Red);
	pe->Graphics->DrawEllipse(pen, System::Drawing::Rectangle(2,2,5,5));
}

UITypeEditorEditStyle float_curve_type_editor::GetEditStyle(ITypeDescriptorContext^ )
{
	return UITypeEditorEditStyle::Modal;
}

Object^ float_curve_type_editor::EditValue(ITypeDescriptorContext^ , System::IServiceProvider^ provider, Object^ value)
{
// get the editor service.
	IWindowsFormsEditorService^ edSvc = safe_cast<IWindowsFormsEditorService^>(provider->GetService(IWindowsFormsEditorService::typeid));

	// create our UI
	float_curve_editor^ ui = gcnew float_curve_editor();
	ui->SelectedObject	= (float_curve^)value;	     
	// initialize the ui with the settings for this vertex
	edSvc->DropDownControl(ui);

	// return the updated value;
	return ui->SelectedObject;		
}

