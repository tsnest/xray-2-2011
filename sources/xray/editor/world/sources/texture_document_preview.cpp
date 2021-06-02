////////////////////////////////////////////////////////////////////////////
//	Created		: 02.04.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "texture_document.h"
#include "texture_editor.h"
#include "texture_tga_to_argb_cook.h"
#include "texture_editor_resource.h"
#include <amd/compress/texture_compressor/api.h>


using namespace System::Drawing::Imaging;
using System::Drawing::Imaging::BitmapData;

using xray::texture_compressor::TgaHeader;

namespace xray {
namespace editor {

namespace texture_document_detail
{
	union bgra {
		#pragma warning(push)
		#pragma warning(disable:4201)
		struct {
			u8 b, g, r, a;
		};
		#pragma warning(pop)
		u32 u;
	};
}

static void draw_image_on_picture(float a, float r, float g, float b, Image^ picture, Image^ image, Image^ background)
{
	System::Drawing::Graphics^	gfx		= System::Drawing::Graphics::FromImage(picture);
	System::Drawing::Rectangle	dest_rect( 0, 0, image->Width, image->Height );
	
	TextureBrush tbrush			( background,System::Drawing::Drawing2D::WrapMode::Tile );
	gfx->FillRectangle			( %tbrush, dest_rect );
	//gfx->DrawImage			( background, dest_rect, 0, 0, 32/*background->Width*/, 32/*background->Height*/, System::Drawing::GraphicsUnit::Pixel, %bk_attribs );

	ColorMatrix^	matrix		= gcnew ColorMatrix();
	matrix->Matrix00			= r;
	matrix->Matrix11			= g;
	matrix->Matrix22			= b;
	matrix->Matrix33			= a;

	ImageAttributes				img_attribs;
	img_attribs.SetColorMatrix	( matrix );
	gfx->DrawImage				( image, dest_rect, 0, 0, image->Width, image->Height, System::Drawing::GraphicsUnit::Pixel, %img_attribs );
}

static void draw_image_on_picture_fast(float a, float r, float g, float b, Bitmap^ picture, Bitmap^ image, Bitmap^ background_)
{
	if(a == 1 && r+g+b > 0)
	{
		draw_image_on_picture					(a, r, g, b, picture, image, background_);
		return;
	}

	System::Drawing::Rectangle	rect		= System::Drawing::Rectangle(0,0,image->Width,image->Height);
	BitmapData^					bmp_data_src	= image->LockBits( rect, System::Drawing::Imaging::ImageLockMode::ReadOnly, image->PixelFormat );
	int*						src			= (int*)(bmp_data_src->Scan0.ToPointer());
	BitmapData^					bmp_data_dst	= picture->LockBits( rect, System::Drawing::Imaging::ImageLockMode::ReadOnly, picture->PixelFormat );
	int*						dst			= (int*)(bmp_data_dst->Scan0.ToPointer());

	texture_document_detail::bgra bgra_bridge;

	int size = image->Width*image->Height;
	if(a+r+g+b == 1)
	{
		for(int i=0; i<size; i++)
		{
			bgra_bridge.u	= (*src);
			u8 lightness	= (u8)math::ceil(bgra_bridge.b*b + bgra_bridge.g*g + bgra_bridge.r*r + bgra_bridge.a*a);
			bgra_bridge.b	= lightness; 
			bgra_bridge.g	= lightness;
			bgra_bridge.r	= lightness;
			bgra_bridge.a	= 255;
			(*dst)			= bgra_bridge.u;
			++dst;
			++src;
		}
	}
	else 
	{
		for(int i=0; i<size; i++)
		{
			bgra_bridge.u	= (*src);
			bgra_bridge.b	*= (u8)b; 
			bgra_bridge.g	*= (u8)g;
			bgra_bridge.r	*= (u8)r;
			bgra_bridge.a	= 255;
			(*dst)			= bgra_bridge.u;
			++dst;
			++src;
		}
	}
	
	bmp_data_dst->Stride -= 500;
	picture->UnlockBits(bmp_data_dst);
	image->UnlockBits(bmp_data_src);
}

void texture_document::tga_loaded( resources::queries_result& data, Object^ param )
{
	if(m_need_suppress_loading)
		return;

	//if not synk call and next query exists, then return
	if(!m_is_tga_synk_called && m_tga_request_id != data.uid())
		return;

	if ( !data.is_successful() )
		return;

	//if previous convert query do not started, suppress them
	if(m_last_buf_struct != NULL)
	{
		if(threading::interlocked_increment(m_last_buf_struct->m_need_process_flag) == 1)
		{
			m_last_image->UnlockBits	( m_last_image_data );
			delete						m_last_image;
		}
	}

	if(m_images->Count == 0)
		delete						m_last_image;

	//Get tga buffer, lock bitmap buffer and pass them to texture_tga_to_argb cooker
	resources::pinned_ptr_const<u8>* tga_buffer	= NEW(resources::pinned_ptr_const<u8>)(data[0].get_managed_resource());
	TgaHeader* header						= (TgaHeader*)tga_buffer->c_ptr();
	m_image_has_alpha						= header->pixel_size == 32;
	m_last_image							= gcnew Bitmap(header->width, header->height);

	System::String^ resource_name		= safe_cast<System::String^>(param);
	texture_editor_resource^ res		= safe_cast<texture_editor_resource^>(m_texture_editor->opened_resources->default[resource_name]);

	res->width							= m_last_image->Width;
	res->height							= m_last_image->Height;

	m_texture_editor->properties_panel->property_grid_control->update();

	// Lock the bitmap's bits.  
	System::Drawing::Rectangle	rect		= System::Drawing::Rectangle(0,0,m_last_image->Width,m_last_image->Height);
	m_last_image_data						= m_last_image->LockBits( rect, System::Drawing::Imaging::ImageLockMode::ReadOnly, m_last_image->PixelFormat );
	texture_tga_argb_buffers*	buf_struct	= NEW(texture_tga_argb_buffers)(
		tga_buffer,
		m_last_image_data->Scan0.ToPointer()
	);

	m_last_buf_struct						= buf_struct;
 	const_buffer buf						((pcvoid)buf_struct, sizeof(texture_tga_argb_buffers));

	query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &texture_document::image_loaded), g_allocator);
	m_argb_last_request_id = resources::query_create_resource	(
													"",
													buf, 
													resources::texture_tga_to_argb_class,  
													boost::bind(&query_result_delegate::callback, q, _1),
													g_allocator
													);

