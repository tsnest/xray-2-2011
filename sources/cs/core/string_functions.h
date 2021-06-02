////////////////////////////////////////////////////////////////////////////
//	Module 		: string_functions.h
//	Created 	: 24.08.2006
//  Modified 	: 24.08.2006
//	Author		: Dmitriy Iassenev
//	Description : string functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_STRING_FUNCTIONS_H_INCLUDED
#define CS_CORE_STRING_FUNCTIONS_H_INCLUDED

#if defined(__GNUC__)
#	include <ctype.h>
#	include <unistd.h>
#endif // #if defined(__GNUC__)

#define sz_chr		strchr
#define sz_str		strstr

inline		u32		sz_len			(shared_string const& string);

inline		pstr	sz_cpy			(pstr destination, u32 const size, pcstr source);
template <int size>
inline		pstr	sz_cpy			(char (&destination)[size], pcstr source);

inline		pstr	sz_ncpy			(pstr destination, u32 const size, pcstr source, u32 count);
template <int size>
inline		pstr	sz_ncpy			(char (&destination)[size], pcstr source, u32 count);

inline		pstr	sz_cat			(pstr destination, u32 const size, pcstr source);
template <int size>
inline		pstr	sz_cat			(char (&destination)[size], pcstr source);

CS_CORE_API	pstr	sz_trim_left	(pstr string);
CS_CORE_API	pstr	sz_trim_right	(pstr string);
inline		pstr	sz_trim			(pstr string);
inline		int		sz_cmp			(pcstr _0, pcstr _1);
inline		int		sz_cmp			(shared_string const& _0, shared_string const& _1);
inline		int		sz_icmp			(pcstr _0, pcstr _1);

inline		pstr	sz_lwr			( pstr source, u32 count );

inline		pstr	sz_concat		(pstr destination, u32 const size, pcstr _0);
template <int size>
inline		pstr	sz_concat		(char (&destination)[size], pcstr _0);

inline		pstr	sz_concat		(pstr destination, u32 const size, pcstr _0, pcstr _1);
template <int size>
inline		pstr	sz_concat		(char (&destination)[size], pcstr _0, pcstr _1);

inline		pstr	sz_concat		(pstr destination, u32 const size, pcstr _0, pcstr _1, pcstr _2);
template <int size>
inline		pstr	sz_concat		(char (&destination)[size], pcstr _0, pcstr _1, pcstr _2);

template <typename predicate_type>
inline		bool	iterate_items	(pcstr string, predicate_type const& predicate, char const& separator = ',');

template <typename predicate_type>
inline		bool	iterate_items	(str_shared string, predicate_type const& predicate, char const& separator = ',');

template <typename predicate_type>
inline		bool	iterate_items	(pcstr string, u32 const length, predicate_type const& predicate, char const& separator = ',');

#include "string_functions_inline.h"

#pragma deprecated(strcmp)

#endif // #ifndef CS_CORE_STRING_FUNCTIONS_H_INCLUDED