////////////////////////////////////////////////////////////////////////////
//	Created		: 14.10.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef ENUM_DEFINES_H_INCLUDED
#define ENUM_DEFINES_H_INCLUDED

namespace xray {
namespace animation_editor {
	public enum class interpolator_type 
	{
		instant = 0,
		linear,
		fermi,
	}; // enum clip_mix_self

	public enum class mix_self 
	{
		mix_continue_self = 0,
		mix_interpolate_self,
	}; // enum clip_mix_self
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ENUM_DEFINES_H_INCLUDED