	m_images->Add(m_argb_last_request_id, m_last_image);
}

void texture_document::image_loaded( resources::queries_result& data )
{
	if(m_need_suppress_loading)
		return;

	if(m_argb_last_request_id == data.uid())
	{
		m_last_image->UnlockBits( m_last_image_data );

		if(m_image_has_alpha)
		{
			m_a_checkbox->Enabled					= true;
			m_ab_checkbox->Enabled					= true;
			if(m_a_checkbox->Checked)
			{
				m_r_checkbox->Enabled					= false;
				m_g_checkbox->Enabled					= false;
				m_b_checkbox->Enabled					= false;
			}
		}
		else
		{
			m_a_checkbox->Enabled					= false;
			m_ab_checkbox->Enabled					= false;
			m_r_checkbox->Enabled					= true;
			m_g_checkbox->Enabled					= true;
			m_b_checkbox->Enabled					= true;
		}

		delete m_picture_image;
		m_picture_image							= gcnew Bitmap(m_last_image->Width, m_last_image->Height);

		if(m_a_checkbox->Enabled && m_a_checkbox->Checked)
		{
			draw_image_on_picture_fast				(1, 0, 0, 0, m_picture_image, m_last_image, m_texture_editor->s_chessboard);
		}
		else
		{
			float			a 						= m_ab_checkbox->Checked;
			float			r 						= m_r_checkbox->Checked;
			float			g 						= m_g_checkbox->Checked;
			float			b 						= m_b_checkbox->Checked;

			draw_image_on_picture_fast				((m_image_has_alpha)?a:0, r, g, b, m_picture_image, m_last_image, m_texture_editor->s_chessboard);
		}
		m_picture_box->Image					= m_picture_image;

		m_last_buf_struct						= NULL;
	}
	else
	{
		delete									m_images[data.uid()];
	}

	//release cooker's buffer, Dispose image and remove it from collection
	const_buffer buf						= data[0].creation_data_from_user();
	DELETE									((texture_tga_argb_buffers*)(buf.c_ptr()));
	m_images->Remove						(data.uid());
}

static bool	flag = false;
void texture_document::rgba_check_boxes_checked_changed	(System::Object^ sender, System::EventArgs^  )
{
	if(flag)
		return;
	XRAY_UNREFERENCED_PARAMETER	(sender);
	flag = true;
	if(m_a_checkbox != sender)
		m_a_checkbox->Checked = false;
	if(m_a_checkbox->Enabled && m_a_checkbox->Checked)
	{
		m_r_checkbox->Enabled = false;
		m_g_checkbox->Enabled = false;
		m_b_checkbox->Enabled = false;
		m_ab_checkbox->Checked = false;
		draw_image_on_picture_fast				(1, 0, 0, 0, m_picture_image, m_last_image, m_texture_editor->s_chessboard);
	}
	else
	{
		m_r_checkbox->Enabled = true;
		m_g_checkbox->Enabled = true;
		m_b_checkbox->Enabled = true;

		float			a 						= m_ab_checkbox->Checked;
		float			r 						= m_r_checkbox->Checked;
		float			g 						= m_g_checkbox->Checked;
		float			b 						= m_b_checkbox->Checked;

		draw_image_on_picture_fast				((m_image_has_alpha)?a:0, r, g, b, m_picture_image, m_last_image, m_texture_editor->s_chessboard);
	}
	m_picture_box->Image		= m_picture_image;
	flag = false;
}

}//namespace editor
}//namespace xray
