////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.08.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_TYPE_EXTENSIONS_H_INCLUDED
#define XRAY_TYPE_EXTENSIONS_H_INCLUDED

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

#if XRAY_PLATFORM_PS3
	// since stlport defines macro std, it doesn't compile if we don't include
	// any file with std namespace before we start using std namespace
#	ifdef __SNC__
#		include <assert.h>
#	endif // #ifdef __SNC__

	namespace std {
		typedef u8				uint8_t;
		typedef s8				int8_t;

		typedef u16				uint16_t;
		typedef s16				int16_t;

		typedef u32				uint32_t;
		typedef s32				int32_t;

		typedef u64				uint64_t;
		typedef s64				int64_t;

		typedef s64				_Longlong;
		typedef u64				_ULonglong;
	} // namespace std
#endif // #if XRAY_PLATFORM_PS3

typedef float					f32;
typedef double					f64;

typedef char*					pstr;
typedef char const*				pcstr;

typedef unsigned char*			pbyte;
typedef unsigned char const*	pcbyte;

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

typedef	char					string_path	[260];

enum 	assert_on_fail_bool		{ assert_on_fail_false, assert_on_fail_true };

namespace xray {
	template <int Left, int Right>
	struct compile_max {
		enum { value = Left >= Right ? Left : Right };
	};

	template <int Left, int Right>
	struct compile_min {
		enum { value = Left <= Right ? Left : Right };
	};
} // namespace xray

#endif // #ifndef XRAY_TYPE_EXTENSIONS_H_INCLUDED