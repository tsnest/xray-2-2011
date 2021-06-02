#include "pch.h"
#include "terrain_import_form.h"
#include "terrain_quad.h"
#include <FreeImage/FreeImage.h>

namespace xray{
namespace editor{


void terrain_import_form::in_constructor()
{
	m_settings						= gcnew terrain_import_export_settings;
}

void terrain_import_form::init_layout()
{
	bool bimport = m_settings->m_b_import_mode;

	//diffuse_rgba_radio->Enabled				= false;//bimport;
	//diffuse_rgb_radio->Enabled				= false;//bimport;
	//diffuse_rgb_height_a_radio->Enabled		= false;//bimport;
	
//	height_a_radio->Checked				= true;

	if(!bimport)
		create_cell_if_empty_check->Checked	= false;

	min_height_up_down->Enabled				= bimport;
	max_height_up_down->Enabled				= bimport;

	up_down_size_x->Enabled					= !bimport;
	up_down_size_z->Enabled					= !bimport;

	create_cell_if_empty_check->Enabled		= bimport;

	ok_button->Enabled						= false;
}

void terrain_import_form::select_source_button_Click(System::Object^, System::EventArgs^)
{
	if(m_settings->m_b_import_mode)
	{
		System::Windows::Forms::OpenFileDialog  openFileDialog;
		
		openFileDialog.FileName			= source_text_box->Text;
		openFileDialog.Filter			= "bitmap files|*.bmp;*.tga;*.dds;*.jpg;*.tiff|All files|*.*";
		openFileDialog.FilterIndex		= 1;

		if(openFileDialog.ShowDialog() == ::DialogResult::OK)
		{
			source_text_box->Text		= openFileDialog.FileName;

			unmanaged_string fn		(openFileDialog.FileName);
			FREE_IMAGE_FORMAT	fif = FreeImage_GetFIFFromFilename(fn.c_str());
			if(fif!=FIF_TIFF)
			{
				file_desc_label->Text = System::String::Format("Only grayscale TIFF 32bpp(float) format supported" );
				ok_button->Enabled		= false;
				return;
			}
			FIBITMAP* fibitmap		= FreeImage_Load(fif, fn.c_str());
			if( FreeImage_GetImageType(fibitmap) != FIT_FLOAT )
			{
				file_desc_label->Text	= System::String::Format("Only grayscale TIFF 32bpp(float) format supported" );
				ok_button->Enabled		= false;
				FreeImage_Unload		(fibitmap);
				return;
			}

			u32 bitmap_width		= FreeImage_GetWidth	(fibitmap);
			u32 bitmap_height		= FreeImage_GetHeight	(fibitmap);
			FreeImage_Unload		( fibitmap );

			u32 size_x			= (bitmap_width-1) / 64;
			u32 size_z			= (bitmap_height-1) / 64;

			if( (size_x*64+1!=bitmap_width) || (size_z*64+1!=bitmap_height) )
			{
				file_desc_label->Text = System::String::Format("Invalid image size {0};{1}", bitmap_width, bitmap_height );
				ok_button->Enabled		= false;
			}

			up_down_size_x->Value	= size_x;
			up_down_size_z->Value	= size_z;

			System::String^ filename = openFileDialog.FileName;
			//a02_blockpost_min(-4.021642)_max(16.24393)
			int _min_idx = filename->IndexOf("_min(");
			
			if(_min_idx!=-1)
			{
				int _max_idx = filename->IndexOf( ")_max(", _min_idx );
				if(_max_idx!=-1)
				{
					int _end = filename->IndexOf(")",_max_idx+1);
					if(_end!=-1)
					{
						System::String^ min_height_str = filename->Substring(_min_idx+5, _max_idx-(_min_idx+5) );
						System::String^ max_height_str = filename->Substring(_max_idx+6, _end-(_max_idx+6));
						min_height_up_down->Value	= System::Convert::ToDecimal( convert_to_single(min_height_str) );
						max_height_up_down->Value	= System::Convert::ToDecimal( convert_to_single(max_height_str) );
					}
					
				}
			}

			ok_button->Enabled		= true;
		}
	}else
	{// export mode
		System::Windows::Forms::SaveFileDialog  saveFileDialog;
		
		saveFileDialog.FileName			= source_text_box->Text;
		saveFileDialog.Filter			= "bitmap files|*.tiff|All files|*.*";
		saveFileDialog.FilterIndex		= 1;

		if(saveFileDialog.ShowDialog() == ::DialogResult::OK)
		{
			source_text_box->Text		= saveFileDialog.FileName;
			ok_button->Enabled		= true;
		}else
			ok_button->Enabled		= false;
	}
}

void terrain_import_form::ok_button_Click(System::Object^, System::EventArgs^)
{
	m_settings->m_source_filename	= source_text_box->Text;
	
	//if(diffuse_rgb_radio->Checked)
	//	m_settings->m_options	= terrain_import_export_settings::options::rgb_diffuse;
	//else
	//if(diffuse_rgba_radio->Checked)
	//	m_settings->m_options	= terrain_import_export_settings::options::rgba_diffuse;
	//else
	//if(diffuse_rgb_height_a_radio->Checked)
	//	m_settings->m_options	= terrain_import_export_settings::options::rgb_diffuse_a_heightmap;
	//else
	//if(height_a_radio->Checked)
	//	m_settings->m_options	= terrain_import_export_settings::options::a_heightmap;

	m_settings->m_options		= terrain_import_export_settings::options::grayscale_heightmap;

	m_settings->m_b_create_cell	= create_cell_if_empty_check->Checked;

	m_settings->lt_x			= System::Decimal::ToInt32(up_down_lt_x->Value);
	m_settings->lt_z			= System::Decimal::ToInt32(up_down_lt_z->Value);

	m_settings->size_x			= System::Decimal::ToInt32(up_down_size_x->Value);
	m_settings->size_z			= System::Decimal::ToInt32(up_down_size_z->Value);

	m_settings->m_min_height	= System::Decimal::ToSingle(min_height_up_down->Value);
	m_settings->m_max_height	= System::Decimal::ToSingle(max_height_up_down->Value);

	DialogResult				= ::DialogResult::OK;
}

void terrain_import_form::cancel_button_Click(System::Object^, System::EventArgs^)
{
	DialogResult = ::DialogResult::Cancel;
}

void terrain_import_form::min_height_up_down_ValueChanged(System::Object^, System::EventArgs^)
{
	System::Decimal mn = min_height_up_down->Value;
	System::Decimal mx = max_height_up_down->Value;
	if(mn>=mx)
	{
		mx = mn + 1;
		max_height_up_down->Value = mx;
	}
}

} // namespace editor
} // namespace xray