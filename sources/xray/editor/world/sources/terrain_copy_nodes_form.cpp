#include "pch.h"
#include "terrain_copy_nodes_form.h"
#include "terrain_object.h"

namespace xray{
namespace editor{
void terrain_copy_nodes_form::button1_Click(System::Object^, System::EventArgs^ )
{

	int sx				= System::Convert::ToInt32(from_x_numericUpDown->Value);
	int sy				= System::Convert::ToInt32(from_y_numericUpDown->Value);

	int tx				= System::Convert::ToInt32(to_x_numericUpDown->Value);
	int ty				= System::Convert::ToInt32(to_y_numericUpDown->Value);

	int size_x			= System::Convert::ToUInt32(size_x_numericUpDown->Value);
	int size_y			= System::Convert::ToUInt32(size_y_numericUpDown->Value);
	
	for(int ix=0; ix<size_x; ++ix)
	{
		for(int iy=0; iy<size_y; ++iy)
		{
			terrain_node_key	from_key(sx+ix, sy-iy);
			terrain_node_key	to_key	(tx+ix, ty-iy);
			if(from_key.eq(to_key))
				continue;

			m_terrain_core->copy_node	( from_key, to_key );
			status_label->Text = System::String::Format("Complete: {0} of {1}", ix*size_y+iy+1, size_x*size_y);
		}
	}
	DialogResult = System::Windows::Forms::DialogResult::OK;
}

} // namespace editor
} // namespace xray
