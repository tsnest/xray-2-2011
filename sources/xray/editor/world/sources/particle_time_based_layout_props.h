////////////////////////////////////////////////////////////////////////////
//	Created		: 09.04.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_TIME_BASED_LAYOUT_PROPS_H_INCLUDED
#define PARTICLE_TIME_BASED_LAYOUT_PROPS_H_INCLUDED

namespace xray {
namespace editor {
	public ref struct particle_time_based_layout_props
	{
	public:
		particle_time_based_layout_props()
		{
			h_scroll_pos	= 0;
			v_scroll_pos	= 0;
			m_scale			= 1;
		}
	
		property int		h_scroll_pos;
		property int		v_scroll_pos;
		property float		m_scale;
			
			

	}; // class particle_time_based_layout_props
}; //namespace editor
}; //namespace xray

#endif // #ifndef PARTICLE_TIME_BASED_LAYOUT_PROPS_H_INCLUDED