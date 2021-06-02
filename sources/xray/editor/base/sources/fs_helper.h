////////////////////////////////////////////////////////////////////////////
//	Created		: 08.02.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef FS_HELPER_H_INCLUDED
#define FS_HELPER_H_INCLUDED

using namespace System;

namespace xray 
{
	namespace editor_base
	{
		public ref class fs_helper 
		{
		public:
			static void remove_to_recycle_bin( String^ path );
		

		}; // class fs_helper

	} // namespace editor_base
} // namespace xray

#endif // #ifndef FS_HELPER_H_INCLUDED