////////////////////////////////////////////////////////////////////////////
//	Created 	: 31.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef UNMANAGED_STRING_INLINE_H_INCLUDED
#define UNMANAGED_STRING_INLINE_H_INCLUDED

inline unmanaged_string::unmanaged_string	( System::String^ string )
{
	//int const buffer_size		= int( (string->Length + 1) * 2 );
	//m_string					= ( pstr ) MALLOC( (u32)buffer_size, "unmanaged string" );
	//pin_ptr<wchar_t const> wch	= PtrToStringChars( string );
	//LPCSTR const default_character	= "_";
	//size_t const converted_characters_count =
	//	WideCharToMultiByte(
	//		GetACP(),
	//		0,
	//		wch,
	//		buffer_size,
	//		m_string,
	//		buffer_size,
	//		default_character,
	//		NULL
	//	);

 pin_ptr<const wchar_t> wch = PtrToStringChars(string);

    // Convert to a char*
    size_t const origsize				= wcslen(wch) + 1;
	size_t const buffer_size			= origsize*2;
    size_t converted_characters_count	= 0;

	m_string					= ( pstr ) MALLOC( buffer_size, "unmanaged string" );

    errno_t err = wcstombs_s(&converted_characters_count, m_string, buffer_size, wch, _TRUNCATE);

	R_ASSERT					( (err==0) &&(converted_characters_count==origsize), 
									"[tostring][failed] : wcstombs_s failed" );
}


inline unmanaged_string::~unmanaged_string	( )
{
	FREE					( m_string );
}

inline pcstr unmanaged_string::c_str		( ) const
{
	return					( m_string );
}

#endif // #ifndef UNMANAGED_STRING_INLINE_H_INCLUDED