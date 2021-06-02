////////////////////////////////////////////////////////////////////////////
//	Created		: 05.08.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef EDITOR_CREATOR_H_INCLUDED
#define EDITOR_CREATOR_H_INCLUDED

namespace xray {
namespace editor_base {
	public ref class editor_creator
	{
	public:
		static void	set_memory_allocator(allocator_type* allocator);
	}; // ref class editor_creator
} // namespace editor_base
} // namespace xray
#endif // #ifndef XRAY_DIALOG_EDITOR_API_H_INCLUDED