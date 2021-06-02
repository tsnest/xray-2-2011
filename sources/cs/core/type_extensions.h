////////////////////////////////////////////////////////////////////////////
//	Module 		: type_extensions.h
//	Created 	: 09.07.2005
//  Modified 	: 09.07.2005
//	Author		: Dmitriy Iassenev
//	Description : Type extensions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_TYPE_EXTENSIONS_H_INCLUDED
#define CS_CORE_TYPE_EXTENSIONS_H_INCLUDED

typedef	signed		char		s8;
typedef	unsigned	char		u8;

typedef	signed		short		s16;
typedef	unsigned	short		u16;

typedef	signed		int			s32;
typedef	unsigned	int			u32;

#if defined(_MSC_VER)
	typedef	signed		__int64	s64;
	typedef	unsigned	__int64	u64;
#else // #if defined(_MSC_VER)
	typedef	signed long long	s64;
	typedef	unsigned long long	u64;
#endif // #if defined(_MSC_VER)

#if CS_PLATFORM_PS3
	// since stlport defines macro std, it doesn't compile if we don't include this
	// (or any other file with std namespace) before we start using std namespace
#	include <stdarg.h>

	namespace std {
		typedef u8				uint8_t;
		typedef s8				int8_t;

		typedef u16				uint16_t;
		typedef s16				int16_t;

		typedef u32				uint32_t;
		typedef s32				int32_t;

		typedef u64				uint64_t;
		typedef s64				int64_t;
	} // namespace std
#endif // #if XRAY_PLATFORM_PS3

typedef float					f32;
typedef double					f64;

typedef char*					pstr;
typedef char const*				pcstr;

typedef void*					pvoid;
typedef void const*				pcvoid;

typedef	char					string16	[16];
typedef	char					string32	[32];
typedef	char					string64	[64];
typedef	char					string128	[128];
typedef	char					string256	[256];
typedef	char					string512	[512];
typedef	char					string1024	[1024];
typedef	char					string2048	[2048];
typedef	char					string4096	[4096];

typedef	string512				string_path;

template < typename first_type, typename second_type >
union horrible_cast {
	first_type			first;
	second_type			second;

	inline	horrible_cast	( first_type const& first ) :
		first			( first )
	{
	}
}; // union horrible_cast

#endif // #ifndef CS_CORE_TYPE_EXTENSIONS_H_INCLUDED