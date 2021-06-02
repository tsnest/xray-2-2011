////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_source_nodes_container.h"
#include <xray/editor/base/images/images9x9.h>
#include <xray/editor/base/images_loading.h>


namespace xray{
namespace editor {
	
void particle_source_nodes_container::in_constructor(){
	m_particle_source_nodes		= gcnew particle_nodes();
	m_enabled					= true;
	m_images					= xray::editor_base::image_loader::load_images("images9x9", 9, safe_cast<int>(editor_base::images9x9_count), this->GetType());
}

void particle_source_nodes_container::OnPaint(PaintEventArgs^ e)
{
	e->Graphics->DrawImage(m_images->Images[m_enabled ? editor_base::image_minus : editor_base::image_plus], System::Drawing::Point(2,2));
	e->Graphics->DrawString		(Text, gcnew Drawing::Font("Arial", 8), gcnew SolidBrush(Drawing::Color::Black),  Drawing::Point(12,0));
}


}//namespace editor 
}//namespace xray
