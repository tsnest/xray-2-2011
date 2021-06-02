////////////////////////////////////////////////////////////////////////////
//	Created		: 11.02.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_node_interval.h"
#include "animation_node_clip.h"

using xray::animation_editor::animation_node_interval;

System::String^ animation_node_interval::ToString()
{
	return m_type.ToString();
}