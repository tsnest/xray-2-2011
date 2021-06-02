////////////////////////////////////////////////////////////////////////////
//	Module 		: string_functions_inline.h
//	Created 	: 24.08.2006
//  Modified 	: 24.08.2006
//	Author		: Dmitriy Iassenev
//	Description : string functions inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_STRING_FUNCTIONS_INLINE_H_INCLUDED
#define CS_CORE_STRING_FUNCTIONS_INLINE_H_INCLUDED

inline u32 sz_len							(pcstr string)
{
	return			(u32)strlen(string);
}

inline u32 sz_len							(shared_string const& string)
{
	return			string.size();
}

inline pstr sz_dup							(pcstr string)
{
	u32				buffer_size = (sz_len(string) + 1)*sizeof(char);
	pstr			result = (pstr)cs_malloc( buffer_size, "sz_dup" );
	mem_copy		(result,string,buffer_size);
	return			result;
}

inline pstr sz_cpy							(pstr destination, u32 const destination_size, pcstr source)
{
#ifdef _MSC_VER
	errno_t const error	= strcpy_s( destination, destination_size, source );
	CS_UNREFERENCED_PARAMETER	( error );
#elif defined(__GCC__) // #ifdef _MSC_VER
	strlcpy			( destination, source, destination_size );
#elif defined(__SNC__) // #ifdef _MSC_VER
	CS_UNREFERENCED_PARAMETER	( destination_size );
	strcpy			( destination, source );
#else // #elif defined(__SNC__)
#	error define strings::copy for your platform here
#endif // #ifdef _MSC_VER
	return			destination;
}

template <int size>
inline pstr sz_cpy							(char (&destination)[size], pcstr source)
{
	return			sz_cpy(&destination[0],size,source);
}

inline pstr sz_ncpy							(pstr destination, u32 const destination_size, pcstr source, u32 const count)
{
#ifdef _MSC_VER
	errno_t const error	= strncpy_s( destination, destination_size, source, count );
	CS_UNREFERENCED_PARAMETER	( error );
#elif defined(__GCC__) // #ifdef _MSC_VER
	strlcpy			( destination, source, destination_size );
#elif defined(__SNC__) // #ifdef _MSC_VER
	strncpy			( destination, source, count );
#else // #elif defined(__SNC__)
#	error define strings::copy for your platform here
#endif // #ifdef _MSC_VER
	return			destination;
}

template <int size>
inline pstr sz_ncpy							(char (&destination)[size], pcstr source, u32 const count)
{
	return			sz_ncpy(&destination[0],size,source,count);
}

inline pstr sz_cat							(pstr destination, u32 const size, pcstr source)
{
#ifdef _MSC_VER
	errno_t const error	= strcat_s( destination, size, source );
	R_ASSERT		( !error );
#else // #ifdef _MSC_VER
	CS_UNREFERENCED_PARAMETER	( size );
	strcat			( destination, source );
#endif // #ifdef _MSC_VER
	return			( destination );
}

template <int size>
inline pstr sz_cat							(char (&destination)[size], pcstr source)
{
	return			sz_cat(&destination[0],size,source);
}

inline pstr sz_trim							(pstr string)
{
	sz_trim_left	(string);
	sz_trim_right	(string);
	return			string;
}

inline int sz_cmp							(pcstr _0, pcstr _1)
{
	return			strcmp(_0,_1);
}

inline int sz_cmp							(shared_string const& _0, shared_string const& _1)
{
	ASSERT			(*_0, "shared string is null");
	ASSERT			(*_1, "shared string is null");
	return			sz_cmp(*_0,*_1);
}

inline int sz_icmp							(pcstr const left, pcstr const right)
{
#if defined(_MSC_VER)
	return			_stricmp( left, right );
#elif defined(__SNC__) || defined(__GCC__)
	pcstr i = left, j = right;
	for ( ; *i && *j; ++i, ++j ) {
		int const lower_i	= tolower(*i);
		int const lower_j	= tolower(*j);
		if ( lower_i < lower_j )
			return	-1;

		if ( lower_i > lower_j )
			return	1;
	}

	return			(*i < *j) ? -1 : ((*i > *j) ? 1 : 0);
#else // #elif defined(__SNC__) || defined(__GCC__)
#	error define here implementation of stricmp for your platform
#endif // #if defined(_MSC_VER)
}

inline pstr sz_lwr							( pstr const source, u32 count )
{
#ifdef _MSC_VER
	errno_t const error	= _strlwr_s( source, count );
	R_ASSERT		( !error, "cannot lowercase string \"%s\" (%d)", source, count );
#else // #ifdef _MSC_VER
	for ( pstr i=source; *i; ++i )
		*i			= tolower( *i );
#endif // #ifdef _MSC_VER
	return			source;
}

inline pstr sz_concat						(pstr destination, u32 const size, pcstr _0)
{
	return			sz_cpy(destination,size,_0);
}

template <int size>
inline pstr sz_concat						(char (&destination)[size], pcstr _0)
{
	return			sz_cpy(destination,size,_0);
}

inline pstr sz_concat						(pstr destination, u32 const size, pcstr _0, pcstr _1)
{
	return			sz_cat(sz_concat(destination,size,_0),size,_1);
}

template <int size>
inline pstr sz_concat						(char (&destination)[size], pcstr _0, pcstr _1)
{
	return			sz_cat(sz_concat(destination,size,_0),size,_1);
}

inline pstr sz_concat						(pstr destination, u32 const size, pcstr _0, pcstr _1, pcstr _2)
{
	return			sz_cat(sz_concat(destination,size,_0,_1),size,_2);
}

template <int size>
inline pstr sz_concat						(char (&destination)[size], pcstr _0, pcstr _1, pcstr _2)
{
	return			sz_cat(sz_concat(destination,size,_0,_1),size,_2);
}

inline pstr sz_concat						(pstr destination, u32 const size, pcstr _0, pcstr _1, pcstr _2, pcstr _3)
{
	return			sz_cat(sz_concat(destination,size,_0,_1,_2),size,_3);
}

template <int size>
inline pstr sz_concat						(char (&destination)[size], pcstr _0, pcstr _1, pcstr _2, pcstr _3)
{
	return			sz_cat(sz_concat(destination,size,_0,_1,_2),size,_3);
}

template <typename predicate_type>
inline bool	iterate_items					(pcstr string, predicate_type const& predicate, char const& separator)
{
	return			iterate_items(string,sz_len(string),predicate,separator);
}

template <typename predicate_type>
inline bool iterate_items					(str_shared string, predicate_type const& predicate, char const& separator)
{
	return			iterate_items(*string,string.length(),predicate,separator);
}

template <typename predicate_type>
inline bool iterate_items					(pcstr string, u32 const length, predicate_type const& predicate, char const& separator)
{
	u32				n = length;
	pstr			temp_string = (pstr)ALLOCA((n+1)*sizeof(char));
	pcstr			I = string;
	pstr			i = temp_string, j = i;
	int				index = 0;
	for ( ; *I; ++I, ++i) {
		if (*I != separator) {
			*i		= *I;
			continue;
		}

		*i			= 0;

		if (!predicate(index++, j, int(i - j), false))
			return	false;

		j			= i + 1;
	}

	if (!index)
		return		predicate(index, string, length, true);

	*i				= 0;
	return			predicate(index, j, int(i - j), true);
}

#endif // #ifndef CS_CORE_STRING_FUNCTIONS_INLINE_H_INCLUDED