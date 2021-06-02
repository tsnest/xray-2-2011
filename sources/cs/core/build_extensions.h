////////////////////////////////////////////////////////////////////////////
//	Module 		: build_extensions.h
//	Created 	: 18.03.2007
//  Modified 	: 18.03.2007
//	Author		: Dmitriy Iassenev
//	Description : build extensions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_BUILD_EXTENSIONS_H_INCLUDED
#define CS_CORE_BUILD_EXTENSIONS_H_INCLUDED

namespace build {
	extern void initialize	();

	struct CS_CORE_API calculator {
		u32	m_build_id;

			calculator		(u32 const day, u32 const month, u32 const year);
	};
}

#endif // #ifndef CS_CORE_BUILD_EXTENSIONS_H_INCLUDED