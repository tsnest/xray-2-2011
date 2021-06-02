////////////////////////////////////////////////////////////////////////////
//	Created		: 22.06.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_LINKAGE_HELPER_H_INCLUDED
#define XRAY_LINKAGE_HELPER_H_INCLUDED

#define XRAY_DECLARE_LINKAGE_ID( linkage_id )  					\
		namespace xray {											\
			namespace linkage_helpers {								\
				int linkage_id;										\
			}														\
		} // namespace xray

#define XRAY_INCLUDE_TO_LINKAGE( linkage_id )  					\
		namespace xray {											\
			namespace linkage_helpers {								\
				extern int linkage_id;								\
				static int* linkage_id ## refferer = &linkage_id;	\
			}														\
		} // namespace xray

// #else // ##ifdef XRAY_STATIC_LIBRARIES
// #	define XRAY_DECLARE_LINKAGE_ID( linkage_id )
// #	define XRAY_INCLUDE_TO_LINKAGE( linkage_id )
// #endif // #ifndef XRAY_STATIC_LIBRARIES

#endif // #ifndef XRAY_LINKAGE_HELPER_H_INCLUDED
